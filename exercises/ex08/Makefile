CFLAGS=-I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lglib-2.0

ex_compile: ex_compile.c
	cc ex_compile.c $(shell pkg-config --cflags --libs glib-2.0) -o ex_compile

word_count: word_count.c
	gcc -o word_count word_count.c -g -Wall $(CFLAGS)
