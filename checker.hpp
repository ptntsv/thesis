#pragma once

#include "spot/tl/formula.hh"
#include "spot/tl/simplify.hh"
#include "spot/twaalgos/translate.hh"
#include <iosfwd>
#include <map>
#include <string>
#include <vector>

using std::vector, std::map, spot::formula, std::string;

// Splits comma/newline separated input into trimmed, non-empty formula strings.
vector<string> split_formulas(std::istream &in);

class SatChecker {
public:
  SatChecker();
  SatChecker(const SatChecker &) = delete; // trans_ holds a pointer to simp_
  SatChecker &operator=(const SatChecker &) = delete;

  formula simplify(formula f);
  bool satisfiable(const vector<formula> &fs);

private:
  spot::tl_simplifier simp_; // must be declared before trans_
  spot::translator trans_{&simp_};
};

// Parses and simplifies every string. Throws spot::parse_error on bad syntax.
vector<formula> parse_formulas(const vector<string> &strs, SatChecker &checker);

struct Result {
  map<formula, vector<formula>> f_to_mus;
  map<formula, vector<formula>> f_to_mss;
  map<formula, vector<formula>>
      f_to_support; // Set S is a support of f iff S |= f and no subset of S
                    // implies f
};

vector<formula> leave_one_out(const formula &f, const vector<formula> &F);

// Maps every f in F to leave_one_out(f, F), i.e. F \ {f}.
map<formula, vector<formula>> leave_one_out_map(const vector<formula> &F);

vector<vector<formula>> find_MUSes(const vector<formula> &F);

Result make_result(const vector<formula> &F, const bool satisfiable);

void print_result(std::ostream &out, const Result &result);

// Whole pipeline: reads formulas from in, writes the report to out and errors
// to err. Returns the process exit code.
int run(std::istream &in, std::ostream &out, std::ostream &err);
