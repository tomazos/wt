#pragma once

#include <map>
#include <set>
#include <vector>

#include "whee/paths.h"

namespace whee {

struct RuleRef {
  string directory;
  string name;

  RuleRef() = default;
  RuleRef(const string& directory_in, const string& name_in)
      : directory(directory_in), name(name_in) {}

  bool operator==(const RuleRef& that) const {
    return directory == that.directory && name == that.name;
  }
  bool operator!=(const RuleRef& that) const { return !(*this == that); }
  bool operator<(const RuleRef& that) const {
    return directory < that.directory ||
           (directory == that.directory && name < that.name);
  }
  bool operator>(const RuleRef& that) const { return that < *this; }
  bool operator<=(const RuleRef& that) const { return !(*this > that); }
  bool operator>=(const RuleRef& that) const { return !(*this < that); }
};

struct Rule {
  string name;
  string full_name;
  enum RuleKind { PROTO, LIBRARY, PROGRAM, TEST };
  RuleKind kind;
  std::vector<string> raw_headers;
  std::vector<string> raw_sources;
  std::vector<string> raw_datafiles;
  std::vector<RuleRef> dependencies;

  std::vector<string> cc_headers() const {
    if (kind != PROTO) {
      return raw_headers;
    }

    std::vector<string> headers;

    for (const string& raw_source : raw_sources)
      headers.push_back(ProtoToH(raw_source));

    return headers;
  }

  std::vector<string> cc_sources() const {
    if (kind != PROTO) {
      return raw_sources;
    }

    std::vector<string> sources;

    for (const string& raw_source : raw_sources)
      sources.push_back(ProtoToCC(raw_source));

    return sources;
  }
};

struct SourceDirectory {
  std::map<string, Rule> rules;
};

using SourceTree = std::map<string, SourceDirectory>;

struct Platform {
  string name;
  string tool_prefix;
  string lib_path;
  string flags;
};

using RuleDeps = std::map<RuleRef, std::set<RuleRef>>;

class Whee {
 public:
  void Main(std::vector<string> args);

 private:
  Paths paths;

  void Help(const std::vector<string>& args);

  void Init(const std::vector<string>& args);

  void TidyFile(const path& p);

  void ForEachSourcePath(std::function<void(const path&)> source_path_function);

  void Tidy(const std::vector<string>& args);

  SourceTree GetSourceTree();

  string GenProtoCommand(const path& source, const path& pb_root,
                         const std::vector<path> include_paths,
                         const path& pb_stderr);

  string CompileCommand(const Platform& platform, const path& source,
                        const path& object, const path& primitives_header,
                        const std::vector<path> include_paths);

  string LibraryCommand(const Platform& platform, const path& library,
                        const std::vector<path>& objects);

  string ProgramCommand(const Platform& platform, const path& program,
                        const std::set<path>& libraries);

  RuleDeps ResolveRuleDeps(const SourceTree& source_tree);

  void HardLinkProtoFiles(const SourceTree& source_tree,
                          const path& protos_superroot);

  void GenProtoFiles(const SourceTree& source_tree,
                     const path& protos_superroot, const path& pb_root,
                     const std::map<RuleRef, std::set<RuleRef>>& ruledeps);

  void HardLinkSourceFiles(const SourceTree& source_tree, const path& pb_root,
                           const path& headers_superroot,
                           const path& units_superroot);

  std::vector<Platform> ReadPlatforms();

  void Build(const std::vector<string>& args);
};

}  // namespace whee
