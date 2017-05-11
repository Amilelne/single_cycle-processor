
#include<iostream>
#include<string>
#include<fstream>
#include<stdio.h>
#include<iomanip>
#include<stdint.h>
#include "cmp.h"
const int N = 258;
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
	bool ovwr = false;
	short int HI_flag = 0;//check if HI has changed
	short int LO_flag = 0;//check if LO has changed
	cycle[29] = (unsigned int)data[0];
	unsigned int iniSP = cycle[29];//save the initial value of SP pointer;
	FILE* snapshot = fopen("snapshot.rpt", "w+");
	FILE* error_file = fopen("error_dump.rpt", "w+");
	fprintf(snapshot, "cycle %d\n", cycle_num);
	for (int i = 0; i < 32; i++)
	{
		fprintf(snapshot, "$%02d: 0x%08X\n", i, cycle[i]);
	}
	fprintf(snapshot, "$HI: 0x%08X\n", HI);
	fprintf(snapshot, "$LO: 0x%08X\n", LO);
	fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
	int I_VPA = (PC - iniPC) / 4 + 2;
	instr = instruction[2];
    uint16_t opcode = (0xfc000000 & instr) >> 26;
	while (opcode != 0x3F) {
		++cycle_num;
		if (opcode == 0x3E) {
			fprintf(snapshot, "cycle %d\n", cycle_num);
			fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
		}
		else if (opcode != 0x3E) {
			if (opcode < 0x20 || opcode>0x2B) {
				fprintf(snapshot, "cycle %d\n", cycle_num);
			}
			opcode = (0xfc000000 & instr) >> 26;
			if (opcode == 0) {
				uint16_t funct = (0x0000003f & instr);
				if (funct >= 0x20 && funct <= 0x2A && funct != 0x23 && funct != 0x29) {
					int rs, rt, rd;
					rs = (0x03e00000 & instr) >> 21;
					rt = (0x001f0000 & instr) >> 16;
					rd = (0x0000f800 & instr) >> 11;
					if (rd == 0) {
						fprintf(error_file, "In cycle %d: Write $0 Error\n", cycle_num);
						int tmp = 0;
						int minu = 0;
						switch (funct)
						{
						case 0x20:
							tmp = cycle[rs] + cycle[rt];
							if ((cycle[rs] > 0 && cycle[rt] > 0 && tmp <= 0) || (cycle[rs] < 0 && cycle[rt] < 0 && tmp >= 0)) {
								fprintf(error_file, "In cycle %d: Number Overflow\n", cycle_num);
							}
							break;
						case 0x22:
							minu = -cycle[rt];
							tmp = cycle[rs] + minu;
							if ((cycle[rs] < 0 && minu< 0 && tmp >= 0) || (cycle[rs]>0 && minu>0 && tmp <= 0)) {
								fprintf(error_file, "In cycle %d: Number Overflow\n", cycle_num);
							}
							break;
						default:
							break;
						}
					}
					else {
						int rd_value = cycle[rd];
						int rs_value = cycle[rs];
						int rt_value = cycle[rt];
						int minus_rt = 0;
						switch (funct) {
						case 0x20:
							cycle[rd] = cycle[rs] + cycle[rt];
							if ((rs_value > 0 && rt_value>0 && cycle[rd] <= 0) || (rs_value < 0 && rt_value<0 && cycle[rd] >= 0)) {
								fprintf(error_file, "In cycle %d: Number Overflow\n", cycle_num);
							}
							break;
						case 0x21:
							cycle[rd] = cycle[rs] + cycle[rt];
							break;
						case 0x22:
							minus_rt = -cycle[rt];
							cycle[rd] = cycle[rs] + minus_rt;
							if ((rs_value > 0 && minus_rt > 0 && cycle[rd] <= 0) || (rs_value < 0 && minus_rt < 0 && cycle[rd] >= 0)) {
								fprintf(error_file, "In cycle %d: Number Overflow\n", cycle_num);
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
						if (rd_value != cycle[rd]) {
							fprintf(snapshot, "$%02d: 0x%08X\n", rd, cycle[rd]);
						}
					}
					PC = PC + 4;
					fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
				}
				else if (funct == 0x00 || funct == 0x02 || funct == 0x03) {
					short int rt, rd, shamt;
					rt = (0x001f0000 & instr) >> 16;
					rd = (0x0000f800 & instr) >> 11;
					shamt = (0x000007c0 & instr) >> 6;
					int rd_value = cycle[rd];
					uint32_t tmp = 0;
					if (rd == 0 && rt == 0 && shamt == 0 && funct == 0) {
						//nop
					}
					else {
						if (rd == 0) {
							fprintf(error_file, "In cycle %d: Write $0 Error\n", cycle_num);
						}
						else {
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
							if (rd_value != cycle[rd]) {
								fprintf(snapshot, "$%02d: 0x%08X\n", rd, cycle[rd]);
							}
						}
					}
					PC = PC + 4;
					fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
				}
				else if (funct == 0x08) {
					short int rs;
					rs = (0x03e00000 & instr) >> 21;
					PC = cycle[rs];
					fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
				}
				else if (funct == 0x18 || funct == 0x19) {
					short int rs, rt;
					rs = (0x03e00000 & instr) >> 21;
					rt = (0x001f0000 & instr) >> 16;
					int64_t product = 0;
					uint32_t HI_value = 0;
					uint32_t LO_value = 0;
					if (funct == 0x18) {
						product = (int64_t)(int32_t)cycle[rs] * (int64_t)(int32_t)cycle[rt];
						HI_value = (uint32_t)((product >> 32) & 0xffffffff);
						LO_value = (uint32_t)(0xffffffff & product);
					}
					else {
						uint64_t tmp = 0x00000000ffffffff;
						uint64_t rsval = (uint64_t)(cycle[rs] & tmp);
						uint64_t rtval = (uint64_t)(cycle[rt] & tmp);
						uint64_t temp = (uint64_t)rsval*(uint64_t)rtval;
						HI_value = (uint32_t)((temp >> 32) & 0xffffffff);
						LO_value = (uint32_t)(0xffffffff & temp);
					}
					++HI_flag;
					++LO_flag;
					if (HI != HI_value)
					{
						HI = HI_value;
						fprintf(snapshot, "$HI: 0x%08X\n", HI);
					}
					if (LO != LO_value) {
						LO = LO_value;
						fprintf(snapshot, "$LO: 0x%08X\n", LO);
					}
					if (ovwr) {//print error to file
						fprintf(error_file, "In cycle %d: Overwrite HI-LO registers\n", cycle_num);
					}
					PC = PC + 4;
					fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
					ovwr = true;
				}
				else if (funct == 0x10 || funct == 0x12) {
					ovwr = false;
					short int rd;
					rd = (0x0000f800 & instr) >> 11;
					int rd_value = cycle[rd];
					if (rd == 0) {
						fprintf(error_file, "In cycle %d: Write $0 Error\n", cycle_num);
					}
					else {
						if (funct == 0x10) {
							cycle[rd] = HI;
						}
						else {
							cycle[rd] = LO;
						}
						if (rd_value != cycle[rd]) {
							fprintf(snapshot, "$%02d: 0x%08X\n", rd, cycle[rd]);
						}
					}
					PC = PC + 4;
					fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
				}
				else {
					cout << "invalid function code" << endl;
					break;
				}
			}
			else if (opcode == 0x08 || opcode == 0x09) {
				int16_t rs, rt, immediate;
				rs = (0x03e00000 & instr) >> 21;
				rt = (0x001f0000 & instr) >> 16;
				int32_t rt_value = cycle[rt];
				int32_t rs_value = cycle[rs];
				immediate = (0x0000ffff & instr);
				if (rt == 0) {
					fprintf(error_file, "In cycle %d: Write $0 Error\n", cycle_num);
					if (opcode == 0x08) {
						int32_t tmp = 0;
						tmp = cycle[rs] + immediate;
						if ((cycle[rs] < 0 && immediate < 0 && tmp >= 0) || (cycle[rs] > 0 && immediate > 0 && tmp <= 0)) {
							fprintf(error_file, "In cycle %d: Number Overflow\n", cycle_num);
						}
					}
				}
				else {
					if (opcode == 0x08) {
						cycle[rt] = cycle[rs] + immediate;
						if ((rs_value > 0 && immediate > 0 && cycle[rt] <= 0) || (rs_value < 0 && immediate < 0 && cycle[rt] >= 0)) {
							fprintf(error_file, "In cycle %d: Number Overflow\n", cycle_num);
						}
					}
					else {
						cycle[rt] = cycle[rs] + immediate;
					}
					if (rt_value != cycle[rt]) {
						fprintf(snapshot, "$%02d: 0x%08X\n", rt, cycle[rt]);
					}
				}
				PC = PC + 4;
				fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
			}
			else if (opcode <= 0x25 && opcode >= 0x20 && opcode != 0x22) {
				int16_t rs, rt, immediate;
				rs = (0x03e00000 & instr) >> 21;
				rt = (0x001f0000 & instr) >> 16;
				int32_t rt_value = cycle[rt];
				immediate = (0x0000ffff & instr);
				bool halt_flag = false;
				if (rt == 0) {
					fprintf(error_file, "In cycle %d: Write $0 Error\n", cycle_num);
				}
				if ((cycle[rs] > 0 && immediate > 0 && cycle[rs] + immediate <= 0) || (cycle[rs] < 0 && immediate < 0 && cycle[rs] + immediate >= 0)) {
					fprintf(error_file, "In cycle %d: Number Overflow\n", cycle_num);
				}
				if (opcode == 0x23) {
					if ((uint32_t)(immediate + cycle[rs]) > 1020) {
						fprintf(error_file, "In cycle %d: Address Overflow\n", cycle_num);
						halt_flag = true;
					}
				}
				else if (opcode == 0x21 || opcode == 0x25) {
					if ((uint32_t)(immediate + cycle[rs]) > 1022) {
						fprintf(error_file, "In cycle %d: Address Overflow\n", cycle_num);
						halt_flag = true;
					}
				}
				else if (opcode == 0x20 || opcode == 0x24) {
					if ((uint32_t)(immediate + cycle[rs]) > 1023) {
						fprintf(error_file, "In cycle %d: Address Overflow\n", cycle_num);
						halt_flag = true;
					}
				}
				if (opcode == 0x21 || opcode == 0x25) {
					if ((((uint32_t)(immediate + cycle[rs])) % 4) == 1 || (((uint32_t)(immediate + cycle[rs])) % 4) == 3) {
						fprintf(error_file, "In cycle %d: Misalignment Error\n", cycle_num);
						halt_flag = true;
					}
				}
				else if (opcode == 0x23) {
					if ((immediate + cycle[rs]) % 4 != 0) {
						fprintf(error_file, "In cycle %d: Misalignment Error\n", cycle_num);
						halt_flag = true;
					}
				}
				if (!halt_flag) {
					fprintf(snapshot, "cycle %d\n", cycle_num);
					if (rt != 0) {
						if (opcode == 0x20) {
							int pos = (immediate + cycle[rs]) / 4 + 2;
							int pos1 = immediate + cycle[rs];
							if (pos1 % 4 == 0)
								cycle[rt] = (int64_t)((int8_t)(data[pos] >> 24));
							else if (pos1 % 4 == 1)
								cycle[rt] = (int64_t)(int8_t)(((data[pos] & 0x00ff0000) >> 16));
							else if (pos1 % 4 == 2)
								cycle[rt] = (int64_t)((int8_t)((data[pos] & 0x0000ff00) >> 8));
							else
								cycle[rt] = (int64_t)((int8_t)(data[pos] & 0x000000ff));
						}
						else if (opcode == 0x21) {
							int pos = (immediate + cycle[rs]) / 4 + 2;
							int pos1 = immediate + cycle[rs];
							if (pos1 % 4 == 0) {
								cycle[rt] = (int32_t)((int16_t)((data[pos] & 0xffff0000) >> 16));
							}
							else if (pos1 % 4 == 2) {
								cycle[rt] = (int32_t)((int16_t)((data[pos] & 0x0000ffff)));
							}
						}
						else if (opcode == 0x23) {
							cycle[rt] = data[2 + (immediate + cycle[rs]) / 4];
						}
						else if (opcode == 0x24) {
							int pos = (immediate + cycle[rs]) / 4 + 2;
							int pos1 = immediate + cycle[rs];
							if (pos1 % 4 == 0)
								cycle[rt] = (unsigned int)((data[pos] & 0xff000000) >> 24);
							else if (pos1 % 4 == 1)
								cycle[rt] = (unsigned int)((data[pos] & 0x00ff0000) >> 16);
							else if (pos1 % 4 == 2)
								cycle[rt] = (unsigned int)((data[pos] & 0x0000ff00) >> 8);
							else
								cycle[rt] = (unsigned int)(data[pos] & 0x000000ff);
						}
						else if (opcode == 0x25) {
							int pos = immediate / 4 + 2;
							int pos1 = immediate + cycle[rs];
							if (pos1 % 4 == 0) {
								cycle[rt] = (uint32_t)((uint16_t)((data[(immediate + cycle[rs]) / 4 + 2] & 0xffff0000) >> 16));
							}
							else if (pos1 % 4 == 2) {
								cycle[rt] = (uint32_t)((uint16_t)((data[(immediate + cycle[rs]) / 4 + 2] & 0x0000ffff)));
							}
						}
					}
				}
				if (halt_flag)//if address error,halt
					break;
				else {
					if (cycle[rt] != rt_value) {
						fprintf(snapshot, "$%02d: 0x%08X\n", rt, cycle[rt]);
					}
					PC = PC + 4;
					fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
				}
			}
			else if (opcode == 0x28 || opcode == 0x29 || opcode == 0x2B) {
				int16_t rs, rt, immediate;
				rs = (0x03e00000 & instr) >> 21;
				rt = (0x001f0000 & instr) >> 16;
				int32_t rt_value = cycle[rt];
				immediate = (0x0000ffff & instr);
				bool halt_flag = false;
				if ((cycle[rs] > 0 && immediate > 0 && cycle[rs] + immediate <= 0) || (cycle[rs] < 0 && immediate < 0 && cycle[rs] + immediate >= 0)) {
					fprintf(error_file, "In cycle %d: Number Overflow\n", cycle_num);
				}
				if (opcode == 0x28) {
					if ((uint32_t)(immediate + cycle[rs])  > 1023) {
						fprintf(error_file, "In cycle %d: Address Overflow\n", cycle_num);
						halt_flag = true;
					}
				}
				else if (opcode == 0x29) {
					if ((uint32_t)(immediate + cycle[rs])  > 1022) {
						fprintf(error_file, "In cycle %d: Address Overflow\n", cycle_num);
						halt_flag = true;
					}
				}
				else if (opcode == 0x2B) {
					if ((uint32_t)(immediate + cycle[rs])  > 1020) {
						fprintf(error_file, "In cycle %d: Address Overflow\n", cycle_num);
						halt_flag = true;
					}
				}
				if (opcode == 0x29) {
					if ((immediate + cycle[rs]) % 4 == 1 || (immediate + cycle[rs]) % 4 == 3) {
						fprintf(error_file, "In cycle %d: Misalignment Error\n", cycle_num);
						halt_flag = true;
					}
				}
				else if (opcode == 0x2B) {
					if ((immediate + cycle[rs]) % 4 != 0) {
						fprintf(error_file, "In cycle %d: Misalignment Error\n", cycle_num);
						halt_flag = true;
					}
				}
				if (!halt_flag) {
					fprintf(snapshot, "cycle %d\n", cycle_num);
					if (opcode == 0x28) {
						int pos = (immediate + cycle[rs]) / 4 + 2;
						int pos1 = immediate + cycle[rs];
						if (pos1 % 4 == 0)
							data[pos] = (data[pos] & 0xffffff) | ((cycle[rt] & 0xff) << 24);
						else if (pos1 % 4 == 1)
							data[pos] = (data[pos] & 0xff00ffff) | ((cycle[rt] & 0xff) << 16);
						else if (pos1 % 4 == 2)
							data[pos] = (data[pos] & 0xffff00ff) | ((cycle[rt] & 0xff) << 8);
						else
							data[pos] = (data[pos] & 0xffffff00) | (cycle[rt] & 0xff);
					}
					else if (opcode == 0x29) {
						int pos = (immediate + cycle[rs]) / 4 + 2;
						int pos1 = immediate + cycle[rs];
						if (pos1 % 4 == 0) {
							data[pos] = (data[pos] & 0x0000ffff) | ((cycle[rt] & 0xffff) << 16);
						}
						else if (pos1 % 4 == 2) {
							data[pos] = (data[pos] & 0xffff0000) | (cycle[rt] & 0xffff);
						}
					}
					else if (opcode == 0x2B) {
						data[(immediate + cycle[rs]) / 4 + 2] = cycle[rt];
					}
				}
				if (halt_flag)
					break;
				else {
					PC = PC + 4;
					fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
				}
			}
			else if (opcode == 0x0F) {
				int16_t rt, immediate;
				rt = (0x001f0000 & instr) >> 16;
				int32_t rt_value = cycle[rt];
				immediate = (0x0000ffff & instr);
				if (rt == 0) {
					fprintf(error_file, "In cycle %d: Write $0 Error\n", cycle_num);
				}
				else {
					cycle[rt] = ((uint32_t)((uint16_t)immediate)) << 16;
					if (cycle[rt] != rt_value) {
						fprintf(snapshot, "$%02d: 0x%08X\n", rt, cycle[rt]);
					}
				}
				PC = PC + 4;
				fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
			}
			else if (opcode == 0x0C || opcode == 0x0D || opcode == 0x0E || opcode == 0x0A) {
				int16_t rs, rt, immediate;
				rs = (0x03e00000 & instr) >> 21;
				rt = (0x001f0000 & instr) >> 16;
				int32_t rt_value = cycle[rt];
				immediate = (0x0000ffff & instr);
				if (rt == 0) {
					fprintf(error_file, "In cycle %d: Write $0 Error\n", cycle_num);
				}
				else {
					if (opcode == 0x0C) {
						cycle[rt] = cycle[rs] & (uint16_t)immediate;
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
					if (cycle[rt] != rt_value) {
						fprintf(snapshot, "$%02d: 0x%08X\n", rt, cycle[rt]);
					}
				}
				PC = PC + 4;
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
					if (cycle[rs] != cycle[rt])
						PC = PC + 4 + 4 * immediate;
					else
						PC = PC + 4;
				}
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
					if (ra_value != cycle[31]) {
						fprintf(snapshot, "$%02d: 0x%08X\n", 31, cycle[31]);
					}
				}
				fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
			}
			else if (opcode == 0x3F) {
				break;
			}
			else {
				cout << "invalid opcode" << endl;
				break;
			}
		}
		if (PC < iniPC) {
			PC = PC + 4;
			opcode = 0x3E;

		}
		else {
			instr = instruction[(PC - iniPC) / 4 + 2];
			opcode = (0xfc000000 & instr) >> 26;
		}
	}
	fclose(snapshot);
	fclose(error_file);
}
int main(int argc,char *argv[])
{
	int argument[10] = { 64,32,8,16,16,4,4,16,4,1 };
	int I_mem_w = 16;
	int D_mem_w = 8;
	int I_PTE_entries = 1024 / 8;
	int D_PTE_entries = 1024 / 16;
	char* buffer;
	char* databuf;
	char iimage[] = "iimage.bin";
	char dimage[] = "dimage.bin";
	long size;
	unsigned int instruction[N] = { 0 };
	unsigned int data[N] = { 0 };
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
	databuf = new char[datasize];
	datafile.read(databuf, datasize);
	datafile.close();
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4; j++) {
			instruction[i] = instruction[i] * 16 * 16 + (unsigned int)((unsigned char)buffer[4 * i + j]);
		}
	}
	for (int i = 0; i < instruction[1] + 2; i++) {
		for (int j = 0; j < 4; j++) {
			instruction[i] = instruction[i] * 16 * 16 + (unsigned int)((unsigned char)buffer[4 * i + j]);
		}
	}
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4; j++) {
			data[i] = data[i] * 16 * 16 + (unsigned int)((unsigned char)(databuf[4 * i + j]));
		}
	}
	for (int i = 2; i < data[1] + 2; i++) {
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
