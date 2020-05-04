SRCDIR 	:= src
INCDIR 	:= src
OBJDIR 	:= obj
BINDIR 	:= bin

DEFINES	:= 	-DNON_MATLAB_PARSING \
			-DMAX_EXT_API_CONNECTIONS=255 \
			-DDO_NOT_USE_SHARED_MEMORY

ifeq ($(OS),Windows_NT)
    MKDIR   := md
	RMDIR   := rd /S /Q	
	TARGET	:= vrep.exe
	COPY	:= copy /Y	$(BINDIR)\$(TARGET)
	LFLAGS	:= -lws2_32 -lwinmm
	VREPPATH:= "C:\Program Files\CoppeliaRobotics\CoppeliaSimEdu"
else
    MKDIR   := mkdir
	RMDIR   := rm -rf
	TARGET	:= vrep
	COPY	:= cp $(BINDIR)/$(TARGET)
	LFLAGS 	:= -lpthread
	VREPPATH:= ~/CoppeliaSim_Edu_V4_0_0_Ubuntu16_04/	
endif

CC 		:= gcc
CFLAGS 	:= -I $(INCDIR) $(DEFINES) -pedantic

SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS	:= $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))
EXE     := $(BINDIR)/$(TARGET)

.PHONY: all run clean

all: $(EXE)
	$(COPY) $(VREPPATH)

$(EXE): $(OBJECTS) | $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR) $(OBJDIR):
	$(MKDIR) $@

clean:
	$(RMDIR) $(OBJDIR) $(BINDIR)