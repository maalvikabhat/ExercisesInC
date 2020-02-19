/*
 * My implementation of tee.c for Exercise 3, Software Systems 2020.
 * Reflection and comparison comments are located at the bottom of the file.
 *
 * Available argument: -a = append to a file (default: overwrite file)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>


void writeToFile(char* fileName, char* mode) { // writes given string to given file
	FILE *f = fopen(fileName, mode); // fileName is the name of file to be edited; mode in which string is to be written (w=write, a=append)
	char buffer[256];

	if (f == NULL) {
		printf("An error occurred while opening the file: %s\n", fileName); // prints if f is null
		exit(1);
	}

	while (!feof(stdin)) {
		fgets(buffer, 256, stdin);
		fprintf(f, "%s", buffer);
		printf("%s", buffer);
	}

	fclose(f);
}

int main(int argc, char *argv[]) {
	char ch;
	int i;
	int append = 0;

	while ((ch = getopt(argc, argv, "a")) != EOF) {
		switch (ch) {
			case 'a':
			append = 1;
			break;
			default:
			puts("Invalid option detected.");
			break;
		}
	}

	argc -= optind;
	argv += optind;

	for (i = 0; i < argc; i++) {
		append ? writeToFile(argv[i], "a") : writeToFile(argv[i], "w");
	}

	return 0;
}

/*
 * Reflection
This was a really daunting task for me to take on. I did not know how to start, and was concerned I wouldn't be able to do this exercise.
After meeting with a NINJA who told me to think of this problem as a series of smaller problems, I realized I could split up tasks that would make the problem as a whole seem less intimidating.
Although I started slow, once I figured out the pseudocode, it got easier. The way I broke it down was:
1. Reading standard input
2. Allowing for args
3. Writing to files, etc.
I began to understand what I was doing and looked up many of these things for more guidance.
I finished my final implementation of tee in about an hour.
 *
 * Next time I'm given some task in C that I don't even know how to begin, I think
 * I'll remember my experience with this exercise and just focus on breaking the
 * task into smaller tasks.
 *
 * Comparison
 * Well, looking at the real thing, I've realized that I wasn't too far off which
 * is really encouraging. The real thing and my code have some similarities, like
 * using a ternary operator to retrieve the file mode (file_mode = (append_mode ?
 *  "a" : "w");), but it splits up the smaller parts of tee differently. In fact,
 *  it doesn't really separate any parts except the parsing of the arguments.
 *
 *  I think my code and the real code do mostly the same things though! I was so
 *  confused about how tee was supposed to work, but I'm relieved to see that I
 *  didn't completely miss the mark.
 */
