#pragma once

#include "spot/tl/formula.hh"
#include "spot/tl/print.hh"
#include <algorithm>
#include <bitset>
#include <format>
#include <generator>
#include <ranges>
#include <vector>

template <> struct std::formatter<spot::formula> {
  std::formatter<std::string_view> underlying;
  constexpr auto parse(std::format_parse_context &ctx) {
    return underlying.parse(ctx);
  }
  auto format(const spot::formula &f, auto &ctx) const {
    return underlying.format(spot::str_psl(f), ctx);
  }
};

template <size_t N>
std::generator<std::bitset<N>> submask_of_len(size_t mask_n, size_t k) {
  std::bitset<N> b;
  std::vector<char> mask(mask_n, 0);
  std::ranges::fill_n(mask.begin(), k, 1);
  do {
    for (size_t i = 0; i < mask_n; i++) {
      b.set(i, mask[i]);
    }
    co_yield b;
  } while (std::ranges::prev_permutation(mask).found);
}

template <size_t N> std::generator<std::bitset<N>> submasks(size_t mask_n) {
  for (size_t i = 0; i <= mask_n; i++) {
    co_yield std::ranges::elements_of(submask_of_len<N>(mask_n, i));
  }
}
