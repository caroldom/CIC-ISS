
/*
Instruction consisting of an address, an opcode and a mnemonic and 
optionally a second opcode and a parameter. 
*/
struct INSTR {
    // components of an instruction
    int addr = -1;
    int opcode = -1;
    int opcode2 = -1;
    std::string mnemonic = "";
    int parameter = -1;

    bool operator ==(const INSTR& x) {
        return true;
    }

    /*
    Fills components according to @line. Throws error if they
    are invalid.
    */
    INSTR(std::string line) {
        if (line != "xxx: second cycle of tl or tml") {
            // getting values out of string
            std::vector<std::string> parts;
            size_t pos = 0;
            size_t newPos;
            while (newPos != std::string::npos) {
                newPos = line.find_first_of(": ", pos);
                std::string part = line.substr(pos, newPos-pos);
                // keep part unless empty or TAB
                if(part != "" && part[0] != 9) 
                    parts.push_back(part);
                if(newPos != std::string::npos)
                    pos = newPos+1;
            }
            
            // filling values into members
            if (parts.size() > 0) {
                int input;
                
                input = std::stoul(parts[0], nullptr, 16);
                if (input < 0) {
                    throw std::invalid_argument("error: negative address in operation");
                }
                addr = input; 

                int longInstr = 0;
                input = std::stoul(parts[1], nullptr, 16);
                if (input < 0) {
                    throw std::invalid_argument("error: negative opcode in operation");
                }
                opcode = input;
                if (parts.size() == 5) {
                    input = std::stoul(parts[2], nullptr, 16);
                    if (input < 0) {
                        throw std::invalid_argument("error: negative opcode in operation");
                    }
                    opcode2 = input;
                    longInstr = 1;
                }

                mnemonic = parts[2+longInstr];
                //erase tab at end of mnemonic
                while (int(mnemonic.back()) == 9) {
                    mnemonic.pop_back();
                }

                if (parts.size() >= 4) {
                    input = std::stoul(parts[3+longInstr], nullptr, 16);
                    if (input < 0) {
                        throw std::invalid_argument("error: negative parameter in operation");
                    }
                    parameter = input;
                }
            }
        } else { //wait in first of two cycles of tl or tml
            opcode = -2;
            mnemonic = "tml";
        }
    }

    /*
    Sets component parameter to @p.
    */
    void setParameter (int p) {
        parameter = p;
    }

    /*
    Sets component address to @a.
    */
    void setAddress (int a) {
        addr = a;
    }

    /*
    Prints entire instruction.
    */
    void show() {
        std::cout << addr << " " << opcode << " " << opcode2 << " " << mnemonic << " " << parameter << std::endl;
    }
};