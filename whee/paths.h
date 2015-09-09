#pragma once

#include <boost/filesystem.hpp>

namespace whee {

using path = filesystem::path;

struct Paths {
  path root;
  path whee;
  optional<path> git;
};

Paths GetPaths();

string ProtoToH(const string& s);
string ProtoToCC(const string& s);

// bool ContainsWheeDirectory(const path& candidate_source_root);

// const path& GetSourceRoot();
// const path& GetWheeDir();
// const optional<path>& GetGitDir();
optional<path> FindSourceRoot(
    const path& start_path = filesystem::current_path());

}  // namespace whee
