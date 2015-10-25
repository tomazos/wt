#pragma once

#include <map>
#include <set>
#include <vector>

#include "whee/paths.h"
#include "whee/rule.pb.h"

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
  RuleProto proto;
  std::vector<RuleRef> dependencies;

  std::vector<string> cc_headers() const {
    if (proto.kind() != RuleProto::PROTO) {
      return {proto.headers().begin(), proto.headers().end()};
    }

    std::vector<string> headers;

    for (const string& raw_source : proto.sources())
      headers.push_back(ProtoToH(raw_source));

    return headers;
  }

  std::vector<string> cc_sources() const {
    if (proto.kind() != RuleProto::PROTO) {
      return {proto.sources().begin(), proto.sources().end()};
    }

    std::vector<string> sources;

    for (const string& raw_source : proto.sources())
      sources.push_back(ProtoToCC(raw_source));

    return sources;
  }

  bool build_on_platform(const string& platform) const {
    if (proto.platforms_size() == 0) return true;

    for (const string& build_on : proto.platforms())
      if (build_on == platform) return true;

    return false;
  }
};

struct SourceDirectory {
  std::map<string, Rule> rules;
};

using SourceTree = std::map<string, SourceDirectory>;

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
                        const std::set<path>& libraries,
                        const std::vector<string>& flags);

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
