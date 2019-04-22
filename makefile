OBJS = toolkit.o compositor.o main.o \
	threads_Playback.o threads_Prompt.o \
	subcmds.o helper.o
EXEFILE = melter

CC = /usr/bin/clang
CCFLAGS = -g
CCLDFLAGS = -lpulse-simple -lpthread -lreadline -ldl

PWD = $(shell pwd)

all: $(EXEFILE)
.PHONY: all

$(EXEFILE): $(OBJS)

	$(CC) $(CCLDFLAGS) $(OBJS) -o $(EXEFILE)

main.o: src/main.c
	$(CC) $(CCFLAGS) -c -Wall -O3 -I$(PWD)/src -o main.o src/main.c

compositor.o: src/compositor.c
	$(CC) $(CCFLAGS) -c -Wall -O3 -I$(PWD)/src -o compositor.o src/compositor.c

toolkit.o: src/toolkit.c
	$(CC) $(CCFLAGS) -c -Wall -O3 -I$(PWD)/src -o toolkit.o src/toolkit.c

threads_Playback.o: src/threads_Playback.c
	$(CC) $(CCFLAGS) -c -Wall -O3 -I$(PWD)/src -o threads_Playback.o src/threads_Playback.c

threads_Prompt.o: src/threads_Prompt.c
	$(CC) $(CCFLAGS) -c -Wall -O3 -I$(PWD)/src -o threads_Prompt.o src/threads_Prompt.c

subcmds.o: src/subcmds.c
	$(CC) $(CCFLAGS) -c -Wall -O3 -I$(PWD)/src -o subcmds.o src/subcmds.c

helper.o: src/helper.c
	$(CC) $(CCFLAGS) -c -Wall -O3 -I$(PWD)/src -o helper.o src/helper.c

.PHONY: clean
clean:
	rm -rf *.o melter
