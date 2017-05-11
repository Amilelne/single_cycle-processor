/*
#include "regfile.h"
#include "exeinstr.h"
#include "cmp.h"
#include<stdint.h>
#include<fstream>
#include<iostream>
#include<vector>
using namespace std;
const int N = 258;
int process_ipte(int cycle,int I_VPN,int I_PPN_SIZE,I_PTE ipte[], I_TLB itlb[],I_cache icache[],int ipte_entry,int icache_entry) {
	int hit = 0;
	bool pte_fresh = false;
	if (ipte[I_VPN].valid) {
		hit = 1;
		ipte[I_VPN].num = cycle;
	}
	else {
		int min_num = ipte[0].num;
		int min_num_pos = 0;
		for (int i = 1; i < ipte_entry; i++) {
			if (ipte[i].num < min_num) {
				min_num = ipte[i].num;
				min_num_pos = i;
			}
		}
		ipte[min_num_pos].valid = false;
		ipte[I_VPN].valid = true;
		ipte[I_VPN].num = cycle;
		ipte[I_VPN].PPN = min_num_pos;
	}
	return hit;
}
int process_itlb(int cycle,int I_VPN,I_TLB ITLB[],int I_TLB_entries) {
	int tlb_hit = 0;
	int min_num = ITLB[0].num;
	int min_num_pos = 0;
	for (int i = 0; i < I_TLB_entries; i++) {
		if (ITLB[i].tag == I_VPN) {
			ITLB[i].num = cycle;
			tlb_hit = 1;
			break;
		}
		if (ITLB[i].num < min_num) {
			min_num = ITLB[i].num;
			min_num_pos = i;
		}
	}
	if (tlb_hit == 0) {
		ITLB[min_num_pos].num = cycle;
		ITLB[min_num_pos].tag = I_VPN;
	}
	return tlb_hit;
}
int process_icache(int I_page_size,int I_VPA,int I_block_size,int I_cache_index,int I_cache_entries,int I_cache_way,int I_PPN,I_cache Icache[]) {
	int offset = I_page_size / 4;
	int pAddr = I_VPA % (offset)+I_PPN*offset;
	int cache_offset = I_block_size / 4;
	int index = (pAddr / cache_offset) % I_cache_index;
	int tag = (pAddr / cache_offset) / I_cache_index;
	int row_size = I_block_size*I_cache_way;
	int hit = 0;
	int MRU = 1;
	int MRU_pos = -1;
	int MRU_one_num = 0;
	for (int i = index*row_size; i < (index + 1)*row_size; i++) {
		if (Icache[i].tag == tag&&Icache[i].valid) {
			hit = 1;
			break;
		}
		if (Icache[i].MRU < MRU) {
			Icache[i].MRU = MRU;
			MRU_pos = i;
		}
		if (Icache[i].MRU == 1)
			++MRU_one_num;
	}
	if (hit == 0) {
		if (MRU_one_num == row_size-1) {
			for (int i = index*row_size; i < (index + 1)*row_size; i++)
				Icache[i].MRU = 0;
			Icache[MRU_pos].tag = tag;
			Icache[MRU_pos].valid = true;
			Icache[MRU_pos].MRU = 1;
		}
		else {
			Icache[MRU_pos].tag = tag;
			Icache[MRU_pos].valid = true;
			Icache[MRU_pos].MRU = 1;
		}
	}
	return hit;
}
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
	int I_PTE_entries = 1024 / argument[2];
	int I_TLB_entries = I_PTE_entries / 4;
	int D_PTE_entries = 1024 / argument[3];
	int I_cache_entries = argument[4] / argument[5];
	int I_cache_index = I_cache_entries / argument[6];
	int I_VPN = 0;
	int I_VPA = 0;
	int D_VPN = 0;
	int I_offset = log2(argument[2]);
	int D_offset = log2(argument[3]);
	int I_PPN_SIZE = argument[0] / argument[2];
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
	vector<I_PTE> IPTE(I_PTE_entries);
	vector<I_cache> Icache(I_cache_entries);
	vector<I_TLB>ITLB(I_TLB_entries);
	while (halt!=1) {
		I_VPA = reg.PC / 4 + 2;
		I_VPN = I_VPA / (pow(2, I_offset));
		halt = exeinstr.process(instr, data, reg, snapshot);
		reg.PC = reg.PC + 4;
		cycle_num++;
		fprintf(snapshot, "PC: 0x%08X\n", reg.PC);
		fprintf(snapshot, "cycle %d\n", cycle_num);
		printf("cycle %d\n", cycle_num);
	}
	return 0;
}
*/