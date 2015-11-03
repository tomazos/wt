#include <algorithm>

#include "core/file_functions.h"
#include "main/args.h"
#include "cm/context.h"
#include "cm/state.h"
#include "cm/value.h"

using namespace cm;

void Main(const std::vector<string>& args) {
  State state;
  Context context(state);
  const string code = GetStandardInput();
  Value f = Compile(code);
  std::vector<Value> vargs;
  for (const string& arg : args) vargs.emplace_back(arg);
  std::vector<Value> results = f(vargs);
  for (size_t i = 0; i < results.size(); ++i)
    std::cout << "[" << i << "] = " << results[i] << std::endl;
}
