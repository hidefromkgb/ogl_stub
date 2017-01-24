CC = clang -fvisibility=hidden
CX = $(CC)

LIB = -framework OpenGL -framework AppKit
CFLAGS = -fno-stack-protector -mstackrealign -Wall
CXFLAGS =

OBJDIR = .obj
OBJ = $(OBJDIR)/core.o $(OBJDIR)/main.o

EXEBASE = ../stub.app
EXEPATH = $(EXEBASE)/Contents/MacOS
EXE = $(EXEPATH)/main


Release: CFLAGS += -O2
Release: build $(OBJ) $(EXE)
Release:
	if type strip &>/dev/null && [ -f $(EXE) ]; then strip -x $(EXE); fi

Debug: CFLAGS += -g
Debug: build $(OBJ) $(EXE)

cleanRelease: clean
cleanDebug: clean


clean:
	rm -rf $(OBJDIR) $(EXEBASE)

build:
	mkdir -p $(OBJDIR) $(EXEPATH) $(EXEPATH)/../Resources
	cp rsrc/*.icns $(EXEPATH)/../Resources
	cp rsrc/Info.plist $(EXEPATH)/..
$(OBJDIR)/core.o: ../core/core.c
	$(CC) $(CFLAGS) -c ../core/core.c -o $(OBJDIR)/core.o
$(OBJDIR)/main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o $(OBJDIR)/main.o
	$(CX) $(CXFLAGS) $(OBJ) $(LIB) -o $(EXE)
