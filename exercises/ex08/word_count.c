#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>

#define MAXCHAR 10000

char* itoa(int num) {
	char* str = malloc(sizeof(char) * MAXCHAR);
	sprintf(str, "%d", num);
	return str;
}

void updateHashtable(GHashTable* hash, char* key) {
	char* val = g_hash_table_lookup(hash, key);
	if (val == NULL) {
		g_hash_table_insert(hash, key, "1");
	} else {
		int new_val = atoi(val) + 1;
		g_hash_table_replace(hash, key, itoa(new_val));
	}
}

GHashTable* createHashtable() {
	GHashTable* hash = g_hash_table_new(g_str_hash, g_str_equal);
	return hash;
}

void readFile(GHashTable* hash, char* fileName) {
	FILE *f = fopen(fileName, "r");

	if (f == NULL) {
		printf("Could not open the given file: %s\n", fileName);
		return;
	}

	char s[MAXCHAR];
	while (fscanf(f, " %9999s", s) == 1) {
		for (int i = 0; i < MAXCHAR; i++) {
			s[i] = tolower(s[i]);
		}
		char* copy = g_strdup(s);
		updateHashtable(hash, copy);
	}
}

void printEntry(gpointer key, gpointer value, gpointer userdata) {
	printf("%s : %s\n", key, value);
}

int main () {
	GHashTable* hash = createHashtable();
	readFile(hash, "pride_and_prejudice.txt");
	printf("There are %d keys in the hash\n", g_hash_table_size(hash));
	g_hash_table_foreach(hash, printEntry, NULL);
	g_hash_table_destroy(hash);
	return 0;
}
