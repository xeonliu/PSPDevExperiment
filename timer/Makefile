TARGET = main
OBJS= main.o cursor.o app.o 

CFLAGS = -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti # RTTI (run-time type information or run-time type identification)

LIBDIR  =
LIBS = -lstdc++ -lSDL3 -lGL -lGLU -lglut -lz \
         -lpspvfpu -lpsphprm -lpspsdk -lpspctrl -lpspumd -lpsprtc -lpsppower -lpspgum -lpspgu -lpspaudiolib -lpspaudio -lpsphttp -lpspssl -lpspwlan \
         -lpspnet_adhocmatching -lpspnet_adhoc -lpspnet_adhocctl -lm -lpspvram

BUILD_PRX = 1
EXTRA_TARGETS   = EBOOT.PBP
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak