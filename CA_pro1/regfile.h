#pragma once
#include<stdio.h>
class regfile {
public:
	int reg[32];
	int HI;
	int LO;
	int PC;
	bool writeback;
	int wb_num;
	int wb_data;
	regfile() {
		reg[32] = { 0 };
		HI = LO = 0;
		PC = 0;
		writeback = false;
		wb_data = wb_num = 0;
	}
	void showall(FILE* &snapshot) {
		for (int i = 0; i < 32; i++) {
			fprintf(snapshot, "$%02d: 0x%08X\n", i, reg[i]);
		}
		fprintf(snapshot, "$HI: 0x%08X\n", reg[32]);
		fprintf(snapshot, "$LO: 0x%08X\n", reg[33]);
		fprintf(snapshot, "PC: 0x%08X\n", PC);
	}
	void show(FILE* &snapshot) {
		fprintf(snapshot, "PC: 0x%08X\n", PC);
		if (writeback) {
			if (wb_num)
				reg[wb_num] = wb_data;
		}
	}
};
