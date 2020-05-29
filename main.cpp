
#include <iostream>
#include <vector>

#include "systemc.h"

#include "loader.cpp"
#include "cic.cpp"
#include "oneCIC.cpp"
#include "twoCICs.cpp"

/*
  Runs CIC-ISS with code from an input file.
  (example input files are in ./input_code/)
*/
int sc_main(int argc, char* argv[]) {
  if (argc != 2 && argc != 3) {
    std::cout << "ERROR: wrong input number of operands" << std::endl;
    return -1;
  }

  std::string filepath = "";
  bool dualMode = false;

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--single")) {
      dualMode = false;
    } else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--dual")) {
      dualMode = true;
    } else if (argv[i][0] != '-' && filepath.empty()) {
      filepath.append(argv[i]);
    }
    else {
      std::cout << "Invalid Argument: " << argv[i] << std::endl;
      return -1;
    }
  }

  if (filepath.empty()){
    std::cout << "ERROR: no File given" << std::endl;
    return -1;
  }

  LOADER loader(filepath);
  loader.showCode();
  std::vector<INSTR> instr = loader.getInstr();

  dualMode ? runTwoCICs(instr) : runOneCIC(instr);
  return 0;
}
