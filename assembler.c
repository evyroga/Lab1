/*
 * assembler.c
 *
 *  Created on: Sep 17, 2018
 *      Author: Vivian
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 255
enum
{
   DONE, OK, EMPTY_LINE
};

// Returns either an OPCODE or a potential LABEL
int isOpcode(char* opcode) {
	char * opcodes[28] = {"add", "and", "brn", "brp", "brnp", "br", "brz", "brnz", "brzp", "brnzp",
			"halt", "jmp", "jsr", "jsrr", "ldb", "ldw", "lea", "nop", "not", "ret", "lshf", "rshfl",
			"rshfa", "rti", "stb", "stw", "trap", "xor"};
	for (int i = 0; i < 28; i++) {
		if (strcmp((opcode), opcodes[i]) == 0) {
			//return opcodes[i];
			return i;
		}
	}

	return (-1);
}

// Takes a line of the input file and parses it into corresponding fields
int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode,
		char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4) {

	char * lRet, * lPtr;
	int i;

	if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
		return( DONE );



	for( i = 0; i < strlen( pLine ); i++ )
		pLine[i] = tolower( pLine[i] );


	// convert entire line to lowercase
	*pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

	// ignore the comments
	lPtr = pLine;

	while( *lPtr != ';' && *lPtr != '\0' && *lPtr != '\n' )
		lPtr++;

	*lPtr = '\0';
	if( !(lPtr = strtok( pLine, "\t\n ," ) ) )
		return( EMPTY_LINE );

	if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) // found a label
	{
		*pLabel = lPtr;
		if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	}

	*pOpcode = lPtr;

	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	*pArg1 = lPtr;

	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	*pArg2 = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	*pArg3 = lPtr;

	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	*pArg4 = lPtr;

	return( OK );

}

void and() {

}

void createOutputObjFile(char * input, char * output) {
	char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
		        *lArg2, *lArg3, *lArg4;

	int lRet;

	FILE * lInFile = fopen(input, "r");

	do
	{
		lRet = readAndParse( lInFile, lLine, &lLabel,
			&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
		if( lRet != DONE && lRet != EMPTY_LINE )
		{
			// lRet = 'OK'
			printf("lLabel is %s\n", lLabel);
			printf("lOpcode is %s\n", lOpcode);
			printf("lArg1 is %s\n", lArg1);
			printf("lArg2 is %s\n", lArg2);
			printf("lArg3 is %s\n", lArg3);
			printf("lArg4 is %s\n\n", lArg4);
			switch(isOpcodes(lOpcode))  {
				case 0 :
					add();
			}


		}
	} while( lRet != DONE );

}

void createSymbolTable(FILE* inputFile) {
	// First pass: Create symbol table
	char lineBuffer[255];
	while(fgets(lineBuffer, 255, (FILE*) inputFile)) {
		// get .ORIG location
		char delimiter[] = " ";
		char* orig = strtok(lineBuffer, delimiter);
		if (strcmp(orig, ".ORIG") == 0) {
			char* firstLocation = strtok(NULL, delimiter);
			// TO DO: CHECK IF LOCATION VALID
			// variable firstLocation contains hex number
			//printf(firstLocation);

		}
	}
}


/*
void createOutputObjFile2(FILE* inputFile, FILE* outputFile) {
	char lineBuffer[255];
	int reachedORIGFlag = 0;
	while(fgets(lineBuffer, 255, (FILE*) inputFile)) {

		// .ORIG has been reached, start ASM -> OBJ translation
		char delimiter[] = " ";
		char* opcode = strtok(lineBuffer, delimiter);
		if (reachedORIGFlag == 1) {
			char* opcode = strtok(lineBuffer, delimiter);
			printf("opcode is: " + translate(opcode));
		}

		// start translating once you reach .ORIG
		char delimiter2[] = " ";
		char* orig = strtok(lineBuffer, delimiter2);
		if (strcmp(orig, ".ORIG") == 0) {
			char* firstLocation = strtok(NULL, delimiter2);
			reachedORIGFlag = 1;
		}
	}
}
*/


char* translate(char* opcode) {
	//checkOpcodes(opcode);

	return ("testing");
}

void main(int argc, char *argv[]) {
	char *input = argv[1];
	char *output = argv[2];
	// read in file
	FILE *inputFile = fopen(input, "r");
	// create output file
	FILE *outputFile = fopen(output, "w");

	if(!inputFile) {
		printf("Error; Cannot open file %s\n", argv[1]);
	}
	if(!outputFile) {
		printf("Error: Cannot open file %s\n", argv[2]);
		exit(4);
	}

	// first pass: create Symbol Table
	createSymbolTable(inputFile);

	// second pass: assembly language to machine language
	createOutputObjFile(input, output);

	fclose(inputFile);
	fclose(outputFile);
	exit(0);
}
