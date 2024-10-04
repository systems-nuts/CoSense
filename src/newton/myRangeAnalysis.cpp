#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/KnownBits.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/PassManager.h"
#include "llvm/ADT/APInt.h"
#include "Range.h"
#include <map>
#include <vector>

using namespace llvm;

namespace
{

// The number of bits needed to store the largest variable of the function
// (APInt).
unsigned MAX_BIT_INT = 1;
// 声明用于表示整数常量的全局变量
APInt Min;
APInt Max;
APInt Zero;
APInt One;

// 宏定义
#define MUL_HELPER(x, y)                                                                  \
	((x).eq(Max) ? ((x).slt(Zero) ? Min : ((x).eq(Zero) ? Zero : Max))                \
		     : ((x).eq(Min) ? ((x).slt(Zero) ? Max : ((x).eq(Zero) ? Zero : Min)) \
				    : ((x) * (y))))

#define MUL_OV(x, y, xy)                                      \
	((x).isStrictlyPositive() == (y).isStrictlyPositive() \
	     ? ((xy).isNegative() ? Max : (xy))               \
	     : ((xy).isStrictlyPositive() ? Min : (xy)))


// Range 类定义
class Range {
	public:
	// 构造函数
	Range();
	Range(const APInt &lb, const APInt &ub, RangeType rType = Regular);

	// 成员函数
	bool isMaxRange() const;
	Range add(const Range &other) const;
	Range sub(const Range &other) const;
	Range mul(const Range &other) const;
	Range urem(const Range &other) const;
	Range srem(const Range &other) const;
	Range shl(const Range &other) const;
	Range lshr(const Range &other) const;
	Range ashr(const Range &other) const;
	Range And(const Range &other) const;
	Range And_conservative(const Range &other) const;
	Range Or(const Range &other) const;

	// 获取范围下界
	const APInt &getLower() const { return l; }

	// 获取范围上界
	const APInt &getUpper() const { return u; }

	// 判断范围是否未知
	bool isUnknown() const { return type == Unknown; }

	// 判断范围是否为空
	bool isEmpty() const { return type == Empty; }

	// 操作符
	bool operator==(const Range &other) const;


	private:
	APInt l, u;
	RangeType type;
};



// Range 类的默认构造函数，初始化为 [Min, Max]
Range::Range() : l(Min), u(Max), type(Regular) {}
// 使用下界和上界以及范围类型初始化 Range
Range::Range(const APInt & lb, const APInt & ub, RangeType rType)
    : l(lb), u(ub), type(rType)
{
	if (lb.sgt(ub))
	{
		type = Empty;  // 如果下界大于上界，则认为范围为空
	}
}


Range Range::Or(const Range &other) const {
	APInt lower = l | other.l;
	APInt upper = u | other.u;
	return Range(lower, upper);
}

// 判断是否为最大范围 [Min, Max]
bool
Range::isMaxRange() const
{
	return this->getLower().eq(Min) && this->getUpper().eq(Max);
}

// 加法运算 [a, b] + [c, d] = [a + c, b + d]
Range
Range::add(const Range & other) const
{
	if (this->isUnknown() || other.isUnknown())
	{
		return Range(Min, Max, Unknown);  // 如果任一范围未知，则结果未知
	}

	const APInt & a = this->getLower();
	const APInt & b = this->getUpper();
	const APInt & c = other.getLower();
	const APInt & d = other.getUpper();
	APInt	      l = Min, u = Max;

	if (a.ne(Min) && c.ne(Min))
	{
		l = a + c;

		// 溢出处理
		if (a.isNegative() == c.isNegative() && a.isNegative() != l.isNegative())
		{
			l = Min;
		}
	}

	if (b.ne(Max) && d.ne(Max))
	{
		u = b + d;

		// 溢出处理
		if (b.isNegative() == d.isNegative() && b.isNegative() != u.isNegative())
		{
			u = Max;
		}
	}

	return Range(l, u);
}

// 减法运算 [a, b] - [c, d] = [a - d, b - c]
Range
Range::sub(const Range & other) const
{
	if (this->isUnknown() || other.isUnknown())
	{
		return Range(Min, Max, Unknown);  // 如果任一范围未知，则结果未知
	}

	const APInt & a = this->getLower();
	const APInt & b = this->getUpper();
	const APInt & c = other.getLower();
	const APInt & d = other.getUpper();
	APInt	      l, u;

	// 计算下界
	if (a.eq(Min) || d.eq(Max))
	{
		l = Min;
	}
	else
	{
		l = a - d;

		// 溢出处理
		if (a.isNegative() != d.isNegative() && d.isNegative() == l.isNegative())
		{
			l = Min;
		}
	}

	// 计算上界
	if (b.eq(Max) || c.eq(Min))
	{
		u = Max;
	}
	else
	{
		u = b - c;

		// 溢出处理
		if (b.isNegative() != c.isNegative() && c.isNegative() == u.isNegative())
		{
			u = Max;
		}
	}

	return Range(l, u);
}

// 乘法运算 [a, b] * [c, d] = [min(a*c, a*d, b*c, b*d), max(a*c, a*d, b*c, b*d)]
Range
Range::mul(const Range & other) const
{
	if (this->isUnknown() || other.isUnknown())
	{
		return Range(Min, Max, Unknown);  // 如果任一范围未知，则结果未知
	}

	if (this->isMaxRange() || other.isMaxRange())
	{
		return Range(Min, Max);	 // 如果任一范围为最大范围，则结果为最大范围
	}

	const APInt & a = this->getLower();
	const APInt & b = this->getUpper();
	const APInt & c = other.getLower();
	const APInt & d = other.getUpper();

	APInt candidates[4];
	candidates[0] = MUL_OV(a, c, MUL_HELPER(a, c));
	candidates[1] = MUL_OV(a, d, MUL_HELPER(a, d));
	candidates[2] = MUL_OV(b, c, MUL_HELPER(b, c));
	candidates[3] = MUL_OV(b, d, MUL_HELPER(b, d));

	// 找到最小和最大值
	APInt * min = &candidates[0];
	APInt * max = &candidates[0];

	for (unsigned i = 1; i < 4; ++i)
	{
		if (candidates[i].sgt(*max))
		{
			max = &candidates[i];
		}
		else if (candidates[i].slt(*min))
		{
			min = &candidates[i];
		}
	}

	return Range(*min, *max);
}

// 无符号取模运算 [a, b] % [c, d]
Range
Range::urem(const Range & other) const
{
	if (this->isUnknown() || other.isUnknown())
	{
		return Range(Min, Max, Unknown);  // 如果任一范围未知，则结果未知
	}

	const APInt & a = this->getLower();
	const APInt & b = this->getUpper();
	const APInt & c = other.getLower();
	const APInt & d = other.getUpper();

	// 处理除以0的情况
	if (c.ule(Zero) && d.uge(Zero))
	{
		return Range(Min, Max);
	}

	APInt candidates[4];
	candidates[0] = Min;
	candidates[1] = Min;
	candidates[2] = Max;
	candidates[3] = Max;

	if (a.ne(Min) && c.ne(Min))
	{
		candidates[0] = a.urem(c);  // lower lower
	}

	if (a.ne(Min) && d.ne(Max))
	{
		candidates[1] = a.urem(d);  // lower upper
	}

	if (b.ne(Max) && c.ne(Min))
	{
		candidates[2] = b.urem(c);  // upper lower
	}

	if (b.ne(Max) && d.ne(Max))
	{
		candidates[3] = b.urem(d);  // upper upper
	}

	// 找到最小和最大值
	APInt * min = &candidates[0];
	APInt * max = &candidates[0];

	for (unsigned i = 1; i < 4; ++i)
	{
		if (candidates[i].sgt(*max))
		{
			max = &candidates[i];
		}
		else if (candidates[i].slt(*min))
		{
			min = &candidates[i];
		}
	}

	return Range(*min, *max);
}

// 有符号取模运算 [a, b] % [c, d]
Range
Range::srem(const Range & other) const
{
	if (other == Range(Zero, Zero) || other == Range(Min, Max, Empty))
	{
		return Range(Min, Max, Empty);
	}

	if (this->isUnknown() || other.isUnknown())
	{
		return Range(Min, Max, Unknown);  // 如果任一范围未知，则结果未知
	}

	const APInt & a = this->getLower();
	const APInt & b = this->getUpper();
	const APInt & c = other.getLower();
	const APInt & d = other.getUpper();

	// 处理除以0的情况
	if (c.sle(Zero) && d.sge(Zero))
	{
		return Range(Min, Max);
	}

	APInt candidates[4];
	candidates[0] = MUL_OV(a, c, MUL_HELPER(a, c));
	candidates[1] = MUL_OV(a, d, MUL_HELPER(a, d));
	candidates[2] = MUL_OV(b, c, MUL_HELPER(b, c));
	candidates[3] = MUL_OV(b, d, MUL_HELPER(b, d));

	// 找到最小和最大值
	APInt * min = &candidates[0];
	APInt * max = &candidates[0];

	for (unsigned i = 1; i < 4; ++i)
	{
		if (candidates[i].sgt(*max))
		{
			max = &candidates[i];
		}
		else if (candidates[i].slt(*min))
		{
			min = &candidates[i];
		}
	}

	return Range(*min, *max);
}

// 左移运算
Range
Range::shl(const Range & other) const
{
	if (isEmpty())
	{
		return Range(*this);
	}
	if (other.isEmpty())
	{
		return Range(other);
	}

	if (this->isUnknown() || other.isUnknown())
	{
		return Range(Min, Max, Unknown);
	}

	const APInt & a = this->getLower();
	const APInt & b = this->getUpper();
	const APInt & c = other.getLower();
	const APInt & d = other.getUpper();

	if (a.eq(Min) || c.eq(Min) || b.eq(Max) || d.eq(Max))
	{
		return Range(Min, Max);
	}

	APInt min = a.shl(c);
	APInt max = b.shl(d);

	APInt Zeros(MAX_BIT_INT, b.countLeadingZeros());
	if (Zeros.ugt(d))
	{
		return Range(min, max);
	}

	// 返回最大范围 [-inf, +inf]
	return Range(Min, Max);
}

// 逻辑右移运算
Range
Range::lshr(const Range & other) const
{
	if (isEmpty())
	{
		return Range(*this);
	}
	if (other.isEmpty())
	{
		return Range(other);
	}

	if (this->isUnknown() || other.isUnknown())
	{
		return Range(Min, Max, Unknown);
	}

	const APInt & a = this->getLower();
	const APInt & b = this->getUpper();
	const APInt & c = other.getLower();
	const APInt & d = other.getUpper();

	if (a.eq(Min) || c.eq(Min) || b.eq(Max) || d.eq(Max))
	{
		return Range(Min, Max);
	}

	APInt max = b.lshr(c);
	APInt min = a.lshr(d);

	return Range(min, max);
}

// 算术右移运算
Range
Range::ashr(const Range & other) const
{
	if (isEmpty())
	{
		return Range(*this);
	}
	if (other.isEmpty())
	{
		return Range(other);
	}

	if (this->isUnknown() || other.isUnknown())
	{
		return Range(Min, Max, Unknown);
	}

	const APInt & a = this->getLower();
	const APInt & b = this->getUpper();
	const APInt & c = other.getLower();
	const APInt & d = other.getUpper();

	if (a.eq(Min) || c.eq(Min) || b.eq(Max) || d.eq(Max))
	{
		return Range(Min, Max);
	}

	APInt max = b.ashr(c);
	APInt min = a.ashr(d);

	return Range(min, max);
}

// 与操作，Hacker's Delight算法
Range
Range::And(const Range & other) const
{
	if (this->isUnknown() || other.isUnknown())
	{
		const APInt & umin = llvm::APIntOps::umin(getUpper(), other.getUpper());
		if (umin.isAllOnesValue())
		{
			return Range(Min, Max);
		}
		return Range(APInt::getNullValue(MAX_BIT_INT), umin);
	}

	APInt	      a = this->getLower();
	APInt	      b = this->getUpper();
	const APInt & c = other.getLower();
	const APInt & d = other.getUpper();

	if (a.eq(Min) || b.eq(Max) || c.eq(Min) || d.eq(Max))
	{
		return Range(Min, Max);
	}

	// 取反所有位
	APInt negA = APInt(a);
	negA.flipAllBits();
	APInt negB = APInt(b);
	negB.flipAllBits();
	APInt negC = APInt(c);
	negC.flipAllBits();
	APInt negD = APInt(d);
	negD.flipAllBits();

	Range inv1 = Range(negB, negA);
	Range inv2 = Range(negD, negC);

	Range invres = inv1.Or(inv2);

	// 取反结果
	APInt invLower = invres.getUpper();
	invLower.flipAllBits();

	APInt invUpper = invres.getLower();
	invUpper.flipAllBits();

	return Range(invLower, invUpper);
}

// 保守的与操作，适用于大于64位的值
Range
Range::And_conservative(const Range & other) const
{
	if (isEmpty())
	{
		return Range(*this);
	}
	if (other.isEmpty())
	{
		return Range(other);
	}

	const APInt & umin = llvm::APIntOps::umin(other.getUpper(), getUpper());
	if (umin.isAllOnesValue())
	{
		return Range(Min, Max);
	}
	return Range(APInt::getNullValue(MAX_BIT_INT), umin);
}





// 范围表示和管理
std::map<Value *, Range> valueRanges;

// 更新常量整数值
void
updateConstantIntegers(unsigned maxBitWidth)
{
	// 使用给定的最大位宽设置最小值和最大值
	Min  = APInt::getSignedMinValue(maxBitWidth);
	Max  = APInt::getSignedMaxValue(maxBitWidth);
	Zero = APInt(maxBitWidth, 0UL, true);  // 确保使用maxBitWidth来表示Zero
	One  = APInt(maxBitWidth, 1UL, true);  // 确保使用maxBitWidth来表示One
}

// 初始化分析
void
initializeAnalysis(Function & F)
{
	// 清除前一次分析的范围信息
	valueRanges.clear();
	errs() << "Starting range analysis for function: " << F.getName() << "\n";
}

// 结束分析
void
finalizeAnalysis(Function & F)
{
	// 结束分析的逻辑
	errs() << "Finished range analysis for function: " << F.getName() << "\n";
}

// 判断指令是否有效
bool
isValidInstruction(const Instruction * I)
{
	switch (I->getOpcode())
	{
		// 算术运算
		case Instruction::Add:
		case Instruction::Sub:
		case Instruction::Mul:
		case Instruction::UDiv:
		case Instruction::SDiv:
		case Instruction::URem:
		case Instruction::SRem:
		// 位操作
		case Instruction::Shl:
		case Instruction::LShr:
		case Instruction::AShr:
		case Instruction::And:
		case Instruction::Or:
		case Instruction::Xor:
		// 类型转换
		case Instruction::Trunc:
		case Instruction::ZExt:
		case Instruction::SExt:
		// 其他操作
		case Instruction::PHI:
		case Instruction::Select:
			return true;
		default:
			return false;
	}
}

// 获取函数中最大位宽
unsigned
getMaxBitWidth(const Function & F)
{
	unsigned max = 0;
	for (const Instruction & I : instructions(F))
	{
		if (I.getType()->isIntegerTy())
		{
			max = std::max(max, static_cast<unsigned>(I.getType()->getPrimitiveSizeInBits().getFixedSize()));
		}

		// 获得指令操作数的最大位宽
		for (const Value * operand : I.operands())
		{
			if (operand->getType()->isIntegerTy())
			{
				max = std::max(max, static_cast<unsigned>(operand->getType()->getPrimitiveSizeInBits().getFixedSize()));
			}
		}
	}
	// 确保位宽最小为1
	return max == 0 ? 1 : max;
}

void handleBinaryOperator(BinaryOperator * binOp);

// 分析每个指令
void
analyzeInstruction(Instruction & I)
{
	if (auto * binOp = dyn_cast<BinaryOperator>(&I))
	{
		handleBinaryOperator(binOp);
	}
}

void
insertOverflowCheck(BinaryOperator * binOp, Value * op1, Value * op2, Range & resultRange)
{
	Function *    F	      = binOp->getFunction();
	LLVMContext & Context = F->getContext();
	IRBuilder<>   Builder(binOp);

	// 检查 x 和 y 是否都是非负数
	Value * xNonNegative	= Builder.CreateICmpSGE(op1, ConstantInt::get(op1->getType(), 0));
	Value * yNonNegative	= Builder.CreateICmpSGE(op2, ConstantInt::get(op2->getType(), 0));
	Value * bothNonNegative = Builder.CreateAnd(xNonNegative, yNonNegative);

	// 如果是，则检查结果是否也是非负数
	Value * addNegative	    = Builder.CreateICmpSLT(binOp, ConstantInt::get(binOp->getType(), 0));
	Value * nonNegativeOverflow = Builder.CreateAnd(bothNonNegative, addNegative);

	// 检查 x 和 y 是否都是负数
	Value * xNegative    = Builder.CreateICmpSLT(op1, ConstantInt::get(op1->getType(), 0));
	Value * yNegative    = Builder.CreateICmpSLT(op2, ConstantInt::get(op2->getType(), 0));
	Value * bothNegative = Builder.CreateAnd(xNegative, yNegative);

	// 如果是，则检查结果是否也是负数
	Value * addNonNegative	 = Builder.CreateICmpSGE(binOp, ConstantInt::get(binOp->getType(), 0));
	Value * negativeOverflow = Builder.CreateAnd(bothNegative, addNonNegative);

	// 最终检查是否发生溢出
	Value * overflow = Builder.CreateOr(nonNegativeOverflow, negativeOverflow);

	// 根据溢出情况创建分支
	BasicBlock * CurrentBB	= binOp->getParent();
	BasicBlock * OverflowBB = BasicBlock::Create(Context, "overflow", F);
	BasicBlock * ContinueBB = BasicBlock::Create(Context, "continue", F);

	Builder.CreateCondBr(overflow, OverflowBB, ContinueBB);

	// 在 OverflowBB 中处理溢出
	Builder.SetInsertPoint(OverflowBB);
	// 调用处理溢出的函数
	Function * HandleOverflowFunc = F->getParent()->getFunction("handle_overflow");
	if (HandleOverflowFunc)
	{
		Builder.CreateCall(HandleOverflowFunc);
	}
	Builder.CreateBr(ContinueBB);

	// 在 ContinueBB 中继续执行
	Builder.SetInsertPoint(ContinueBB);
}

// 处理二元操作指令
void
handleBinaryOperator(BinaryOperator * binOp)
{
	auto op1 = binOp->getOperand(0);
	auto op2 = binOp->getOperand(1);

	// 获取操作数的范围
	Range range1 = valueRanges.count(op1) ? valueRanges[op1] : Range(Min, Max);
	Range range2 = valueRanges.count(op2) ? valueRanges[op2] : Range(Min, Max);

	// 计算新范围
	Range resultRange;

	if (binOp->getOpcode() == Instruction::Add)
	{
		resultRange = range1.add(range2);

		// 插入溢出检测逻辑
		insertOverflowCheck(binOp, op1, op2, resultRange);
	}

	//	switch (binOp->getOpcode()) {
	//		case Instruction::Add:
	//			// 调用 Range::add 方法
	//			resultRange = range1.add(range2);
	//			break;
	//		case Instruction::Sub:
	//			resultRange = range1.sub(range2);
	//			break;
	//		case Instruction::Mul:
	//			resultRange = range1.mul(range2);
	//			break;
	//		case Instruction::UDiv:
	//			resultRange = range1.udiv(range2);
	//			break;
	//		case Instruction::SDiv:
	//			resultRange = range1.sdiv(range2);
	//			break;
	//		case Instruction::URem:
	//			resultRange = range1.urem(range2);
	//			break;
	//		case Instruction::SRem:
	//			resultRange = range1.srem(range2);
	//			break;
	//		// 处理其他运算符，如位移、位操作等
	//		default:
	//			resultRange = Range(Min, Max, Unknown);
	//			break;
	//	}

	// 存储结果范围
	valueRanges[binOp] = resultRange;
}

// 处理函数调用指令
void
handleCallInst(CallInst * callInst)
{
	// 处理函数调用的范围分析逻辑
}

// 获取某个值的范围
std::pair<double, double>
getValueRange(Value * val)
{
	if (isa<ConstantInt>(val))
	{
		auto   cInt = dyn_cast<ConstantInt>(val);
		double v    = cInt->getSExtValue();
		return std::make_pair(v, v);
	}
	else if (valueRanges.find(val) != valueRanges.end())
	{
		Range range = valueRanges[val];
		return std::make_pair(range.getLower().getSExtValue(), range.getUpper().getSExtValue());
	}
	else
	{
		return std::make_pair(-INFINITY, INFINITY);
	}
}

// 加法的范围分析
std::pair<double, double>
analyzeAddition(std::pair<double, double> range1, std::pair<double, double> range2)
{
	return std::make_pair(range1.first + range2.first, range1.second + range2.second);
}

// 减法的范围分析
std::pair<double, double>
analyzeSubtraction(std::pair<double, double> range1, std::pair<double, double> range2)
{
	return std::make_pair(range1.first - range2.second, range1.second - range2.first);
}

// 乘法的范围分析
std::pair<double, double>
analyzeMultiplication(std::pair<double, double> range1, std::pair<double, double> range2)
{
	double lz = std::min({range1.first * range2.first, range1.first * range2.second, range1.second * range2.first, range1.second * range2.second});
	double uz = std::max({range1.first * range2.first, range1.first * range2.second, range1.second * range2.first, range1.second * range2.second});
	return std::make_pair(lz, uz);
}

// 除法的范围分析
std::pair<double, double>
analyzeDivision(std::pair<double, double> range1, std::pair<double, double> range2)
{
	if (range2.first > 0 || range2.second < 0)
	{
		double lz = std::min({range1.first / range2.first, range1.first / range2.second, range1.second / range2.first, range1.second / range2.second});
		double uz = std::max({range1.first / range2.first, range1.first / range2.second, range1.second / range2.first, range1.second / range2.second});
		return std::make_pair(lz, uz);
	}
	// 处理除零的情况
	return std::make_pair(-INFINITY, INFINITY);
}

// 打印变量名
// void printVarName(const Value *V, raw_ostream &OS) {
//	if (const Argument *A = dyn_cast<Argument>(V)) {
//		OS << A->getParent()->getName() << "." << A->getName();
//	} else if (const Instruction *I = dyn_cast<Instruction>(V)) {
//		OS << I->getParent()->getParent()->getName() << "."
//		   << I->getParent()->getName() << "." << I->getName();
//	} else {
//		OS << V->getName();
//	}
//}

// SymbInterval 打印函数
// void printSymbInterval(raw_ostream &OS, const SymbInterval &SI) {
//	switch (SI.getOperation()) {
//		case ICmpInst::ICMP_EQ: // equal
//			OS << "[lb(";
//			printVarName(SI.getBound(), OS);
//			OS << "), ub(";
//			printVarName(SI.getBound(), OS);
//			OS << ")]";
//			break;
//		case ICmpInst::ICMP_SLE: // sign less or equal
//			OS << "[-inf, ub(";
//			printVarName(SI.getBound(), OS);
//			OS << ")]";
//			break;
//		case ICmpInst::ICMP_SLT: // sign less than
//			OS << "[-inf, ub(";
//			printVarName(SI.getBound(), OS);
//			OS << ") - 1]";
//			break;
//		case ICmpInst::ICMP_SGE: // sign greater or equal
//			OS << "[lb(";
//			printVarName(SI.getBound(), OS);
//			OS << "), +inf]";
//			break;
//		case ICmpInst::ICMP_SGT: // sign greater than
//			OS << "[lb(";
//			printVarName(SI.getBound(), OS);
//			OS << " - 1), +inf]";
//			break;
//		default:
//			OS << "Unknown Instruction.\n";
//	}
//}

// 主方法：运行范围分析
void
runRangeAnalysis(Function & F)
{
	initializeAnalysis(F);

	// 获取函数的最大位宽
	unsigned maxBitWidth = getMaxBitWidth(F);

	// 更新常量整数值
	updateConstantIntegers(maxBitWidth);

	for (auto & BB : F)
	{
		for (auto & I : BB)
		{
			if (isValidInstruction(&I))
			{
				analyzeInstruction(I);
			}
		}
	}
	finalizeAnalysis(F);
}

class RangeAnalysisPass : public PassInfoMixin<RangeAnalysisPass> {
	public:
	PreservedAnalyses
	run(Function & F, FunctionAnalysisManager & FAM)
	{
		runRangeAnalysis(F);
		return PreservedAnalyses::all();
	}
};

// 注册新的 Pass 机制
extern "C" ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo()
{
	return {LLVM_PLUGIN_API_VERSION, "RangeAnalysisPass", LLVM_VERSION_STRING,
		[](PassBuilder & PB) {
			PB.registerPipelineParsingCallback(
			    [](StringRef Name, FunctionPassManager & FPM,
			       ArrayRef<PassBuilder::PipelineElement>) {
				    if (Name == "range-analysis")
				    {
					    FPM.addPass(RangeAnalysisPass());
					    return true;
				    }
				    return false;
			    });
		}};
}

}  // namespace

// end of anonymous namespace
