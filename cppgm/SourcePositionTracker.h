#pragma once

struct SourcePosition {
  string filename;
  size_t linenum;
};

string CalcRelFilePath(const string& base, const string& path);

class SourcePositionTracker {
 public:
  string get_file_str() { return get_pos().filename; }

  SourcePosition get_pos() { return positions.back(); }

  void set_line(size_t linenum) { positions.back().linenum = linenum; }

  void set_line(size_t linenum, const string& filename) {
    positions.back() = SourcePosition{filename, linenum};
  }

  void inc_line() { positions.back().linenum++; }

  void push_include(const string& filename) {
    positions.emplace_back(SourcePosition{filename, 1});
  }

  void pop_include() { positions.pop_back(); }

  string calc_rel_top(const string& pathf) {
    string current = positions.back().filename;

    return CalcRelFilePath(current, pathf);
  }

 private:
  vector<SourcePosition> positions;
};
