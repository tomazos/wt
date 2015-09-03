#pragma once

#include <boost/filesystem.hpp>
#include <google/protobuf/message.h>

#include "core/bigint.h"

string GetFileContents(const filesystem::path& path);

void SetFileContents(const filesystem::path& path, const string& content);

void GetFileAttribute(const filesystem::path& path, const string& name,
                      protobuf::Message& message);

void SetFileAttribute(const filesystem::path& path, const string& name,
                      const protobuf::Message& message);

int64 LastModificationTime(const filesystem::path& p);

class FileLock {
 public:
  FileLock(const filesystem::path& path);
  ~FileLock();

 private:
  filesystem::path path_;
  int fd_;
};
