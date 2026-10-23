#include "checker.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <input-file>\n";
    return 1;
  }
  std::ifstream in(argv[1]);
  if (!in) {
    std::cerr << "cannot open file: " << argv[1] << '\n';
    return 1;
  }
  return run(in, std::cout, std::cerr);
}
