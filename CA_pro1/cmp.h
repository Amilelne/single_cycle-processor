#pragma once
class I_PTE {
public:
	int VPN;
	bool valid;
	int PPN;
	I_PTE() {
		VPN = 0;
		valid = false;
		PPN = -1;
	}
};
class I_cache {
	int tag;
	bool valid;
	int content;
	I_cache() {
		tag = -1;
		valid = false;
		content = -1;
	}
};
