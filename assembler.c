/*
 * assembler.c
 *
 *  Created on: Sep 17, 2018
 *      Author: Vivian
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void createSymbolTable(FILE* inputFile) {
	// First pass: Create symbol table
	char lineBuffer[255];
	while(fgets(lineBuffer, 255, (FILE*) inputFile)) {
		// get .ORIG location
		char delimiter[] = " ";
		char* instruction = strtok(lineBuffer, delimiter);
		if (strcmp(instruction, ".ORIG") == 0) {
			char* firstLocation = strtok(NULL, delimiter);
			// TO DO: CHECK IF LOCATION VALID
			// variable firstLocation contains hex number
			printf(firstLocation);

		}
	}
}

void main(int argc, char *argv[]) {
	char *input = argv[1];
	char *output = argv[2];
	// read in file
	FILE *inputFile = fopen(input, "r");
	// create output file
	FILE *newFilePointer = fopen(output, "w");

	// create Symbol Table
	createSymbolTable(inputFile);

	exit(0);
}
