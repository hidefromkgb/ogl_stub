CC = gcc
CX = gcc
RC = windres

CFLAGS = -mno-stack-arg-probe -Dstrdup=_strdup -Dvsnprintf=_vsnprintf -Wall
CXFLAGS = -lopengl32 -lcomctl32 -lkernel32 -lshell32 -luser32 -lgdi32
CXFLAGS += -lmsvcrt -lm -nostdlib -e_WinMain@16 -Wl,--subsystem,windows
RCFLAGS = -J rc -O coff

OBJDIR = .obj
OBJ = $(OBJDIR)\rsrc.o $(OBJDIR)\core.o $(OBJDIR)\main.o

EXE = ..\stub.exe


Release: CFLAGS += -O2
Release: CXFLAGS += -s
Release: build $(OBJ)

Debug: CFLAGS += -g
Debug: build $(OBJ)

cleanRelease: clean
cleanDebug: clean


clean:
	if exist $(EXE) del /F /Q $(EXE)
	if exist $(OBJDIR) rmdir /S /Q $(OBJDIR)

build:
	if not exist $(OBJDIR) mkdir $(OBJDIR)
$(OBJDIR)\rsrc.o: rsrc\rsrc.rc
	$(RC) $(RCFLAGS) -i rsrc\rsrc.rc -o $(OBJDIR)\rsrc.o
$(OBJDIR)\core.o: ..\core\core.c
	$(CC) $(CFLAGS) -c ..\core\core.c -o $(OBJDIR)\core.o
$(OBJDIR)\main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o $(OBJDIR)\main.o
	$(CX) $(OBJ) $(CXFLAGS) -o $(EXE)
