
#include <sstream>

#include "register.cpp"
#include "memory.cpp"
#include "opcode.h"

/*
One instance of a CIC with RAM, registers, PC-Stack and instruction-set according
to architecture of CIC.
*/
SC_MODULE (CIC) {
    
    bool skipFlag = false;
    // variables for temporary values
    int vA;
    unsigned int vM;
    int vBL;
    int vBH;
    int vX;
    int vPC;

    MEMORY memory;
    REGISTERS registers;
    std::string name;

    // input ports
    sc_in<bool> clock;      // Pin 6
    sc_in<bool> dataIn;     // P0.1
    sc_in<bool> seed;       // P0.2
    sc_in<bool> lockKey;    // P0.3
    sc_in<bool> reset;      // Pin 7

    // output ports
    sc_out<bool> dataOut;       // P0.0
    sc_out<bool> hostReset;     // P1.0
    sc_out<bool> sendReset;     // P1.1


    /*
    Initializes memory and defines that funktion "run_step"
    is executed on every rising edge of clock.
    */
    CIC(sc_module_name name, 
           std::vector<INSTR> pInstr, std::string n)
        : sc_module(name), memory{MEMORY (pInstr)}, name(n) {

        SC_METHOD(run_step);
        /* dont execute function automatically at beginning
        so first opcode isn't missing in wavetrace-file  */
        dont_initialize();
        sensitive << clock.pos();
    }
    SC_HAS_PROCESS(CIC);

    /*
    Returns pointer-register B construced out of registers
    BLow and BHigh.
    */
    int constructB () {
        vBL = registers.read(registers.BL);
        vBH = registers.read(registers.BH);
        return (vBH << 4) | (vBL);
    };

    /*
    Prints current state of memory into a file.
    */
    void show() {
        FILE *cicState;

        //create filename
        std::stringstream ss;
        ss << "cicState_" << name.c_str() << ".txt";
        std::string filename = ss.str();

        cicState = fopen(filename.c_str(), "w");
        fprintf(cicState, "Status of %s:\n", name.c_str());

        registers.show(*cicState);
        fprintf(cicState, "\n");
        memory.showPCStackTop(*cicState);
        fprintf(cicState, "\n");
        memory.showRAM(*cicState);

        fclose(cicState);
    }
    
    /*
    Resets memory, PCStack and registers of the ISS. Keeps code.
    */
    void resetAll() {
        skipFlag = false;
        memory.reset();
        registers.reset();
    }

    /* 
    Returns instance of Opcode that corresponds to @mnemonic in instruction.
    Throws error when @mnemonic is invalid.
    */
    Opcode stoopc(std::string mnemonic) {
        if (mnemonic == "adi"){
            return Opcode::adi;
        } else if (mnemonic == "nop") {
            return Opcode::nop;  
        } else if (mnemonic == "skai") {
            return Opcode::skai;
        } else if (mnemonic == "lbli") {
            return Opcode::lbli;
        } else if (mnemonic == "ldi") {
            return Opcode::ldi;
        } else if (mnemonic == "l") {
            return Opcode::l;
        } else if (mnemonic == "x") {
            return Opcode::x;
        } else if (mnemonic == "xi") {
            return Opcode::xi;
        } else if (mnemonic == "xd") {
            return Opcode::xd;
        } else if (mnemonic == "nega") {
            return Opcode::nega;
        } else if (mnemonic == "out") {
            return Opcode::out;
        } else if (mnemonic == "out0") {
            return Opcode::out0;
        } else if (mnemonic == "sc") {
            return Opcode::sc;
        } else if (mnemonic == "rc") {
            return Opcode::rc;
        } else if (mnemonic == "s") {
            return Opcode::s;
        } else if (mnemonic == "rit") {
            return Opcode::rit;
        } else if (mnemonic == "ritsk") {
            return Opcode::ritsk;
        } else if (mnemonic == "li") {
            return Opcode::li;
        } else if (mnemonic == "coma") {
            return Opcode::coma;
        } else if (mnemonic == "in") {
            return Opcode::in;
        } else if (mnemonic == "xal") {
            return Opcode::xal;
        } else if (mnemonic == "lxa") {
            return Opcode::lxa;
        } else if (mnemonic == "xax") {
            return Opcode::xax;
        } else if (mnemonic == "???") { // mystery instruction
            return Opcode::mystery;
        } else if (mnemonic == "skm") {
            return Opcode::skm;
        } else if (mnemonic == "ska") {
            return Opcode::ska;
        } else if (mnemonic == "rm") {
            return Opcode::rm;
        } else if (mnemonic == "sm") {
            return Opcode::sm;
        } else if (mnemonic == "ad") {
            return Opcode::ad;
        } else if (mnemonic == "adc") {
            return Opcode::adc;
        } else if (mnemonic == "adcsk") {
            return Opcode::adcsk;
        } else if (mnemonic == "lbmi") {
            return Opcode::lbmi;
        } else if (mnemonic == "tl") {
            return Opcode::tl;
        } else if (mnemonic == "tml") {
            return Opcode::tml;
        } else if (mnemonic == "t") {
            return Opcode::t;
        } else {
            throw std::invalid_argument("invalid mnemonic");
        }
    }
    
    /* 
    Executes next instruction and increases program counter.
    Throws error if the instruction's address and the current program
    counter don't match.
    */
    void run_step(){
        // reset if reset-pin is on
        if (reset.read() == 1) {
            resetAll();
            return;
        }


        INSTR instruction = memory.getInstr(registers.lookup.getDecPC(registers.vPC));

        // check if address and pc match
        if (instruction.addr != registers.vPC) {
            throw std::invalid_argument("invalid address in file");
        }

        registers.increasePC();
        if(skipFlag == true){
            skipFlag = false; 
        }
        else{
            switch (stoopc(instruction.mnemonic)) {
                case Opcode::adi:
                    vA = registers.read(registers.A)+ instruction.parameter;
                    if(registers.write(registers.A, vA)){
                        skipFlag = true;
                    };                                        
                    break;
                case Opcode::nop:
                    break;
                case Opcode::skai:
                    if(registers.read(registers.A) == (unsigned int)instruction.parameter){
                        skipFlag = true;
                    };
                    break;
                case Opcode::lbli:
                    registers.write(registers.BL, instruction.parameter);
                    break;
                case Opcode::ldi:
                    registers.write(registers.A, instruction.parameter);
                    break;
                case Opcode::l:
                    vM = memory.readRAM(constructB());
                    registers.write(registers.A, vM);
                    break;
                case Opcode::x:
                    vA = registers.read(registers.A);
                    vM = memory.readRAM(constructB());
                    registers.write(registers.A, vM);
                    memory.writeRAM(constructB(), vA);
                    break;
                case Opcode::xi:
                    vA = registers.read(registers.A);
                    vM = memory.readRAM(constructB());
                    registers.write(registers.A, vM);
                    memory.writeRAM(constructB(), vA);
                    vBL = registers.read(registers.BL) + 1;
                    if(registers.write(registers.BL, vBL)){
                        skipFlag = true;
                    };
                    break;
                case Opcode::xd:
                    vA = registers.read(registers.A);
                    vM = memory.readRAM(constructB());
                    registers.write(registers.A, vM);
                    memory.writeRAM(constructB(), vA);
                    vBL = registers.read(registers.BL) - 1;
                    if(registers.write(registers.BL, vBL)){
                        skipFlag = true;
                    };
                    break;
                case Opcode::nega:
                    vA = ~ registers.read(registers.A);
                    vA++;
                    registers.write(registers.A, (vA & 0b1111));
                    break;
                case Opcode::out:
                    if (registers.read(registers.BL) == 0) {
                        dataOut.write(registers.read(registers.A) & 0b1); //P0.1
                    } else if (registers.read(registers.BL) == 1) {
                        hostReset.write(registers.read(registers.A) & 0b1); //P1.0
                        sendReset.write((registers.read(registers.A) & 0b10) >> 1); //P1.1
                    }
                    break;
                case Opcode::out0:
                    if (registers.read(registers.BL) == 0) {
                        dataOut.write(0); //P0.1
                    } else if (registers.read(registers.BL) == 1) {
                        hostReset.write(0); //P1.0
                        sendReset.write(0); //P1.1
                    }
                    break;
                case Opcode::sc:
                    registers.write(registers.C, 1);
                    break;
                case Opcode::rc:
                    registers.write(registers.C, 0);
                    break;
                case Opcode::s:
                    memory.writeRAM(constructB(), (registers.read(registers.A)));
                    break;
                case Opcode::rit:
                    vPC = memory.popPCStack();
                    registers.write(registers.PC, vPC);
                    break;
                case Opcode::ritsk:
                    vPC = memory.popPCStack();
                    registers.write(registers.PC, vPC);
                    skipFlag = true;
                    break;
                case Opcode::li:
                    vM = memory.readRAM(constructB());
                    registers.write(registers.A, vM);
                    vBL = registers.read(registers.BL) + 1;
                    if(registers.write(registers.BL, vBL)){
                        skipFlag = true;
                    };
                    break;
                case Opcode::coma:
                    vA = ~ registers.read(registers.A);
                    registers.write(registers.A, (vA & 0b1111));
                    break;
                case Opcode::in:
                    if (registers.read(registers.BL) == 0) {
                        int val = (dataIn.read() << 1);
                        val |= (seed.read() << 2);
                        val |= (lockKey.read() << 3);
                        registers.write(registers.A, val);
                    }
                    break;
                case Opcode::xal:
                    vA = registers.read(registers.A);
                    vBL = registers.read(registers.BL);
                    registers.write(registers.A, vBL);
                    registers.write(registers.BL, vA);
                    break;
                case Opcode::lxa:
                    registers.write(registers.X, registers.read(registers.A));
                    break;
                case Opcode::xax:
                    vA = registers.read(registers.A);
                    vX = registers.read(registers.X);
                    registers.write(registers.A, vX);
                    registers.write(registers.X, vA);
                    break;
                case Opcode::mystery:
                    std::cout<< "Jugend forscht 2020" << std::endl;
                    break;
                case Opcode::skm:
                    vM = memory.readRAM(constructB());
                    if((vM&(0b0001<<instruction.parameter))>>instruction.parameter == 1)
                        skipFlag = true;
                    break;
                case Opcode::ska:
                    vA = registers.read(registers.A);
                    if((vA&(0b0001<<instruction.parameter))>>instruction.parameter == 1)
                        skipFlag = true;
                    break;
                case Opcode::rm:
                    vM = memory.readRAM(constructB());
                    vM &= ~(1<<instruction.parameter);
                    memory.writeRAM(constructB(), vM);
                    break;
                case Opcode::sm:
                    vM = memory.readRAM(constructB());
                    vM |= (1<<instruction.parameter);
                    memory.writeRAM(constructB(), vM);
                    break;
                case Opcode::ad:
                    vM = memory.readRAM(constructB());
                    vA = registers.read(registers.A) + vM;
                    registers.write(registers.A, vA);
                    break;
                case Opcode::adc:
                    vM = memory.readRAM(constructB());
                    vA = registers.read(registers.A) + registers.read(registers.C) + vM;
                    registers.write(registers.A, vA);
                    break;
                case Opcode::adcsk:
                    vM = memory.readRAM(constructB());
                    vA = registers.read(registers.A) + registers.read(registers.C) + vM;
                    if(registers.write(registers.A, vA)) {
                        skipFlag = true;
                    };
                    break;
                case Opcode::lbmi:
                    registers.write(registers.BH, instruction.parameter);
                    break;
                case Opcode::tl: //needs two cycles
                    if (memory.getInstr(registers.lookup.getDecPC(registers.vPC)).opcode == -2) { // next instr second cycle, this one first
                        // save jump adress into next cycle, set address of next instr
                        memory.getInstr(registers.lookup.getDecPC(registers.vPC)).setParameter(instruction.parameter);
                        memory.getInstr(registers.lookup.getDecPC(registers.vPC)).setAddress(registers.vPC);
                    } else { //this instr second cycle
                        registers.write(registers.PC, instruction.parameter);
                    }
                    break;
                case Opcode::tml: //needs two cycles
                    if (memory.getInstr(registers.lookup.getDecPC(registers.vPC)).opcode == -2) { // next instr second cycle, this one first
                        // save jump adress into next cycle, set address of next instr
                        memory.getInstr(registers.lookup.getDecPC(registers.vPC)).setParameter(instruction.parameter);
                        memory.getInstr(registers.lookup.getDecPC(registers.vPC)).setAddress(registers.vPC);

                        // push next PC on stack
                        vPC = registers.vPC;
                        vPC = registers.lookup.getDecPC(vPC) + 1;
                        memory.pushPCStack(registers.lookup.getPolyPC(vPC));
                    } else { //this instr second cycle
                        registers.write(registers.PC, instruction.parameter);
                    }
                    break;
                case Opcode::t:
                    vPC = registers.vPC & 0b1100000000; // sets lower bits -> keep upper bits
                    registers.write(registers.PC, (instruction.parameter & 0b11111111)+vPC);
                    break;
            }
        }
    }

    
};