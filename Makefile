all: dictionary

dictionary: main.c
	gcc `pkg-config --cflags gtk+-3.0` -o dictionary main.c `pkg-config --libs gtk+-3.0` -w libbt.a libfdr.a -rdynamic

clean:
	rm -f *.o dictionary
