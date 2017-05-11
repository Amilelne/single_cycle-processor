#pragma once
class I_PTE {
public:
	bool valid;
	int PPN;
	int num;
	I_PTE() {
		valid = false;
		PPN = -1;
		num = 0;
	}
};
class I_cache {
public:
	int tag;
	bool valid;
	bool MRU;
	I_cache() {
		tag = -1;
		valid = false;
		MRU = false;
	}
};
class I_TLB {
public:
	int tag;
	bool valid;
	int num;
	I_TLB() {
		num = 0;
		tag = -1;
		valid = false;
	}
};
