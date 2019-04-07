all: main

main: toolkit.o compositor.o main.o threads_Playback.o threads_Prompt.o subcmds.o
	gcc -o main toolkit.o compositor.o main.o threads_Playback.o threads_Prompt.o subcmds.o -lpulse-simple -lpthread -lreadline -g

main.o:
	gcc -c -Wall -O2 -Isrc/ -o main.o src/main.c -g

compositor.o:
	gcc -c -Wall -O2 -Isrc/ -o compositor.o src/compositor.c -g

toolkit.o:
	gcc -c -Wall -O2 -Isrc/ -o toolkit.o src/toolkit.c -g

threads_Playback.o:
	gcc -c -Wall -O2 -Isrc/ -o threads_Playback.o src/threads_Playback.c -g

threads_Prompt.o:
	gcc -c -Wall -O2 -Isrc/ -o threads_Prompt.o src/threads_Prompt.c -g

subcmds.o:
	gcc -c -Wall -O2 -Isrc/ -o subcmds.o src/subcmds.c -g

clean:
	rm -rf *.o main