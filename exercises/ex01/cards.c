/*
 * Exercise 01 Maalvika Bhat SoftSys2020
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * Prompts the user for the name of the card that has been played.
 *
 * prompt: Message to display
 * card_Name: User-input result
 */
void getCardName(char *prompt, char *card_name) {
	puts(prompt);
	scanf("%2s", card_name);
}

/*
 * Returns the value by which the running total should increase.
 *
 * val: The number value of the card that has been played
 */
int getIncrement(int val) {
	if ((val > 2) && (val < 7)) {
		return 1;
	} else if (val == 10) {
		return -1;
	} else {
		return 0;
	}
}

/*
 * Prints the total of the cards played so far.
 *
 * total: The current total
 */
void printTotal(int current) {
	printf("Current count: %i\n", current);
}

/*
 * Simulates a game of blackjack.
 */
int play() {
	char card_name[3];
	int count = 0;
	int val = 0;
	do {
		getCardName("Enter the card_name: ", card_name);
		switch(card_name[0]) {
			case 'K':
			case 'Q':
			case 'J':
				val = 10;
				break;
			case 'A':
				val = 11;
				break;
			case 'X':
				continue;
			default:
				val = atoi(card_name);
				if ((val < 1) || (val > 10)) {
					puts("I don't understand that value!");
					continue;
				}
		}
		printTotal(count + getIncrement(val));
	} while (card_name[0] != 'X');
	return 0;
}

/*
 * The main function.
 */
int main() {
	return play();
}
