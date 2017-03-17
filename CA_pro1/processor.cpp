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
	unsigned int cycle[32] = { 0 };
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
	ofstream error_file;
	error_file.open("error_dump.rpt");
	instr = instruction[2];
	unsigned short int opcode = (0xfc000000 & instr) >> 26;
	if (opcode == 0) {
		unsigned short int funct = (0x0000003f & instr);
		if (funct >= 0x20 && funct <= 0x2A && funct != 0x23 && funct != 0x29) {
			int rs, rt, rd;
			rs = (0x03e00000 & instr) >> 21;
			rt = (0x001f0000 & instr) >> 16;
			rd = (0x0000f800 & instr) >> 11;
			switch (funct) {
			case 0x20:

			}
		}
		else if (funct == 0x00 || funct == 0x02 || funct == 0x03) {

		}
		else if (funct == 0x08) {

		}
		else if (funct == 0x18 || funct == 0x19) {

		}
		else if (funct == 0x10 || funct == 0x12) {

		}
		else {
			cout << "invalid function code" << endl;
		}
	}
	else if (opcode == 0x08 || opcode == 0x09 || (opcode <= 0x2B && opcode >= 0x20 && opcode != 0x22 && opcode != 0x26 && opcode != 0x27 && opcode != 0x2A)) {

	}
	else if (opcode == 0x0F) {

	}
	else if (opcode == 0x0C || opcode == 0x0D || opcode == 0x0E || opcode == 0x0A) {

	}
	else if (opcode == 0x04 || opcode == 0x05) {

	}
	else if (opcode == 0x07) {

	}
	else if (opcode == 0x02 || opcode == 0x03) {

	}
	else if (opcode == 0x3F) {

	}
	else {
		cout << "invalid opcode" << endl;
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
	get_operation(instruction, exec_instru, data, load_data);
	return 0;
}