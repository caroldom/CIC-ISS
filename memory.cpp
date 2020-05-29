
#include <stack>

/*
Contains the RAM and a PC stack and functions to manipulate them
and the instructions of the input file.
*/
struct MEMORY {
    // RAM: 32 Nibbles in 16 8-Bit entries
    // ( entry 1 - entry 0
    //   entry 3 - entry 2
    //   ... )
    uint8_t ram[16];

    // PCStack with 4 possible entries
    std::stack<int> pcStack;

    // instructions from input code
    std::vector<INSTR> instructions;

    // an empty instruction as return value
    INSTR emptyInstr = INSTR ("");


    /*
    Initializes RAM with 0.
    */
    MEMORY (std::vector<INSTR> pInstr): instructions(pInstr) {
        memset(ram, 0, sizeof(ram));
    }

    /*
    Reads out the Instructions at @i in the vector instructions. Return empty instruction when vector is empty.
    */
    INSTR& getInstr(int i){
        if (i < (int)instructions.size()) {
            return instructions[i];
        } else {
            return emptyInstr;
        }
    };

    /*
    Reads out of RAM at @index. Throws error when @index out of bounds.
    */
    unsigned int readRAM (int index) {
        int result = 0;
        if (index < 32 && index >= 0) {
            result = (ram[index >> 1] >> ((index & 1) << 2)) & 0x0F;
        } else {
            throw std::range_error("MEMORY::readRAM: index invalid");
        }
        return result;
    }

    /*
    Writes @val into RAM at @index. Throws error when @val negative or @index invalid.
    */
    void writeRAM (int index, int val) {
        if (index < 0 && index >= 32) {
            throw std::range_error("MEMORY::writeRAM: index invalid");
        }
        if (val >= 0) {
            if ((index & 1) == 0) { // index even
                ram[index >> 1] &= 0b11110000;
            } else { // index odd
                ram[index >> 1] &= 0b00001111;
            }
            ram[index >> 1] |= (val & 0x0F) << ((index & 1) << 2);
        } else {
            throw std::range_error("MEMORY::writeRAM: negative values can't be written");
        }
    }

    /*
    Prints current state of RAM into @file.
    */
    void showRAM (FILE& file) {
        fprintf(&file, " RAM:         ");
        // lower nibbles
        for (int i = 0; i<16; ++i) {
            int lower = unsigned(ram[i]) & 0b1111;
            fprintf(&file, "%-3d", lower);
        }
        fprintf(&file, "\n              ");
        // higher nibbles
        for (int i = 0; i<16; ++i) {
            int higher = (unsigned(ram[i]) & 0b11110000) >> 4;
            fprintf(&file, "%-3d", higher);
        }
    }

    /*
    Pushes @pc to top of PCStack, if stack contains less than four elements
    and @pc not negative. Size of entry is 10 Bits.
    */
    void pushPCStack(int pc) {
        if (pcStack.size() < 4 && pc > 0) {
            pcStack.push(pc & 0b1111111111);
        }
    }

    /*
    Pops top entry of PCStack and returns it. Throws error if pcStack is empty.
    */
    int popPCStack() {
        if (pcStack.empty()) {
            throw std::range_error("MEMORY::popPCStack: stack empty");
        }
        int result = pcStack.top();
        pcStack.pop();
        return result;
    }

    /*
    Prints current state of PCStack into @file.
    */
    void showPCStackTop(FILE& file) {
        if (!pcStack.empty()) {
            fprintf(&file, " PCStack top: 0x%03x  0d%03d", pcStack.top(), pcStack.top());
        } else {
            fprintf(&file, " PCStack top: empty");
        }
        fprintf(&file, "\n");
    }

    /*
    Resets PCStack and RAM.
    */
    void reset() {
        //reset RAM
        for (int i=0; i<32; ++i) {
            writeRAM(i, 0);
        }

        //reset PCStack
        while (pcStack.size() != 0) {
            popPCStack();
        }
    }
};