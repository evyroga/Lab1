/*
 * assembler.c
 *
 *  Created on: Sep 17, 2018
 *      Author: Vivian
 */
#include <stdio.h>
#include <stdlib.h>

void main(int argc, char *argv[]) {
	char *input = argv[1];
	char *output = argv[2];
	// read in file
	FILE *inputFile = fopen(input, "r");
	// create output file
	FILE *newFilePointer = fopen(output, "w");

	// Parse lines from input file and write to output file
	char lineBuffer[255];
	while(fgets(lineBuffer, 255, (FILE*) inputFile)) {
		fprintf(newFilePointer, lineBuffer);
	}
	exit(0);
}
