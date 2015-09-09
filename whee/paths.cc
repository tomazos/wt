#include "whee/paths.h"

#include "core/must.h"

namespace whee {

string ProtoToH(const string& s) { return path(s).stem().string() + ".pb.h"; }

string ProtoToCC(const string& s) { return path(s).stem().string() + ".pb.cc"; }

const path& GetSourceRoot() {
  static optional<path> source_root;
  if (!source_root) {
    source_root = FindSourceRoot();
  }
  MUST(source_root, "Not inside a whee source tree.");
  return *source_root;
}

const path& GetWheeDir() {
  static optional<path> whee_dir;
  if (!whee_dir) {
    whee_dir = GetSourceRoot() / ".whee";
  }
  return whee_dir.value();
}

const optional<path>& GetGitDir() {
  static optional<path> git_dir;
  if (!git_dir) {
    const path candidate_git_dir = GetSourceRoot() / ".git";
    if (exists(candidate_git_dir)) {
      git_dir = candidate_git_dir;
    }
  }
  return git_dir;
}

bool ContainsWheeDirectory(const path& candidate_source_root) {
  const path whee_dir = candidate_source_root / ".whee";
  return is_directory(whee_dir);
}

optional<path> FindSourceRoot(const path& start_path) {
  MUST(exists(start_path), start_path, "is not a directory.");
  for (path p = start_path; exists(p); p = p.parent_path())
    if (ContainsWheeDirectory(p)) return p;

  return nullopt;
}

Paths GetPaths() {
  const path root = GetSourceRoot();
  const path whee = GetWheeDir();
  const optional<path> git = GetGitDir();
  return {root, whee, git};
}

}  // namespace
