/*
 * assembler.c
 *
 *  Created on: Sep 17, 2018
 *      Author: Vivian, Morgan Murrell
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define MAX_LINE_LENGTH 255
#define MAX_SYMBOLS 255
#define MAX_LABEL_LEN 20 
<<<<<<< HEAD

=======
<<<<<<< HEAD

typedef int bool;
#define true 1
#define false 0
=======
#define INT_MAX 32768
>>>>>>> 620838eaa47428d4f7629c6a6f637c6969c4b8ac
>>>>>>> 094771180303164adfbc49033353c60a2fcbf2e5
enum
{
   DONE, OK, EMPTY_LINE
};

//Symbol Table struct and array:
struct TableEntry{
	char label[MAX_LABEL_LEN+1];	// label name, +1 because null terminated
	int location; //memory address, not including 'x'
};
struct TableEntry symboltable[MAX_SYMBOLS];
int Tablesize = 0; //number of symbols on table

/*** convert string to int ***/
int toNum( char * pStr ){
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;

   orig_pStr = pStr;
   if( *pStr == '#' )				/* decimal */
   {
     pStr++;
     if( *pStr == '-' )				/* dec is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isdigit(*t_ptr))
       {
	 printf("Error: invalid decimal operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNum = atoi(pStr);
     if (lNeg)
       lNum = -lNum;

     return lNum;
   }
   else if( *pStr == 'x' )	/* hex     */
   {
     pStr++;
     if( *pStr == '-' )				/* hex is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isxdigit(*t_ptr))
       {
	 printf("Error: invalid hex operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
     lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
     if( lNeg )
       lNum = -lNum;
     return lNum;
   }else{
	printf( "Error: invalid operand, %s\n", orig_pStr);
	exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
   }
}



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

<<<<<<< HEAD
/*** convert string to int ***/
int toNum( char * pStr ){
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;
=======
/*
void checkNumOperands(int expectedNum, char * lArg1, char * lArg2, char* lArg3, char * lArg4) {
	if (expectedNum == 0 && )
}*/

void add(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4) {
	printf("This is an add");

}
>>>>>>> 094771180303164adfbc49033353c60a2fcbf2e5

void and(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4) {
	printf("This is an and");
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

			if (strcmp(lArg4, "\0") == 0) {
				printf("lArg4 is '\0'");
			}

			if (strcmp(lOpcode, "add") == 0) add(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "and") == 0) and(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			/*else if (strcmp(lOpcode, "brn") == 0) brn(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "brp") == 0) brp(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "brnp") == 0) brnp(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "br") == 0) br(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "brz") == 0) brz(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "brnz") == 0) brnz(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "brzp") == 0) brzp(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "brnzp") == 0) brnzp(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "halt") == 0) halt(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "jmp") == 0) jmp(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "jsr") == 0) jsr(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "jsrr") == 0) jsrr(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "ldb") == 0) ldb(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "ldw") == 0) ldw(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "lea") == 0) lea(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "nop") == 0) nop(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "not") == 0) not(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "ret") == 0) ret(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "lshf") == 0) lsfh(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "rshfl") == 0) rshfl(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "rshfa") == 0) rshfa(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "rti") == 0) rti(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "stb") == 0) stb(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "stw") == 0) stw(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "trap") == 0) trap(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			else if (strcmp(lOpcode, "xor") == 0) xor(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
			// TO DO: Change to error!
			else printf("Not a valid opcode!");
*/
		}
	} while( lRet != DONE );

}

/*
void createSymbolTable(FILE* inputFile) {
	// First pass: Create symbol table

	char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
			        *lArg2, *lArg3, *lArg4;
	int lRet;
	char lineBuffer[255];
	int cu_address;

		do{
			//increment address every loop
			lRet = readAndParse(inputFile, lLine, &lLabel,
				&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );

			if( lRet != DONE && lRet != EMPTY_LINE ){
					// lRet = 'OK'
				if(strcmp(lOpcode, ".ORIG")){
					//check if valid
					int check_address = toNum(lArg1);
					if(check_address >= 0 && check_address <= 32768){
						cu_address = check_address;
					}else{
						exit(3); //ERROR: invalid constant
					}
				}
				if(*(lLabel) != '0'){
					//check if valid
					int i_sym = 0;
					while(*(lLabel+i_sym) != '0'){
						if(isalnum(*(lLabel+i_sym)) == false){
							goto next_address;
							}
					}
					for(int check = 0; check < Tablesize; check++){
						if(strcmp(lLabel, symboltable[check].label)){
							exit(4); //ERROR: label appears more than once
						}
					}
					for(int input = 0; input < i_sym; input++){
						//copy into symbol table
						symboltable[Tablesize].label[input] = *(lLabel + input);
					}
					symboltable[Tablesize].label[i_sym] = '0'; //null terminate char array
					symboltable[Tablesize].location = cu_address;
					Tablesize++;
				}
			}
			next_address:
			cu_address = cu_address + 2; //increment current address
		} while( lRet != DONE );

}
*/


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
	//createSymbolTable(inputFile);

	// second pass: assembly language to machine language
	createOutputObjFile(input, output);

	fclose(inputFile);
	fclose(outputFile);
	exit(0);
}
