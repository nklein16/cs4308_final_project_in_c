/*
 ============================================================================
 Name        : FinalProject_Hangman.c
 Author      : Nathaniel Klein
 Class		 : Concepts in Programming Languages (CS4308)
 Professor	 : Dr. Amber Wagner
 Date		 : December 7, 2015
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define BUFF_SIZE 250

/* 1 */ void drawScaffold();
/* 2 */ void drawBlanks(int wordL, char *compareWord);
/* 3 */ int alreadyFound(int wordLength, char playerLetter, char * compareWord);
/* 4 */ void replaceBlanks(char playerLetter, char *compareWord, int index, int wordLength);
/* 5 */ void compareWordIs(int wordLength, char *compareWord);
/* 6 */ void drawHangman(int incorrect, int wordLength, char *compareWord);
/* 7 */ char getCharacter();
/* 8 */ void playAgain(int winTotal, int lossTotal);

int winTotal = 0;			// Total # of wins during this session
int lossTotal = 0;			// Total # of losses during this session

int main(void) {

		FILE *inputFilePointer;		// input file pointer for words
		char lines[10][BUFF_SIZE];	// 'lines' stores an array of all the words in the file [#lines = 10][#letters in word-room to be dynamic]
		char *word;					// reference pointer to a word in the file; initialized later in program
		int wordLength;			// the length of the word used in the game
		char playerLetter;		// Letter entered by the player
		char temp;				// Stores end of line character or inadvertent 'Enter' character (to discard them) when getting new character for playerLetter
		int matchLttr = 0;		// Number of letters matched in word comparison
		int incorrect = 0;		// Number of incorrect guesses.  Needed to pass to drawHangman()

	// Open the file for reading
	inputFilePointer = fopen("file.txt", "r");
	if(inputFilePointer == NULL){
		perror("Error opening file!\n");
		//exit(1);
	}

	int i = 0;

	// while not end of file, get each line and store it in line array
	while(!feof(inputFilePointer))
	{
		// gets each line
		fscanf(inputFilePointer, "%s", lines[i]);
		if(lines[i] == NULL){
			perror("Error getting line!\n");
			//exit(1);
		}
		i++;
	}

	printf("\t\t\t\t\tWelcome to Hangman!\n\n");
	printf("\tIn order to save the man on trial, you must guess the correct letters.\n"
			"\tYou are permitted only six incorrect guesses to save his life, so choose wisely!");

	// Draws the initial scaffold for the game
	drawScaffold();

	// Gets a single random number using a seed from the system time so that the number can change on each run
	srand(time(NULL));
	unsigned int randInt = (unsigned int)rand();

	/* By using randInt%, we convert our random number to a one-digit number anywhere from 0-9.  We then store the
	 * number in lineNo, to be used as a reference to the individual lines in array 'lines'. */
	int lineNo = randInt%10;

	// Provides word with size of lines[lineNo]+10 (for good measure)
	word = malloc(sizeof(lines[lineNo])+10);

	// Stores the contents of the line in word
	word = lines[lineNo];

	// Finds the length of the word and stores it in wordLength
	wordLength = strlen(word);
	printf("\n\nWord length is %d:   ", wordLength);

	// 'compareWord' is a char array that will be used later to store the guessed letters that match those in 'word' for printing
	// (but first it stores the blanks [below])
	char compareWord[wordLength];

	// Places blanks in compareWord char array and prints them out to the console
	drawBlanks(wordLength, compareWord);

	// Get letter from player by calling getCharacter()
	printf("\n\nEnter a letter: \n");
	playerLetter = getCharacter();

	while(incorrect < 7){

		// Do this if playerLetter was already found and placed in compareWord, or if playerLetter is not found in word
		// Order of operation is important in this if statement.  It is evaluated from left to right.  If strchr() came first,
		// we would never properly test for when playerLetter was already found.  This order forces that test to occur first.
		if(alreadyFound(wordLength, playerLetter, compareWord) == 1 || (strchr(word, playerLetter) == NULL)){

			incorrect++;
			drawHangman(incorrect, wordLength, compareWord);
			if(incorrect == 6){

				printf("\nSorry, you lose!  Game Over!\n");
				printf("The word was %s\n", word);
				lossTotal++;
				playAgain(winTotal, lossTotal);

			} // if incorrect == 6
		} // if no letters matched

		// Do this if playerLetter is found in word and it was not found there already (from above)
		else if(strchr(word, playerLetter) != NULL){

			// loop through word and compare playerLetter with each letter in word
			for(int i = 0; i < wordLength; i++){

				// If playerLetter is found at word[i], increment matchLttr, and call replaceBlanks, which stores playerLetter in compareWord at index i,
				// and replaces the blank at index i with the letter.  If not, go back to top of for loop and start comparing playerLetter and the letter
				// in 'word' at the next index.
				if(word[i] == playerLetter){

					matchLttr++;
					replaceBlanks(playerLetter, compareWord, i, wordLength);
				}

				if(matchLttr == wordLength){

					printf("\nYou won! Great job!\n");
					winTotal++;
					playAgain(winTotal, lossTotal);

				} // if player wins
			} // for loop for each word
		} // if playerLetter exists somewhere in word

		// Get new letter from player by calling getCharacter()
		printf("\nEnter another letter: ");
		playerLetter = getCharacter();
	}

	fclose(inputFilePointer);
	free(word);
	return 0;
}

/* Draws the scaffolding and man on trial at the beginning of the game */

void drawScaffold(){

	printf("\n\n"
		   "\t\t\t\t\t |---------|\n"
		   "\t\t\t\t\t |         |\n"
		   "\t\t\t\t\t           |\n"
		   "\t\t\t\t\t           |\n"
		   "\t\t\t\t\t           |\t\t  0 \n"
		   "\t\t\t\t\t           |\t\t /|\\ \n"
		   "\t\t\t\t\t       ____|____\t / \\");
}


/* Create array of blanks of size wordL and print them out.  Takes the length of the 'word' ('wordL') and the char array 'compareWord' as a parameter. */
void drawBlanks(int wordL, char *compareWord){

	// stores blanks in compareWord array and prints them out
	for(int i = 0; i < wordL; i++){
		compareWord[i] = '_';
		printf("%c ", compareWord[i]);
	}
}

/* Get new letter to test from player
 * Variable 'temp' permits player to accidentally hit the enter key before keying in the letter and still recover to continue playing.
 * It also serves to eat up the end of line character that follows the first call to getchar().
 */
char getCharacter(){

	char playerLetter;
	char temp;
	playerLetter = getchar();
	temp = getchar();
	return playerLetter;
}

// Tests to see if playerLetter is already in compareWord.  Used in main() at beginning of while loop to force player to be wrong
// when guessing the same letter a second time
int alreadyFound(int wordLength, char playerLetter, char *compareWord){

	for(int i = 0; i < wordLength; i++){

		if(playerLetter == compareWord[i]){
			return 1;
		}
	}
	return 0;
}

// Letters will replace the blanks as they are matched correctly
void replaceBlanks(char playerLetter, char *compareWord, int index, int wordLength){

	compareWord[index] = playerLetter;
	printf("compareWord is now: ");
	compareWordIs(wordLength, compareWord);
}

void compareWordIs(int wordLength, char *compareWord){

	for(int i = 0; i < wordLength; i++){
		printf("%c ", compareWord[i]);
	}
	printf("\n");
}

// Order of evaluation for adding body parts: Head (1), body (2), left arm (3), right arm (4), left leg (5), right leg (6).
void drawHangman(int incorrect, int wordLength, char *compareWord){

	// Must use double back-slashes in structure when displaying right side of body (rightArm, leftLeg, and rightLeg) to escape the escape character
	// and print it out to the console

	char *head, *body, *leftArm, *rightArm, *leftLeg, *rightLeg;
	printf("\n");

	head =   "\t\t\t\t\t |---------|\n"
			 "\t\t\t\t\t |         |\n"
			 "\t\t\t\t\t 0         |\n"
			 "\t\t\t\t\t           |\n"
			 "\t\t\t\t\t           |\n"
			 "\t\t\t\t\t           |\n"
			 "\t\t\t\t\t       ____|____\n\n";

	body = 	 "\t\t\t\t\t |---------|\n"
			 "\t\t\t\t\t |         |\n"
			 "\t\t\t\t\t 0         |\n"
			 "\t\t\t\t\t |         |\n"
			 "\t\t\t\t\t           |\n"
			 "\t\t\t\t\t           |\n"
			 "\t\t\t\t\t       ____|____\n\n";

	leftArm = "\t\t\t\t\t |---------|\n"
			  "\t\t\t\t\t |         |\n"
			  "\t\t\t\t\t 0         |\n"
			  "\t\t\t\t\t/|         |\n"
			  "\t\t\t\t\t           |\n"
			  "\t\t\t\t\t           |\n"
			  "\t\t\t\t\t       ____|____\n\n";

	rightArm = "\t\t\t\t\t |---------|\n"
			   "\t\t\t\t\t |         |\n"
			   "\t\t\t\t\t 0         |\n"
			   "\t\t\t\t\t/|\\        |\n"
			   "\t\t\t\t\t           |\n"
			   "\t\t\t\t\t           |\n"
			   "\t\t\t\t\t       ____|____";

	leftLeg = "\t\t\t\t\t |---------|\n"
			  "\t\t\t\t\t |         |\n"
			  "\t\t\t\t\t 0         |\n"
			  "\t\t\t\t\t/|\\        |\n"
			  "\t\t\t\t\t/          |\n"
			  "\t\t\t\t\t           |\n"
			  "\t\t\t\t\t       ____|____";

	rightLeg = "\t\t\t\t\t |---------|\n"
			   "\t\t\t\t\t |         |\n"
			   "\t\t\t\t\t 0         |\n"
			   "\t\t\t\t\t/|\\        |\n"
			   "\t\t\t\t\t/ \\        |\n"
			   "\t\t\t\t\t           |\n"
			   "\t\t\t\t\t       ____|____\n";

	if(incorrect == 1){

		printf("%s", head);
		printf("\ncompareWord is still: ");
		compareWordIs(wordLength, compareWord);
		printf("You have %d guesses remaining", 6-incorrect);
	}

	else if(incorrect == 2){

		printf("%s", body);
		printf("\ncompareWord is still: ");
		compareWordIs(wordLength, compareWord);
		printf("You have %d guesses remaining", 6-incorrect);
	}

	else if(incorrect == 3){

		printf("%s", leftArm);
		printf("\ncompareWord is still: ");
		compareWordIs(wordLength, compareWord);
		printf("You have %d guesses remaining", 6-incorrect);
	}

	else if(incorrect == 4){

		printf("%s", rightArm);
		printf("\ncompareWord is still: ");
		compareWordIs(wordLength, compareWord);
		printf("You have %d guesses remaining", 6-incorrect);
	}

	else if(incorrect == 5){

		printf("%s", leftLeg);
		printf("\ncompareWord is still: ");
		compareWordIs(wordLength, compareWord);
		printf("You have %d guess remaining", 6-incorrect);
	}

	else if(incorrect == 6){

		printf("%s", rightLeg);
		printf("\ncompareWord is still: ");
		compareWordIs(wordLength, compareWord);
	}
}

void playAgain(int winTotal, int lossTotal){

	printf("Would you like to play again? Type 'Y' or 'N': ");

	char response = getCharacter();

	if(response != 'Y' && response != 'y' && response != 'N' && response != 'n'){
		printf("Please enter a valid response: ");
	}

	if(response == 'Y' || response == 'y'){
		main();
	}

	else if(response == 'N' || response == 'n'){

		printf("Thank you for playing!");
		printf("\nYour score this session:\n");
		printf("Wins: %d\nLosses: %d\n", winTotal, lossTotal);
		exit(1);
	}
}
