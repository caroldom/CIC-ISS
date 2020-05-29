
/*
Get the counterpart of polynomial/decimal counter.
*/
struct PCLOOKUP {

    const int LOOKUP_SIZE = (4*127)+2;
    std::vector<int> lookupTable;

    /*
    Initialize lookup-table.
    */
    PCLOOKUP () {
        unsigned int start = 0;
        lookupTable.push_back(0);

        for (int i=1; i<LOOKUP_SIZE; ++i) {
            
            if ((start&0b1111111) == 1) { //x1
                start |= 0b1111111;
            } else if ((start&0b1111111) == 0b1111111) { //x7f
                if (start == 0b0001111111) {
                    start = 0b0100000000;
                } else if (start == 0b0101111111) {
                    start = 0b1000000000;
                } else if (start == 0b1001111111) {
                    start = 0b1100000000;
                }
            } else { // polynomial increasing
                unsigned int upperBits = start & 0b1100000000;
                unsigned int lowerBits = start & 0b0001111111;
                lowerBits = lowerBits >> 1;
                int a = start & 0b0000000010;
                a = a >> 1;
                int b = start & 0b0000000001;

                unsigned int val;
                if (a == b) {
                    val = lowerBits | 0b0001000000;
                } else {
                    val = lowerBits & 0b1110111111;
                }
                val |= upperBits;
                start = val;
            }
            lookupTable.push_back(start);

        }
    }

    /*
    Prints lookup-table.
    */
    void printLookup() {
        for (int i=0; i<LOOKUP_SIZE; ++i) {
            std::cout << lookupTable[i] << std::endl;
        }
    }

    /*
    Get the polynomial counterpart to a binary counter.
    */
    int getDecPC(int polyPC) {
        std::vector<int>::iterator it = std::find(lookupTable.begin(), lookupTable.end(), polyPC);
        int decPC = -1;
        if (it != lookupTable.end()) {
            decPC = std::distance(lookupTable.begin(), it);
        }
        return decPC;
    }

    /*
    Get the binary counterpart to a polynomial counter.
    */
    int getPolyPC(int decPC) {
        return lookupTable[decPC];
    }

};