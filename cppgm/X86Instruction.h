#pragma once

#include "Text.h"
#include "X86Register.h"

struct X86Instruction
{
	bool raw_data_active = false;
	vector<byte> raw_data;

	void set_raw_data(const vector<byte>& raw_data)
	{
		raw_data_active = true;
		this->raw_data = raw_data;
	}

	void fixup(size_t addr)
	{
		vector<byte>& data = (raw_data_active ? raw_data : immediate);

		if (data.size() != 8)
			throw logic_error("unexpected label fixup size: " + to_string(data.size()));

		data = to_byte_vector(addr);
	}

	bool rex_on_byte_reg = false;
	bool operand_size_prefix = false;
	bool REX = false;
	bool REX_W = false;
	bool REX_R = false;
	bool REX_X = false;
	bool REX_B = false;
	byte opcode1;
	bool opcode2_active = false;
	byte opcode2;
	bool modrm_active = false;
	int mod = 0;
	int reg = 0;
	int rm = 0;
	bool sib_active = false;
	int scale = 0;
	int index = 0;
	int base = 0;
	vector<byte> displacement;
	vector<byte> immediate;

	void dump()
	{
		cerr << "rex_on_byte_reg: " << rex_on_byte_reg << endl;
		cerr << "operand_size_prefix: " << operand_size_prefix << endl;
		cerr << "REX: " << REX << endl;
		cerr << "REX_W: " << REX_W << endl;
		cerr << "REX_R: " << REX_R << endl;
		cerr << "REX_X: " << REX_X << endl;
		cerr << "REX_B: " << REX_B << endl;
		cerr << "opcode1: " << opcode1 << endl;
		cerr << "opcode2_active: " << opcode2_active << endl;
		cerr << "opcode2: " << opcode2 << endl;
		cerr << "modrm_active: " << modrm_active << endl;
		cerr << "mod: " << mod << endl;
		cerr << "reg: " << reg << endl;
		cerr << "rm: " << rm << endl;
		cerr << "sib_active: " << sib_active << endl;
		cerr << "scale: " << scale << endl;
		cerr << "index: " << index << endl;
		cerr << "base: " << base << endl;
		cerr << "displacement: " << HexDump(displacement) << endl;
		cerr << "immediate: " << HexDump(immediate) << endl;

	}

	vector<byte> get_machine_code() const
	{
		if (raw_data_active)
			return raw_data;

		vector<byte> result;

		if (operand_size_prefix)
			result.push_back(0x66);

		if (REX || REX_W || REX_R || REX_X || REX_B)
		{
			byte rex = 0x40;

			if (REX_W) rex |= 8;
			if (REX_R) rex |= 4;
			if (REX_X) rex |= 2;
			if (REX_B) rex |= 1;

			result.push_back(rex);
		}

		result.push_back(opcode1);

		if (opcode2_active)
			result.push_back(opcode2);

		if (modrm_active)
		{
			byte modrm_byte = 0;

			modrm_byte |= mod << 6;
			modrm_byte |= reg << 3;
			modrm_byte |= rm;

//			cout << "modrm_byte: " << int(modrm_byte) << endl;
			result.push_back(modrm_byte);
		}

		if (sib_active)
		{
			byte sib_byte = 0;

			sib_byte |= scale << 6;
			sib_byte |= index << 3;
			sib_byte |= base;

			result.push_back(sib_byte);
		}

		result.insert(result.end(), displacement.begin(), displacement.end());
		result.insert(result.end(), immediate.begin(), immediate.end());

		return result;
	}

	size_t size() const { return get_machine_code().size(); }

	template<class T> void set_displacement(T t)
	{
		displacement.resize(sizeof(T));
		memcpy(&displacement[0], &t, sizeof(T));
	}

	void set_noary(byte opcode1_in, byte opcode2_in)
	{
		opcode1 = opcode1_in;
		opcode2_active = true;
		opcode2 = opcode2_in;
	}

	void set_unary(byte opcode_8, byte opcode_16_64,
	               size_t operand_size)
	{
		if (operand_size == 1)
		{
			rex_on_byte_reg = true;
			opcode1 = opcode_8;
		}
		else
			opcode1 = opcode_16_64;

		if (operand_size == 2)
			operand_size_prefix = true;

		if (operand_size == 8)
			REX_W = true;
	}

	void set_plus_reg(int code)
	{
		if (code < 0 || code >= 16)
			throw logic_error("invalid reg code");

		if (code & 8)
			REX_B = true;

		if (opcode2_active)
			opcode2 += code & 7;
		else
			opcode1 += code & 7;
	}

	void set_plus_reg(X86Register reg_in)
	{
		check_rex_reg(reg_in);

		int code = X86RegisterCode(reg_in);

		set_plus_reg(code);
	}

	void set_unary_reg_mem(byte opcode_origin,
	                       int reg_op, size_t operand_size)
	{
		if (reg > 7)
			throw logic_error("invalid reg/unary");

		set_unary(opcode_origin, opcode_origin+1, operand_size);
		modrm_active = true;
		reg = reg_op;
	}

	void set_binary_reg_mem(byte opcode_origin, size_t operand_size)
	{
		if (operand_size == 1)
		{
			rex_on_byte_reg = true;
			opcode1 = opcode_origin;
		}
		else
			opcode1 = opcode_origin+1;

		if (operand_size == 2)
			operand_size_prefix = true;

		if (operand_size == 8)
			REX_W = true;
	}

	void set_reg(int code)
	{
		modrm_active = true;

		if (code < 0 || code >= 16)
			throw logic_error("invalid reg code");

		if (code & 8)
			REX_R = true;

		reg = code & 7;
	}

	void check_rex_reg(X86Register reg_in)
	{
		if (rex_on_byte_reg && (reg_in == XR_SIL || reg_in == XR_DIL
			|| reg_in == XR_BPL || reg_in == XR_SPL))
			REX = true;
	}

	void set_reg(X86Register reg_in)
	{
		check_rex_reg(reg_in);
		set_reg(X86RegisterCode(reg_in));
	}

	void set_rm_reg(int code)
	{
		modrm_active = true;

		mod = 3;
		if (code & 8)
			REX_B = true;

		rm = code & 7;
	}

	void set_rm_reg(X86Register reg_in)
	{
		check_rex_reg(reg_in);
		set_rm_reg(X86RegisterCode(reg_in));
	}

	void set_rm_mem(X86Register reg_in, signed long int delta = 0)
	{
		if (X86RegisterWidth(reg_in) != 8)
			throw logic_error("internal error: non-64-bit addressing not supproted");

		modrm_active = true;

		if (delta == 0 && reg_in != XR_RBP && reg_in != XR_R13)
		{
			mod = 0;
		}
		else if (-0x100 <= delta && delta < 0x100)
		{
			signed char disp8 = delta;
			displacement = to_byte_vector(disp8);

			mod = 1;
		}
		else if (-0x100000000L <= delta && delta < 0x100000000L)
		{
			signed int disp32 = delta;
			displacement = to_byte_vector(disp32);

			mod = 2;
		}
		else
			throw logic_error("internal error: delta out-of-bounds");

		int code = X86RegisterCode(reg_in);

		if (code & 8)
				REX_B = true;

		rm = code & 7;

		if (rm == 4)
		{
			sib_active = true;

			base = code & 7;

			if (code & 8)
				REX_B = true;

			index = 4;
		}
	}
};

enum X86OperandCategory
{
	XOC_REGISTER,
	XOC_MEMORY
};

struct X86Operand
{
	X86Operand(X86Register reg_in)
		: operand_category(XOC_REGISTER)
		, base_register(reg_in)
		, operand_size(X86RegisterWidth(reg_in))
	{

	}

	struct memory_marker {};

	X86Operand(memory_marker, X86Register base_reg, signed long int delta, size_t operand_size)
		: operand_category(XOC_MEMORY)
		, base_register(base_reg)
		, memory_delta(delta)
		, operand_size(operand_size)
	{}

	X86OperandCategory operand_category;
	X86Register base_register;
	signed long int memory_delta = 0;
	size_t operand_size = 0;
};

inline X86Operand mem(X86Register base_register, signed long int delta = 0, size_t width = 0)
{
	return X86Operand(X86Operand::memory_marker{}, base_register, delta, width);
}

template<size_t i>
inline X86Operand mems(X86Register base_register, signed long int delta = 0)
{
	return X86Operand(X86Operand::memory_marker{}, base_register, delta, i);
}

inline X86Instruction X86NoaryInstruction(byte opcode1, byte opcode2)
{
	X86Instruction out;
	out.set_noary(opcode1, opcode2);
	return out;
}

inline X86Instruction X86UnaryInstruction(byte opcode1, int reg_op, X86Operand o, size_t operand_size = 0)
{
	X86Instruction out;

	if (o.operand_size == 0)
		o.operand_size = operand_size;

	if (operand_size != 0 && o.operand_size != operand_size)
		throw logic_error("invalid operand size");

	if (o.operand_category == XOC_MEMORY && o.operand_size == 0)
		throw logic_error("unary instruction with mem op requires explicit operand size");

	out.set_unary_reg_mem(opcode1, reg_op, o.operand_size);

	switch (o.operand_category)
	{
	case XOC_MEMORY: out.set_rm_mem(o.base_register, o.memory_delta); break;
	case XOC_REGISTER: out.set_rm_reg(o.base_register); break;
	}

	return out;
}

inline X86Instruction X86UnaryByteInstruction(byte opcode1, byte opcode2, X86Operand o)
{
	if (o.operand_size == 0)
		o.operand_size = 1;

	if (o.operand_size != 1)
		throw logic_error("invalid operand size");

	X86Instruction out;

	out.rex_on_byte_reg = true;
	out.opcode1 = opcode1;
	out.opcode2_active = opcode2;
	out.opcode2 = opcode2;

	switch (o.operand_category)
	{
	case XOC_MEMORY: out.set_rm_mem(o.base_register, o.memory_delta); break;
	case XOC_REGISTER: out.set_rm_reg(o.base_register); break;
	}

	return out;
}

inline X86Instruction X86RegImmInstruction(byte opcode_8, byte opcode_16_64,
                                    size_t operand_size, X86Register reg_in,
                                    vector<unsigned char> imm)
{
	X86Instruction out;

	out.set_unary(opcode_8, opcode_16_64, operand_size);

	out.set_plus_reg(reg_in);

	out.immediate = imm;

	return out;
}

inline X86Instruction X86MemUnaryInstruction(byte opcode1, int reg_op, X86Operand o)
{
	if (o.operand_category != XOC_MEMORY)
		throw logic_error("expected memory operand");

	X86Instruction out;

	out.opcode1 = opcode1;

	out.set_reg(reg_op);

	out.set_rm_mem(o.base_register, o.memory_delta);

	return out;
}

inline X86Instruction X86BinaryInstruction(byte opcode_left, byte opcode_right,
                                    X86Operand dest, X86Operand src)
{
	X86Instruction out;

	if (dest.operand_category == XOC_MEMORY && src.operand_category == XOC_MEMORY)
		throw logic_error("binary instruction with two memory operands");
	else if (dest.operand_category == XOC_REGISTER && src.operand_category == XOC_REGISTER)
	{
		if (dest.operand_size != src.operand_size)
			throw logic_error("operand size mismatch");

		out.set_binary_reg_mem(opcode_left, dest.operand_size);

		out.set_rm_reg(dest.base_register);
		out.set_reg(src.base_register);
	}
	else if (dest.operand_category == XOC_REGISTER && src.operand_category == XOC_MEMORY)
	{
		if (src.operand_size != 0 && src.operand_size != dest.operand_size)
			throw logic_error("operand size mismatch");

		out.set_binary_reg_mem(opcode_right, dest.operand_size);

		out.set_rm_mem(src.base_register, src.memory_delta);

		out.set_reg(dest.base_register);

	}
	else if (dest.operand_category == XOC_MEMORY && src.operand_category == XOC_REGISTER)
	{
		if (dest.operand_size != 0 && src.operand_size != dest.operand_size)
			throw logic_error("operand size mismatch");

		out.set_binary_reg_mem(opcode_left, src.operand_size);

		out.set_rm_mem(dest.base_register, dest.memory_delta);

		out.set_reg(src.base_register);
	}
	else
		throw logic_error("unexpected");

	return out;
}

inline X86Instruction XI_RAW(vector<byte> raw_data)
{
	X86Instruction out;
	out.set_raw_data(raw_data);
	return out;
}

inline X86Instruction XI_FADDP()   { return X86NoaryInstruction(0xDE, 0xC1); }
inline X86Instruction XI_FSUBP()   { return X86NoaryInstruction(0xDE, 0xE9); }
inline X86Instruction XI_FMULP()   { return X86NoaryInstruction(0xDE, 0xC9); }
inline X86Instruction XI_FDIVP()   { return X86NoaryInstruction(0xDE, 0xF9); }
inline X86Instruction XI_FCOMIP()  { return X86NoaryInstruction(0xDF, 0xF1); }
inline X86Instruction XI_FINCSTP() { return X86NoaryInstruction(0xD9, 0xF7); }
inline X86Instruction XI_FDECSTP() { return X86NoaryInstruction(0xD9, 0xF6); }
inline X86Instruction XI_SYSCALL() { return X86NoaryInstruction(0x0F, 0x05); }
inline X86Instruction XI_FFREE(int st) { return X86NoaryInstruction(0xDD, 0xC0+st); }

inline X86Instruction XI_SETE(X86Operand o)    { return X86UnaryByteInstruction(0x0F, 0x94, o); }
inline X86Instruction XI_SETNE(X86Operand o)   { return X86UnaryByteInstruction(0x0F, 0x95, o); }
inline X86Instruction XI_SETA(X86Operand o)    { return X86UnaryByteInstruction(0x0F, 0x97, o); }
inline X86Instruction XI_SETAE(X86Operand o)   { return X86UnaryByteInstruction(0x0F, 0x93, o); }
inline X86Instruction XI_SETB(X86Operand o)    { return X86UnaryByteInstruction(0x0F, 0x92, o); }
inline X86Instruction XI_SETBE(X86Operand o)   { return X86UnaryByteInstruction(0x0F, 0x96, o); }
inline X86Instruction XI_SETL(X86Operand o)    { return X86UnaryByteInstruction(0x0F, 0x9C, o); }
inline X86Instruction XI_SETLE(X86Operand o)   { return X86UnaryByteInstruction(0x0F, 0x9E, o); }
inline X86Instruction XI_SETG(X86Operand o)    { return X86UnaryByteInstruction(0x0F, 0x9F, o); }
inline X86Instruction XI_SETGE(X86Operand o)   { return X86UnaryByteInstruction(0x0F, 0x9D, o); }

inline X86Instruction XI_NOT(X86Operand o)  { return X86UnaryInstruction(0xF6, 2, o); }
inline X86Instruction XI_SHL(X86Operand o)  { return X86UnaryInstruction(0xD2, 4, o); }
inline X86Instruction XI_SHR(X86Operand o)  { return X86UnaryInstruction(0xD2, 5, o); }
inline X86Instruction XI_SAR(X86Operand o)  { return X86UnaryInstruction(0xD2, 7, o); }
inline X86Instruction XI_MUL(X86Operand o)  { return X86UnaryInstruction(0xF6, 4, o); }
inline X86Instruction XI_IMUL(X86Operand o) { return X86UnaryInstruction(0xF6, 5, o); }
inline X86Instruction XI_DIV(X86Operand o)  { return X86UnaryInstruction(0xF6, 6, o); }
inline X86Instruction XI_IDIV(X86Operand o) { return X86UnaryInstruction(0xF6, 7, o); }

inline X86Instruction XI_MOV(X86Operand dest, X86Operand src) { return X86BinaryInstruction(0x88, 0x8A, dest, src); }
inline X86Instruction XI_AND(X86Operand dest, X86Operand src) { return X86BinaryInstruction(0x20, 0x22, dest, src); }
inline X86Instruction XI_OR(X86Operand dest, X86Operand src)  { return X86BinaryInstruction(0x08, 0x0A, dest, src); }
inline X86Instruction XI_XOR(X86Operand dest, X86Operand src) { return X86BinaryInstruction(0x30, 0x32, dest, src); }
inline X86Instruction XI_ADD(X86Operand dest, X86Operand src) { return X86BinaryInstruction(0x00, 0x02, dest, src); }
inline X86Instruction XI_SUB(X86Operand dest, X86Operand src) { return X86BinaryInstruction(0x28, 0x2A, dest, src); }
inline X86Instruction XI_CMP(X86Operand dest, X86Operand src) { return X86BinaryInstruction(0x38, 0x3A, dest, src); }

inline X86Instruction XI_MOV(X86Operand dest, vector<byte> imm)
{
	if (dest.operand_category != XOC_REGISTER)
		throw logic_error("memory operand to MOV");

	if (dest.operand_size != imm.size())
		throw logic_error("operand size mismatch");

	return X86RegImmInstruction(0xB0, 0xB8, dest.operand_size, dest.base_register, imm);
}

inline X86Instruction XI_JMP(X86Operand target)
{
	if (target.operand_size != 0 && target.operand_size != 8)
		throw logic_error("operand size 8 required for JMP");

	X86Instruction out;
	out.opcode1 = 0xFF;
	out.reg = 4;
	out.modrm_active = true;

	switch (target.operand_category)
	{
	case XOC_MEMORY: out.set_rm_mem(target.base_register, target.memory_delta); break;
	case XOC_REGISTER: out.set_rm_reg(target.base_register); break;
	}

	return out;
}

inline X86Instruction XI_CALL(X86Operand target)
{
	if (target.operand_size != 0 && target.operand_size != 8)
		throw logic_error("operand size 8 required for JMP");

	X86Instruction out;
	out.opcode1 = 0xFF;
	out.reg = 2;
	out.modrm_active = true;

	switch (target.operand_category)
	{
	case XOC_MEMORY: out.set_rm_mem(target.base_register, target.memory_delta); break;
	case XOC_REGISTER: out.set_rm_reg(target.base_register); break;
	}

	return out;
}

inline X86Instruction XI_RET()
{
	X86Instruction instruction;

	instruction.opcode1 = 0xC3;

	return instruction;
}

inline X86Instruction XI_JNE(signed char delta)
{
	X86Instruction instruction;

	instruction.opcode1 = 0x75;
	instruction.immediate = to_byte_vector(delta);

	return instruction;
}

inline X86Instruction XI_JNE(int delta)
{
	X86Instruction instruction = X86NoaryInstruction(0xF, 0x85);

	instruction.immediate = to_byte_vector(delta);

	return instruction;
}

inline X86Instruction XI_JE(signed char delta)
{
	X86Instruction instruction;

	instruction.opcode1 = 0x74;
	instruction.immediate = to_byte_vector(delta);

	return instruction;
}

inline X86Instruction XI_JGE(signed char delta)
{
	X86Instruction instruction;

	instruction.opcode1 = 0x7D;
	instruction.immediate = to_byte_vector(delta);

	return instruction;
}

inline X86Instruction XI_JE(int delta)
{
	X86Instruction instruction = X86NoaryInstruction(0xF, 0x84);

	instruction.immediate = to_byte_vector(delta);

	return instruction;
}

//inline X86Instruction XI_JMP(signed char delta)
//{
//	X86Instruction instruction;
//
//	instruction.opcode1 = 0xEB;
//	instruction.set_immediate(delta);
//
//	return instruction;
//}


inline X86Instruction XI_FILD(X86Operand src)
{
	switch (src.operand_size)
	{
	case 2: return X86MemUnaryInstruction(0xDF, 0, src);
	case 4: return X86MemUnaryInstruction(0xDB, 0, src);
	case 8: return X86MemUnaryInstruction(0xDF, 5, src);
	default: throw logic_error("FILD invalid operand size: " + to_string(src.operand_size));
	}
}

inline X86Instruction XI_FLD(X86Operand src)
{
	switch (src.operand_size)
	{
	case 4: return X86MemUnaryInstruction(0xD9, 0, src);
	case 8: return X86MemUnaryInstruction(0xDD, 0, src);
	case 10: return X86MemUnaryInstruction(0xDB, 5, src);
	default: throw logic_error("FLD invalid operand size" + to_string(src.operand_size));
	}
}

inline X86Instruction XI_FISTP(X86Operand dest)
{
	switch (dest.operand_size)
	{
	case 2: return X86MemUnaryInstruction(0xDF, 3, dest);
	case 4: return X86MemUnaryInstruction(0xDB, 3, dest);
	case 8: return X86MemUnaryInstruction(0xDF, 7, dest);
	default: throw logic_error("FISTP invalid operand size");
	}
}

inline X86Instruction XI_FSTP(X86Operand dest)
{
	switch (dest.operand_size)
	{
	case 4: return X86MemUnaryInstruction(0xD9, 3, dest);
	case 8: return X86MemUnaryInstruction(0xDD, 3, dest);
	case 10: return X86MemUnaryInstruction(0xDB, 7, dest);
	default: throw logic_error("FSTP invalid operand size");
	}
}
