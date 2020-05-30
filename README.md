# CIC-ISS

The CIC-ISS is an instruction set simulator for the CIC-lockout-chip in the NES console.  
You can run code written with instructions of the cic instruction set as explained [here](https://hackmii.com/2010/01/the-weird-and-wonderful-cic/) by Segher on it.
It can be executed on one or two CICs. With two CICs one acts as the CIC in the console and the other one as the one in the game.  
The iss creates a vcd-file containing the pin-signals of the CIC. It furthermore prints the state of the RAM, registers and PC-Stack into a file. This happens once at the end when executed on one CIC (into "cicState_CIC.txt") or after every instruction when exectued on two CICs (into "memoryState.txt").  
Since the real CIC runs eternally the runtime of the iss has been limited to 20.000 instructions. This can be changed in line 87 in `twoCICs.cpp`.

The CIC-ISS was written as part of the project SystemXrunner carried out in the computer science bachelors degree of the University of Bremen (http://www.informatik.uni-bremen.de/projekttag/2020/).  

## Build an exectue

- install systemC on path "/usr/local/systemc-2.3.3/" (or change path in makefile to your installation path)
- `make`
- `./cic_iss [options] filename`
    - -s --single: execute CIC-ISS with one cic-instance (default mode)
    - -d --dual: execute CIC-ISS with two cic-instances
    - filename: path to textfile that contains the code for the iss

## Input code examples
There are a few examples of input files in the folder "input_code".  
**"cicDiasassembly.txt"** is the disassembled cic code by René Richard which can be found [here](https://github.com/db-electronics/CIC16F/tree/master/cic%20disassembly). It can be run on two CICs.  
A version of this Disassembly that has been modified to resemble the CIC on an european NES is **"cicModified.txt"**. The edited parts are marked.  
**"test.txt"** is a simple example for one CIC and **"testReset.txt"** one for two CICs.