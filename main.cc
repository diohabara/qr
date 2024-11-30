#include <iostream>

#include "qr.h"

int main(int argc, char *argv[]) {
  QrCode qr;
  if (argc != 2) {
    std::cerr << "the number of arguments must be 1 but got "
              << std::to_string(argc) << "\n";
    return 1;
  }
  std::cout << "Input: " << argv[1] << '\n';
  qr.createQrCode(argv[1]);
  qr.printCells();
  // for (auto i = 0; i < 21; i++) {
  //   for (auto j = 0; j < 21; j++) {
  //     std::cout << qr.computeByMask(i, j, true) << ' ';
  //   }
  //   std::cout << '\n';
  // }
}
