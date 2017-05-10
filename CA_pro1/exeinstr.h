#pragma once
#include<stdint.h>
#include"regfile.h"
class exe {
public:
	int process(int instr, int data[], regfile &reg, FILE* &snapshot) {
		bool write_reg = false;
		uint16_t opcode = (0xfc000000 & instr) >> 26;
		if (opcode == 0) {
			uint16_t funct = (0x0000003f & instr);
			int rs, rt, rd, shamt;
			rs = (0x03e00000 & instr) >> 21;
			rt = (0x001f0000 & instr) >> 16;
			rd = (0x0000f800 & instr) >> 11;
			shamt = (0x000007c0 & instr) >> 6;
			int result = 0;
			int sub_num = 0;
			int shamt_tmp = 0;
			int HI_value;
			int LO_value;
			long int product;
			switch (funct) {
			case 0x20:case 0x21:
				if (rd != 0) {
					result = reg.reg[rs] + reg.reg[rt];
					if (result != reg.reg[rd])
						write_reg = true;
				}
				break;
			case 0x22:
				if (rd != 0) {
					sub_num = -reg.reg[rt];
					result = reg.reg[rs] + sub_num;
					if (result != reg.reg[rd])
						write_reg = true;
				}
				break;
			case 0x24:
				if (rd != 0) {
					result = reg.reg[rs] & reg.reg[rt];
					if (result != reg.reg[rd])
						write_reg = true;
				}
				break;
			case 0x25:
				if (rd != 0) {
					result = reg.reg[rs] | reg.reg[rt];
					if (result != reg.reg[rd])
						write_reg = true;
				}
				break;
			case 0x26:
				if (rd != 0) {
					result = reg.reg[rs] ^ reg.reg[rt];
					if (result != reg.reg[rd])
						write_reg = true;
				}
				break;
			case 0x27:
				if (rd != 0) {
					result = ~(reg.reg[rs] | reg.reg[rt]);
					if (result != reg.reg[rd])
						write_reg = true;
				}
				break;
			case 0x28:
				if (rd != 0) {
					result = ~(reg.reg[rs] & reg.reg[rt]);
					if (result != reg.reg[rd])
						write_reg = true;
				}
				break;
			case 0x2A:
				if (rd != 0) {
					result = (reg.reg[rs] < reg.reg[rt]);
					if (result != reg.reg[rd])
						write_reg = true;
				}
				break;
			case 0x00:
				if (rd != 0) {
					result = reg.reg[rt] << shamt;
					if (result != reg.reg[rd])
						write_reg = true;
				}
				break;
			case 0x02:
				if (rd != 0) {
					shamt_tmp = (uint32_t)reg.reg[rt];
					result = shamt_tmp >> shamt;
					if (result != reg.reg[rd])
						write_reg = true;
				}
				break;
			case 0x03:
				if (rd != 0) {
					result = reg.reg[rt] >> shamt;
					if (result != reg.reg[rd])
						write_reg = true;
				}
				break;
			case 0x08:
				reg.PC = reg.reg[rs];
				fprintf(snapshot, "PC: 0x%08X\n\n\n", reg.PC);
				break;
			case 0x18:
				product = (int64_t)(int32_t)reg.reg[rs] * (int64_t)(int32_t)reg.reg[rt];
				HI_value = (uint32_t)((product >> 32) & 0xffffffff);
				LO_value = (uint32_t)(0xffffffff & product);
				if (reg.HI != HI_value) {
					fprintf(snapshot, "$HI: 0x%08X\n", HI_value);
					reg.HI = HI_value;
				}
				if (reg.LO != LO_value) {
					fprintf(snapshot, "$HI: 0x%08X\n", LO_value);
					reg.LO = LO_value;
				}
				break;
			case 0x10:
				if (rd != 0) {
					result = reg.HI;
					if (result != reg.reg[rd])
						write_reg = true;
				}
				break;
			case 0x12:
				if (rd != 0) {
					result = reg.LO;
					if (result != reg.reg[rd])
						write_reg = true;
				}
				break;
			default:
				break;
			}
			if (write_reg) {
				reg.reg[rd] = result;
				fprintf(snapshot, "$%02d: 0x%08X\n", rd, reg.reg[rd]);
			}
		}
	}
};
