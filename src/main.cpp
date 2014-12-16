#include <iostream>
#include "scene_lua.hpp"
#include "algebra.hpp"
#include <stdlib.h>

int THREAD_NUM = 1;
int DOF_SAMPLES = 0;

int main(int argc, char** argv)
{
  std::string filename = "sample.lua";
  if (argc >= 2) {
    filename = argv[1];
  }

  if (argc >= 3) {
  	THREAD_NUM = atoi(argv[2]);
  }

  if (argc >= 4) {
  	DOF_SAMPLES = atoi(argv[3]);
  }

  if (!run_lua(filename)) {
    std::cerr << "Could not open " << filename << std::endl;
    return 1;
  }
}

