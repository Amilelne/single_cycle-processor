
#include<iostream>
#include<string>
#include<fstream>
#include<stdio.h>
#include<iomanip>
#include<stdint.h>
const int N = 1000;
using namespace std;
void get_operation(unsigned int instruction[], unsigned int execu_instru, unsigned int data[], unsigned int load_data) {
	unsigned int instr = instruction[0];
    int cycle[32] = { 0 };
	unsigned int HI = 0;
	unsigned int LO = 0;
	unsigned int PC = (unsigned int)instr;
	unsigned int iniPC = PC;//save the initial value of PC
	int cycle_num = 0;//count how many cycle have run
	int stack[100] = { 0 };
	short int HI_flag = 0;//check if HI has changed
	short int LO_flag = 0;//check if LO has changed
	cycle[29] = (unsigned int)data[0];
	unsigned int iniSP = cycle[29];//save the initial value of SP pointer;
	FILE* snapshot = fopen("snapshot.rpt", "w+");
	FILE* error_file = fopen("error_dump.rpt", "w+");
	printf("cycle %d\n", cycle_num);
	fprintf(snapshot,"cycle %d\n", cycle_num);
	for (int i = 0; i < 32; i++)
	{
		printf("$%02d: 0x%08X\n", i, cycle[i]);
		fprintf(snapshot, "$%02d: 0x%08X\n", i, cycle[i]);
	}
	printf( "HI: 0x%08X\n", HI);
	printf( "LO: 0x%08X\n", LO);
	printf( "PC: 0x%08X\n\n", PC);
	fprintf(snapshot, "HI: 0x%08X\n", HI);
	fprintf(snapshot, "LO: 0x%08X\n", LO);
	fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
	instr = instruction[2];
	unsigned short int opcode = (0xfc000000 & instr) >> 26;
	while (opcode != 0x3F) {
		++cycle_num;
		printf("cycle %d\n", cycle_num);
		fprintf(snapshot,"cycle %d\n", cycle_num);
		opcode = (0xfc000000 & instr) >> 26;
		if (opcode == 0) {
			unsigned short int funct = (0x0000003f & instr);
			if (funct >= 0x20 && funct <= 0x2A && funct != 0x23 && funct != 0x29) {
				int rs, rt, rd;
				rs = (0x03e00000 & instr) >> 21;
				rt = (0x001f0000 & instr) >> 16;
				rd = (0x0000f800 & instr) >> 11;
				if (rd == 0) {
					fprintf(error_file, "In cycle %d:Write $0 Error\n", cycle_num);
					printf("In cycle %d:Write $0 Error\n", cycle_num);
				}
				else {
					int rd_value = cycle[rd];
					int rs_value = cycle[rs];
					int rt_value = cycle[rt];
					switch (funct) {
					case 0x20:
						cycle[rd] = cycle[rs] + cycle[rt];
						if ((rs_value > 0 && rt_value>0 && cycle[rd] <= 0) || (rs_value < 0 && rt_value<0 && cycle[rd] >= 0)) {
							fprintf(error_file, "In cycle %d:Number Overflow\n", cycle_num);
							printf("In cycle %d:Number Overflow\n", cycle_num);
						}
						break;
					case 0x21:
						cycle[rd] = cycle[rs] + cycle[rt];
						break;
					case 0x22:
						cycle[rd] = cycle[rs] - cycle[rt];
						if ((cycle[rs] > 0 && cycle[rt] < 0&&cycle[rd] <=0) || (cycle[rs] < 0&& cycle[rt]>0&&cycle[rd] >=0)) {
							printf( "In cycle %d:Number Overflow\n", cycle_num);
							fprintf(error_file, "In cycle %d:Number Overflow\n", cycle_num);
						}
						break;
					case 0x24:
						cycle[rd] = cycle[rs] & cycle[rt];
						break;
					case 0x25:
						cycle[rd] = cycle[rs] | cycle[rt];
						break;
					case 0x26:
						cycle[rd] = cycle[rs] ^ cycle[rt];
						break;
					case 0x27:
						cycle[rd] = ~(cycle[rs] | cycle[rt]);
						break;
					case 0x28:
						cycle[rd] = ~(cycle[rs] & cycle[rt]);
						break;
					case 0x2A:
						cycle[rd] = (cycle[rs] < cycle[rt]);
					}
					if (rd_value != cycle[rd])
						printf_s("$%02d: 0x%08X\n", rd, cycle[rd]);
						fprintf(snapshot, "$%02d: 0x%08X\n", rd, cycle[rd]);
				}
				PC = PC + 4;
				fprintf(snapshot, "PC: 0x%08X\n\n", PC);
				printf("PC: 0x%08X\n\n", PC);
			}
			else if (funct == 0x00 || funct == 0x02 || funct == 0x03) {
				short int rt, rd, shamt;
				rt = (0x001f0000 & instr) >> 16;
				rd = (0x0000f800 & instr) >> 11;
				shamt = (0x000007c0 & instr) >> 6;
				int rd_value = cycle[rd];
				uint32_t tmp = 0;
				switch (funct)
				{
				case 0x00:
					cycle[rd] = cycle[rt] << shamt;
					break;
				case 0x02:
				    tmp = (uint32_t)cycle[rt];
					cycle[rd] = tmp >> shamt;
					break;
				case 0x03:
					cycle[rd] = cycle[rt] >> shamt;
					break;
				default:
					break;
				}
				if (rd_value != cycle[rd])
					printf("$%02d: 0x%08X\n", rd, cycle[rd]);
					fprintf(snapshot, "$%02d: 0x%08X\n", rd, cycle[rd]);
				PC = PC + 4;
				fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
				printf("PC: 0x%08X\n\n\n", PC);
			}
			else if (funct == 0x08) {
				short int rs;
				rs = (0x03e00000 & instr) >> 21;
				PC = cycle[rs];
				printf( "PC: 0x%08X\n\n\n", PC);
				fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
			}
			else if (funct == 0x18 || funct == 0x19) {
				short int rs, rt;
				rs = (0x03e00000 & instr) >> 21;
				rt = (0x001f0000 & instr) >> 16;
				uint64_t product = 0;
				if (funct == 0x18) {
					product = (uint64_t)((cycle[rs] * cycle[rt]) & 0xffffffffffffffff);
				}
				else {
					uint64_t tmp = 0x00000000ffffffff;
					uint64_t rsval = (uint64_t)(cycle[rs] & tmp);
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
					fprintf(snapshot,"HI: 0x%08X\n", HI);
				}
				if (LO != LO_value) {
					LO = LO_value;
					++LO_flag;
					printf("LO: 0x%08X\n", LO);
					fprintf(snapshot,"LO: 0x%08X\n", LO);
				}
				if (HI_flag>1 || LO_flag>1) {//print error to file
					printf("In cycle %d:Overwrite HI-LO registers\n", cycle_num);
					fprintf(error_file, "In cycle %d:Overwrite HI-LO registers\n", cycle_num);
				}
				PC = PC + 4;
				printf("PC: 0x%08X\n\n", PC);
				fprintf(snapshot,"PC: 0x%08X\n\n\n", PC);
			}
			else if (funct == 0x10 || funct == 0x12) {
				short int rd;
				rd = (0x0000f800 & instr) >> 11;
				if (rd == 0) {
					printf("In cycle %d:Write $0 Error\n", cycle_num);
					fprintf(error_file, "In cycle %d:Write $0 Error\n", cycle_num);
				}
				else {
					if (funct == 0x10) {
						cycle[rd] = HI;
						printf("$%02d: 0x%08X\n", rd, cycle[rd]);
						fprintf(snapshot, "$%02d: 0x%08X\n", rd, cycle[rd]);
					}
					else {
						cycle[rd] = LO;
						printf("$%02d: 0x%08X\n", rd, cycle[rd]);
						fprintf(snapshot,"$%02d: 0x%08X\n", rd, cycle[rd]);
					}
				}
				PC = PC + 4;
				printf("PC: 0x%08X\n", PC);
				fprintf(snapshot,"PC: 0x%08X\n\n\n", PC);
			}
			else {
				cout << "invalid function code" << endl;
			}
		}
		else if (opcode == 0x08 || opcode == 0x09) {
			int16_t rs, rt, immediate;
			rs = (0x03e00000 & instr) >> 21;
			rt = (0x001f0000 & instr) >> 16;
			int32_t rt_value = cycle[rt];
			immediate = (0x0000ffff & instr);
			if (rt == 0) {
				printf("In cycle %d:Write $0 Error\n", cycle_num);
				fprintf(error_file, "In cycle %d:Write $0 Error\n", cycle_num);
			}
			else {
				if (opcode == 0x08) {
					int32_t tmp = 0;
					if (immediate < 0) {
						tmp = (0xffff0000 | immediate);
					}
					else {
						tmp = (int32_t)immediate;
					}
					cycle[rt] = cycle[rs] + tmp;
					if ((cycle[rs]>0 && tmp>0&&cycle[rt]<=0) || (cycle[rs]<0&& tmp<0&& cycle[rt]>=0))
						printf("In cycle %d:Number Overflow\n", cycle_num);
						fprintf(error_file, "In cycle %d:Number Overflow\n", cycle_num);
				}
				else {
					cycle[rt] = cycle[rs] + (int32_t)immediate;
				}
				if (rt_value != cycle[rt]) 
					printf("$%02d: 0x%08X\n", rt, cycle[rt]);
					fprintf(snapshot, "$%02d: 0x%08X\n", rt, cycle[rt]);
			}
			PC = PC + 4;
			printf("PC: 0x%08X\n\n\n", PC);
			fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
		}
		else if (opcode <= 0x25 && opcode >= 0x20 && opcode != 0x22) {
			int16_t rs, rt, immediate;
			rs = (0x03e00000 & instr) >> 21;
			rt = (0x001f0000 & instr) >> 16;
			int32_t rt_value = cycle[rt];
			immediate = (0x0000ffff & instr);
			bool halt_flag = false;
			if (immediate / 4 > load_data + 2) {
				printf("In cycle %d: Address Overflow\n", cycle_num);
				halt_flag = true;
			}
			if (opcode == 0x20) {
				if (rs == 0) {
					int pos = immediate / 4 + 2;
					if (immediate % 4 == 0)
						cycle[rt] = (int64_t)((int8_t)(data[pos] >> 24));
					else if (immediate % 4 == 1)
						cycle[rt] = (int64_t)(int8_t)(((data[pos] & 0x00ff0000) >> 16));
					else if (immediate % 4 == 2)
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
			}
			else if (opcode == 0x21) {
				if (rs == 0) {
					int pos = immediate / 4 + 2;
					if (immediate % 4 == 0) {
						cycle[rt] = (int32_t)((int16_t)((data[immediate / 4 + 2] & 0xffff0000) >> 16));
					}
					else if (immediate % 4 == 2) {
						cycle[rt] = (int32_t)((int16_t)((data[immediate / 4 + 2] & 0x0000ffff)));
					}
					else {
						printf("In cycle %d:Misalignment Error\n", cycle);
						halt_flag = true;
					}
				}
				else {
					int pos = immediate / 4 + 2;
					if (immediate % 4 == 0) {
						cycle[rt] = (int32_t)((int16_t)((stack[immediate / 4 + (iniSP - cycle[29]) / 4 + 1] & 0xffff0000) >> 16));
					}
					else if (immediate % 4 == 2) {
						cycle[rt] = (int32_t)((int16_t)((stack[immediate / 4 + (iniSP - cycle[29]) / 4 + 1] & 0x0000ffff)));
					}
					else {
						printf("In cycle %d:Misalignment Error\n", cycle);
						halt_flag = true;
					}
				}
			}
			else if (opcode == 0x23) {
				if (immediate % 4 != 0) {
					printf("In cycle %d:Misalignment Error\n", cycle);
					halt_flag = true;
				}
				else {
					if (rs != 0) {
						cycle[rt] = stack[immediate / 4 + (iniSP - cycle[29]) / 4 - 1];
					}
					else {
						cycle[rt] = data[2 + immediate / 4];
					}
				}	
			}
			else if (opcode == 0x24) {
				if (rs == 0) {
					int pos = immediate / 4 + 2;
					if (immediate % 4 == 0)
						cycle[rt] = (unsigned int)((data[pos] & 0xff000000) >> 24);
					else if (immediate % 4 == 1)
						cycle[rt] = (unsigned int)((data[pos] & 0x00ff0000) >> 16);
					else if (immediate % 4 == 2)
						cycle[rt] = (unsigned int)((data[pos] & 0x0000ff00) >> 8);
					else
						cycle[rt] = (unsigned int)(data[pos] & 0x000000ff);
				}
			}
			else if (opcode == 0x25) {
				if (rs == 0) {
					int pos = immediate / 4 + 2;
					if (immediate % 4 == 0) {
						cycle[rt] = (uint32_t)((uint16_t)((data[immediate / 4 + 2] & 0xffff0000) >> 16));
					}
					else if (immediate % 4 == 2) {
						cycle[rt] = (uint32_t)((uint16_t)((data[immediate / 4 + 2] & 0x0000ffff) ));
					}
					else {
						printf("In cycle %d:Misalignment Error\n", cycle);
						halt_flag = true;
					}
				}
				else {
					int pos = immediate / 4 + 2;
					if (immediate % 4 == 0) {
						cycle[rt] = (uint32_t)((uint16_t)((stack[immediate / 4 + (iniSP - cycle[29]) / 4 + 1] & 0xffff0000) >> 16));
					}
					else if (immediate % 4 == 2) {
						cycle[rt] = (uint32_t)((uint16_t)((stack[immediate / 4 + (iniSP - cycle[29]) / 4 + 1] & 0x0000ffff)));

					}
					else {
						printf("In cycle %d:Misalignment Error\n", cycle);
						halt_flag = true;
					}
				}
			}
			if (halt_flag)//if address error,halt
				break;
			else {
				if (cycle[rt] != rt_value)
					printf("$%02d: 0x%08X\n", rt, cycle[rt]);
				fprintf(snapshot,"$%02d: 0x%08X\n", rt, cycle[rt]);
				PC = PC + 4;
				printf("PC: 0x%08X\n\n", PC);
				fprintf(snapshot,"PC: 0x%08X\n\n\n", PC);
			}
		}
		else if (opcode == 0x28 || opcode == 0x29 || opcode == 0x2B) {
			int16_t rs, rt, immediate;
			rs = (0x03e00000 & instr) >> 21;
			rt = (0x001f0000 & instr) >> 16;
			int32_t rt_value = cycle[rt];
			immediate = (0x0000ffff & instr);
			bool halt_flag = false;
			if (rs == 0) {
				if (immediate / 4 >= execu_instru) {
					printf("In cycle %d:Address Overflow\n", cycle_num);
					halt_flag = true;
				}
			}
			if (opcode == 0x28) {
				if (rs == 0) {
					int pos = immediate / 4 + 2;
					if (immediate % 4 == 0)
						data[pos] = (data[pos] & 0xffffff) | ((cycle[rt] & 0xff) << 24);
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
			}
			else if (opcode == 0x29) {
				if (rs == 0) {
					int pos = immediate / 4 + 2;
					if (immediate % 2 == 0) {
						data[pos] = (data[pos] & 0xffff) | (cycle[rt] << 16);
					}
					else {
						printf("In cycle %d:Misalignment Error\n", cycle_num);
						halt_flag = true;
					}
				}
				else {
					int pos = immediate / 4 + 1 + (iniSP - cycle[29]) / 4;
					if (immediate % 2 == 0) {
						stack[pos] = (stack[pos] & 0xffff) | (cycle[rt] << 16);
					}
					else {
						printf("In cycle %d:Misalignment Error\n", cycle_num);
						halt_flag = true;
					}
				}
			}
			else if (opcode == 0x2B) {
				if (immediate % 4 != 0) {
					printf("In cycle %d:Misalignment Error\n", cycle_num);
					halt_flag = true;
				}
				else {
					if (rs != 0) {
						//save ra in stack
						stack[immediate / 4 + (iniSP - cycle[29]) / 4 - 1] = cycle[rt];
					}
					else {
						data[immediate / 4 + 2] = cycle[rt];
					}
				}
			}
			if (halt_flag)
				break;
			else {
				PC = PC + 4;
				printf("PC: 0x%08X\n\n\n", PC);
				fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
			}
		}
		else if (opcode == 0x0F) {
			int16_t rt, immediate;
			rt = (0x001f0000 & instr) >> 16;
			int32_t rt_value = cycle[rt];
			immediate = (0x0000ffff & instr);
			if (rt == 0) {
				printf("In cycle %d:Write $0 Error\n", cycle_num);
				fprintf(error_file, "In cycle %d:Write $0 Error\n", cycle_num);
			}
			else {
				cycle[rt] = ((uint32_t)((uint16_t)immediate)) << 16;
				if (cycle[rt] != rt_value)
					printf("$%02d: 0x%08X\n", rt, cycle[rt]);
					fprintf(snapshot, "$%02d: 0x%08X\n", rt, cycle[rt]);
			}
			PC = PC + 4;
			printf("PC: 0x%08X\n\n\n", PC);
			fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
		}
		else if (opcode == 0x0C || opcode == 0x0D || opcode == 0x0E || opcode == 0x0A) {
			int16_t rs, rt, immediate;
			rs = (0x03e00000 & instr) >> 21;
			rt = (0x001f0000 & instr) >> 16;
			int32_t rt_value = cycle[rt];
			immediate = (0x0000ffff & instr);
			if (rt == 0) {
				printf("In cycle %d:Write $0 Error\n", cycle_num);
				fprintf(error_file, "In cycle %d:Write $0 Error\n", cycle_num);
			}
			else {
				if (opcode == 0x0C) {
					cycle[rt] = cycle[rs] & immediate;
				}
				else if (opcode == 0x0D) {
					cycle[rt] = cycle[rs] | (uint16_t)immediate;
				}
				else if (opcode == 0x0E) {
					cycle[rt] = ~(cycle[rs] | (uint16_t)immediate);
				}
				else if (opcode == 0x0A)
				{
					cycle[rt] = (cycle[rs] <(int16_t)immediate);
				}
				if (cycle[rt] != rt_value)
					printf( "$%02d: 0x%08X\n\n\n", rt, cycle[rt]);
					fprintf(snapshot, "$%02d: 0x%08X\n\n\n", rt, cycle[rt]);
			}
			PC = PC + 4;
			printf("PC: 0x%08X\n\n\n", PC);
			fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
		}
		else if (opcode == 0x04 || opcode == 0x05) {
			int16_t rs, rt, immediate;
			rs = (0x03e00000 & instr) >> 21;
			rt = (0x001f0000 & instr) >> 16;
			int32_t rt_value = cycle[rt];
			immediate = (0x0000ffff & instr);
			if (opcode == 0x04) {
				if (cycle[rs] == cycle[rt])
					PC = PC + 4 + 4 * immediate;
				else
					PC = PC + 4;
			}
			else if (opcode == 0x05) {
				if(cycle[rs] != cycle[rt])
					PC = PC + 4 + 4 * immediate;
			    else
				    PC = PC + 4;
			}
			printf("PC: 0x%08X\n\n\n", PC);
			fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
		}
		else if (opcode == 0x07) {
			int16_t rs, immediate;
			rs = (0x03e00000 & instr) >> 21;
			immediate = (0x0000ffff & instr);
			if (cycle[rs] > 0)
				PC = PC + 4 + 4 * immediate;
			else
				PC = PC + 4;
			printf("PC: 0x%08X\n\n\n", PC);
			fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
		}
		else if (opcode == 0x02 || opcode == 0x03) {
			int32_t address;
			address = (0x03ffffff & instr);
			if (opcode == 0x02) {
				PC = (((PC + 4) >> 28) << 28) + 4 * address;
			}
			else if (opcode == 0x03) {
				int ra_value = cycle[31];
				cycle[31] = PC + 4;
				PC = (((PC + 4) >> 28) << 28) + 4 * address;
				if (ra_value != cycle[31])
					printf("$%02d: 0x%08X\n", 31, cycle[31]);
					fprintf(snapshot,"$%02d: 0x%08X\n", 31, cycle[31]);
			}
			printf("PC: 0x%08X\n\n\n", PC);
			fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
		}
		else if (opcode == 0x3F) {
			break;
		}
		else {
			cout << "invalid opcode" << endl;
		}
		instr = instruction[(PC - iniPC) / 4 + 2];
		opcode = (0xfc000000 & instr) >> 26;
	}
	fclose(snapshot);
	fclose(error_file);
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
	get_operation(instruction, exec_instru, data, load_data);
	return 0;
}