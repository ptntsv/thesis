#include "spot/tl/formula.hh"
#include "spot/tl/parse.hh"
#include "spot/twaalgos/postproc.hh"
#include "spot/twaalgos/translate.hh"
#include <fstream>
#include <iostream>
#include <spot/tl/simplify.hh>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

static std::string trim(std::string_view s) {
  const char *ws = " \t\r\n";
  auto begin = s.find_first_not_of(ws);
  if (begin == std::string_view::npos)
    return {};
  auto end = s.find_last_not_of(ws);
  return std::string(s.substr(begin, end - begin + 1));
}

static std::vector<std::string> read_formulas_as_strs(const std::string &path) {
  std::ifstream in(path);
  if (!in)
    throw std::runtime_error("cannot open file: " + path);

  std::vector<std::string> formulas;
  std::string line;
  while (std::getline(in, line)) {
    std::istringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ',')) {
      std::string f = trim(item);
      if (!f.empty())
        formulas.push_back(std::move(f));
    }
  }
  return formulas;
}

bool satisfiable(const std::vector<spot::formula> &fs,
                 spot::translator &trans) {
  auto conjunction = spot::formula::And(fs);
  return !trans.run(conjunction)->is_empty();
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <input-file>\n";
    return 1;
  }
  std::vector<std::string> formulas_strs;
  std::vector<spot::formula> formulas;
  spot::tl_simplifier simp{};
  spot::translator trans{&simp};
  trans.set_type(spot::postprocessor::GeneralizedBuchi);
  trans.set_pref(spot::postprocessor::Any);
  trans.set_level(spot::postprocessor::Low);
  try {
    formulas_strs = read_formulas_as_strs(argv[1]);
    for (const auto &fstr : formulas_strs) {
      auto formula = spot::parse_formula(fstr);
      formula = simp.simplify(formula);
      formulas.push_back(std::move(formula));
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }

  if (!satisfiable(formulas, trans)) { // Fast-path
    // In the next series...
  }

  for (const auto &f : formulas)
    std::cout << f << '\n';

  return 0;
}
