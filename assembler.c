/*
 * assembler.c
 *
 *  Created on: Sep 17, 2018
 *
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

typedef int bool;
#define true 1
#define false 0

enum
{
   DONE, OK, EMPTY_LINE
};

//Symbol Table struct and array:
typedef struct TableEntry{
	int location; //memory address, not including 'x'
	char label[MAX_LABEL_LEN+1];	// label name, +1 because null terminated
} TableEntry;
TableEntry symboltable[MAX_SYMBOLS];
int Tablesize = 0; //number of symbols on table
int Current = 0; //set when .orig is read

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
	} else {
		printf( "Error: invalid operand, %s\n", orig_pStr);
		exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
	}
}

/*** ***/

//find a^b
int intPower(int a, int b){
	int c = 1;
	for(int i=0; i != b; i++){
		c = c*a;
	}
	return c;
}

int convertBReptoInt(int bitrep[16]){
    int result = 0;
    for(int br_index = 0; br_index < 16; br_index++){
        if(bitrep[br_index] == 1){
            result = result + intPower(2, (15 - br_index));
        }
    }
    return result;
}


//convert from char array to int (hex)
int convertCAtoHex(char* num){
	int hex = 0x0000;
	int bit = 0x0001;
	int decimal = toNum(num);
	int n;
	if(decimal == 0){
		return 0x0000;
	}
	while(decimal >0) {
		for (n = 15; n >= 0; n--) {
			int power = intPower(2, n);
			if ((decimal - power) >= 0) {
				break;
			}
		}
		decimal = decimal - intPower(2, n);
		hex = hex + (0x0001 << n);
	}

	return hex;
}

//converts 4 bits to a hex value (1010 = 'A')
char BintoHex(int binary[16], int index){
	int b_total = 0;
	switch(index) {
		//bit[15:12]
		case 1:
			for(int b=0; b<4; b++){
				if(binary[b] != 0){
					b_total = b_total + intPower(2, (3-b));
				}
			}
			break;
		//bit[11:8]
		case 2:
			for(int b=4; b<8; b++){
				if(binary[b] != 0){
					b_total = b_total + intPower(2, (4-(b-3)));
				}
			}
			break;
		//bit[7:4]
		case 3:
			for(int b=8; b<12; b++){
				if(binary[b] != 0){
					b_total = b_total + intPower(2, (8-(b-3)));
				}
			}
			break;
		//bit[3:0]
		case 4:
			for(int b=12; b<16; b++){
				if(binary[b] != 0){
					b_total = b_total + intPower(2, (12-(b-3)));
				}
			}
			break;
	}
	if(b_total < 10){
		return ('0' + b_total);
	}else{
		b_total = 15 - b_total;
		return ('F' - b_total);
	}
}

//hexval can be in hex or decimal form
//example: 0x3000 = 12288
char* convertHextoCA(int hexval, char hex_arr[5]){
	int binary_arr[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	//char hex_arr[5] = {'x', '0', '0', '0', '0'};

	//convert to binary
	for(int n = 15; n != 0; n--){
		//check if negative
		if((hexval - intPower(2, n)) >= 0){
			binary_arr[15-n] = 1;
			hexval = hexval - intPower(2,n);
		}
	}
	//convert each 4 bits to a hex value
	hex_arr[0] = 'x';
	for(int hex_in = 1; hex_in < 5; hex_in++){
		hex_arr[hex_in] = BintoHex(binary_arr, hex_in);
	}
	return hex_arr;
}

//binary to hex representation (0001000111111111 = x11FF)
void convertBReptoHex(int* bitrep, char hex_arr[5]){
	//char hex_arr[5] = {'x', '0', '0', '0', '0'};

	for(int hex_in = 1; hex_in < 5; hex_in++){
		hex_arr[hex_in] = BintoHex(bitrep, hex_in);
	}
}

int labelInSymbolTable(char * lLabel) {
    for (int i = 0; i < Tablesize; i++) {
        // found matching label
        if(strcmp(symboltable[i].label, lLabel) == 0) {
            return i;
        }
    }
    return -1;
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

//Returns -1 if NOT a pseudo op
int isPseudo(char* olabel){
	char* pseudo[4] = {"getc", "in", "out", "puts"};
	for(int i = 0; i < 4; i++){
		if(strcmp(pseudo[i], olabel) == 0){
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

bool checkValidRegister(char * lArg) {
    if (strcmp(lArg[0], "r") == 0 && lArg[1] >= 0 && lArg[1] <= 7) {
        return true;
    }
    return false;
}

// Check if the instruction has the correct number of operands
bool checkNumOperands(int expectedNum, char * lArg1, char * lArg2, char* lArg3, char * lArg4) {
    int operandsCount = 0;
    if (!*lArg1 == NULL) {
        operandsCount ++;
    }
    if (!*lArg2 == NULL) {
        operandsCount ++;
    }
    if (!*lArg3 == NULL) {
        operandsCount ++;
    }
    if (!*lArg4 == NULL) {
        operandsCount ++;
    }

    if(expectedNum == operandsCount) {
        return true;
    }
    return false;
}


char * convertDecToHex() {
	char * hexValue = "xDEAD";
	return hexValue;
}

//converts r0, r1, r2, etc to binary and sets corresponding bits in bit representation array
//pass in bit rep. array, register value in decimal form, and starting index,
//example: DR = R1, starting index is 4
void convertRegister(int bits[16], int regnum, int b_index){
	for(int p = 2; p >=0; p--){
		if(regnum - intPower(2, p) >=0){
			bits[b_index] = 1;
			regnum = regnum - intPower(2,p);
		}
		b_index++;
	}
}

//similar to convertRegister but converts immediate value or offset value to binary
//and set corresponding bits in bit represenation array.
//ionum = imm/offset in decimal form, length = imm/offset size (5 for ADD/AND, 9 for LEA, etc)
void convertOffset(int bits[16], int ionum, int b_index, int length){
	int negLF = 0; //negative flag, 1 = negative imm/offset
	int copy_index = b_index; //get copy of b_index in case it is zero
	if(ionum < 0){
		negLF = 1;
		ionum = (-1)*ionum;
	}

	for(int p = (length-1); p >= 0; p--){
		if((ionum - intPower(2, p)) >= 0){
			bits[b_index] = 1;
			ionum = ionum - intPower(2, p);
		}
		b_index++;
	}

	if(negLF == 1){ //convert positive to negative (binary)
		// NOT binary, ADD 1 to binary.
		for(int p = 0; p < length; p++){
			bits[copy_index] = !(bits[copy_index]);
			copy_index++;
		}
		copy_index--; //go back to last bit changed
		//if first bit is 1, carry bit = 1;
		if(bits[copy_index] != 0){
			for(int n = 0; n < length; n++){
				if(bits[copy_index] == 0){
					break;
				}
				bits[copy_index] = !(bits[copy_index]);
				copy_index--;
			}
			bits[copy_index] = 1;
		}else{
			bits[copy_index] = !(bits[copy_index]);
		}
	}else{
		return;
	}
}

void add(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitRep[16]) {
    // Check if the number of operands is correct
    int expectedNumOperands = 3;

    // add opcode 0001
    bitRep[3] = 1;

    if(!checkNumOperands(expectedNumOperands, lArg1, lArg2, lArg3, lArg4)) {
        exit(4);
    }
    // Check if 1st and 2nd registers are valid
    if (*lArg1 == 'r' && *(lArg1+1) >= '0' && *(lArg1+1) <= '7'
        && *lArg2 == 'r' && *(lArg2+1) >= '0' && *(lArg2+1) <= '7') {
        int num_rep1 = ((*(lArg1+1) - '0'));
        int num_rep2 = ((*(lArg2+1) - '0'));
        convertRegister(bitRep, num_rep1, 4);
        convertRegister(bitRep, num_rep2, 7);

    }
    else {
        exit (4);
    }
	if(strlen(lArg1) != 2 || strlen(lArg2) != 2){
		exit(4); // ERROR: invalid operand format
	}
    // Check if 3rd register is a valid register
    if (*lArg3 == 'r' && *(lArg3+1) >= '0' && *(lArg3+1) <= '7' && strlen(lArg3) == 2) {
    	int num_rep3 = ((*(lArg3+1) - '0'));
        convertRegister(bitRep, num_rep3, 13 );
    }
        // Check if 3rd register is a valid immediate
    else if((*lArg3 == '#') || (*lArg3 == 'x') ) {
        int dec = toNum(lArg3);
        if(dec >= -16 && dec <= 15) {
            bitRep[10] = 1;
            convertOffset(bitRep, dec, 11, 5);
        }
        else exit (3); // invalid constant
    }
    else {
        exit(4); // invalid operand
    }
}


void and(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]) {
	printf("This is an and \n");
	//check if 3 operands,
	//check if each operand is in correct format. first check if first element is r and/or #
	//after, check if second element (For r) is 0-7 or if #-- matches amount of immediate bits
	//for and, this must be imm5
	if((*lArg1 == NULL) || (*lArg2 == NULL) || (*lArg3 == NULL) || (*lArg4 != NULL)){
		exit(4); //ERROR: incorrect amount of operands
	}
	if((*lArg1 != 'r') || (*lArg2 != 'r') || ((*lArg3 != 'r') && (*lArg3 != '#') && (*lArg3 != 'x'))){
		exit(4);
	}
	if((strlen(lArg1) != 2) || (strlen(lArg2) != 2)){
		exit(4); //ERROR: invalid operand format
	}
	if((*(lArg1+1) < '0' ) || (*(lArg1+1) > '7') || (*(lArg1+1) < '0') || (*(lArg2+1) > '7')){
		exit(4); //ERROR: invalid operand format
	}
	//at this point, we know lArg1 and lArg2 are correct
	if(*(lArg3) == 'r'){
		//register
		if((strlen(lArg3) != 2) || (*(lArg3+1) < '0') || (*(lArg3+1) > '7')){
			exit(4); //ERROR: invalid operand format
		}
	}else{
		//imm5
		int check_num = toNum(lArg3);
		if((check_num < -16) || (check_num > 15)){
			exit(3); //invalid constant
		}
	}
	//now we know the instruction is in the correct format
	//convert to bit representation
	/**/
	//int bitrep[16] = {0,1,0,1, 0,0,0, 0,0,0, 0,0,0, 0,0,0};
	bitrep[1] = 1; bitrep[3] = 1;

	/**/
	int b_index = 4; //index starts at bit[11]
	//NOTE: maybe put vv in a separate function since it is used anytime a register appears?
	int num_rep = ((*(lArg1+1) - '0'));
	convertRegister(bitrep, num_rep, b_index);

	num_rep = (*(lArg2+1) - '0'); //next register
	b_index = 7;
	convertRegister(bitrep, num_rep, b_index);

	if(*lArg3 == 'r'){
		b_index = 13; //keep bit[5:3] = 0
		num_rep = (*(lArg3+1) - '0');
		convertRegister(bitrep, num_rep, b_index);
	}else{
		bitrep[10] = 1;
		b_index = 11;
		num_rep = toNum(lArg3);
		convertOffset(bitrep, num_rep, b_index, 5);
	}

	return;
	//^^ CHANGE AFTER FIXING createOutputObjFile()
	//because we need to pass in the int arr[16] so it does not go out of scope

}

/*      brn     0
        brz             1
        brp             2
        br              3
        brzp    4
        brnp    5
        brnz    6
        brnzp   7       */
void br_(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4,
		 int bitrep[16], int version) {

	switch(version) {
		case 0:
			bitrep[4] = 1;
			break;
		case 1:
			bitrep[5] = 1;
			break;
		case 2:
			bitrep[6] = 1;
			break;
		case 3:
			bitrep[4] = 1;
			bitrep[5] = 1;
			bitrep[6] = 1;
		case 4:
			bitrep[5] = 1;
			bitrep[6] = 1;
			break;
		case 5:
			bitrep[4] = 1;
			bitrep[6] = 1;
			break;
		case 6:
			bitrep[4] = 1;
			bitrep[5] = 1;
			break;
		case 7:
			bitrep[4] = 1;
			bitrep[5] = 1;
			bitrep[6] = 1;
			break;
	}

	// check number of operands
	if(!checkNumOperands(1, lArg1, lArg2, lArg3, lArg4)) {
		exit(4);
	}

	int labelIndex = labelInSymbolTable(lArg1);
	// label not in symbol table
	if( labelIndex == -1){
		exit(1); // Undefined labels
	}
	// label exists
	if (labelIndex != -1){
		int labelLocation = symboltable[labelIndex].location;
		int decOffset = labelLocation - (Current+2);
		int pcOffset = decOffset / 2;

		// check if PCoffset9 within range
		if (pcOffset >= -256 && pcOffset <= 255) {
			convertOffset(bitrep, pcOffset, 7, 9);
		}else{
			exit(4); // label and instruction that uses it are too far apart and offset
			// cannot be specified in machine code
		}
	}
}




void halt(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
    //should only be TRAP x25
    //no other way
    if(!checkNumOperands(0, lArg1, lArg2, lArg3, lArg4)){
        exit(4); //ERROR: invalid amount of operands
    }

    //correct format
    bitrep[0] = 1; bitrep[1] = 1; bitrep[2] = 1; bitrep[3] = 1; //0xF000
    bitrep[10] = 1;  //0x0020
    bitrep[13] = 1; bitrep[15] = 1;  //0x0005
}


void jmp(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
    if(!checkNumOperands(1, lArg1, lArg2, lArg3, lArg4)){
        exit(4);
    }
    bitrep[0] = 1;
    bitrep[1] = 1;
    // check if arg1 is valid register
	if(strlen(lArg1) != 2){
		exit(4); // ERROR: invalid operand format
	}
    if (*lArg1 == 'r' && *(lArg1+1) >= '0' && *(lArg1+1) <= '7') {
        int num_rep = ((*(lArg1+1) - '0'));
        convertRegister(bitrep, num_rep, 7 );
    }
    else {
    	exit(4); // invalid register number
    }
}


void jsr(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
	if((*lArg1 == NULL) || (*lArg2 != NULL) || (*lArg3 != NULL) ||(*lArg4 != NULL)){
		exit(4); //ERROR: invalid amount of operands
	}
	int loca = 0;
	for(int sym = 0; sym <Tablesize; sym++){
		if(strcmp(lArg1, symboltable[sym].label) == 0){
			loca = symboltable[sym].location; //get location (decimal)
			goto FoundJSR;
		}
	}
	exit(1); //ERROR: label does not exist!
	FoundJSR: //location was found

	if((loca < (Current - 1024)) || (loca > (Current + 1023))){
		exit(4); //ERROR: out of range
	}
	int pcOff = (loca - (Current+2))/2;
	bitrep[1] = 1;
	bitrep[4] = 1;
	convertOffset(bitrep, pcOff, 5, 11);
}

void jsrr(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
    /* has to have label. no registers.
     * make sure it is in range
     *
     */

    bitrep[1] = 1;
    // Check if number of operands is correct
    if(!checkNumOperands(1, lArg1, lArg2, lArg3, lArg4)){
        exit(4);
    }
	if(strlen(lArg1) != 2){
		exit(4); // ERROR: invalid operand format
	}
    // check valid register
    if (*lArg1 == 'r' && *(lArg1+1) >= '0' && *(lArg1+1) <= '7') {
        int num_rep = ((*(lArg1+1) - '0'));
        convertRegister(bitrep, num_rep, 7 );
    }
    else {
    	exit(4);
    }
}

void ldb(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
    bitrep[2] = 1;
    // Check if number of operands is correct
    if(!checkNumOperands(3, lArg1, lArg2, lArg3, lArg4)){
        exit(4);
    }
	if(strlen(lArg1) != 2 || strlen(lArg2) != 2){
		exit(4); // ERROR: invalid operand format
	}
    // check valid DR
    if (*lArg1 == 'r' && *(lArg1+1) >= '0' && *(lArg1+1) <= '7') {
        int num_rep = ((*(lArg1+1) - '0'));
        convertRegister(bitrep, num_rep, 4 );
    }
    else {
    	exit(4);
    }
    // check valid BaseR
    if (*lArg2 == 'r' && *(lArg2+1) >= '0' && *(lArg2+1) <= '7') {
        int num_rep = ((*(lArg2+1) - '0'));
        convertRegister(bitrep, num_rep, 7 );
    }
    else {
    	exit(4);
    }
    // check valid boffset6
    int decOffset = toNum(lArg3);

    if (decOffset >= -32 && decOffset <= 31) {
        convertOffset(bitrep, decOffset, 10, 6);
    }
    else {
        exit(3); // invalid constant
    }
}

void ldw(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
    bitrep[1] = 1;
    bitrep[2] = 1;
    // Check if number of operands is correct
    if(!checkNumOperands(3, lArg1, lArg2, lArg3, lArg4)){
        exit(4);
    }
	if(strlen(lArg1) != 2 || strlen(lArg2) != 2){
		exit(4); // ERROR: invalid operand format
	}
    // check valid DR
    if (*lArg1 == 'r' && *(lArg1+1) >= '0' && *(lArg1+1) <= '7') {
        int num_rep = ((*(lArg1+1) - '0'));
        convertRegister(bitrep, num_rep, 4 );
    } else exit(4);
    // check valid BaseR
    if (*lArg2 == 'r' && *(lArg2+1) >= '0' && *(lArg2+1) <= '7') {
        int num_rep = ((*(lArg2+1) - '0'));
        convertRegister(bitrep, num_rep, 7 );
    }else exit (4);
    // check valid boffset6
    int decOffset = toNum(lArg3);

    if (decOffset >= -32 && decOffset <= 31) {
        convertOffset(bitrep, decOffset, 10, 6);
    }
    else {
        exit(3); // invalid constant
    }
}


void lea(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
	// ^ same
	if((*lArg1 == NULL) || (*lArg2 == NULL) || (*lArg3 != NULL) ||(*lArg4 != NULL)){
		exit(4); //ERROR: invalid amount of operands
	}
	if(*lArg2 == 'x'){
		exit(1); //ERROR: label cannot start with x
	}
	if((*lArg1 != 'r') || (strlen(lArg1) != 2)){
		exit(4); // ERROR: invalid operand format
	}
	if((*(lArg1+1) < '0') || (*(lArg1+1) > '7')){
		exit(4); //ERROR: invalid operand format
	}
	//check if label exists
	int loca = 0;
	for(int sym = 0; sym <Tablesize; sym++){
		if(strcmp(lArg2, symboltable[sym].label) == 0){
			loca = symboltable[sym].location; //get location (decimal)
			goto Found;
		}
	}
	exit(1); //ERROR: Label does not exist!
	Found: //label found
	if((loca < (Current - 256)) || (loca > (Current + 255))){
		exit(4); //ERROR: out of range
	}
	int pcOff = loca - (Current+2) ; //label location - current location
	pcOff = pcOff/2;
	bitrep[0] = 1; bitrep[1] = 1; bitrep[2] = 1;

	int regnum = (*(lArg1+1) - '0');
	convertRegister(bitrep, regnum, 4);

	convertOffset(bitrep, pcOff, 7,9);

}

void nop(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
	// nothing
}

void not(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
	//similar to and but no imm5. only 2 operands. must start with 'r', 0-7
	if((*lArg1 == NULL) || (*lArg2 == NULL) || (*lArg3 != NULL) || (*lArg4 != NULL)){
		exit(4); //ERROR: invalid amount of operands
	}
	if((*lArg1 != 'r') || (*lArg2 != 'r')){
		exit(4); //ERROR: invalid operand format
	}
	if((strlen(lArg1) != 2) || (strlen(lArg2) != 2)){
		exit(4); //ERROR: invalid operand format
	}
	if((*(lArg1 +1) < '0') || (*(lArg1+1) > '7') || (*(lArg2+1) < '0') || (*(lArg2+1) > '7')){
		exit(4); //ERROR: invalid operand format
	}
	bitrep[0] = 0; bitrep[3] = 1; //1001
	bitrep[10] = 1; bitrep[11] = 1; bitrep[12] = 1; bitrep[13] = 1; bitrep[14] = 1; bitrep[15] = 1;	//bit[5:0] = 1
	int num_rep = (*(lArg1+1) - '0');
	convertRegister(bitrep, num_rep, 4);
	num_rep = (*(lArg2+1) - '0');
	convertRegister(bitrep, num_rep, 7);

}

void ret(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
	//must be 'RET' otherwise error...
	if((*lArg1 != NULL) || (*lArg2 != NULL) || (*lArg3 != NULL) || (*lArg4 != NULL)){
		exit(4); //ERROR: invalid amount of operands. it shouldn't have any
	}
	bitrep[0] = 1; bitrep[1] = 1; // 0xA000
	bitrep[7] = 1; //0x0100
	bitrep[8] = 1; bitrep[9] = 1; // 0x00A0
}

void lshf(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
	if((*lArg1 == NULL) || (*lArg2 == NULL) || (*lArg3 == NULL) || (*lArg4 !=NULL)){
		exit(4); //ERROR: invalid amount of operands
	}
	if((*lArg1 != 'r') || (*lArg2 != 'r') || (*lArg3 != '#')){
		exit(4); //ERROR: invalid operand format
	}
	if((strlen(lArg1) != 2) || (strlen(lArg2) != 2)){
		exit(4); //ERROR: invalid operand format
	}
	//check r0-r7
	//check #

	if((*(lArg1+1) < '0') || (*(lArg1+1) > '7') || (*(lArg2+1) < '0') || (*(lArg2+1) > '7')){
		exit(4); //ERROR: invalid operand format
	}
	int amfour = toNum(lArg3); // ** AMOUNT4 DECIMAL HERE
	if((amfour < 0) || (amfour > 15)){
		exit(3); //ERROR: invalid constant
	}

	bitrep[0] = 1; bitrep[1] = 1; bitrep[3] = 1;
	int regnum = (*(lArg1+1) - '0');
	convertRegister(bitrep, regnum, 4);  //DR
	regnum = (*(lArg2) - '0');
	convertRegister(bitrep, regnum, 7);  //SR

	convertOffset(bitrep, amfour, 12, 4);

}

void rshfl(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
	if((*lArg1 == NULL) || (*lArg2 == NULL) || (*lArg3 == NULL) || (*lArg4 !=NULL)){
		exit(4); //ERROR: invalid amount of operands
	}
	if((*lArg1 != 'r') || (*lArg2 != 'r') || (*lArg3 != '#')){
		exit(4); //ERROR: invalid operand format
	}
	if((strlen(lArg1) != 2) || (strlen(lArg2) != 2)){
		exit(4); //ERROR: invalid operand format
	}
	//check r0-r7
	//check #

	if((*(lArg1+1) < '0') || (*(lArg1+1) > '7') || (*(lArg2+1) < '0') || (*(lArg2+1) > '7')){
		exit(4); //ERROR: invalid operand format
	}
	int amfour = toNum(lArg3); // ** AMOUNT4 DECIMAL HERE
	if((amfour < 0) || (amfour > 15)){
		exit(3); //ERROR: invalid constant
	}

	bitrep[0] = 1; bitrep[1] = 1; bitrep[3] = 1;
	int regnum = (*(lArg1+1) - '0');
	convertRegister(bitrep, regnum, 4);  //DR
	regnum = (*(lArg2) - '0');
	convertRegister(bitrep, regnum, 7);  //SR
	bitrep[11] = 1;

	convertOffset(bitrep, amfour, 12, 4);

}

void rshfa(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
	if((*lArg1 == NULL) || (*lArg2 == NULL) || (*lArg3 == NULL) || (*lArg4 !=NULL)){
		exit(4); //ERROR: invalid amount of operands
	}
	if((*lArg1 != 'r') || (*lArg2 != 'r') || (*lArg3 != '#')){
		exit(4); //ERROR: invalid operand format
	}
	if((strlen(lArg1) != 2) || (strlen(lArg2) != 2)){
		exit(4); //ERROR: invalid operand format
	}
	//check r0-r7
	//check #

	if((*(lArg1+1) < '0') || (*(lArg1+1) > '7') || (*(lArg2+1) < '0') || (*(lArg2+1) > '7')){
		exit(4); //ERROR: invalid operand format
	}
	int amfour = toNum(lArg3); // ** AMOUNT4 DECIMAL HERE
	if((amfour < 0) || (amfour > 15)){
		exit(3); //ERROR: invalid constant
	}

	bitrep[0] = 1; bitrep[1] = 1; bitrep[3] = 1;
	int regnum = (*(lArg1+1) - '0');
	convertRegister(bitrep, regnum, 4);  //DR
	regnum = (*(lArg2) - '0');
	convertRegister(bitrep, regnum, 7);  //SR
	bitrep[10] = 1; bitrep[11] = 1;

	convertOffset(bitrep, amfour, 12, 4);
}

void rti(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
	if((*lArg1 != NULL) || (*lArg2 != NULL) || (*lArg3 != NULL) || (*lArg4 != NULL)){
		exit(4); //ERROR: invalid amount of operands
	}
	bitrep[0] = 1;
}

void stb(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
	// must have label or #_?
	if((*lArg1 == NULL) || (*lArg2 == NULL) || (*lArg3 == NULL) || (*lArg4 != NULL)){
		exit(4);
	}
	if((*lArg1 != 'r') || (*lArg2 != 'r') || (*lArg3 != '#')){
		exit(4); //ERROR: invalid operand format
	}
	if((strlen(lArg1) != 2) || (strlen(lArg2) != 2)){
		exit(4); //ERROR invalid operand format
	}
	if((*(lArg1+1) <'0') || (*(lArg1+1) > '7') || (*(lArg2+1) < '0') || (*(lArg2+1) > '7')){
		exit(4); //ERROR: invalid operand format
	}
	int offset = toNum(lArg3);
	if((offset < -32) || (offset > 31)){
		exit(3); //ERROR: invalid constant
	}

	bitrep[2] = 1; bitrep[3] = 1;
	int regnum = (*(lArg1+1) - '0');
	convertRegister(bitrep, regnum, 4);
	regnum = (*(lArg2+1) - '0');
	convertRegister(bitrep, regnum, 7);

	//check if in range here

	convertOffset(bitrep, offset, 10, 6);

}

void stw(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
	//same
	// must have label or #_?
	if((*lArg1 == NULL) || (*lArg2 == NULL) || (*lArg3 == NULL) || (*lArg4 != NULL)){
		exit(4);
	}
	if((*lArg1 != 'r') || (*lArg2 != 'r') || (*lArg3 != '#')){
		exit(4); //ERROR: invalid operand format
	}
	if((strlen(lArg1) != 2) || (strlen(lArg2) != 2)){
		exit(4); //ERROR invalid operand format
	}
	if((*(lArg1+1) <'0') || (*(lArg1+1) > '7') || (*(lArg2+1) < '0') || (*(lArg2+1) > '7')){
		exit(4); //ERROR: invalid operand format
	}
	int offset = toNum(lArg3);
	if((offset < -32) || (offset > 31)){
		exit(3); //ERROR: invalid constant
	}

	bitrep[1] = 1; bitrep[2] = 1; bitrep[3] = 1;
	int regnum = (*(lArg1+1) - '0');
	convertRegister(bitrep, regnum, 4);
	regnum = (*(lArg2+1) - '0');
	convertRegister(bitrep, regnum, 7);

	//check if in range here

	convertOffset(bitrep, offset, 10, 6);
}

void trap(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4, int bitrep[16]){
	//should only be TRAP x25
	//no other way
	if((*(lArg1) == NULL) || (*lArg2 != NULL) || (*lArg3 != NULL) || (*lArg4 != NULL)){
		exit(4); //ERROR: invalid amount of operands
	}
	int trapvec = toNum(lArg1);
	if (trapvec < 0) {
		exit(3); // trap vector must be non-negative
	}

	if(strcmp(lArg1, "x25") != 0){
		exit(4); //ERROR: invalid operand (?)
	}
	//correct format
	bitrep[0] = 1; bitrep[1] = 1; bitrep[2] = 1; bitrep[3] = 1; //0xF000
	bitrep[10] = 1;  //0x0020
	bitrep[13] = 1; bitrep[15] = 1;  //0x0005
}

void xor(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4,int bitrep[16]){
	//similar to not
	if((*lArg1 == NULL) || (*lArg2 == NULL) || (*lArg3 == NULL) || (*lArg4 != NULL)){
		exit(4); // ERROR: invalid amount of operands
	}
	if((*lArg1 != 'r') || (*lArg2 != 'r') || ((*lArg3 != 'r') && (*lArg3 != '#') && (*lArg3 != 'x'))){
		exit(4); //ERROR: invalid operand format
	}
	if((strlen(lArg1) != 2) || (strlen(lArg2) != 2)){
		exit(4); //ERROR: invlaid operand format
	}
	if((*(lArg1+1) < '0' ) || (*(lArg1+1) > '7') || (*(lArg1+1) < '0') || (*(lArg2+1) > '7')){
		exit(4); //ERROR: invalid operand format
	}

	if(*(lArg3) == 'r'){
		//register
		if((strlen(lArg3) != 2) || (*(lArg3+1) < '0') || (*(lArg3+1) > '7')){
			exit(4); //ERROR: invalid operand format
		}
	}else{
		//imm5
		int check_num = toNum(lArg3);
		if((check_num < -16) || (check_num > 15)){
			exit(3); //invalid constant
		}
	}

	bitrep[0] = 1; bitrep[3] = 1;
	int regnum = (*(lArg1+1) - '0');
	convertRegister(bitrep, regnum, 4);
	regnum = (*(lArg2+1) - '0');
	convertRegister(bitrep, regnum, 7);

	if(*lArg3 == 'r'){
		regnum = (*(lArg3+1) - '0');
		convertRegister(bitrep, regnum, 13);
	}else{
		int imm = toNum(lArg3);
		bitrep[10] = 1;
		convertOffset(bitrep, imm, 11, 5);
	}
}

void fill(char * lLabel, char * lOpcode, char * lArg1, char * lArg2, char * lArg3, char * lArg4,int bitrep[16]){
    // take first argument
    if((*lArg1 == NULL) || (*lArg2 != NULL) || (*lArg3 != NULL) || (*lArg4 != NULL)){ exit(4); }
    if(*lArg1 != 'x'){
        exit(4);
    }
}


void createOutputObjFile(char* input, FILE* output) {
	char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
		        *lArg2, *lArg3, *lArg4;

	int lRet;

	FILE * lInFile = fopen(input, "r");

	do {
        int bitrep[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        char hexrep[5] = {'x', '0', '0', '0', '0'};

        lRet = readAndParse(lInFile, lLine, &lLabel,
							&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
		if (lRet != DONE && lRet != EMPTY_LINE) {
			// lRet = 'OK'
			printf("lLabel is %s\n", lLabel);
			printf("lOpcode is %s\n", lOpcode);
			printf("lArg1 is %s\n", lArg1);
			printf("lArg2 is %s\n", lArg2);
			printf("lArg3 is %s\n", lArg3);
			printf("lArg4 is %s\n\n", lArg4);

            if(strcmp(lOpcode, ".orig") == 0){
                fprintf(output, "0x%.4X\n", Current);
                continue;
            }

            Current = Current + 0x0002;
            if (Current > 65536) exit(4); // PC cannot go past xFFFF


            if (strcmp(lOpcode, "add") == 0) {
                add(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "and") == 0){
                and(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "brn") == 0) {
                br_(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep, 0);
            }else if(strcmp(lOpcode, "brz") == 0) {
                br_(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep,1);
            }else if(strcmp(lOpcode, "brp") == 0) {
                br_(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep,2);
            }else if(strcmp(lOpcode, "br") == 0) {
                br_(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep,3);
            }else if(strcmp(lOpcode, "brzp") == 0) {
                br_(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep, 4);
            }else if(strcmp(lOpcode, "brnp") == 0) {
                br_(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep, 5);
            }else if(strcmp(lOpcode, "brnz") == 0) {
                br_(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep, 6);
            }else if(strcmp(lOpcode, "brnzp") == 0) {
                br_(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep, 7);
            }else if(strcmp(lOpcode, "halt") == 0) {
                halt(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "jmp") == 0) {
                jmp(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "jsrr") == 0) {
                jsrr(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "ldb") == 0) {
                ldb(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "ldw") == 0) {
                ldw(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "lea") == 0) {
                lea(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "nop") == 0) {
                nop(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "not") == 0) {
                ret(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "lshf") == 0) {
                lshf(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "rshfl") == 0) {
                rshfl(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "rshfa") == 0) {
                rshfa(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "rti") == 0) {
                rti(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "stb") == 0) {
                stb(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "stw") == 0) {
                stw(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "trap") == 0) {
                trap(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "xor") == 0) {
                xor(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "and") == 0) {
                and(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, "jsr") == 0) {
                jsr(lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, bitrep);
            }else if(strcmp(lOpcode, ".fill") == 0){
            	fill(lLabel,lOpcode,lArg1,lArg2,lArg3,lArg4,bitrep);
                int fill_num = toNum(lArg1);
                if (fill_num >= -65536 && fill_num <= 65535) {
                    fprintf(output, "0x%.4X\n", fill_num); //CHECK TO fprintf
                    continue;
                } else exit(3); // invalid constant
            }else if(strcmp(lOpcode, ".end") == 0){
                return;
            }else{
                printf("Not a valid opcode!");
                exit(2);
            }
            int inst = convertBReptoInt(bitrep);
            fprintf(output, "0x%.4X\n", inst);

		}
	}while (lRet != DONE);
}


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

				printf("lLabel is %s\n", lLabel);
				printf("lOpcode is %s\n", lOpcode);
				printf("lArg1 is %s\n", lArg1);
				printf("lArg2 is %s\n", lArg2);
				printf("lArg3 is %s\n", lArg3);
				printf("lArg4 is %s\n\n", lArg4);


				if(strcmp(lOpcode, ".orig") == 0){
					//check if valid
					int check_address = toNum(lArg1);
					if((check_address % 2) != 0){
						exit(3); //ERROR: odd constant
					}
					if(check_address >= 0 && check_address <= 65535){
						cu_address = check_address;
						Current = cu_address;
					}else{
						exit(3); //ERROR: invalid constant
					}
				}
				if(strcmp(lOpcode, ".end") == 0){
					return; //done
				}
				//check args
				if((isOpcode(lArg1) != -1) || (isOpcode(lArg2) != -1) || (isOpcode(lArg3) != -1) || (isOpcode(lArg4) != -1)){
					exit(1);
				}
				if(*lLabel != NULL){
					if(isOpcode(lLabel) != -1){
						exit(1); //ERROR: label is an instruction
					}
					if(isPseudo(lLabel) != -1){
						exit(1); //ERROR: label is a pseudo-op
					}
					if (*lLabel == 'x') {
						exit(1); // label starts with 'x'
					}
					int i_sym = 0;
					while(*(lLabel+i_sym) != NULL){
						if(isalnum(*(lLabel+i_sym)) == false){
							exit(4); //ERROR: invalid label format
						}
						i_sym ++;
					}
					for(int check = 0; check < Tablesize; check++){
						if(strcmp(lLabel, symboltable[check].label) == 0){
							exit(4); //ERROR: label appears more than once
						}
					}
					for(int input = 0; input < i_sym; input++){
						//copy into symbol table
						symboltable[Tablesize].label[input] = *(lLabel + input);
					}
					if (cu_address > 65535) {
						exit (4); // the location for the address is out of bounds
					}
					symboltable[Tablesize].location = cu_address;
					Tablesize++;
				}
				next_address:
				cu_address = cu_address + 0x0002; //increment current address
			}
		} while( lRet != DONE );
}


void main(int argc, char *argv[]) {
	char *input = argv[1];
	char *output = argv[2];
	// read in file
	FILE *inputFile = fopen(input, "r");
	// create output file
	FILE *outputFile = fopen(output, "w");

	if(!inputFile) {
		printf("Error; Cannot open input file %s\n", argv[1]);
	}
	if(!outputFile) {
		printf("Error: Cannot open output file %s\n", argv[2]);
		exit(4);
	}

	// first pass: create Symbol Table
	createSymbolTable(inputFile);
	fclose(inputFile);

	// second pass: assembly language to machine language
	createOutputObjFile(input, outputFile);

	fclose(inputFile);
	fclose(outputFile);
	exit(0);

}
