/*
#include<stdio.h>
#include<string.h>
#include<string>
char* trans_opcode(char s[]) {
	int bits[32] = { 0 };
	int opcode = 0;
	int cnt = 0;
	unsigned long int hex = 0;
	char *operation;
	for (int i = 0; i < 4; i++) {
		unsigned int num = (unsigned int)((unsigned char)s[i]);
		hex = hex * 16*16 +(unsigned int)num;
		for (int j = 0; j < 8; j++) {
			if (num % 2 == 1) {
				bits[7 + 8 * i - j] = 1;
			}
			++cnt;
			num = num / 2;
		}
	}
	
	for (int i = 0; i < 6; i++) {
		opcode = opcode * 2 + bits[i];
		if (opcode == 0) {
			int funct = 0;
			for (int i = 26; i < 32; i++) {
				funct = funct * 2 + bits[i];
			}
			switch (funct) {
			case 0:
				operation = "sll";
				break;
			case 2:
				operation = "srl";
				break;
			case 3:
				operation = "sra";
				break;
			case 8:
				operation = "jr";
				break;
			case 16:
				operation = "mfhi";
				break;
			case 18:
				operation = "mfho";
				break;
			case 24:
				operation = "mult";
				break;
			case 25:
				operation = "multu";
				break;
			case 32:
				operation = "add";
				break;
			case 33:
				operation = "addu";
				break;
			case 34:
				operation = "sub";
				break;
			case 36:
				operation = "and";
				break;
			case 37:
				operation = "or";
				break;
			case 38:
				operation = "xor";
				break;
			case 39:
				operation = "nor";
				break;
			case 40:
				operation = "nand";
				break;
			case 42:
				operation = "slt";
				break;
			default:
				operation = "error_function_code";
			}
		}
		else if(opcode != 0){
			switch (opcode) {
			case 2:
				operation = "j";
				break;
			case 3:
				operation = "jal";
				break;
			case 4:
				operation = "beq";
				break;
			case 5:
				operation = "bne";
				break;
			case 7:
				operation = "bgtz";
				break;
			case 8:
				operation = "addi";
				break;
			case 9:
				operation = "addiu";
				break;
			case 10:
				operation = "slti";
				break;
			case 12:
				operation = "andi";
				break;
			case 13:
				operation = "ori";
				break;
			case 14:
				operation = "nori";
				break;
			case 15:
				operation = "lui";
				break;
			case 32:
				operation = "lb";
				break;
			case 33:
				operation = "lh";
				break;
			case 35:
				operation = "lw";
				break;
			case 36:
				operation = "lbu";
				break;
			case 37:
				operation = "lhu";
				break;
			case 40:
				operation = "sb";
				break;
			case 41:
				operation = "sh";
				break;
			case 43:
				operation = "sw";
				break;
			case 63:
				operation = "halt";
				break;
			default:
				operation = "opcode_error";
			}
		}
	}
	printf("operation=%s\n", operation);
	if (operation == "add" || operation == "addu" || operation == "sub" || operation == "and" || operation == "or" || operation == "xor" || operation == "nor" || operation == "nand" || operation == "slt") {
		int rs, rt, rd;
		rs = (0x03e00000 & hex)>>21;
		rt = (0x001f0000 & hex)>>16;
		rd = (0x0000f800 & hex)>>11;
		printf("rs=%d,rt=%d,rd=%d\n", rs, rt, rd);

	}
	else if (operation == "addi"||operation == "addiu"||operation == "lw"||operation == "lh" || operation == "lhu" || operation == "lb" || operation == "lbu" || operation == "sw" || operation == "sh" || operation == "sb" || operation == "andi" || operation == "ori" || operation == "nori" || operation == "slti" || operation == "beq" || operation == "bne" ) {
	short int rs, rt,immediate;
		rs = (0x03e00000 & hex)>>21;
		rt = (0x001f0000 & hex)>>16;
		immediate = (0x0000ffff & hex);
		printf("rs=%d,rt=%d,immediate=%d\n", rs, rt,immediate);
	}
	else if (operation == "sll" || operation == "srl" || operation == "sra") {
		short int rt, rd, shamt;
		rt = (0x001f0000 & hex) >> 16;
		rd = (0x0000f800 & hex) >> 11;
		shamt = (0x000007c0 & hex) >> 6;
		printf("rt=%d,rd=%d,shamt=%d\n", rt, rd, shamt);
	}
	else if (operation == "jr") {
		short int rs;
		rs = (0x03e00000 & hex) >> 21;
		printf("rs=%d\n", rs);
	}
	else if (operation == "mult" || operation == "multu") {
		short int rs, rt;
		rs = (0x03e00000 & hex) >> 21;
		rt = (0x001f0000 & hex) >> 16;
		printf("rs=%d,rt=%d\n", rs, rt);
	}
	else if (operation == "mfhi" || operation == "mflo") {
		short int rd;
		rd = (0x0000f800 & hex) >> 11;
		printf("rd=%d\n", rd);
	}
	else if (operation == "lui") {
		short int rt, immediate;
		rt = (0x001f0000 & hex) >> 16;
		immediate = (0x0000ffff & hex);
		printf("rt=%d,immediate=%d\n",  rt, immediate);
	}
	else if (operation == "bgtz") {
		short int rs, immediate;
		rs = (0x03e00000 & hex) >> 21;
		immediate = (0x0000ffff & hex);
		printf("rs=%d,immediate=%d\n", rs, immediate);
	}
	else if (operation == "j" || operation == "jal") {
		int address;
		address = (0x03ffffff & hex);
		printf("address=%d\n", address);
	}
	else if (operation == "halt") {
	}
	else {
		printf("error with operation\n");
	}
	return operation;
}

int main()
{
	FILE *fp;
	char ch;
	char instru[1024];
	char instruction[100][4];
	int num = 0;
	int cycle[32] = { 0 };
	int HI;
	int LO;
	int PC;
	if ((fp = fopen("iimage.bin", "rb")) == NULL)
	{
		printf("cannot open file\n");
		return 0;
	}
	ch = fgetc(fp);
	instru[num] = (unsigned char)ch;
	++num;
	while (!feof(fp))
	{
		printf("%02x", ((unsigned char)ch));//unsigned char and signed char
		ch = fgetc(fp);
		instru[num] = (unsigned char)ch;
		++num;
	}
	printf("\n");
	for (int i = 0; i < num-1; i++) {
		printf("%02x",(unsigned char)instru[i]);
	}
	printf("\n");
	char size[4];
	int tmp[4];
	for (int j = 0; j < 4; j++) {
		size[j] = (unsigned char)instru[4 + j];
		printf("%02x", (unsigned char)size[j]);
		tmp[j] = (unsigned int)((unsigned char)size[j]);
	}
	printf("\n");
	int ins_size = 0;
	for (int k = 0; k < 4;k++) {
		ins_size = ins_size * 16 + (unsigned int)tmp[k];
	}
	for (int k = 0; k < ins_size+2; k++) {
		for (int j = 0; j < 4; j++) {
			instruction[k][j] = (unsigned char)instru[4*k+j];
		}
   }
	for (int i = 0; i < ins_size+2; i++) {
		for (int k = 0; k < 4; k++) {
			printf("%02x", (unsigned char)instruction[i][k]);
		}
		printf("\n");
	}
	for (int k = 2; k < ins_size+2; k++) {
		char * s = trans_opcode(instruction[k]);
	}
	
	
	fclose(fp);

	return 0;
}
*/