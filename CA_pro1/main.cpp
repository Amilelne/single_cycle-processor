#include "regfile.h"
#include "exeinstr.h"
#include<stdint.h>
#include<fstream>
#include<iostream>
using namespace std;
const int N = 258;
int main(int argc,char * argv[]) {
	char* buffer;
	char* databuf;
	char iimage[] = "iimage.bin";
	char dimage[] = "dimage.bin";
	long size;
	int instruction[N] = { 0 };
	int data[N] = { 0 };
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

	int argument[10] = { 64,32,8,16,16,4,4,16,4,1 };
	int I_mem_w = 16;
	int D_mem_w = 8;
	int I_PTE_entries = 1024 / 8;
	int D_PTE_entries = 1024 / 16;
	int I_VPA = 0;
	FILE* snapshot = fopen("snapshot.rpt", "w+");
	FILE* error_file = fopen("error_dump.rpt", "w+");
	regfile reg;
	exe exeinstr;
	int cycle_num = 0;
	int iniPC = instruction[0];
	reg.reg[29] = data[0];
	reg.PC = instruction[0];
	fprintf(snapshot, "cycle %d\n", cycle_num);
	reg.showall(snapshot);
	int instr = instruction[2];
	int halt = 0;
	while (halt!=1) {
		halt = exeinstr.process(instr, data, reg, snapshot);

		cycle_num++;
		fprintf(snapshot, "cycle %d\n", cycle_num);
	}
}