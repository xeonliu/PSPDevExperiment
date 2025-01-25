TARGET = client
OBJS = client.o dump.o exit_callback.o module_exp.o module_man.o
CFLAGS = -O2 -G0 -Wall
ASFLAGS = $(CFLAGS)

INCLUDES = -I. -I./include

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Client

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak