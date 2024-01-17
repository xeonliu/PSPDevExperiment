# Name of the prx file
TARGET = MainProgram
# Name of the Module source file(.c->.o)
OBJS = main.o ./module/ModuleLib.o
# SOURCE = ./module/
LIBS =   -lm -lz  -lGL -lGLU -lglut -lz \
         -lpspvfpu -lpsphprm -lpspsdk -lpspctrl -lpspumd -lpsprtc -lpsppower -lpspgum -lpspgu -lpspaudiolib -lpspaudio -lpsphttp -lpspssl -lpspwlan \
         -lpspnet_adhocmatching -lpspnet_adhoc -lpspnet_adhocctl -lm -lpspvram

# Define to build this as a prx (instead of a static elf)
BUILD_PRX=1
EXTRA_TARGETS = EBOOT.PBP

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak