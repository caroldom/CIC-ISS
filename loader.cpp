
#include "instruction.cpp"

/*
Loads file with code, which can be turned into instructions as
preparation for being used by cic-iss.
*/
struct LOADER {

    std::vector<std::string> codeLines;

    LOADER (std::string filename) {
        std::string filepath = "";
        filepath.append(filename);
        std::ifstream code;
        code.open (filepath);
        if (code.is_open()) {
            std::string line;
            while (getline (code, line)) {
                if (line != "") {
                    codeLines.push_back(line);

                    //add empty line for tml, tl (need two cycles)
                    if (line.find("tl") != std::string::npos || line.find("tml") != std::string::npos) {
                        codeLines.push_back("xxx: second cycle of tl or tml");
                    }
                }
            }
        } else {
            throw std::invalid_argument("invalid filename");
        }
        code.close();
    }

    /*
    Turns each line from input file into an instruction and
    removes comments. Returns vector of instructions.
    */
    std::vector<INSTR> getInstr() {
        rmComments();
        std::vector<INSTR> instr;
        for (std::string& s: codeLines) {
            INSTR oneInstr(s);
            if (oneInstr.mnemonic != "")
                instr.push_back(oneInstr);
        }
        return instr;
    }

    /*
    Removes comments beginning with ; / ' or " in lines
    from input file.
    */
    void rmComments (){
        for (std::string& s: codeLines) {
            size_t pos=s.find_first_of(";/'");
            if (pos != std::string::npos) {
                s.erase(pos, std::string::npos);
            }
        }
    }

    /*
    Prints every line from input file.
    */
    void showCode () {
        std::cout << "#########" << std::endl << "CODE:" << std::endl;
        for (std::string& s: codeLines) {
            std::cout << s << std::endl;
        }
        std::cout << "#########" << std::endl;
    }
    
};
