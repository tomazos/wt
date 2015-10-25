#include "whee/whee.h"

#include <boost/algorithm/string.hpp>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "core/env.h"
#include "core/file_functions.h"
#include "core/must.h"
#include "core/process.h"
#include "main/args.h"
#include "whee/source_file_attributes.pb.h"
#include "whee/source_root_sentinal.h"
#include "xxua/api.h"
#include "xxua/context.h"
#include "xxua/proto.h"
#include "xxua/value.h"

namespace whee {

using namespace boost::filesystem;
using namespace xxua;
using boost::filesystem::path;

const string source_root_sentinal = GetSourceRootSentinal();

void Whee::Main(std::vector<string> args) {
  if (args.empty()) {
    args.push_back("help");
  }
  string subcommand_name = args[0];
  std::vector<string> rest_of_args(args.begin() + 1, args.end());

  using SubcommandFunction =
      std::function<void(const std::vector<string>& rest_of_args)>;
  static const std::map<string, SubcommandFunction> subcommand_function_map = {
      {"help", [&](const std::vector<string>& x) { return Help(x); }},
      {"init", [&](const std::vector<string>& x) { return Init(x); }},
      {"tidy", [&](const std::vector<string>& x) { return Tidy(x); }},
      {"build", [&](const std::vector<string>& x) { return Build(x); }}};

  const SubcommandFunction* subcommand_function =
      FindOrNull(subcommand_function_map, subcommand_name);
  if (subcommand_function)
    (*subcommand_function)(rest_of_args);
  else
    FAIL("No such subcommand: whee ", subcommand_name);
}

void Whee::Help(const std::vector<string>& args) {
  static constexpr char helptext[] = R"(
Whee is a C++ build system.

Usage:

    $ whee <subcommand> [...]

Subcommands:

 - whee help: this message
 - whee init <dir>: Sets up dir as a source tree root.
 - whee tidy: tidies up the source tree.
 - whee build: builds the source tree.
)";

  static constexpr char helptext_init[] = R"(
Usage:

    $ whee init <dir>

Sets up dir as a source tree root.  This creates and populates a .whee folder.
)";

  if (args.empty()) {
    std::cout << helptext + 1 << std::endl;
  } else {
    string subcommand = args[0];
    if (subcommand == "init") {
      std::cout << helptext_init + 1 << std::endl;
    } else {
      FAIL("Unknown subcommand: whee ", subcommand);
    }
  }
}

void Whee::Init(const std::vector<string>& args) {
  MUST_EQ(1u, args.size(), "Usage: whee init <dir>");
  const path source_root(args[0]);
  MUST(!FindSourceRoot(source_root));
  MUST(is_directory(source_root), source_root, " is not a directory.");
  const path whee_dir = source_root / ".whee";
  MUST(!exists(whee_dir), whee_dir,
       " already exists.  Delete it first to re-initialize.");
  create_directory(whee_dir);
  SetFileContents(whee_dir / "lock", "");
}

void Whee::TidyFile(const path& p) {
  int64 mod_time = LastModificationTime(p);
  SourceFileAttributes attributes;

  GetFileAttribute(p, "user.srcfile", attributes);
  if (attributes.last_tidy() != mod_time) {
    if (file_size(p) < 500'000) {
      ExecuteShellCommand("clang-format-3.6 -style=Google -i ", p);
    }
    attributes.set_last_tidy(LastModificationTime(p));
    SetFileAttribute(p, "user.srcfile", attributes);
  }
}

void Whee::ForEachSourcePath(
    std::function<void(const path&)> source_path_function) {
  source_path_function(canonical(paths.root));
  for (recursive_directory_iterator it(paths.root);
       it != recursive_directory_iterator(); ++it) {
    const path source_path = it->path();
    if (equivalent(source_path, paths.whee) ||
        (paths.git && equivalent(source_path, *paths.git)))
      it.no_push();
    else {
      source_path_function(canonical(source_path));
    }
  }
}

void Whee::Tidy(const std::vector<string>& args) {
  paths = GetPaths();
  FileLock l(paths.whee / "lock");
  MUST(args.empty());

  ForEachSourcePath([&](const path& source_path) {
    const path extension = source_path.extension();
    if (extension == ".h" || extension == ".cc" || extension == ".c") {
      TidyFile(source_path);
    }
  });
}

static RuleProto::Kind RuleNameToRuleKind(string_view rule_name) {
  if (rule_name == "proto") {
    return RuleProto::PROTO;
  } else if (rule_name == "library") {
    return RuleProto::LIBRARY;
  } else if (rule_name == "program") {
    return RuleProto::PROGRAM;
  } else if (rule_name == "test") {
    return RuleProto::TEST;
  } else {
    FAIL("no such rule kind ", rule_name);
  }
}

static Values AddRule(std::vector<RuleProto>& rules, string_view rule_name,
                      const Values& args) {
  if (args.size() != 1) Throw("expected 1 argument");
  rules.emplace_back();
  TableToProto(args.at(0), rules.back());
  rules.back().set_kind(RuleNameToRuleKind(rule_name));
  return {};
}

static std::vector<RuleProto> ParseRulesFile(
    const filesystem::path& rules_file) {
  State state;
  Context context(state);

  Value add_rules = Compile(GetFileContents(rules_file));
  std::vector<RuleProto> rules;
  for (string_view rule_name : {"test", "library", "program", "proto"})
    Global().insert(rule_name,
                    MakeFunction([&, rule_name](const Values& args) -> Values {
                      return AddRule(rules, rule_name, args);
                    }));
  add_rules({});
  return rules;
}

SourceTree Whee::GetSourceTree() {
  SourceTree source_tree;

  const string source_root_string = paths.root.string() + "/";
  const size_t source_root_strlen = source_root_string.size();
  ForEachSourcePath([&](const path& source_path) {
    if (!is_regular_file(source_path)) return;
    const path directory_path = canonical(source_path.parent_path());
    string directory = directory_path.string();
    if (directory + "/" == source_root_string) {
      directory = "";
    } else {
      MUST_EQ(source_root_string, directory.substr(0, source_root_strlen),
              directory);
      directory = directory.substr(source_root_strlen);
    }
    if (source_path.filename() == "RULES.cm") {
      std::vector<RuleProto> rule_protos = ParseRulesFile(source_path);

      for (const RuleProto& rule_proto : rule_protos) {
        Rule rule;
        rule.proto = rule_proto;

        for (const string& dependency : rule.proto.dependencies()) {
          MUST(!dependency.empty());
          RuleRef rule_ref;
          if (dependency[0] == '/') {
            size_t pos = dependency.find_last_of("/");
            if (pos == 0) {
              rule_ref.directory = "";
            } else {
              rule_ref.directory = dependency.substr(1, pos - 1);
            }
            rule_ref.name = dependency.substr(pos + 1);
          } else {
            rule_ref.directory = directory;
            rule_ref.name = dependency;
          }
          rule.dependencies.push_back(rule_ref);
        }

        Insert(source_tree[directory].rules, rule.proto.name(), rule);
      }
    }
  });
  return source_tree;
}

string Whee::GenProtoCommand(const path& source, const path& pb_root,
                             const std::vector<path> include_paths,
                             const path& pb_stderr) {
  std::vector<string> include_paths_strings;
  for (const path& include_path : include_paths)
    include_paths_strings.push_back("--proto_path=" + include_path.string());

  return EncodeAsString("protoc ",
                        boost::algorithm::join(include_paths_strings, " "),
                        " --cpp_out=", pb_root.string(), " ", source.string(),
                        " 2> ", pb_stderr.string());
}

string Whee::CompileCommand(const Platform& platform, const path& source,
                            const path& object, const path& primitives_header,
                            const std::vector<path> include_paths) {
  std::vector<string> include_paths_strings;
  for (const path& include_path : include_paths)
    include_paths_strings.push_back("-I" + include_path.string());

  return EncodeAsString(
      platform.tool_prefix(), "g++ -c -g -std=gnu++14 -Wall -Werror -O3 ",
      platform.flags(), " ", boost::algorithm::join(include_paths_strings, " "),
      " -include ", primitives_header.string(), " -o ", object.string(), " ",
      source.string(), " 2> ", object.string(), ".stderr");
}

string Whee::LibraryCommand(const Platform& platform, const path& library,
                            const std::vector<path>& objects) {
  std::vector<string> object_strings;
  for (const path& object : objects) object_strings.push_back(object.string());
  return EncodeAsString(platform.tool_prefix(), "ar rcs ", library.string(),
                        " ", boost::algorithm::join(object_strings, " "));
}

string Whee::ProgramCommand(const Platform& platform, const path& program,
                            const std::set<path>& libraries,
                            const std::vector<string>& flags) {
  std::vector<string> library_strings;
  for (const path& library : libraries)
    library_strings.push_back(library.string());
  return EncodeAsString(
      platform.tool_prefix(), "g++ -std=gnu++14 -g -O3 ", platform.flags(),
      " -o ", program.string(), " -Wl,--start-group -Wl,--whole-archive ",
      boost::algorithm::join(library_strings, " "), " -Wl,--no-whole-archive ",
      boost::algorithm::join(flags, " "), " -Wl,--no-whole-archive ",
      //      " -Wl,--no-whole-archive -lboost_filesystem -lboost_system
      //      -lprotobuf "
      //      "-Wl,--whole-archive -lpthread -Wl,--no-whole-archive -lmmal "
      //      "-lmmal_core -lmmal_util -lmmal_components -lvcos "
      //      "-Wl,--whole-archive -lmmal_vc_client -Wl,--no-whole-archive "
      //      "-lvchiq_arm -lmmal -lmmal_core -lmmal_util -lmmal_components "
      //      "-lvcos -lvcsm -Wl,--end-group ",
      platform.lib_path());
}

RuleDeps Whee::ResolveRuleDeps(const SourceTree& source_tree) {
  RuleDeps ruledeps;

  RuleRef primitives_rule("", "primitives");

  for (const auto& directory_kv : source_tree) {
    const string& directory_name = directory_kv.first;
    const SourceDirectory& directory = directory_kv.second;

    for (const auto& rule_kv : directory.rules) {
      const string& rule_name = rule_kv.first;
      const Rule& rule = rule_kv.second;

      RuleRef me(directory_name, rule_name);
      std::set<RuleRef> deps;
      deps.insert(me);
      if (rule.proto.kind() != RuleProto::PROTO) deps.insert(primitives_rule);
      bool deps_added;
      do {
        deps_added = false;
        std::set<RuleRef> deps_to_add;
        for (const RuleRef& inner : deps) {
          MUST(ContainsKey(source_tree, inner.directory) &&
                   ContainsKey(source_tree.at(inner.directory).rules,
                               inner.name),
               "bad rule ref: ", inner.directory, "/", inner.name);
          const Rule& inner_rule =
              source_tree.at(inner.directory).rules.at(inner.name);
          for (const RuleRef& outer : inner_rule.dependencies) {
            if (!Contains(deps, outer)) {
              deps_added = true;
              deps_to_add.insert(outer);
            }
          }
        }
        for (const RuleRef& dep_to_add : deps_to_add) {
          deps.insert(dep_to_add);
        }
      } while (deps_added);

      ruledeps[me] = deps;
    }
  }

  return ruledeps;
}

void Whee::HardLinkProtoFiles(const SourceTree& source_tree,
                              const path& protos_superroot) {
  remove_all(protos_superroot);

  for (const auto& directory_kv : source_tree) {
    const string& directory_name = directory_kv.first;
    const SourceDirectory& directory = directory_kv.second;

    for (const auto& rule_kv : directory.rules) {
      const string& rule_name = rule_kv.first;
      const Rule& rule = rule_kv.second;

      if (rule.proto.kind() != RuleProto::PROTO) continue;

      const path protos_root =
          protos_superroot / directory_name / rule_name / source_root_sentinal;

      for (const string& proto : rule.proto.sources()) {
        const path origin_proto = paths.root / directory_name / proto;
        const path linked_proto = protos_root / directory_name / proto;
        create_directories(protos_root / directory_name);
        create_hard_link(origin_proto, linked_proto);
      }
    }
  }
}

void Whee::GenProtoFiles(const SourceTree& source_tree,
                         const path& protos_superroot, const path& pb_root,
                         const std::map<RuleRef, std::set<RuleRef>>& ruledeps) {
  for (const auto& directory_kv : source_tree) {
    const string& directory_name = directory_kv.first;
    const SourceDirectory& directory = directory_kv.second;

    const path pb_directory = pb_root / directory_name;

    create_directories(pb_directory);

    for (const auto& rule_kv : directory.rules) {
      const string& rule_name = rule_kv.first;
      const Rule& rule = rule_kv.second;

      if (rule.proto.kind() != RuleProto::PROTO) continue;

      RuleRef me(directory_name, rule_name);

      const path protos_root =
          protos_superroot / directory_name / rule_name / source_root_sentinal;

      std::vector<path> include_paths;
      int64 protos_last_write_time = 0;

      for (const RuleRef& dep : ruledeps.at(me)) {
        include_paths.push_back(protos_superroot / dep.directory / dep.name /
                                source_root_sentinal);
        const SourceDirectory& dep_directory = source_tree.at(dep.directory);
        const Rule& dep_rule = dep_directory.rules.at(dep.name);
        for (const string& proto_name : dep_rule.proto.sources()) {
          int64 proto_last_write_time =
              LastModificationTime(paths.root / dep.directory / proto_name);
          if (proto_last_write_time > protos_last_write_time)
            protos_last_write_time = proto_last_write_time;
        }
      }

      for (const string& source_file : rule.proto.sources()) {
        const path source = protos_root / directory_name / source_file;
        const path pb_header = pb_directory / ProtoToH(source_file);
        const path pb_source = pb_directory / ProtoToCC(source_file);
        const path pb_stderr =
            pb_directory / (source.stem().string() + ".pb.stderr");

        if (!exists(pb_header) || !exists(pb_source) ||
            LastModificationTime(pb_header) <= protos_last_write_time ||
            LastModificationTime(pb_source) <= protos_last_write_time) {
          optional<Error> error;
          try {
            std::cout << " [PROTO] " << directory_name << "/" << source_file
                      << std::endl;
            ExecuteShellCommand(
                GenProtoCommand(source, pb_root, include_paths, pb_stderr));
          } catch (const Error& e) {
            error = e;
          }

          string stderr = GetFileContents(pb_stderr);
          std::cerr << ReplaceSourceRootSentinal(stderr);
          std::cerr.flush();
          if (error) {
            throw * error;
          }
        }
      }
    }
  }
}

void Whee::HardLinkSourceFiles(const SourceTree& source_tree,
                               const path& pb_root,
                               const path& headers_superroot,
                               const path& units_superroot) {
  remove_all(headers_superroot);
  remove_all(units_superroot);

  for (const auto& directory_kv : source_tree) {
    const string& directory_name = directory_kv.first;
    const SourceDirectory& directory = directory_kv.second;
    const path pb_directory = pb_root / directory_name;

    for (const auto& rule_kv : directory.rules) {
      const string& rule_name = rule_kv.first;
      const Rule& rule = rule_kv.second;

      const path headers_root =
          headers_superroot / directory_name / rule_name / source_root_sentinal;
      const path units_root =
          units_superroot / directory_name / rule_name / source_root_sentinal;

      if (rule.proto.kind() == RuleProto::PROTO) {
        for (const string& proto : rule.proto.sources()) {
          const path pb_header = pb_directory / ProtoToH(proto);
          const path pb_unit = pb_directory / ProtoToCC(proto);

          const path linked_pb_header =
              headers_root / directory_name / ProtoToH(proto);
          const path linked_pb_unit =
              units_root / directory_name / ProtoToCC(proto);
          create_directories(headers_root / directory_name);
          create_hard_link(pb_header, linked_pb_header);
          create_directories(units_root / directory_name);
          create_hard_link(pb_unit, linked_pb_unit);
        }
      } else {
        for (const string& header : rule.proto.headers()) {
          const path origin_header = paths.root / directory_name / header;
          const path linked_header = headers_root / directory_name / header;
          create_directories(headers_root / directory_name);
          create_hard_link(origin_header, linked_header);
        }

        for (const string& unit : rule.proto.sources()) {
          const path origin_unit = paths.root / directory_name / unit;
          const path linked_unit = units_root / directory_name / unit;
          create_directories(units_root / directory_name);
          create_hard_link(origin_unit, linked_unit);
        }
      }
    }
  }
}

// std::vector<Platform> Whee::ReadPlatforms() {
//  std::vector<Platform> result;
//  string s = GetFileContents(paths.root / "CONFIG.cm");

//  using namespace xxua;
//  State state;
//  Context context(state);
//  PushGlobalTable();
//  PushString("add_platform");
//  PushFunction(&state, [&result] {
//    if (StackSize() != 1 || GetType(1) != Type::TABLE)
//      Error("invalid arguments to add_platform");
//    PushNil();
//    Platform platform;
//    while (Next(1)) {
//      string_view key = ToString(-2);
//      string_view val = ToString(-1);
//      if (key == "name")
//        platform.name = val.to_string();
//      else if (key == "tool_prefix")
//        platform.tool_prefix = val.to_string();
//      else if (key == "lib_path")
//        platform.lib_path = val.to_string();
//      else if (key == "flags")
//        platform.flags = val.to_string();
//      else
//        Error(EncodeAsString("unknown platform field: ", key));
//      Pop();
//    }
//    result.push_back(platform);
//    return 0;
//  });
//  PopField(-3);
//  LoadFromString(s);
//  Call(0, 0);
//  return result;
//}

static Values AddPlatform(std::vector<Platform>& platforms,
                          const Values& args) {
  if (args.size() != 1) Throw("expected 1 argument");
  platforms.emplace_back();
  TableToProto(args.at(0), platforms.back());
  return {};
}

static std::vector<Platform> ParsePlatformsFile(
    const filesystem::path& platforms_file) {
  State state;
  Context context(state);

  std::vector<Platform> platforms;
  Value add_platforms = Compile(GetFileContents(platforms_file));
  Global().insert("add_platform",
                  MakeFunction([&](const Values& args) -> Values {
                    return AddPlatform(platforms, args);
                  }));
  add_platforms({});
  return platforms;
}

void Whee::Build(const std::vector<string>& args) {
  paths = GetPaths();

  SetEnv("SOURCE_ROOT", paths.root.string());

  FileLock l(paths.whee / "lock");

  std::vector<Platform> platforms =
      ParsePlatformsFile(paths.root / "CONFIG.cm");
  const SourceTree source_tree = GetSourceTree();

  std::map<RuleRef, std::set<RuleRef>> ruledeps = ResolveRuleDeps(source_tree);

  const path primitives_header = canonical(paths.root / "primitives.h");

  const path protos_superroot = paths.whee / "proto";
  const path pb_root = paths.whee / "proto-gen";
  const path headers_superroot = paths.whee / "headers";
  const path units_superroot = paths.whee / "units";

  HardLinkProtoFiles(source_tree, protos_superroot);

  GenProtoFiles(source_tree, protos_superroot, pb_root, ruledeps);

  HardLinkSourceFiles(source_tree, pb_root, headers_superroot, units_superroot);

  for (const Platform& platform : platforms) {
    const path build_root = paths.whee / "build" / platform.name();
    const path programs_root = paths.whee / "programs" / platform.name();

    std::map<RuleRef, path> library_files;

    for (const auto& directory_kv : source_tree) {
      const string& directory_name = directory_kv.first;
      const SourceDirectory& directory = directory_kv.second;

      const path build_directory = build_root / directory_name;

      create_directories(build_directory);

      for (const auto& rule_kv : directory.rules) {
        const string& rule_name = rule_kv.first;
        const Rule& rule = rule_kv.second;

        if (!rule.build_on_platform(platform.name())) continue;

        RuleRef me(directory_name, rule_name);

        const path headers_root = headers_superroot / directory_name /
                                  rule_name / source_root_sentinal;
        const path units_root =
            units_superroot / directory_name / rule_name / source_root_sentinal;

        std::vector<path> include_paths;
        int64 source_last_write_time = 0;

        for (const string& unit_name : rule.cc_sources()) {
          int64 unit_last_write_time =
              LastModificationTime(units_root / directory_name / unit_name);
          if (unit_last_write_time > source_last_write_time)
            source_last_write_time = unit_last_write_time;
        }

        for (const RuleRef& dep : ruledeps.at(me)) {
          const path dep_headers_root = headers_superroot / dep.directory /
                                        dep.name / source_root_sentinal;
          include_paths.push_back(dep_headers_root);
          const SourceDirectory& dep_directory = source_tree.at(dep.directory);
          const Rule& dep_rule = dep_directory.rules.at(dep.name);
          for (const string& header_name : dep_rule.cc_headers()) {
            int64 header_last_write_time = LastModificationTime(
                dep_headers_root / dep.directory / header_name);
            if (header_last_write_time > source_last_write_time)
              source_last_write_time = header_last_write_time;
          }
        }

        std::vector<path> objects;
        for (const string& source_file : rule.cc_sources()) {
          const path source = units_root / directory_name / source_file;
          const path object = build_directory / (source.stem().string() + ".o");
          if (!exists(object) ||
              LastModificationTime(object) <= source_last_write_time) {
            optional<Error> error;
            try {
              std::cout << " [CC] " << platform.name() << " " << directory_name
                        << "/" << source_file << std::endl;
              ExecuteShellCommand(CompileCommand(
                  platform, source, object, primitives_header, include_paths));
            } catch (const Error& e) {
              error = e;
            }

            string stderr = GetFileContents(object.string() + ".stderr");
            std::cerr << ReplaceSourceRootSentinal(stderr);
            std::cerr.flush();
            if (error) {
              throw * error;
            }
          }
          objects.push_back(object);
        }

        if (!objects.empty()) {
          int64 objects_last_write_time = 0;
          for (const path& object : objects) {
            int64 object_last_write_time = LastModificationTime(object);
            if (object_last_write_time > objects_last_write_time) {
              objects_last_write_time = object_last_write_time;
            }
          }
          const path library = build_directory / (rule_name + ".a");
          if (!exists(library) ||
              LastModificationTime(library) <= objects_last_write_time) {
            remove(library);

            std::cout << " [AR] " << platform.name() << " " << directory_name
                      << "/" << rule_name << std::endl;
            ExecuteShellCommand(LibraryCommand(platform, library, objects));
          }
          library_files[me] = library;
        }
      }
    }

    for (const auto& directory_kv : source_tree) {
      const string& directory_name = directory_kv.first;
      const SourceDirectory& directory = directory_kv.second;

      const path programs_directory = programs_root / directory_name;

      create_directories(programs_directory);

      for (const auto& rule_kv : directory.rules) {
        const string& rule_name = rule_kv.first;
        const Rule& rule = rule_kv.second;

        if (!rule.build_on_platform(platform.name())) continue;

        RuleRef me(directory_name, rule_name);

        if (rule.proto.kind() != RuleProto::PROGRAM &&
            rule.proto.kind() != RuleProto::TEST)
          continue;

        const path program = programs_directory / rule_name;

        std::set<path> libs;
        for (const RuleRef& dep : ruledeps.at(me)) {
          auto* p = FindOrNull(library_files, dep);
          if (p) libs.insert(*p);
        }

        int64 libs_last_write_time = 0;
        for (const path& lib : libs) {
          const int64 lib_last_write_time = LastModificationTime(lib);
          if (lib_last_write_time > libs_last_write_time) {
            libs_last_write_time = lib_last_write_time;
          }
        }
        if (!exists(program) ||
            LastModificationTime(program) <= libs_last_write_time) {
          std::cout << " [LN] " << platform.name() << " " << directory_name
                    << "/" << rule_name << std::endl;

          std::vector<string> flags;
          flags.push_back("-Wl,--no-whole-archive");
          bool whole_archive = false;
          for (const RuleRef& dep : ruledeps.at(me)) {
            const Rule& dep_rule =
                source_tree.at(dep.directory).rules.at(dep.name);
            for (const string& syslib : dep_rule.proto.syslibs()) {
              if (dep_rule.proto.whole_archive() != whole_archive) {
                whole_archive = dep_rule.proto.whole_archive();
                flags.push_back(whole_archive ? "-Wl,--whole-archive"
                                              : "-Wl,--no-whole-archive");
              }
              flags.push_back(syslib);
            }
          }

          string program_command =
              ProgramCommand(platform, program, libs, flags);
          //          LOGEXPR(program_command);
          ExecuteShellCommand(program_command);
        }
        if (rule.proto.kind() == RuleProto::TEST && platform.test()) {
          int64 program_last_mod = LastModificationTime(program);
          SourceFileAttributes attributes;
          GetFileAttribute(program, "user.srcfile", attributes);
          if (attributes.last_tested() != program_last_mod) {
            std::cout << " [TEST] " << platform.name() << " " << directory_name
                      << "/" << rule_name << std::endl;
            ExecuteShellCommand(program.string());
            attributes.set_last_tested(program_last_mod);
            SetFileAttribute(program, "user.srcfile", attributes);
          }
        }
      }
    }
  }
  std::cerr << "/:1:1: Build complete." << std::endl;
}

}  // namespace whee

void Main(const std::vector<string>& args) {
  whee::Whee whee;
  whee.Main(args);
}
