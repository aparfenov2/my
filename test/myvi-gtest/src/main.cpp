#include "gtest/gtest.h"
#include <iostream>


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();

  std::string name;
  std::getline (std::cin,name);

  return ret;
}