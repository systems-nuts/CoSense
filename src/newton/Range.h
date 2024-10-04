#ifndef RANGE_H
#define RANGE_H

#include "llvm/ADT/APInt.h"
using llvm::APInt;

#include "llvm/Support/raw_ostream.h"
using llvm::raw_ostream;

enum RangeType { Unknown, Regular, Empty };

class Range {
	private:
	APInt l; // The lower bound of the range.
	APInt u; // The upper bound of the range.
	RangeType type{Regular};

	public:
	Range();
	Range(const APInt &lb, const APInt &ub, RangeType rType = Regular);
	~Range() = default;
	Range(const Range &other) = default;
	Range(Range &&) = default;
	Range &operator=(const Range &other) = default;
	Range &operator=(Range &&) = default;

	const APInt& getLower() const { return l; }
	const APInt& getUpper() const { return u; }
	void setLower(const APInt &newl) { this->l = newl; }
	void setUpper(const APInt &newu) { this->u = newu; }
	bool isUnknown() const { return type == Unknown; }
	void setUnknown() { type = Unknown; }
	bool isRegular() const { return type == Regular; }
	void setRegular() { type = Regular; }
	bool isEmpty() const { return type == Empty; }
	void setEmpty() { type = Empty; }
	bool isMaxRange() const;
	void print(raw_ostream &OS) const;
	Range add(const Range &other) const;
	Range sub(const Range &other) const;
	Range mul(const Range &other) const;
	Range udiv(const Range &other) const;
	Range sdiv(const Range &other) const;
	Range urem(const Range &other) const;
	Range srem(const Range &other) const;
	Range shl(const Range &other) const;
	Range lshr(const Range &other) const;
	Range ashr(const Range &other) const;
	Range And(const Range &other) const;
	Range And_conservative(const Range &other) const;
	Range Or(const Range &other) const;
	Range Or_conservative(const Range &other) const;
	Range Xor(const Range &other) const;
	Range truncate(unsigned bitwidth) const;
	//	Range signExtend(unsigned bitwidth) const;
	//	Range zeroExtend(unsigned bitwidth) const;
	Range sextOrTrunc(unsigned bitwidth) const;
	Range zextOrTrunc(unsigned bitwidth) const;
	Range intersectWith(const Range &other) const;
	Range unionWith(const Range &other) const;
	bool operator==(const Range &other) const;
	bool operator!=(const Range &other) const;
};

#endif // RANGE_H