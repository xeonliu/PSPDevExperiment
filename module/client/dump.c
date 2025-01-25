#include "include/dump.h"

void pspDumpMemRegion(MemAddress start, MemAddress end)
{
    dbg_log("Dumping memory region from %x to %x\n", start, end);
    MIPSInstruction I;
    for (MemAddress addr = start; addr < end; addr += 1)
    {
        I = (MIPSInstruction)(*addr);
        printf("instr: 0x%08X: 0x%08X\n", (u32)addr, (u32)I);
    }
}