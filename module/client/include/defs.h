#pragma once

#define PSPDEBUG
#define printf pspDebugScreenPrintf

#include <pspkernel.h>

#define MIPS_J_ADDRESS(x) (((u32)((x)) & 0x3fffffff) >> 2)

#define NOP 0x000C0000
#define JAL_TO(x) (0x0E000000 | MIPS_J_ADDRESS(x))
#define J_TO(x) (0x08000000 | MIPS_J_ADDRESS(x))
#define LUI(x, y) (0x3C000000 | ((x & 0x1f) << 0x10) | (y & 0xffff))

#if defined(PSPDEBUG)
#define dbg_log pspDebugScreenPrintf
#else
#define dbg_log(...)
#endif

typedef u32 PspNID;
typedef u32 *MemAddress;
typedef u32 MIPSInstruction;