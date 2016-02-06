#pragma once

#include "Tokens.h"

struct ASTBase;

struct astempty_t {};

extern astempty_t astempty;

struct AST {
  AST() {}
  AST(std::nullptr_t) {}
  AST(astempty_t);
  AST(const PToken& token);

  AST(const AST&);
  AST(AST&&);

  AST& operator=(const AST&);
  AST& operator=(AST&&);

  explicit operator bool() const;
  bool empty() const;

  const AST& operator[](const string& key) const;
  const AST& operator[](size_t idx) const;

  AST& operator[](const string& key);
  AST& operator[](size_t idx);

  bool has(const string& key) const;

  void write(size_t depth, ostream&) const;

  bool getp(const string& key) const { return props.count(key); }
  AST& setp(const string& key) {
    props.insert(key);
    return (*this);
  }

 private:
  set<string> props;
  shared_ptr<ASTBase> m;
};

ostream& operator<<(ostream&, const AST&);
