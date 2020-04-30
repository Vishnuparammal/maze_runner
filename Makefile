CC = gcc
CFLAGS = -I $(INCDIR)

SRCDIR = .
INCDIR = .
OBJDIR = .
BINDIR = .

all: $(BINDIR)/bot

$(BINDIR)/bot: $(SRCDIR)/bot.c $(OBJDIR)/maze.o
	$(CC) $^ -o $(BINDIR)/bot

$(OBJDIR)/maze.o: $(SRCDIR)/maze.c $(INCDIR)/maze.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/maze.c -o $(OBJDIR)/maze.o

.PHONY: clean
clean:
	rm -f $(OBJDIR)/maze.o