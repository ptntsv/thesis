#include "checker.hpp"
#include "spot/tl/parse.hh"
#include "utils.hpp"
#include <algorithm>
#include <istream>
#include <ostream>
#include <print>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string_view>

static string trim(std::string_view s) {
  const char *ws = " \t\r\n";
  auto begin = s.find_first_not_of(ws);
  if (begin == std::string_view::npos)
    return {};
  auto end = s.find_last_not_of(ws);
  return string(s.substr(begin, end - begin + 1));
}

vector<string> split_formulas(std::istream &in) {
  vector<string> formulas;
  string line;
  while (std::getline(in, line)) {
    std::istringstream ss(line);
    string item;
    while (std::getline(ss, item, ',')) {
      string f = trim(item);
      if (!f.empty())
        formulas.push_back(std::move(f));
    }
  }
  return formulas;
}

SatChecker::SatChecker() {
  trans_.set_type(spot::postprocessor::GeneralizedBuchi);
  trans_.set_pref(spot::postprocessor::Any);
  trans_.set_level(spot::postprocessor::Low);
}

formula SatChecker::simplify(formula f) { return simp_.simplify(f); }

bool SatChecker::satisfiable(const vector<formula> &fs) {
  auto conjunction = formula::And(fs);
  return !trans_.run(conjunction)->is_empty();
}

vector<formula> parse_formulas(const vector<string> &strs,
                               SatChecker &checker) {
  vector<formula> F;
  for (const auto &fstr : strs)
    F.push_back(checker.simplify(spot::parse_formula(fstr)));
  return F;
}

vector<formula> leave_one_out(const formula &f, const vector<formula> &F) {
  vector<formula> items{};
  for (const auto &g : F) {
    if (f == g)
      continue;
    items.push_back(g);
  }
  return items;
}

map<formula, vector<formula>> leave_one_out_map(const vector<formula> &F) {
  map<formula, vector<formula>> f_to_rest{};
  for (const auto &f : F)
    f_to_rest[f] = leave_one_out(f, F);
  return f_to_rest;
}

static void print_set(std::ostream &out, const vector<formula> &fs) {
  out << '{';
  bool first = true;
  for (const auto &f : fs) {
    if (!first)
      out << ", ";
    out << f;
    first = false;
  }
  out << '}';
}

constexpr size_t MAX_FORMULAS_N = 8;
using mask_t = std::bitset<MAX_FORMULAS_N>;

static vector<formula> pick(const vector<formula> &F, const mask_t &mask) {
  vector<formula> S{};
  for (size_t i = 0; i < F.size(); ++i) {
    if (mask[i])
      S.push_back(F[i]);
  }
  return S;
}

static bool is_subset(const mask_t &s1, const mask_t &s2) {
  return (s1 & s2) == s1;
}

vector<vector<formula>> find_MUSes(SatChecker &checker,
                                   const vector<formula> &F) {
  if (MAX_FORMULAS_N < F.size()) {
    throw std::invalid_argument("too many formulas for mask_t");
  }
  vector<mask_t> found{};
  for (auto &&submask : submasks<MAX_FORMULAS_N>(F.size())) {
    auto contains_mus = std::ranges::any_of(
        found, [&](mask_t &m) { return is_subset(m, submask); });
    if (contains_mus) {
      continue;
    }
    if (!checker.satisfiable(pick(F, submask))) {
      found.push_back(submask);
    }
  }
  return found |
         std::views::transform([&](const mask_t &m) { return pick(F, m); }) |
         std::ranges::to<vector>();
}

Result make_result(const vector<formula> &F, const bool satisfiable) {
  Result res{};
  if (satisfiable) {
    res.f_to_mss = leave_one_out_map(F);
  } else {
  }
  return res;
}

void print_result(std::ostream &out, const Result &result) {}

int run(std::istream &in, std::ostream &out, std::ostream &err) {
  SatChecker checker;
  vector<formula> F;
  try {
    F = parse_formulas(split_formulas(in), checker);
  } catch (const std::exception &e) {
    err << e.what() << '\n';
    return 1;
  }

  auto sat = checker.satisfiable(F);
  auto result = make_result(F, sat);
  std::println("{}", find_MUSes(checker, F));

  return 0;
}
