TARGET = supervisor
OBJS = supervisor_main.o

CFLAGS = -O2 -G0 -Wall
ASFLAGS = $(CFLAGS)
LDFLAGS = -nostartfiles

BUILD_PRX = 1
PRX_EXPORTS = supervisor_exp.exp

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak