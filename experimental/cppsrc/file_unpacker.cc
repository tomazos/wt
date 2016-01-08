#include <map>

#include "core/file_functions.h"
#include "core/must.h"
#include "core/process.h"
#include "core/string_functions.h"
#include "database/postgresql/connection.h"
#include "database/postgresql/result.h"
#include "main/args.h"

using database::postgresql::Connection;
using database::postgresql::Result;
using database::postgresql::bytea;

string_view category(string_view package) {
  if (package.substr(0, 3) == "lib")
    return package.substr(0, 4);
  else
    return package.substr(0, 1);
}

bool ends_with(string_view subject, string_view suffix) {
  return subject.substr(subject.size() - suffix.size(), suffix.size()) ==
         suffix;
}

void process_dsc(Connection& connection, string_view component,
                 string_view package, const filesystem::path& path) {
  static filesystem::path working_dir = "/home/zos/file_processor/working_dir";
  static filesystem::path build_dir = working_dir / "build";
  static filesystem::path files_dir = "/media/second/cppsrc/files";
  filesystem::create_directories(working_dir);
  filesystem::remove_all(working_dir);
  filesystem::create_directories(working_dir);
  filesystem::current_path(working_dir);
  ExecuteShellCommand("dpkg-source -x ", path.string(), " ",
                      build_dir.string());
  for (filesystem::recursive_directory_iterator it =
           filesystem::recursive_directory_iterator(build_dir);
       it != filesystem::recursive_directory_iterator(); ++it) {
    filesystem::path src = *it;
    bool is_regular_file = false;
    try {
      is_regular_file = filesystem::is_regular_file(src);
    } catch (...) {
    }

    if (is_regular_file) {
      string fullpath_string = src.string();
      static string build_dir_string = build_dir.string() + "/";
      MUST(fullpath_string.substr(0, build_dir_string.size()) ==
           build_dir_string);
      string relative_string = fullpath_string.substr(build_dir_string.size());
      filesystem::path dest = files_dir / component.to_string() /
                              category(package).to_string() /
                              package.to_string() / relative_string;
      filesystem::create_directories(dest.parent_path());
      filesystem::copy_file(src, dest);
    }
  }

  filesystem::remove_all(working_dir);
}

void Main(const std::vector<string>& args) {
  filesystem::path prefix = args.at(0);
  MUST(filesystem::is_directory(prefix));
  Connection connection(
      "postgresql://postgres:klaseddOcs0@localhost?dbname=cppsrc");
  Result result = connection.ExecuteCommand(
      "select component, package, value from package_attributes where key = "
      "'Files'");
  std::vector<string> errors;
  std::vector<string> failed_packages;
  for (int i = 0; i < result.NumRows(); ++i) {
    string component = result.Get<string>(i, 0).value();
    string package = result.Get<string>(i, 1).value();
    string value = result.Get<string>(i, 2).value();
    std::vector<string_view> values_split;
    Scan(value, " ", [&](string_view v) { values_split.push_back(v); });
    MUST_EQ(values_split.size(), 3u, value);
    string_view filename = values_split[2];

    filesystem::path path = prefix / component / category(package).to_string() /
                            package / filename.to_string();
    MUST(filesystem::is_regular_file(path));
    if (ends_with(path.string(), ".dsc")) {
      try {
        process_dsc(connection, component, package, path);
      } catch (std::exception& e) {
        errors.push_back(e.what());
        failed_packages.push_back(package);
      }
    }
  }
  std::cout << result.NumRows() << " packages processed." << std::endl;
  std::cout << failed_packages.size() << " failed:" << std::endl;
  for (size_t i = 0; i < failed_packages.size(); ++i) {
    std::cout << "  " << failed_packages[i] << ": " << errors[i] << std::endl;
  }
}
