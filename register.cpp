
#include "pcLookup.cpp"

/*
Contains the registers and functions to manipulate them.
*/
struct REGISTERS {
    // Accumulator (4B), General Purpose (4B)
    unsigned int vA: 4, vX: 4;
    // Pointer(low) (4B), Pointer(high) (2B)
    unsigned int vBL: 4, vBH: 2;
    // Program Counter (10B), Carry flag (1B)
    unsigned int vPC: 10, vC: 1;

    // lookup -table for polynomial counter
    PCLOOKUP lookup;

    enum registers {
        A = 0,
        X,
        BL,
        BH,
        PC,
        C
    };


    /*
    Initializes registers with 0.
    */
    REGISTERS() : vA(0), vX(0), vBL(0), vBH(0), vPC(0), vC(0), lookup{PCLOOKUP()} {}

    /*
    Writes @value in register @pReg. Returns true, if over-/underflow when writing in register.
    */
    bool write(registers reg, int value){
        bool overUnderFlow = false;
        switch (reg) {
            case A:
                if (value<0 || value>15) {
                    overUnderFlow = true;
                }
                vA = value;
                break;
            case X:
                if (value<0 || value>15) {
                    overUnderFlow = true;
                }
                vX = value;
                break;
            case BL:
                if (value<0 || value>15) {
                    overUnderFlow = true;
                }
                vBL = value;
                break;
            case BH:
                if (value<0 || value>3) {
                    overUnderFlow = true;
                }
                vBH = value;
                break;
            case PC:
                vPC = value;
                break;
            case C:
                vC = value;
                break;
        }
        return overUnderFlow;
    }

    /*
    Returns value of register @reg. Returns 0 if there is no such register.
    */
    unsigned int read(registers reg){
        unsigned int r = 0;
        switch (reg) {
            case A:
                r = vA;
                break;
            case X:
                r = vX;
                break;
            case BL:
                r = vBL;
                break;
            case BH:
                r = vBH;
                break;
            case PC:
                r = vPC;
                break;
            case C:
                r = vC;
                break;
        }
        return r;
    }

    /*
    Increments PC polynomially in lower 7 Bits and normally in higher two Bits.
    */
    void increasePC () {
        unsigned int val;

        if ((vPC&0b1111111) == 1) { //x1
            vPC |= 0b1111111;
        } else if ((vPC&0b1111111) == 0b1111111) { //x7f
            if (vPC == 0b0001111111) {
                vPC = 0b0100000000;
            } else if (vPC == 0b0101111111) {
                vPC = 0b1000000000;
            } else if (vPC == 0b1001111111) {
                vPC = 0b1100000000;
            }
        } else { // polynomial increasing
            unsigned int upperBits = vPC & 0b1100000000;
            unsigned int lowerBits = vPC & 0b0001111111;
            lowerBits = lowerBits >> 1;
            int a = vPC & 0b0000000010;
            a = a >> 1;
            int b = vPC & 0b0000000001;

            if (a == b) {
                val = lowerBits | 0b0001000000;
            } else {
                val = lowerBits & 0b1110111111;
            }
            val |= upperBits;
        }

        write (PC, val);
    }

    /*
    Reset all registers to value 0.
    */
    void reset() {
        write(registers::A, 0);
        write(registers::X, 0);
        write(registers::BL, 0);
        write(registers::BH, 0);
        write(registers::PC, 0);
        write(registers::C, 0);
    }

    /*
    Prints current state of registers into @file.
    */
    void show(FILE& file){
        fprintf(&file, " registers:   ");

        fprintf(&file, "%-5s%-5s%-5s%-5s%-5s%-5s\n", "Acc", "X", "BH", "BL", "PC", "C");
        fprintf(&file, "              ");
        fprintf(&file, "%-5d%-5d%-5d%-5d%-5d%-5d\n", vA, vX, vBH, vBL, lookup.getDecPC(vPC), vC);
    }
};