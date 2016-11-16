#include "gtest/gtest.h"

#include <iostream>
#include <string>

int main(int argc, char **argv) 
{
    // std::string tmp;
    // std::cout << "DEBUG MODE. type any key to continue...\n";
    // std::cin >> tmp;

  	testing::InitGoogleTest(&argc, argv);
  	return RUN_ALL_TESTS();
}
