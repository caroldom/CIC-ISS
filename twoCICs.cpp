
/*
Runs @instructions on two instances of the CIC-ISS. 
*/
void runTwoCICs (std::vector<INSTR> instructions) {
    // for both instances
    sc_clock cicClock ("cicClock", 1, SC_US);
    sc_signal<bool> sigDataCartToCons;
    sc_signal<bool> sigDataConsToCart;
    sc_signal<bool> sigConsResetsCart;
    // console only
    sc_signal<bool> sigSeedCons;
    sc_signal<bool> sigLockKeyCons;
    sc_signal<bool> sigHostResetCons;
    sc_signal<bool> sigResetCons;
    // cartridge only
    sc_signal<bool> sigSeedCart;
    sc_signal<bool> sigLockKeyCart;
    sc_signal<bool> sigHostResetCart;
    sc_signal<bool> sigSendResetCart;

    // generate seed (random number 0 <= seed <= 15)
    int seed;
    srand(time(NULL));
    seed =  rand() % 16;
    // set seed manually:
    // seed = 15;

    // file for state of memory per instruction
    FILE *memoryState;
    memoryState = fopen("memoryState.txt", "w");

    // Console (Lock)
    CIC cicConsole ("CONSOLE-CIC", instructions, "CONSOLE-CIC");
    // incoming signals
    cicConsole.clock(cicClock);
    cicConsole.dataIn(sigDataCartToCons);
    cicConsole.seed(sigSeedCons);
    cicConsole.lockKey(sigLockKeyCons);
    cicConsole.reset(sigResetCons);
    // outgoing signals
    cicConsole.dataOut(sigDataConsToCart);
    cicConsole.hostReset(sigHostResetCons);
    cicConsole.sendReset(sigConsResetsCart);

    // Cartridge (Key)
    CIC cicCartridge ("CARTRIDGE-CIC", instructions, "CARTRIDGE-CIC");
    // incoming signals
    cicCartridge.clock(cicClock);
    cicCartridge.dataIn(sigDataConsToCart);
    cicCartridge.seed(sigSeedCart);
    cicCartridge.lockKey(sigLockKeyCart);
    cicCartridge.reset(sigConsResetsCart);
    // outgoing signals
    cicCartridge.dataOut(sigDataCartToCons);
    cicCartridge.hostReset(sigHostResetCart);
    cicCartridge.sendReset(sigSendResetCart);


    // vcd file for wavetraces
    sc_trace_file *signals = sc_create_vcd_trace_file("cic_signals");
    sc_trace(signals, cicClock, "Clock");
    sc_trace(signals, sigDataCartToCons, "Cartridge-DataOut->Console-DataIn");
    sc_trace(signals, sigDataConsToCart, "Console-DataOut->Cartridge-DataIn");
    sc_trace(signals, sigConsResetsCart, "ConsoleResetsCartridge");

    sc_trace(signals, sigSeedCons, "Console-Seed");
    sc_trace(signals, sigLockKeyCons, "Console-LockKey");
    sc_trace(signals, sigHostResetCons, "Console-HostReset");
    sc_trace(signals, sigResetCons, "Console-Reset");

    sc_trace(signals, sigSeedCart, "Cartridge-Seed");
    sc_trace(signals, sigLockKeyCart, "Cartridge-LockKey");
    sc_trace(signals, sigHostResetCart, "Cartridge-HostReset");
    sc_trace(signals, sigSendResetCart, "Cartridge-SendReset");

    // set necessary signals
    sigLockKeyCons.write(1);
    sigLockKeyCart.write(0);
    sigSeedCons.write(1);

    // set seed-timer (number of instructions Console-Seed-Pin is on)
    // 9: number of instructions in beginning, 8: instructions of one seed-loop
    int seedTime = 9+(seed*8);

    // how long iss runs (maximum) = numberOfInstr
    int numberOfInstr = 20000;
    int counter = 0;
    
    // run code until maximum number of instructions
    while (cicConsole.memory.getInstr(cicConsole.registers.lookup.getDecPC(cicConsole.registers.vPC)).opcode != -1 && counter<numberOfInstr) {
        //reset seed after n (seedTime) instructions
        if (seedTime >= 0) {
            if (seedTime == 0) {
                sigSeedCons.write(0);
            }
            seedTime--;
        }
        counter++;

        // write PCs into file
        fprintf(memoryState, "%s: 0x%03x  ", cicConsole.name.c_str(), cicConsole.registers.vPC);
        fprintf(memoryState, "0d%03d     ", cicConsole.registers.lookup.getDecPC(cicConsole.registers.vPC));
        fprintf(memoryState, "%s: 0x%03x  ", cicCartridge.name.c_str(), cicCartridge.registers.vPC);
        fprintf(memoryState, "0d%03d\n", cicCartridge.registers.lookup.getDecPC(cicCartridge.registers.vPC));

        // execute instruction
        sc_start(1, SC_US);

        // write memory status into file after instruction execution 
        // state of console-instance
        fprintf(memoryState, "%s:\n", cicConsole.name.c_str());
        cicConsole.registers.show(*memoryState);
        cicConsole.memory.showPCStackTop(*memoryState);
        cicConsole.memory.showRAM(*memoryState);
        fprintf(memoryState, "\n");
        // state of cartridge-instance
        fprintf(memoryState, "%s:\n", cicCartridge.name.c_str());
        cicCartridge.registers.show(*memoryState);
        cicCartridge.memory.showPCStackTop(*memoryState);
        cicCartridge.memory.showRAM(*memoryState);
        fprintf(memoryState, "\n");

        fprintf(memoryState, "\n");
    }

    fclose(memoryState);
    
    sc_close_vcd_trace_file(signals);
}
