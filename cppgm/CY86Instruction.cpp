#include "std.pch"

#include "CY86Instruction.h"

struct CY86OpCodeName
{
	ECY86OpCode opcode;
	const char* str;
};

const vector<CY86OpCodeName>& CY86OpCodeNames()
{
	static vector<CY86OpCodeName> v{
	{ YO_DATA8, "data8" },
	{ YO_DATA16, "data16" },
	{ YO_DATA32, "data32" },
	{ YO_DATA64, "data64" },
	{ YO_MOVE8, "move8" },
	{ YO_MOVE16, "move16" },
	{ YO_MOVE32, "move32" },
	{ YO_MOVE64, "move64" },
	{ YO_MOVE80, "move80" },
	{ YO_JUMP, "jump" },
	{ YO_JUMPIF, "jumpif" },
	{ YO_CALL, "call" },
	{ YO_RET, "ret" },
	{ YO_NOT8, "not8" },
	{ YO_NOT16, "not16" },
	{ YO_NOT32, "not32" },
	{ YO_NOT64, "not64" },
	{ YO_AND8, "and8" },
	{ YO_AND16, "and16" },
	{ YO_AND32, "and32" },
	{ YO_AND64, "and64" },
	{ YO_OR8, "or8" },
	{ YO_OR16, "or16" },
	{ YO_OR32, "or32" },
	{ YO_OR64, "or64" },
	{ YO_XOR8, "xor8" },
	{ YO_XOR16, "xor16" },
	{ YO_XOR32, "xor32" },
	{ YO_XOR64, "xor64" },
	{ YO_LSHIFT8, "lshift8" },
	{ YO_LSHIFT16, "lshift16" },
	{ YO_LSHIFT32, "lshift32" },
	{ YO_LSHIFT64, "lshift64" },
	{ YO_SRSHIFT8, "srshift8" },
	{ YO_SRSHIFT16, "srshift16" },
	{ YO_SRSHIFT32, "srshift32" },
	{ YO_SRSHIFT64, "srshift64" },
	{ YO_URSHIFT8, "urshift8" },
	{ YO_URSHIFT16, "urshift16" },
	{ YO_URSHIFT32, "urshift32" },
	{ YO_URSHIFT64, "urshift64" },
	{ YO_S8CONVF80, "s8convf80" },
	{ YO_S16CONVF80, "s16convf80" },
	{ YO_S32CONVF80, "s32convf80" },
	{ YO_S64CONVF80, "s64convf80" },
	{ YO_U8CONVF80, "u8convf80" },
	{ YO_U16CONVF80, "u16convf80" },
	{ YO_U32CONVF80, "u32convf80" },
	{ YO_U64CONVF80, "u64convf80" },
	{ YO_F32CONVF80, "f32convf80" },
	{ YO_F64CONVF80, "f64convf80" },
	{ YO_F80CONVS8, "f80convs8" },
	{ YO_F80CONVS16, "f80convs16" },
	{ YO_F80CONVS32, "f80convs32" },
	{ YO_F80CONVS64, "f80convs64" },
	{ YO_F80CONVU8, "f80convu8" },
	{ YO_F80CONVU16, "f80convu16" },
	{ YO_F80CONVU32, "f80convu32" },
	{ YO_F80CONVU64, "f80convu64" },
	{ YO_F80CONVF32, "f80convf32" },
	{ YO_F80CONVF64, "f80convf64" },
	{ YO_IADD8, "iadd8" },
	{ YO_IADD16, "iadd16" },
	{ YO_IADD32, "iadd32" },
	{ YO_IADD64, "iadd64" },
	{ YO_FADD32, "fadd32" },
	{ YO_FADD64, "fadd64" },
	{ YO_FADD80, "fadd80" },
	{ YO_ISUB8, "isub8" },
	{ YO_ISUB16, "isub16" },
	{ YO_ISUB32, "isub32" },
	{ YO_ISUB64, "isub64" },
	{ YO_FSUB32, "fsub32" },
	{ YO_FSUB64, "fsub64" },
	{ YO_FSUB80, "fsub80" },
	{ YO_SMUL8, "smul8" },
	{ YO_SMUL16, "smul16" },
	{ YO_SMUL32, "smul32" },
	{ YO_SMUL64, "smul64" },
	{ YO_UMUL8, "umul8" },
	{ YO_UMUL16, "umul16" },
	{ YO_UMUL32, "umul32" },
	{ YO_UMUL64, "umul64" },
	{ YO_FMUL32, "fmul32" },
	{ YO_FMUL64, "fmul64" },
	{ YO_FMUL80, "fmul80" },
	{ YO_SDIV8, "sdiv8" },
	{ YO_SDIV16, "sdiv16" },
	{ YO_SDIV32, "sdiv32" },
	{ YO_SDIV64, "sdiv64" },
	{ YO_UDIV8, "udiv8" },
	{ YO_UDIV16, "udiv16" },
	{ YO_UDIV32, "udiv32" },
	{ YO_UDIV64, "udiv64" },
	{ YO_FDIV32, "fdiv32" },
	{ YO_FDIV64, "fdiv64" },
	{ YO_FDIV80, "fdiv80" },
	{ YO_SMOD8, "smod8" },
	{ YO_SMOD16, "smod16" },
	{ YO_SMOD32, "smod32" },
	{ YO_SMOD64, "smod64" },
	{ YO_UMOD8, "umod8" },
	{ YO_UMOD16, "umod16" },
	{ YO_UMOD32, "umod32" },
	{ YO_UMOD64, "umod64" },
	{ YO_IEQ8, "ieq8" },
	{ YO_IEQ16, "ieq16" },
	{ YO_IEQ32, "ieq32" },
	{ YO_IEQ64, "ieq64" },
	{ YO_FEQ32, "feq32" },
	{ YO_FEQ64, "feq64" },
	{ YO_FEQ80, "feq80" },
	{ YO_INE8, "ine8" },
	{ YO_INE16, "ine16" },
	{ YO_INE32, "ine32" },
	{ YO_INE64, "ine64" },
	{ YO_FNE32, "fne32" },
	{ YO_FNE64, "fne64" },
	{ YO_FNE80, "fne80" },
	{ YO_SLT8, "slt8" },
	{ YO_SLT16, "slt16" },
	{ YO_SLT32, "slt32" },
	{ YO_SLT64, "slt64" },
	{ YO_ULT8, "ult8" },
	{ YO_ULT16, "ult16" },
	{ YO_ULT32, "ult32" },
	{ YO_ULT64, "ult64" },
	{ YO_FLT32, "flt32" },
	{ YO_FLT64, "flt64" },
	{ YO_FLT80, "flt80" },
	{ YO_SGT8, "sgt8" },
	{ YO_SGT16, "sgt16" },
	{ YO_SGT32, "sgt32" },
	{ YO_SGT64, "sgt64" },
	{ YO_UGT8, "ugt8" },
	{ YO_UGT16, "ugt16" },
	{ YO_UGT32, "ugt32" },
	{ YO_UGT64, "ugt64" },
	{ YO_FGT32, "fgt32" },
	{ YO_FGT64, "fgt64" },
	{ YO_FGT80, "fgt80" },
	{ YO_SLE8, "sle8" },
	{ YO_SLE16, "sle16" },
	{ YO_SLE32, "sle32" },
	{ YO_SLE64, "sle64" },
	{ YO_ULE8, "ule8" },
	{ YO_ULE16, "ule16" },
	{ YO_ULE32, "ule32" },
	{ YO_ULE64, "ule64" },
	{ YO_FLE32, "fle32" },
	{ YO_FLE64, "fle64" },
	{ YO_FLE80, "fle80" },
	{ YO_SGE8, "sge8" },
	{ YO_SGE16, "sge16" },
	{ YO_SGE32, "sge32" },
	{ YO_SGE64, "sge64" },
	{ YO_UGE8, "uge8" },
	{ YO_UGE16, "uge16" },
	{ YO_UGE32, "uge32" },
	{ YO_UGE64, "uge64" },
	{ YO_FGE32, "fge32" },
	{ YO_FGE64, "fge64" },
	{ YO_FGE80, "fge80" },
	{ YO_SYSCALL0, "syscall0" },
	{ YO_SYSCALL1, "syscall1" },
	{ YO_SYSCALL2, "syscall2" },
	{ YO_SYSCALL3, "syscall3" },
	{ YO_SYSCALL4, "syscall4" },
	{ YO_SYSCALL5, "syscall5" },
	{ YO_SYSCALL6, "syscall6" }
	};
	return v;
}

ECY86Register StringToCY86Register(const string& str)
{
	static map<string, ECY86Register> m
	{
	    {"sp", YR_SP},
	    {"bp", YR_BP},
	    {"x8", YR_X8},
	    {"x16", YR_X16},
	    {"x32", YR_X32},
	    {"x64", YR_X64},
	    {"y8", YR_Y8},
	    {"y16", YR_Y16},
	    {"y32", YR_Y32},
	    {"y64", YR_Y64},
	    {"z8", YR_Z8},
	    {"z16", YR_Z16},
	    {"z32", YR_Z32},
	    {"z64", YR_Z64},
	    {"t8", YR_T8},
	    {"t16", YR_T16},
	    {"t32", YR_T32},
	    {"t64", YR_T64}
	};

	auto it = m.find(str);

	if (it == m.end())
		return YR_UNKNOWN;
	else
		return it->second;
}

string CY86RegisterToString(ECY86Register reg)
{
	static map<ECY86Register, string> m
	{
	    {YR_SP, "sp"},
	    {YR_BP, "bp"},
	    {YR_X8, "x8"},
	    {YR_X16, "x16"},
	    {YR_X32, "x32"},
	    {YR_X64, "x64"},
	    {YR_Y8, "y8"},
	    {YR_Y16, "y16"},
	    {YR_Y32, "y32"},
	    {YR_Y64, "y64"},
	    {YR_Z8, "z8"},
	    {YR_Z16, "z16"},
	    {YR_Z32, "z32"},
	    {YR_Z64, "z64"},
	    {YR_T8, "t8"},
	    {YR_T16, "t16"},
	    {YR_T32, "t32"},
	    {YR_T64, "t64"}
	};

	auto it = m.find(reg);

	if (it == m.end())
		throw logic_error("internal error: unknown register");
	else
		return it->second;
}

ECY86OpCode StringToCY86OpCode(const string& s)
{
	static map<string, ECY86OpCode> m = []
	{
		map<string, ECY86OpCode> res;

		for (auto x : CY86OpCodeNames())
			res[x.str] = x.opcode;
		return res;
	}();

	auto it = m.find(s);

	if (it == m.end())
		throw logic_error("unknown opcode: " + s);

	return it->second;

}

string CY86OpCodeToString(ECY86OpCode e)
{
	static map<ECY86OpCode, string> m = []
	{
		map<ECY86OpCode, string> res;

		for (auto x : CY86OpCodeNames())
			res[x.opcode] = x.str;
		return res;
	}();

	auto it = m.find(e);

	if (it == m.end())
		throw logic_error("internal error: opcode to string");

	return it->second;
}

map<ECY86OpCode, vector<CY86OperandConstraint>> build_CY86OperandConstraints()
{
	constexpr auto i = YOT_IMMEDIATE;
	constexpr auto a = YOT_IMMEDIATE | YOT_REGISTER | YOT_MEMORY;
	constexpr auto w = YOT_REGISTER | YOT_MEMORY;

	map<ECY86OpCode, vector<CY86OperandConstraint>> m
	{
		{ YO_DATA8, { { i, 1 } } },
		{ YO_DATA16, { { i, 2 } } },
		{ YO_DATA32, { { i, 4 } } },
		{ YO_DATA64, { { i, 8 } } },
		{ YO_MOVE8, { { w, 1 }, { a, 1 } } },
		{ YO_MOVE16, { { w, 2 }, { a, 2 } } },
		{ YO_MOVE32, { { w, 4 }, { a, 4 } } },
		{ YO_MOVE64, { { w, 8 }, { a, 8 } } },
		{ YO_MOVE80, { { w, 10 }, { a, 10 } } },
		{ YO_JUMP, { { a, 8 } } },
		{ YO_JUMPIF, { { a, 1 }, { a, 8 } } },
		{ YO_CALL, { { a, 8 } } },
		{ YO_RET, { } },
		{ YO_NOT8, { { w, 1 }, { a, 1 } } },
		{ YO_NOT16, { { w, 2 }, { a, 2 } } },
		{ YO_NOT32, { { w, 4 }, { a, 4 } } },
		{ YO_NOT64, { { w, 8 }, { a, 8 } } },
		{ YO_AND8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_AND16, { { w, 2 }, { a, 2 }, { a, 2 } } },
		{ YO_AND32, { { w, 4 }, { a, 4 }, { a, 4 } } },
		{ YO_AND64, { { w, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_OR8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_OR16, { { w, 2 }, { a, 2 }, { a, 2 } } },
		{ YO_OR32, { { w, 4 }, { a, 4 }, { a, 4 } } },
		{ YO_OR64, { { w, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_XOR8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_XOR16, { { w, 2 }, { a, 2 }, { a, 2 } } },
		{ YO_XOR32, { { w, 4 }, { a, 4 }, { a, 4 } } },
		{ YO_XOR64, { { w, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_LSHIFT8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_LSHIFT16, { { w, 2 }, { a, 2 }, { a, 1 } } },
		{ YO_LSHIFT32, { { w, 4 }, { a, 4 }, { a, 1 } } },
		{ YO_LSHIFT64, { { w, 8 }, { a, 8 }, { a, 1 } } },
		{ YO_SRSHIFT8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_SRSHIFT16, { { w, 2 }, { a, 2 }, { a, 1 } } },
		{ YO_SRSHIFT32, { { w, 4 }, { a, 4 }, { a, 1 } } },
		{ YO_SRSHIFT64, { { w, 8 }, { a, 8 }, { a, 1 } } },
		{ YO_URSHIFT8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_URSHIFT16, { { w, 2 }, { a, 2 }, { a, 1 } } },
		{ YO_URSHIFT32, { { w, 4 }, { a, 4 }, { a, 1 } } },
		{ YO_URSHIFT64, { { w, 8 }, { a, 8 }, { a, 1 } } },
		{ YO_S8CONVF80, { { w, 10 }, { a, 1 } } },
		{ YO_S16CONVF80, { { w, 10 }, { a, 2 } } },
		{ YO_S32CONVF80, { { w, 10 }, { a, 4 } } },
		{ YO_S64CONVF80, { { w, 10 }, { a, 8 } } },
		{ YO_U8CONVF80, { { w, 10 }, { a, 1 } } },
		{ YO_U16CONVF80, { { w, 10 }, { a, 2 } } },
		{ YO_U32CONVF80, { { w, 10 }, { a, 4 } } },
		{ YO_U64CONVF80, { { w, 10 }, { a, 8 } } },
		{ YO_F32CONVF80, { { w, 10 }, { a, 4 } } },
		{ YO_F64CONVF80, { { w, 10 }, { a, 8 } } },
		{ YO_F80CONVS8, { { w, 1 }, { a, 10 } } },
		{ YO_F80CONVS16, { { w, 2 }, { a, 10 } } },
		{ YO_F80CONVS32, { { w, 4 }, { a, 10 } } },
		{ YO_F80CONVS64, { { w, 8 }, { a, 10 } } },
		{ YO_F80CONVU8, { { w, 1 }, { a, 10 } } },
		{ YO_F80CONVU16, { { w, 2 }, { a, 10 } } },
		{ YO_F80CONVU32, { { w, 4 }, { a, 10 } } },
		{ YO_F80CONVU64, { { w, 8 }, { a, 10 } } },
		{ YO_F80CONVF32, { { w, 4 }, { a, 10 } } },
		{ YO_F80CONVF64, { { w, 8 }, { a, 10 } } },
		{ YO_IADD8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_IADD16, { { w, 2 }, { a, 2 }, { a, 2 } } },
		{ YO_IADD32, { { w, 4 }, { a, 4 }, { a, 4 } } },
		{ YO_IADD64, { { w, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_FADD32, { { w, 4 }, { a, 4 }, { a, 4 } } },
		{ YO_FADD64, { { w, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_FADD80, { { w, 10 }, { a, 10 }, { a, 10 } } },
		{ YO_ISUB8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_ISUB16, { { w, 2 }, { a, 2 }, { a, 2 } } },
		{ YO_ISUB32, { { w, 4 }, { a, 4 }, { a, 4 } } },
		{ YO_ISUB64, { { w, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_FSUB32, { { w, 4 }, { a, 4 }, { a, 4 } } },
		{ YO_FSUB64, { { w, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_FSUB80, { { w, 10 }, { a, 10 }, { a, 10 } } },
		{ YO_SMUL8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_SMUL16, { { w, 2 }, { a, 2 }, { a, 2 } } },
		{ YO_SMUL32, { { w, 4 }, { a, 4 }, { a, 4 } } },
		{ YO_SMUL64, { { w, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_UMUL8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_UMUL16, { { w, 2 }, { a, 2 }, { a, 2 } } },
		{ YO_UMUL32, { { w, 4 }, { a, 4 }, { a, 4 } } },
		{ YO_UMUL64, { { w, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_FMUL32, { { w, 4 }, { a, 4 }, { a, 4 } } },
		{ YO_FMUL64, { { w, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_FMUL80, { { w, 10 }, { a, 10 }, { a, 10 } } },
		{ YO_SDIV8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_SDIV16, { { w, 2 }, { a, 2 }, { a, 2 } } },
		{ YO_SDIV32, { { w, 4 }, { a, 4 }, { a, 4 } } },
		{ YO_SDIV64, { { w, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_UDIV8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_UDIV16, { { w, 2 }, { a, 2 }, { a, 2 } } },
		{ YO_UDIV32, { { w, 4 }, { a, 4 }, { a, 4 } } },
		{ YO_UDIV64, { { w, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_FDIV32, { { w, 4 }, { a, 4 }, { a, 4 } } },
		{ YO_FDIV64, { { w, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_FDIV80, { { w, 10 }, { a, 10 }, { a, 10 } } },
		{ YO_SMOD8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_SMOD16, { { w, 2 }, { a, 2 }, { a, 2 } } },
		{ YO_SMOD32, { { w, 4 }, { a, 4 }, { a, 4 } } },
		{ YO_SMOD64, { { w, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_UMOD8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_UMOD16, { { w, 2 }, { a, 2 }, { a, 2 } } },
		{ YO_UMOD32, { { w, 4 }, { a, 4 }, { a, 4 } } },
		{ YO_UMOD64, { { w, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_IEQ8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_IEQ16, { { w, 1 }, { a, 2 }, { a, 2 } } },
		{ YO_IEQ32, { { w, 1 }, { a, 4 }, { a, 4 } } },
		{ YO_IEQ64, { { w, 1 }, { a, 8 }, { a, 8 } } },
		{ YO_FEQ32, { { w, 1 }, { a, 4 }, { a, 4 } } },
		{ YO_FEQ64, { { w, 1 }, { a, 8 }, { a, 8 } } },
		{ YO_FEQ80, { { w, 1 }, { a, 10 }, { a, 10 } } },
		{ YO_INE8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_INE16, { { w, 1 }, { a, 2 }, { a, 2 } } },
		{ YO_INE32, { { w, 1 }, { a, 4 }, { a, 4 } } },
		{ YO_INE64, { { w, 1 }, { a, 8 }, { a, 8 } } },
		{ YO_FNE32, { { w, 1 }, { a, 4 }, { a, 4 } } },
		{ YO_FNE64, { { w, 1 }, { a, 8 }, { a, 8 } } },
		{ YO_FNE80, { { w, 1 }, { a, 10 }, { a, 10 } } },
		{ YO_SLT8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_SLT16, { { w, 1 }, { a, 2 }, { a, 2 } } },
		{ YO_SLT32, { { w, 1 }, { a, 4 }, { a, 4 } } },
		{ YO_SLT64, { { w, 1 }, { a, 8 }, { a, 8 } } },
		{ YO_ULT8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_ULT16, { { w, 1 }, { a, 2 }, { a, 2 } } },
		{ YO_ULT32, { { w, 1 }, { a, 4 }, { a, 4 } } },
		{ YO_ULT64, { { w, 1 }, { a, 8 }, { a, 8 } } },
		{ YO_FLT32, { { w, 1 }, { a, 4 }, { a, 4 } } },
		{ YO_FLT64, { { w, 1 }, { a, 8 }, { a, 8 } } },
		{ YO_FLT80, { { w, 1 }, { a, 10 }, { a, 10 } } },
		{ YO_SGT8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_SGT16, { { w, 1 }, { a, 2 }, { a, 2 } } },
		{ YO_SGT32, { { w, 1 }, { a, 4 }, { a, 4 } } },
		{ YO_SGT64, { { w, 1 }, { a, 8 }, { a, 8 } } },
		{ YO_UGT8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_UGT16, { { w, 1 }, { a, 2 }, { a, 2 } } },
		{ YO_UGT32, { { w, 1 }, { a, 4 }, { a, 4 } } },
		{ YO_UGT64, { { w, 1 }, { a, 8 }, { a, 8 } } },
		{ YO_FGT32, { { w, 1 }, { a, 4 }, { a, 4 } } },
		{ YO_FGT64, { { w, 1 }, { a, 8 }, { a, 8 } } },
		{ YO_FGT80, { { w, 1 }, { a, 10 }, { a, 10 } } },
		{ YO_SLE8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_SLE16, { { w, 1 }, { a, 2 }, { a, 2 } } },
		{ YO_SLE32, { { w, 1 }, { a, 4 }, { a, 4 } } },
		{ YO_SLE64, { { w, 1 }, { a, 8 }, { a, 8 } } },
		{ YO_ULE8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_ULE16, { { w, 1 }, { a, 2 }, { a, 2 } } },
		{ YO_ULE32, { { w, 1 }, { a, 4 }, { a, 4 } } },
		{ YO_ULE64, { { w, 1 }, { a, 8 }, { a, 8 } } },
		{ YO_FLE32, { { w, 1 }, { a, 4 }, { a, 4 } } },
		{ YO_FLE64, { { w, 1 }, { a, 8 }, { a, 8 } } },
		{ YO_FLE80, { { w, 1 }, { a, 10 }, { a, 10 } } },
		{ YO_SGE8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_SGE16, { { w, 1 }, { a, 2 }, { a, 2 } } },
		{ YO_SGE32, { { w, 1 }, { a, 4 }, { a, 4 } } },
		{ YO_SGE64, { { w, 1 }, { a, 8 }, { a, 8 } } },
		{ YO_UGE8, { { w, 1 }, { a, 1 }, { a, 1 } } },
		{ YO_UGE16, { { w, 1 }, { a, 2 }, { a, 2 } } },
		{ YO_UGE32, { { w, 1 }, { a, 4 }, { a, 4 } } },
		{ YO_UGE64, { { w, 1 }, { a, 8 }, { a, 8 } } },
		{ YO_FGE32, { { w, 1 }, { a, 4 }, { a, 4 } } },
		{ YO_FGE64, { { w, 1 }, { a, 8 }, { a, 8 } } },
		{ YO_FGE80, { { w, 1 }, { a, 10 }, { a, 10 } } },
		{ YO_SYSCALL0, { { w, 8 }, { a, 8 } } },
		{ YO_SYSCALL1, { { w, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_SYSCALL2, { { w, 8 }, { a, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_SYSCALL3, { { w, 8 }, { a, 8 }, { a, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_SYSCALL4, { { w, 8 }, { a, 8 }, { a, 8 }, { a, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_SYSCALL5, { { w, 8 }, { a, 8 }, { a, 8 }, { a, 8 }, { a, 8 }, { a, 8 }, { a, 8 } } },
		{ YO_SYSCALL6, { { w, 8 }, { a, 8 }, { a, 8 }, { a, 8 }, { a, 8 }, { a, 8 }, { a, 8 }, { a, 8 } } }
	};

	return m;
}

const map<ECY86OpCode, vector<CY86OperandConstraint>>& CY86OperandConstraints()
{
	static const map<ECY86OpCode, vector<CY86OperandConstraint>> m
		= build_CY86OperandConstraints();

	return m;
}

const vector<CY86OperandConstraint>& CY86OperandConstraints(ECY86OpCode opcode)
{
	auto it = CY86OperandConstraints().find(opcode);

	if (it == CY86OperandConstraints().end())
		throw logic_error("internal error: no such opcode");

	return it->second;
}
