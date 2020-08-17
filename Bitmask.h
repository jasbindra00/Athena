#pragma once
#include <stdint.h>
#include <bitset>
#include <iostream>
#include <vector>

using Bitset = uint32_t;
class Bitmask {
public:
	Bitmask() : bits(0) {}
	Bitmask(const Bitset& l_bits) : bits(l_bits) {}
	Bitmask(const std::vector<unsigned int>& states):bits(0) {
		for (const auto& state : states) {
			ToggleBit(state);
		}
	}
	Bitset GetMask() const { return bits; }
	void SetMask(const Bitset& l_value) { bits = l_value; }

	bool Matches(const Bitmask& l_bits,
		const Bitset& l_relevant = 0)const
	{
		return(l_relevant ? ((l_bits.GetMask() & l_relevant) == (bits & l_relevant))
			: (l_bits.GetMask() == bits));
	}

	bool GetBit(const unsigned int& l_pos)const {
		//REFACTOR BITMASK
		std::bitset<32> myset(bits);
		std::cout << myset << std::endl;
		return (myset[31 -l_pos] == true);
	}
	void TurnOnBit(const unsigned int& l_pos) {
		bits |= 1 << l_pos;
	}
	void TurnOnBits(const Bitset& l_bits) {
		bits |= l_bits;
	}
	void ClearBit(const unsigned int& l_pos) {
		bits &= ~(1 << l_pos);
	}
	void ToggleBit(const unsigned int& l_pos) {
		bits ^= 1 << l_pos;
	}

	void Clear() { bits = 0; }
private:
	Bitset bits;
};