enum Opcode {
        adi,         // add immediate, skip if Akkumulator-Register overflow
        nop,         // no operation
        skai,        // skip Akkumulator-Register if A = N
        lbli,        // load B low immediate (reg BL)
        ldi,         // load immediate

        l,           // load
        x,           // exchange 
        xi,          // exchange and increment, increment BL skip overflow
        xd,          // exchange and decrement, decrement BL skip underflow
        nega,        // negate A
        out,         // ouput PN = A
        out0,        // output 0
        sc,          // set carry C = 1
        rc,          // reset carry C = 0
        s,           // store A
        rit,         // return, pop PC from Stack
        ritsk,       // return and skip, pop PC from Stack
        li,          // load and increment, increment BL skip overflow
        coma,        // complemnt A (A= ~A)
        in,          // input
        xal,         // exchange A with BL
        lxa,         // load X with A (X=A)
        xax,         // exchange X with A
        mystery,

        skm,         // skip memory (M.N = 1)
        ska,         // skipp A (A.N = 1)
        rm,          // reset memory (M.N = 0)
        sm,          // set memory (M.N = 1)

        ad,          // add
        adc,         // add with carry
        adcsk,       // add with carry and skip if overflow

        lbmi,        // load B high immediate (BM)

        tl,          // transfer long (PC=...)
        tml,         // transfer module Long, push PC+2, PC = ...
        t,           // transfer low bits of PC
    };