#include <map>

#include "core/file_functions.h"
#include "core/must.h"
#include "core/process.h"
#include "core/string_functions.h"
#include "core/utf8.h"
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

filesystem::path cppsrc_files = "/media/second/cppsrc/files";

void Main(const std::vector<string>& args) {
  Connection connection(
      "postgresql://postgres:klaseddOcs0@localhost?dbname=cppsrc");

  Result packages = connection.ExecuteCommand(
      "select distinct component, package from package_attributes");

  for (int i = 0; i < packages.NumRows(); ++i) {
    string component = packages.Get<string>(i, 0).value();
    string package = packages.Get<string>(i, 1).value();
    filesystem::path package_root =
        cppsrc_files / component / category(package).to_string() / package;
    MUST(filesystem::is_directory(package_root));

    connection.ExecuteCommand("begin");

    try {
      for (filesystem::recursive_directory_iterator it =
               filesystem::recursive_directory_iterator(package_root);
           it != filesystem::recursive_directory_iterator(); ++it) {
        filesystem::path abspath = *it;
        bool is_regular_file = false;
        try {
          is_regular_file = filesystem::is_regular_file(abspath);
        } catch (...) {
        }

        if (is_regular_file) {
          std::vector<filesystem::path> parts;
          for (filesystem::path part : abspath) {
            parts.push_back(part);
          }
          filesystem::path relpath = parts.at(8);
          for (size_t i = 9; i < parts.size(); ++i) relpath /= parts.at(i);
          int64 size = filesystem::file_size(abspath);

          string srelpath = relpath.string();
          string sabspath = abspath.string();
          std::cout << srelpath << std::endl;
          if (!IsValidUtf8(srelpath)) {
            srelpath = ConvertLatin1ToUtf8(srelpath);
            std::cout << srelpath << std::endl;
            sabspath = ConvertLatin1ToUtf8(sabspath);
          }
          connection.ExecuteCommand(
              "insert into files (component, package, relpath, abspath, size) "
              "values "
              "($1, $2, $3, $4, $5)",
              component, package, srelpath, sabspath, size);
        }
      }

      connection.ExecuteCommand("commit");
    } catch (std::exception& e) {
      std::cerr << "Failed to insert " << package << std::endl;
      std::cerr << "Because " << e.what() << std::endl;
      connection.ExecuteCommand("rollback");
    }
  }
}
