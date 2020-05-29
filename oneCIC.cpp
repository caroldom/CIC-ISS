
/*
Runs @instructions on an instance of the CIC-ISS. 
*/
void runOneCIC (std::vector<INSTR> instructions) {
    // incoming signals
    sc_clock cicClock ("cicClock", 1, SC_US);
    sc_signal<bool> sigDataIn;
    sc_signal<bool> sigSeed;
    sc_signal<bool> sigLockKey;
    sc_signal<bool> sigReset;
    // outgoing signals
    sc_signal<bool> sigDataOut;
    sc_signal<bool> sigHostReset;
    sc_signal<bool> sigSendReset;

    // attach signals to ports
    CIC cic ("CIC", instructions, "CIC");
    // incoming signals
    cic.clock(cicClock);
    cic.dataIn (sigDataIn);
    cic.seed (sigSeed);
    cic.lockKey (sigLockKey);
    cic.reset(sigReset);
    // outgoing signals
    cic.dataOut (sigDataOut);
    cic.hostReset (sigHostReset);
    cic.sendReset (sigSendReset);

    // vcd file for wavetraces
    sc_trace_file *signals = sc_create_vcd_trace_file("cic_signals");
    sc_trace(signals, cicClock, "Clock");
    sc_trace(signals, sigDataIn, "DataIn");
    sc_trace(signals, sigSeed, "Seed");
    sc_trace(signals, sigLockKey, "Lock/Key");
    sc_trace(signals, sigReset, "Reset");
    sc_trace(signals, sigDataOut, "DataOut");
    sc_trace(signals, sigHostReset, "HostReset");
    sc_trace(signals, sigSendReset, "SendReset");

    // run code while theres instructions left
    while (cic.memory.getInstr(cic.registers.lookup.getDecPC(cic.registers.vPC)).opcode != -1) {
        sc_start(1, SC_US);
    }
        
    // prints state of memory into file
    cic.show();
    
    sc_close_vcd_trace_file(signals);
}