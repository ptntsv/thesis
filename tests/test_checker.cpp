#include "checker.hpp"
#include "spot/tl/parse.hh"
#include <catch2/catch_test_macros.hpp>
#include <sstream>

using spot::parse_formula;

TEST_CASE("split_formulas trims and skips empty items") {
  std::istringstream in("a, !a\n b ,, a | b\n\n");
  REQUIRE(split_formulas(in) == vector<string>{"a", "!a", "b", "a | b"});
}

TEST_CASE("split_formulas on empty input") {
  std::istringstream in("");
  REQUIRE(split_formulas(in).empty());
}

TEST_CASE("parse_formulas rejects bad syntax") {
  SatChecker checker;
  REQUIRE_THROWS(parse_formulas({"a &"}, checker));
}

TEST_CASE("SatChecker::satisfiable") {
  SatChecker checker;
  auto a = parse_formula("a"), na = parse_formula("!a"), b = parse_formula("b");
  CHECK(checker.satisfiable({a, b}));
  CHECK_FALSE(checker.satisfiable({a, na}));
  CHECK_FALSE(
      checker.satisfiable({parse_formula("G a"), parse_formula("F !a")}));
  CHECK(checker.satisfiable({})); // empty conjunction is true
}

TEST_CASE("leave_one_out removes exactly f") {
  auto a = parse_formula("a"), b = parse_formula("b"), c = parse_formula("c");
  auto items = leave_one_out_map({a, b, c});
  REQUIRE(items.size() == 3);
  CHECK(items[a] == vector{b, c});
  CHECK(items[b] == vector{a, c});
  CHECK(items[c] == vector{a, b});
}

TEST_CASE("leave_one_out drops every copy of f") {
  auto a = parse_formula("a"), b = parse_formula("b");
  auto items = leave_one_out_map({a, b, a});
  CHECK(items[a] == vector{b});
}

TEST_CASE("make_result fills every part") {
  auto a = parse_formula("a"), b = parse_formula("b");
  auto result = make_result({a, b}, true);
  CHECK(result.f_to_mus.empty());
  REQUIRE(result.f_to_mss.size() == 2);
  CHECK(result.f_to_mss[a] == vector{b});
  CHECK(result.f_to_mss[b] == vector{a});
  CHECK(result.f_to_support.empty());
}

TEST_CASE("make_result with no formulas") {
  auto result = make_result({}, true);
  CHECK(result.f_to_mus.empty());
  REQUIRE(result.f_to_mss.empty());
  CHECK(result.f_to_support.empty());
}
