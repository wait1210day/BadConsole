all: main
.PHONY: all

main: toolkit.o compositor.o main.o \
	threads_Playback.o threads_Prompt.o \
	subcmds.o helper.o

	gcc -lpulse-simple -lpthread -lreadline -ldl \
		-o main \
		toolkit.o \
		compositor.o \
		main.o \
		threads_Playback.o \
		threads_Prompt.o \
		subcmds.o \
		helper.o

main.o: src/main.c
	gcc -c -Wall -O2 -Isrc/ -o main.o src/main.c -g

compositor.o: src/compositor.c
	gcc -c -Wall -O2 -Isrc/ -o compositor.o src/compositor.c -g

toolkit.o: src/toolkit.c
	gcc -c -Wall -O2 -Isrc/ -o toolkit.o src/toolkit.c -g

threads_Playback.o: src/threads_Playback.c
	gcc -c -Wall -O2 -Isrc/ -o threads_Playback.o src/threads_Playback.c -g

threads_Prompt.o: src/threads_Prompt.c
	gcc -c -Wall -O2 -Isrc/ -o threads_Prompt.o src/threads_Prompt.c -g

subcmds.o: src/subcmds.c
	gcc -c -Wall -O2 -Isrc/ -o subcmds.o src/subcmds.c -g

helper.o: src/helper.c
	gcc -c -Wall -O2 -Isrc/ -o helper.o src/helper.c -g

.PHONY: clean
clean:
	rm -rf *.o main