#include "spot/tl/formula.hh"
#include "spot/tl/parse.hh"
#include "spot/twaalgos/postproc.hh"
#include "spot/twaalgos/translate.hh"
#include <iostream>
#include <vector>

using namespace spot;
using std::vector;

bool is_unsat(spot::formula f) {
  spot::translator trans;
  trans.set_pref(spot::postprocessor::Low);
  auto aut = trans.run(f);
  // If the language is empty, there is no model -> unsat.
  return aut->is_empty();
}

bool is_superset(uint64_t mask, const vector<uint64_t> &mincovers) {
  for (uint64_t cover : mincovers) {
    if ((mask & cover) == cover) {
      return true;
    }
  }
  return false;
}

spot::formula build_conjunction(const vector<formula> &Phi, uint64_t mask) {
  std::vector<formula> terms;
  for (size_t i = 0; i < Phi.size(); ++i) {
    if ((mask >> i) & 1) {
      terms.push_back(Phi[i]);
    }
  }
  return spot::formula::And(terms);
}

std::string formula_to_string(spot::formula f) {
  std::ostringstream os;
  os << f;
  return os.str();
}

std::string mask_to_string(const vector<formula> &Phi, uint64_t mask) {
  if (mask == 0)
    return "{ } (True/Empty Set)";
  std::string res = "{ ";
  for (size_t i = 0; i < Phi.size(); ++i) {
    if ((mask >> i) & 1) {
      res += formula_to_string(Phi[i]) + ", ";
    }
  }
  res.erase(res.length() - 2);
  res += " }";
  return res;
}

void dumb_red_check(const vector<formula> &Phi,
                    vector<vector<uint64_t>> &subsets_by_size) {
  auto N = Phi.size();
  for (auto i = 0; i < N; ++i) {
    formula phi = Phi[i];
    vector<uint64_t> mincovers;
    for (size_t size = 1; size <= N - 1; ++size) {
      for (uint64_t mask : subsets_by_size[size]) {
        if ((mask >> i) & 1)
          continue; // Phi' must not contain phi itself

        if (is_superset(mask, mincovers))
          continue;

        // Phi' |= phi  <=>  (Phi' AND NOT phi) is unsat
        spot::formula phi_prime = build_conjunction(Phi, mask);
        spot::formula entail_check =
            spot::formula::And({phi_prime, spot::formula::Not(phi)});

        if (is_unsat(entail_check)) {
          mincovers.push_back(mask);
        }
      }
    }
    std::cout << "Formula [" << formula_to_string(phi)
              << "] minimal covering sets:\n";
    if (mincovers.empty()) {
      std::cout << "  None (Formula is not redundant)\n";
    } else {
      for (uint64_t mask : mincovers) {
        std::cout << "  " << mask_to_string(Phi, mask) << "\n";
      }
    }
  }
}

int main() {
  parsed_formula pf;
  vector string_formulas = {"a", "b", "a & b", "!a", "F a", "G a"};
  vector<formula> Phi;
  for (const auto &s : string_formulas) {
    pf = spot::parse_infix_psl(s);
    if (pf.format_errors(std::cerr))
      return 1;
    Phi.push_back(pf.f);
  }
  size_t N = Phi.size();
  if (N > 63) {
    std::cerr << "Too many formulas for a 64-bit bitmask approach."
              << std::endl;
    return 1;
  }
  vector<vector<uint64_t>> subsets_by_size(N + 1);
  for (uint64_t mask = 0; mask < (1u << N); ++mask) {
    subsets_by_size[std::popcount(mask)].push_back(mask);
  }
  dumb_red_check(Phi, subsets_by_size);
}
