#pragma once
#include<stdint.h>
#include"regfile.h"
class exe {
public:
	int process(int instr, int data[], regfile &reg, FILE* &snapshot) {
		uint16_t opcode = (0xfc000000 & instr) >> 26;
		if (opcode == 0) {
			bool write_reg = false;
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
		else {
			bool write_reg_rt = false;
			int rs = (0x03e00000 & instr) >> 21;
			int rt = (0x001f0000 & instr) >> 16;
			int immediate = (0x0000ffff & instr);
			int address = (0x03ffffff & instr);
			int result = 0;
			int pos = 0;
			int pos1 = 0;
			switch (opcode) {
			case 0x02:
				reg.PC = (((reg.PC + 4) >> 28) << 28) + 4 * address - 4;
				break;
			case 0x03:
				result = reg.PC + 4;
				reg.PC = (((reg.PC + 4) >> 28) << 28) + 4 * address - 4;
				if (result != reg.reg[31]) {
					write_reg_rt = true;
					rt = 31;
				}
				break;
			case 0x08:case 0x09:
				if (rt != 0) {
					result = reg.reg[rs] + immediate;
					if (result != reg.reg[rt])
						write_reg_rt = true;
				}
				break;
			case 0x23:
				if ((immediate + reg.reg[rs]) > 1020 || (immediate + reg.reg[rs]) < 0 || (immediate + reg.reg[rs]) % 4 != 0)
					return 1;
				else {
					if (rt != 0) {
						result = data[2 + (immediate + reg.reg[rs]) / 4];
						if (result != reg.reg[rt])
							write_reg_rt = true;
					}	
				}
				break;
			case 0x21:
				if ((immediate + reg.reg[rs]) > 1020 || (immediate + reg.reg[rs]) < 0 || (immediate + reg.reg[rs]) % 4 == 1 || (immediate + reg.reg[rs]) % 4 == 3)
					return 1;
				else {
					if (rt != 0) {
						pos = (immediate + reg.reg[rs]) / 4 + 2;
						pos1 = immediate + reg.reg[rs];
						if (pos1 % 4 == 0) {
							result = (int32_t)((int16_t)((data[pos] & 0xffff0000) >> 16));
						}
						else if (pos1 % 4 == 2) {
							result = (int32_t)((int16_t)((data[pos] & 0x0000ffff)));
						}
						if (result != reg.reg[rt])
							write_reg_rt = true;
					}
				}
				break;
			case 0x25:
				if ((immediate + reg.reg[rs]) > 1022 || (immediate + reg.reg[rs]) < 0 || (immediate + reg.reg[rs]) % 4 == 1 || (immediate + reg.reg[rs]) % 4 == 3)
					return 1;
				else {
					if (rt != 0) {
						pos = immediate / 4 + 2;
						pos1 = immediate + reg.reg[rs];
						if (pos1 % 4 == 0) {
							result = (uint32_t)((uint16_t)((data[(immediate + reg.reg[rs]) / 4 + 2] & 0xffff0000) >> 16));
						}
						else if (pos1 % 4 == 2) {
							result = (uint32_t)((uint16_t)((data[(immediate + reg.reg[rs]) / 4 + 2] & 0x0000ffff)));
						}
						if (result != reg.reg[rt])
							write_reg_rt = true;
					}
				}
				break;
			case 0x20:
				if ((immediate + reg.reg[rs]) > 1023 || (immediate + reg.reg[rs]) < 0 )
					return 1;
				else {
					if (rt != 0) {
						pos = (immediate + reg.reg[rs]) / 4 + 2;
						pos1 = immediate + reg.reg[rs];
						if (pos1 % 4 == 0)
							result = (int64_t)((int8_t)(data[pos] >> 24));
						else if (pos1 % 4 == 1)
							result = (int64_t)(int8_t)(((data[pos] & 0x00ff0000) >> 16));
						else if (pos1 % 4 == 2)
							result = (int64_t)((int8_t)((data[pos] & 0x0000ff00) >> 8));
						else
							result = (int64_t)((int8_t)(data[pos] & 0x000000ff));
						if (result != reg.reg[rt])
							write_reg_rt = true;
					}
				}
				break;
			case 0x24:
				if ((immediate + reg.reg[rs]) > 1023 || (immediate + reg.reg[rs]) < 0)
					return 1;
				else {
						if (rt != 0) {
							pos = (immediate + reg.reg[rs]) / 4 + 2;
							pos1 = immediate + reg.reg[rs];
							if (pos1 % 4 == 0)
								result = (unsigned int)((data[pos] & 0xff000000) >> 24);
							else if (pos1 % 4 == 1)
								result = (unsigned int)((data[pos] & 0x00ff0000) >> 16);
							else if (pos1 % 4 == 2)
								result = (unsigned int)((data[pos] & 0x0000ff00) >> 8);
							else
								result = (unsigned int)(data[pos] & 0x000000ff);
							if (result != reg.reg[rt])
								write_reg_rt = true;
						}
					}
				break;
			case 0x2B:
				if ((immediate + reg.reg[rs]) > 1020 || (immediate + reg.reg[rs]) < 0 || (immediate + reg.reg[rs]) % 4 == 1 || (immediate + reg.reg[rs]) % 4 == 3)
					return 1;
				else {
					data[(immediate + reg.reg[rs]) / 4 + 2] = reg.reg[rt];
				}
				break;
			case 0x29:
				if ((immediate + reg.reg[rs]) > 1022 || (immediate + reg.reg[rs]) < 0 || (immediate + reg.reg[rs]) % 4 == 1 || (immediate + reg.reg[rs]) % 4 == 3)
					return 1;
				else {
					pos = (immediate + reg.reg[rs]) / 4 + 2;
					pos1 = immediate + reg.reg[rs];
					if (pos1 % 4 == 0) {
						data[pos] = (data[pos] & 0x0000ffff) | ((reg.reg[rt] & 0xffff) << 16);
					}
					else if (pos1 % 4 == 2) {
						data[pos] = (data[pos] & 0xffff0000) | (reg.reg[rt] & 0xffff);
					}
				}
				break;
			case 0x28:
				if ((immediate + reg.reg[rs]) > 1023 || (immediate + reg.reg[rs]) < 0)
					return 1;
				else {
				    pos = (immediate + reg.reg[rs]) / 4 + 2;
					pos1 = immediate + reg.reg[rs];
					if (pos1 % 4 == 0)
						data[pos] = (data[pos] & 0xffffff) | ((reg.reg[rt] & 0xff) << 24);
					else if (pos1 % 4 == 1)
						data[pos] = (data[pos] & 0xff00ffff) | ((reg.reg[rt] & 0xff) << 16);
					else if (pos1 % 4 == 2)
						data[pos] = (data[pos] & 0xffff00ff) | ((reg.reg[rt] & 0xff) << 8);
					else
						data[pos] = (data[pos] & 0xffffff00) | (reg.reg[rt] & 0xff);
				}
				break;
			case 0x0F:
				if (rt != 0) {
					result = ((uint32_t)((uint16_t)immediate)) << 16;
					if (result != reg.reg[rt])
						write_reg_rt = true;
				}
				break;
			case 0x0C:
				if (rt != 0) {
					result = reg.reg[rs] & (uint16_t)immediate;
					if (result != reg.reg[rt])
						write_reg_rt = true;
				}
				break;
			case 0x0D:
				if (rt != 0) {
					result = reg.reg[rs] | (uint16_t)immediate;
					if (result != reg.reg[rt])
						write_reg_rt = true;
				}
				break;
			case 0x0E:
				if (rt != 0) {
					result = ~(reg.reg[rs] | (uint16_t)immediate);
					if (result != reg.reg[rt])
						write_reg_rt = true;
				}
				break;
			case 0x0A:
				if (rt != 0) {
					result = (reg.reg[rs] <(int16_t)immediate);
					if (result != reg.reg[rt])
						write_reg_rt = true;
				}
				break;
			case 0x04:
				if (reg.reg[rs] == reg.reg[rt])
					reg.PC = reg.PC  + 4 * immediate;
				break;
			case 0x05:
				if (reg.reg[rs] != reg.reg[rt])
					reg.PC = reg.PC + 4 * immediate;
				break;
			case 0x07:
				if (reg.reg[rs] > 0)
					reg.PC = reg.PC + 4 * immediate;
				break;
			case 0x3F:
				return 1;
			default:
				break;
			}
			if (write_reg_rt) {
				reg.reg[rt] = result;
				fprintf(snapshot, "$%02d: 0x%08X\n", rt, reg.reg[rt]);
			}
		}
		return 0;
	}
};
