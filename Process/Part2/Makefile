CC = gcc
EXEC = run

SOURCES=OS_Assignment1_Part2.c

OBJS = $(SOURCES:.cpp=.o)
INCLUDES = -I.
CLEANFILES = *.o 

all: $(SOURCES) $(EXEC)
$(EXEC): $(OBJS) 
	$(CC) $(INCLUDES) $(OBJS) -o $@

.PHONY: clean
clean:
	rm ${EXEC}

