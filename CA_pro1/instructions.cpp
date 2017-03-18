/*
#include<iostream>
#include<string>
#include<fstream>
#include<stdio.h>
#include<iomanip>
#include<stdint.h>
const int N = 1000;
using namespace std;
char* get_operation(unsigned short int opcode, unsigned int instr) {
	char* operation = "error";
	if (opcode == 0) {
		unsigned short int funct = (0x0000003f & instr);
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
			operation = "mflo";
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
	else if (opcode != 0) {
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
	return operation;
}
void trans_opcode(unsigned int instruction[],unsigned int execu_instru,unsigned int data[],unsigned int load_data) {
	unsigned int instr = instruction[0];
	unsigned int cycle[32] = { 0 };
	unsigned int HI = 0;
	unsigned int LO = 0;
	unsigned int PC =(unsigned int) instr;
	unsigned int iniPC = PC;//save the initial value of PC
	int cycle_num = 0;//count how many cycle have run
	int stack[100] = { 0 };
	short int HI_flag = 0;//check if HI has changed
	short int LO_flag = 0;//check if LO has changed
	cycle[29] = (unsigned int)data[0];
	unsigned int iniSP = cycle[29];//save the initial value of SP pointer;
	ofstream error_file;
	error_file.open("error_dump.rpt");
	std::cout << "cycle " << cycle_num << endl;
	for (int i = 0; i < 32; i++) {
		printf("$%02d: 0x%08X\n", i, (unsigned int)cycle[i]);
	}
	printf("HI: 0x%08X\n", HI);
	printf("LO: 0x%08X\n", LO);
	printf("PC: 0x%08X\n", PC);
	std::cout << "\n\n";
	instr = instruction[2];
	unsigned short int opcode = (0xfc000000 & instr) >> 26;
	char* operation = "error";
	//according to the opcode get the name of operation
	operation = get_operation(opcode, instr);
	std::cout << operation << endl;
	while (operation&&operation!="halt") {
		++cycle_num;
		std::cout << "cycle " << dec<<cycle_num << endl;
		if (operation == "add" || operation == "addu" || operation == "sub" || operation == "and" || operation == "or" || operation == "xor" || operation == "nor" || operation == "nand" || operation == "slt") {
			int rs, rt, rd;
			rs = (0x03e00000 & instr) >> 21;
			rt = (0x001f0000 & instr) >> 16;
			rd = (0x0000f800 & instr) >> 11;
			printf("rs=%d,rt=%d,rd=%d\n", rs, rt, rd);
			int rd_value = cycle[rd];
			if (operation == "add") {
				cycle[rd] = cycle[rs] + cycle[rt];
				if(cycle[rd]!=rd_value)
					printf("$%02d: 0x%08X\n", rd, cycle[rd]);
			}
			else if (operation == "slt") {
				cycle[rd] = (cycle[rs] < cycle[rt]);
				if(cycle[rd]!=rd_value)
					printf("$%02d: 0x%08X\n", rd, cycle[rd]);
			}
			else if (operation == "or") {
				cycle[rd] = cycle[rs] | cycle[rt];
				if (cycle[rd] != rd_value)
					printf("$%02d: 0x%08X\n", rd, cycle[rd]);
			}
			else if (operation == "sub") {
				cycle[rd] = cycle[rs] - cycle[rt];
				if (cycle[rd] != rd_value)
					printf("$%02d: 0x%08X\n", rd, cycle[rd]);
			}
			PC = PC + 4;
			printf("PC: 0x%08X\n", PC);
		}
		else if (operation == "addi" || operation == "addiu" || operation == "lw" || operation == "lh" || operation == "lhu" || operation == "lb" || operation == "lbu" || operation == "sw" || operation == "sh" || operation == "sb" || operation == "andi" || operation == "ori" || operation == "nori" || operation == "slti" || operation == "beq" || operation == "bne") {
			short int rs, rt, immediate;
			rs = (0x03e00000 & instr) >> 21;
			rt = (0x001f0000 & instr) >> 16;
			int rt_value = cycle[rt];
			immediate = (0x0000ffff & instr);
			printf("rs=%d,rt=%d,immediate=%d\n", rs, rt, immediate);
			if (operation == "addi") {//注意溢出错误
				int tmp = 0;
				if (immediate < 0) {
					tmp = (0xffff0000 | immediate);
				}
				else {
					tmp = (int)immediate;
				}
				cycle[rt] = cycle[rs] +tmp;
				PC = PC + 4;
				if(cycle[rt]!=rt_value)
					printf("$%02d: 0x%08X\n", rt, cycle[rt]);
				printf("PC: 0x%08X\n", PC);
			}
			else if (operation == "sw" || operation == "bne"||operation == "beq"||operation == "sb"||operation == "sh") {
				if (operation == "sw") {
					if (rs != 0) {
						//save ra in stack
						stack[immediate / 4+(iniSP-cycle[29])/4-1] = cycle[rt];
					}
					else {
						data[immediate / 4 + 2] = cycle[rt];
					}
					PC = PC + 4;
				}
				else if (operation == "sh") {
					if (rs == 0) {
						int pos = immediate / 4 + 2;
						if (immediate % 2 == 0) {
							data[pos] = (data[pos] & 0xffff) | (cycle[rt] << 16);
						}
						else {
							data[pos] = (data[pos] & 0xffff0000) | cycle[rt];//unknow whether need to expand to int32_t
						}
					}
					else {
						int pos = immediate / 4 + 1+(iniSP-cycle[29])/4;
						if (immediate % 2 == 0) {
							stack[pos] = (stack[pos] & 0xffff) | (cycle[rt] << 16);
						}
						else {
							stack[pos] = (stack[pos] & 0xffff0000) | cycle[rt];
						}
					}
					PC = PC + 4;
				}
				else if(operation == "sb"){
					if (rs == 0) {
						int pos = immediate / 4 + 2;
						if (immediate % 4 == 0)
							data[pos] = (data[pos] & 0xffffff) | ((cycle[rt] & 0xff)<<24);
						else if (immediate % 4 == 1)
							data[pos] = (data[pos] & 0xff00ffff) | ((cycle[rt] & 0xff) << 16);
						else if (immediate % 4 == 2)
							data[pos] = (data[pos] & 0xffff00ff) | ((cycle[rt] & 0xff) << 8);
						else
							data[pos] = (data[pos] & 0xffffff00) | (cycle[rt] & 0xff);
					}
					else {
						int pos = immediate / 4 + 1 + (iniSP - cycle[29]) / 4;
						if (immediate % 4 == 0)
							stack[pos] = (stack[pos] & 0xffffff) | ((cycle[rt] & 0xff) << 24);
						else if (immediate % 4 == 1)
							stack[pos] = (stack[pos] & 0xff00ffff) | ((cycle[rt] & 0xff) << 16);
						else if (immediate % 4 == 2)
							stack[pos] = (stack[pos] & 0xffff00ff) | ((cycle[rt] & 0xff) << 8);
						else
							stack[pos] = (stack[pos] & 0xffffff00) | (cycle[rt] & 0xff);
					}
					PC = PC + 4;
				}
				else if (operation == "bne") {
					if (cycle[rs] != cycle[rt])
						PC = PC + 4 + 4 * immediate;
					else
						PC = PC + 4;
				}
				else if (operation == "beq") {
					if (cycle[rs] == cycle[rt])
						PC = PC + 4 + 4 * immediate;
					else
						PC = PC + 4;
				}
				cout << "PC: " << hex << setfill('0') << setw(8) << PC << endl;
			}
			else if (operation == "lw") {
				if (rs != 0) {
					//get data from stack
					cycle[rt] = stack[immediate / 4+(iniSP-cycle[29])/4-1];
				}
				else {
					cycle[rt] = data[2+immediate / 4];
				}
				PC = PC + 4;
				if (cycle[rt] != rt_value)
					printf("$%02d: 0x%08X\n", rt, cycle[rt]);
				printf("PC: 0x%08X\n", PC);
			}
			else if (operation == "addiu") {
				int tmp = 0;
				if (immediate < 0) {
					tmp = 0xffff0000 | immediate;
				}
				else {
					tmp = (int)immediate;
				}
				cycle[rt] = cycle[rs] + tmp;
				PC = PC + 4;
				if (cycle[rt] != rt_value)
					printf("$%02d: 0x%08X\n", rt, cycle[rt]);
				printf("PC: 0x%08X\n", PC);
			}
			else if (operation == "lhu") {
				if (rs == 0) {
					int pos = immediate / 4 + 2;
					if (immediate % 2 == 0) {
						cycle[rt] = (uint32_t)((uint16_t)((data[immediate / 4 + 2] & 0xffff0000)>>16));
					}
					else {
						cycle[rt] = (uint32_t)((uint16_t)(data[immediate / 4 + 2] & 0xffff0000));
					}
				}
				else {
					int pos = immediate / 4 + 2;
					if (immediate % 2 == 0) {
						cycle[rt] = (uint32_t)((uint16_t)((stack[immediate / 4 + (iniSP - cycle[29]) / 4 + 1] & 0xffff0000) >> 16));
					}
					else {
						cycle[rt] = (uint32_t)((uint16_t)(stack[immediate / 4 + (iniSP - cycle[29]) / 4 + 1] & 0xffff0000));
					}
				}
				PC = PC + 4;
				if (cycle[rt] != rt_value)
					printf("$%02d: 0x%08X\n", rt, cycle[rt]);
				printf("PC: 0x%08X\n", PC);
			}
			else if (operation == "lh") {
				if (rs == 0) {
					int pos = immediate / 4 + 2;
					if (immediate % 2 == 0) {
						cycle[rt] = (int32_t)((int16_t)((data[immediate / 4 + 2] & 0xffff0000) >> 16));
					}
					else {
						cycle[rt] = (int32_t)((int16_t)(data[immediate / 4 + 2] & 0xffff0000));
					}
				}
				else {
					int pos = immediate / 4 + 2;
					if (immediate % 2 == 0) {
						cycle[rt] = (int32_t)((int16_t)((stack[immediate / 4 + (iniSP - cycle[29]) / 4 + 1] & 0xffff0000) >> 16));
					}
					else {
						cycle[rt] = (int32_t)((int16_t)(stack[immediate / 4 + (iniSP - cycle[29]) / 4 + 1] & 0xffff0000));
					}
				}
				PC = PC + 4;
				if (cycle[rt] != rt_value)
					printf("$%02d: 0x%08X\n", rt, cycle[rt]);
				printf("PC: 0x%08X\n", PC);
			}
			else if (operation == "ori") {
				cycle[rt] = cycle[rs] | (unsigned int)immediate;
				PC = PC + 4;
				if (cycle[rt] != rt_value)
					printf("$%02d: 0x%08X\n", rt, cycle[rt]);
				printf("PC: 0x%08X\n", PC);
			}
			else if (operation == "lbu") {
				if (rs == 0) {
					int pos = immediate / 4 + 2;
					if (immediate%4 == 0)
						cycle[rt] = (unsigned int)((data[pos] & 0xff000000) >> 24);
					else if (immediate%4 == 1)
						cycle[rt] = (unsigned int)((data[pos] & 0x00ff0000) >> 16);
					else if (immediate%4 == 2)
						cycle[rt] = (unsigned int)((data[pos] & 0x0000ff00) >> 8);
					else
						cycle[rt] = (unsigned int)(data[pos] & 0x000000ff);
				}
				PC = PC + 4;
				if (cycle[rt] != rt_value)
					printf("$%02d: 0x%08X\n", rt, cycle[rt]);
				printf("PC: 0x%08X\n", PC);
			}
			else if (operation == "lb") {
				if(rs == 0) {
					int pos = immediate/4+2;
					if (immediate%4 == 0)
						cycle[rt] = (int64_t)((int8_t)(data[pos]>>24));
					else if(immediate%4 == 1)
						cycle[rt] = (int64_t)(int8_t)(((data[pos] & 0x00ff0000) >> 16));
					else if(immediate%4 == 2)
						cycle[rt] = (int64_t)((int8_t)((data[pos] & 0x0000ff00) >> 8));
					else
						cycle[rt] = (int64_t)((int8_t)(data[pos] & 0x000000ff));
				}
				else {
					int pos = immediate / 4 + (iniSP - cycle[29]) / 4 - 1;
					if (immediate % 4 == 0)
						cycle[rt] = (int64_t)((int8_t)(stack[pos] >> 24));
					else if (immediate % 4 == 1)
						cycle[rt] = (int64_t)(int8_t)(((stack[pos] & 0x00ff0000) >> 16));
					else if (immediate % 4 == 2)
						cycle[rt] = (int64_t)((int8_t)((stack[pos] & 0x0000ff00) >> 8));
					else
						cycle[rt] = (int64_t)((int8_t)(stack[pos] & 0x000000ff));;
				}
				PC = PC + 4;
				if (cycle[rt] != rt_value)
					printf("$%02d: 0x%08X\n", rt, cycle[rt]);
				printf("PC: 0x%08X\n", PC);
			}
			else if (operation == "slti") {
				cycle[rt] = (cycle[rs] < (int32_t)immediate);
				PC = PC + 4;
				if(cycle[rt]!=rt_value)
					printf("$%02d: 0x%08X\n", rt, cycle[rt]);
				printf("PC: 0x%08X\n", PC);
			}
		}
		else if (operation == "sll" || operation == "srl" || operation == "sra") {
			short int rt, rd, shamt;
			rt = (0x001f0000 & instr) >> 16;
			rd = (0x0000f800 & instr) >> 11;
			shamt = (0x000007c0 & instr) >> 6;
			printf("rt=%d,rd=%d,shamt=%d\n", rt, rd, shamt);
		}
		else if (operation == "jr") {
			short int rs;
			rs = (0x03e00000 & instr) >> 21;
			printf("rs=%d\n", rs);
			PC = cycle[rs];
			printf("PC: 0x%08X\n", PC);
		}
		else if (operation == "mult" || operation == "multu") {//what is the meaning of overflow of mult
			short int rs, rt;
			rs = (0x03e00000 & instr) >> 21;
			rt = (0x001f0000 & instr) >> 16;
			printf("rs=%d,rt=%d\n", rs, rt);
			uint64_t product = 0;
			if (operation == "mult") {
				product = (uint64_t)((cycle[rs] * cycle[rt])&0xffffffffffffffff);	
			}
			else {
				uint64_t tmp = 0x00000000ffffffff;
				uint64_t rsval = (uint64_t)(cycle[rs]&tmp);
				uint64_t rtval = (uint64_t)(cycle[rt] & tmp);
				product = (uint64_t)rsval*(uint64_t)rtval;
			}
			int HI_value = (int32_t)((product & 0xffffffff00000000) >> 32);
			int LO_value = (int32_t)(0xffffffff & product);
			if (HI != HI_value)
			{
				HI = HI_value;
				++HI_flag;
				printf("HI: 0x%08X\n", HI);
			}
			if (LO != LO_value) {
				LO = LO_value;
				++LO_flag;
				printf("LO: 0x%08X\n", LO);
			}
			if (HI_flag>1||LO_flag>1) {//print error to file
				error_file<< "In cycle " << cycle_num << ": Overwrite HI-LO registers" << endl;
			}
			PC = PC + 4;
			printf("PC: 0x%08X\n", PC);
		}
		else if (operation == "mfhi" || operation == "mflo") {
			short int rd;
			rd = (0x0000f800 & instr) >> 11;
			printf("rd=%d\n", rd);
			if (operation == "mfhi") {//需要测试如果是$zero,mflo时是否会显示出$zero
				if (rd != 0) {
					cycle[rd] = HI;
					printf("$%02d: 0x%08X\n", rd, cycle[rd]);
				}
				else {
					error_file << "In cycle " << cycle_num << ": Write $0 Error" << endl;
				}
			}
			else {
				if (rd != 0) {
					cycle[rd] = LO;
					printf("$%02d: 0x%08X\n", rd, cycle[rd]);
				}
				else {
					error_file << "In cycle " << cycle_num << ": Write $0 Error" << endl;
				}
			}
			PC = PC + 4;
			printf("PC: 0x%08X\n", PC);
		}
		else if (operation == "lui") {
			short int rt, immediate;
			rt = (0x001f0000 & instr) >> 16;
			immediate = (0x0000ffff & instr);
			printf("rt=%d,immediate=%d\n", rt, immediate);
		}
		else if (operation == "bgtz") {
			short int rs, immediate;
			rs = (0x03e00000 & instr) >> 21;
			immediate = (0x0000ffff & instr);
			printf("rs=%d,immediate=%d\n", rs, immediate);
			if (cycle[rs] > 0)
				PC = PC + 4 + 4 * immediate;
		}
		else if (operation == "j" || operation == "jal") {
			int address;
			address = (0x03ffffff & instr);
			printf("address=%d\n", address);
			if (operation == "jal") {
				int ra_value = cycle[31];
				cycle[31] = PC + 4;
				PC = (((PC + 4) >> 28) << 28) + 4 * address;
				if(ra_value!=cycle[31])
					printf("$%02d: 0x%08X\n", 31, cycle[31]);
				printf("PC: 0x%08X\n", PC);
			}
			else {
				PC = (((PC + 4) >> 28) << 28) + 4 * address;
				printf("PC: 0x%08X\n", PC);
			}
		}
		else if (operation == "halt") {
		}
		else {
			printf("error with operation\n");
		}
		instr = instruction[(PC - iniPC) / 4 + 2];
		opcode = (0xfc000000 & instr) >> 26;
		operation = get_operation(opcode, instr);
		std::cout << operation << endl;
	}
}
int main()
{
	char* buffer;
	char* databuf;
	char* iimage = "iimage.bin";
	char* dimage = "dimage.bin";
	long size;
	unsigned int instruction[N];
	unsigned int data[N];
	int instru_num = 0;
	int data_num = 0;
	ifstream file(iimage, ios::in | ios::binary | ios::ate);
	size = file.tellg();
	instru_num = size / 4;
	file.seekg(0, ios::beg);
	buffer = new char[size];
	file.read(buffer, size);
	file.close();
	ifstream datafile(dimage, ios::in | ios::binary | ios::ate);
	long datasize = datafile.tellg();
	datafile.seekg(0, ios::beg);
	databuf = new char[size];
	datafile.read(databuf, datasize);
	datafile.close();
	for (int i = 0; i < instru_num; i++) {
		for (int j = 0; j < 4; j++) {
			instruction[i] = instruction[i] * 16 * 16 + (unsigned int)((unsigned char)buffer[4 * i + j]);
		}
	}
	for (int i = 0; i < datasize / 4; i++) {
		for (int j = 0; j < 4; j++) {
			data[i] = data[i] * 16 * 16 + (unsigned int)((unsigned char)(databuf[4 * i + j]));
		}
	}
	//number of words to be loadede into I memory
	unsigned int exec_instru = (unsigned int)instruction[1];
	//number of datas to be loaded into D memory
	unsigned int load_data = (unsigned int)data[1];
	trans_opcode(instruction,exec_instru,data,load_data);
	return 0;
}
*/