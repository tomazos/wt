#pragma once

#include "Pooled.h"
#include "X86Program.h"

enum ECY86LiteralType {
  YLT_FLOATING,
  YLT_SIGNEDINTEGER,
  YLT_UNSIGNEDINTEGER,
  YLT_OTHER
};

template <class T>
inline void generic_negate(vector<byte>& data, size_t bytes = sizeof(T)) {
  T t;
  memcpy(&t, &data[0], bytes);
  t = -t;
  memcpy(&data[0], &t, bytes);
}

inline void float_negate(vector<byte>& data) {
  if (data.size() < 4)
    throw logic_error("float smaller than 4 bytes??");
  else if (data.size() < 8)
    generic_negate<float>(data);
  else if (data.size() < 10)
    generic_negate<double>(data);
  else
    generic_negate<long double>(data, 10);
}

inline void int_negate(vector<byte>& data) {
  if (data.size() == 1)
    generic_negate<signed char>(data);
  else if (data.size() == 2)
    generic_negate<short int>(data);
  else if (data.size() == 4)
    generic_negate<int>(data);
  else if (data.size() == 8)
    generic_negate<long int>(data);
  else
    throw logic_error("internal error: cant negate int of that size?");
}

struct CY86Literal {
  CY86Literal(ECY86LiteralType type, const vector<byte>& data, size_t alignment)
      : type(type), data(data), alignment(alignment) {}

  ECY86LiteralType type;
  vector<byte> data;
  size_t alignment;

  size_t width() const { return data.size(); }

  void write(ostream& o) const {
    switch (type) {
      case YLT_FLOATING:
        o << "floating";
        break;
      case YLT_SIGNEDINTEGER:
        o << "signed";
        break;
      case YLT_UNSIGNEDINTEGER:
        o << "unsigned";
        break;
      case YLT_OTHER:
        o << "other";
        break;
      default:
        throw logic_error("internal error ECY86LiteralType");
    }

    o << " " << HexDump(data) << " align=" << alignment;
  }

  void negate() {
    switch (type) {
      case YLT_FLOATING:
        float_negate(data);

      case YLT_SIGNEDINTEGER:
      case YLT_UNSIGNEDINTEGER:
        int_negate(data);
        break;

      case YLT_OTHER:
        throw logic_error("invalid literal type to negate");
    }
  }

  signed long int to_delta64() {
    vector<byte> out = data;

    if (type == YLT_SIGNEDINTEGER) {
      if (out.size() > 8) throw logic_error("unexpected oversized literal");

      if (out.back() & 0x80)
        out.insert(out.end(), 8 - out.size(), 0xFF);
      else
        out.insert(out.end(), 8 - out.size(), 0x00);

      signed long int sli;
      memcpy(&sli, &out[0], 8);
      return sli;
    } else if (type == YLT_UNSIGNEDINTEGER) {
      if (out.size() > 8) throw logic_error("unexpected oversized literal");

      out.insert(out.end(), 8 - out.size(), 0x00);

      signed long int sli;
      memcpy(&sli, &out[0], 8);
      return sli;
    } else
      throw logic_error("expected integral delta type");
  }

  void adjust_width(size_t width) {
    if (width == data.size()) return;
    if (width < data.size()) {
      data.resize(width);

      if (width < alignment) alignment = width;
    } else {
      if (type == YLT_SIGNEDINTEGER) {
        if (data.back() & 0x80)
          data.insert(data.end(), width - data.size(), 0xFF);
        else
          data.insert(data.end(), width - data.size(), 0);
      } else
        data.insert(data.end(), width - data.size(), 0);
    }
  }
};

enum ECY86OpCode {
  YO_DATA8,
  YO_DATA16,
  YO_DATA32,
  YO_DATA64,
  YO_MOVE8,
  YO_MOVE16,
  YO_MOVE32,
  YO_MOVE64,
  YO_MOVE80,
  YO_JUMP,
  YO_JUMPIF,
  YO_CALL,
  YO_RET,
  YO_NOT8,
  YO_NOT16,
  YO_NOT32,
  YO_NOT64,
  YO_AND8,
  YO_AND16,
  YO_AND32,
  YO_AND64,
  YO_OR8,
  YO_OR16,
  YO_OR32,
  YO_OR64,
  YO_XOR8,
  YO_XOR16,
  YO_XOR32,
  YO_XOR64,
  YO_LSHIFT8,
  YO_LSHIFT16,
  YO_LSHIFT32,
  YO_LSHIFT64,
  YO_SRSHIFT8,
  YO_SRSHIFT16,
  YO_SRSHIFT32,
  YO_SRSHIFT64,
  YO_URSHIFT8,
  YO_URSHIFT16,
  YO_URSHIFT32,
  YO_URSHIFT64,
  YO_S8CONVF80,
  YO_S16CONVF80,
  YO_S32CONVF80,
  YO_S64CONVF80,
  YO_U8CONVF80,
  YO_U16CONVF80,
  YO_U32CONVF80,
  YO_U64CONVF80,
  YO_F32CONVF80,
  YO_F64CONVF80,
  YO_F80CONVS8,
  YO_F80CONVS16,
  YO_F80CONVS32,
  YO_F80CONVS64,
  YO_F80CONVU8,
  YO_F80CONVU16,
  YO_F80CONVU32,
  YO_F80CONVU64,
  YO_F80CONVF32,
  YO_F80CONVF64,
  YO_IADD8,
  YO_IADD16,
  YO_IADD32,
  YO_IADD64,
  YO_FADD32,
  YO_FADD64,
  YO_FADD80,
  YO_ISUB8,
  YO_ISUB16,
  YO_ISUB32,
  YO_ISUB64,
  YO_FSUB32,
  YO_FSUB64,
  YO_FSUB80,
  YO_SMUL8,
  YO_SMUL16,
  YO_SMUL32,
  YO_SMUL64,
  YO_UMUL8,
  YO_UMUL16,
  YO_UMUL32,
  YO_UMUL64,
  YO_FMUL32,
  YO_FMUL64,
  YO_FMUL80,
  YO_SDIV8,
  YO_SDIV16,
  YO_SDIV32,
  YO_SDIV64,
  YO_UDIV8,
  YO_UDIV16,
  YO_UDIV32,
  YO_UDIV64,
  YO_FDIV32,
  YO_FDIV64,
  YO_FDIV80,
  YO_SMOD8,
  YO_SMOD16,
  YO_SMOD32,
  YO_SMOD64,
  YO_UMOD8,
  YO_UMOD16,
  YO_UMOD32,
  YO_UMOD64,
  YO_IEQ8,
  YO_IEQ16,
  YO_IEQ32,
  YO_IEQ64,
  YO_FEQ32,
  YO_FEQ64,
  YO_FEQ80,
  YO_INE8,
  YO_INE16,
  YO_INE32,
  YO_INE64,
  YO_FNE32,
  YO_FNE64,
  YO_FNE80,
  YO_SLT8,
  YO_SLT16,
  YO_SLT32,
  YO_SLT64,
  YO_ULT8,
  YO_ULT16,
  YO_ULT32,
  YO_ULT64,
  YO_FLT32,
  YO_FLT64,
  YO_FLT80,
  YO_SGT8,
  YO_SGT16,
  YO_SGT32,
  YO_SGT64,
  YO_UGT8,
  YO_UGT16,
  YO_UGT32,
  YO_UGT64,
  YO_FGT32,
  YO_FGT64,
  YO_FGT80,
  YO_SLE8,
  YO_SLE16,
  YO_SLE32,
  YO_SLE64,
  YO_ULE8,
  YO_ULE16,
  YO_ULE32,
  YO_ULE64,
  YO_FLE32,
  YO_FLE64,
  YO_FLE80,
  YO_SGE8,
  YO_SGE16,
  YO_SGE32,
  YO_SGE64,
  YO_UGE8,
  YO_UGE16,
  YO_UGE32,
  YO_UGE64,
  YO_FGE32,
  YO_FGE64,
  YO_FGE80,
  YO_SYSCALL0,
  YO_SYSCALL1,
  YO_SYSCALL2,
  YO_SYSCALL3,
  YO_SYSCALL4,
  YO_SYSCALL5,
  YO_SYSCALL6,
};

typedef unsigned char CY86OperandTypes;
constexpr CY86OperandTypes YOT_IMMEDIATE = 1 << 0;
constexpr CY86OperandTypes YOT_REGISTER = 1 << 1;
constexpr CY86OperandTypes YOT_MEMORY = 1 << 2;

struct CY86OperandConstraint {
  CY86OperandTypes allowed_types;
  size_t width;

  string to_string() {
    ostringstream o;

    if (allowed_types & YOT_IMMEDIATE) o << "I";
    if (allowed_types & YOT_REGISTER) o << "R";
    if (allowed_types & YOT_MEMORY) o << "M";

    o << width;

    return o.str();
  }
};

ECY86OpCode StringToCY86OpCode(const string& s);

string CY86OpCodeToString(ECY86OpCode);

const map<ECY86OpCode, vector<CY86OperandConstraint>>& CY86OperandConstraints();

enum ECY86Register {
  YR_UNKNOWN,
  YR_SP,
  YR_BP,
  YR_X8,
  YR_X16,
  YR_X32,
  YR_X64,
  YR_Y8,
  YR_Y16,
  YR_Y32,
  YR_Y64,
  YR_Z8,
  YR_Z16,
  YR_Z32,
  YR_Z64,
  YR_T8,
  YR_T16,
  YR_T32,
  YR_T64
};

inline X86Register CY86RegisterToX86(ECY86Register reg) {
  switch (reg) {
    case YR_SP:
      return XR_RSP;
    case YR_BP:
      return XR_RBP;
    case YR_X8:
      return XR_R12L;
    case YR_X16:
      return XR_R12W;
    case YR_X32:
      return XR_R12D;
    case YR_X64:
      return XR_R12;
    case YR_Y8:
      return XR_R13L;
    case YR_Y16:
      return XR_R13W;
    case YR_Y32:
      return XR_R13D;
    case YR_Y64:
      return XR_R13;
    case YR_Z8:
      return XR_R14L;
    case YR_Z16:
      return XR_R14W;
    case YR_Z32:
      return XR_R14D;
    case YR_Z64:
      return XR_R14;
    case YR_T8:
      return XR_R15L;
    case YR_T16:
      return XR_R15W;
    case YR_T32:
      return XR_R15D;
    case YR_T64:
      return XR_R15;
    default:
      throw logic_error("unknown cy86 register?");
  }
}
ECY86Register StringToCY86Register(const string& str);

string CY86RegisterToString(ECY86Register);

inline size_t RegisterWidth(ECY86Register reg) {
  switch (reg) {
    case YR_SP:
    case YR_BP:
    case YR_X64:
    case YR_Y64:
    case YR_Z64:
    case YR_T64:
      return 8;

    case YR_X32:
    case YR_Y32:
    case YR_Z32:
    case YR_T32:
      return 4;

    case YR_X16:
    case YR_Y16:
    case YR_Z16:
    case YR_T16:
      return 2;

    case YR_X8:
    case YR_Y8:
    case YR_Z8:
    case YR_T8:
      return 1;

    default:
      throw logic_error("ECY86Register internal error");
  }
}

constexpr X86Register XR_SCRATCH64 = XR_RDI;
constexpr X86Register XR_SCRATCH32 = XR_EDI;
constexpr X86Register XR_SCRATCH16 = XR_DI;
constexpr X86Register XR_SCRATCH8 = XR_DIL;

constexpr X86Register XR_SCROTCH64 = XR_RSI;
constexpr X86Register XR_SCROTCH32 = XR_ESI;
constexpr X86Register XR_SCROTCH16 = XR_SI;
constexpr X86Register XR_SCROTCH8 = XR_SIL;

inline X86Register XR_SCRATCHW(size_t width) {
  switch (width) {
    case 1:
      return XR_SCRATCH8;
    case 2:
      return XR_SCRATCH16;
    case 4:
      return XR_SCRATCH32;
    case 8:
      return XR_SCRATCH64;
    default:
      throw logic_error("XR_SCRATCH Unknown size");
  }
}

inline X86Register XR_SCROTCHW(size_t width) {
  switch (width) {
    case 1:
      return XR_SCROTCH8;
    case 2:
      return XR_SCROTCH16;
    case 4:
      return XR_SCROTCH32;
    case 8:
      return XR_SCROTCH64;
    default:
      throw logic_error("XR_SCRATCH Unknown size");
  }
}

struct CY86Operand : Pooled {
  CY86Operand(CY86OperandTypes type) : type(type) {}

  CY86OperandTypes type;
  size_t width = 0;

  void apply_constraint(CY86OperandConstraint constraint) {
    if (!(type & constraint.allowed_types)) {
      cerr << "DEBUG: " << constraint.to_string() << endl;
      cerr << "DEBUG: " << int(type) << endl;

      throw logic_error("invalid operand type");
    }

    adjust_width(constraint.width);
  }

  virtual void adjust_width(size_t width) = 0;

  virtual void assemble_load(X86Program& program, X86Register reg) = 0;
  virtual void assemble_store(X86Program& program, X86Register reg) = 0;
  virtual void assemble_raw(X86Program&) {
    throw logic_error("internal error: immediate expected");
  }
  virtual void assemble_fpush(X86Program&) = 0;
  virtual void assemble_fpop(X86Program&) = 0;
  virtual void assemble_fipush(X86Program&, bool sgned) = 0;
  virtual void assemble_fipop(X86Program&, bool sgned) = 0;

  virtual void assemble_fild(X86Program& program, bool sgned) {
    if (sgned && width > 1) {
      program.add_instruction(XI_FILD(mem(XR_RSP, -16, width)));
    } else if (sgned && width == 1) {
      program.add_instruction(XI_MOV(XR_CL, to_byte_vector((byte)8)));
      program.add_instruction(XI_SHL(mem(XR_RSP, -16, 2)));
      program.add_instruction(XI_SAR(mem(XR_RSP, -16, 2)));
      program.add_instruction(XI_FILD(mem(XR_RSP, -16, 2)));
    } else if (!sgned && width < 8) {
      program.add_instruction(XI_XOR(XR_SCRATCH64, XR_SCRATCH64));
      program.add_instruction(XI_MOV(mem(XR_RSP, -16 + width), XR_SCRATCH64));
      program.add_instruction(XI_FILD(mem(XR_RSP, -16, 8)));
    } else if (!sgned && width == 8) {
      program.add_instruction(XI_FILD(mem(XR_RSP, -16, 8)));
      program.add_instruction(XI_XOR(XR_SCRATCH64, XR_SCRATCH64));
      program.add_instruction(XI_CMP(mem(XR_RSP, -16), XR_SCRATCH64));
      X86Position jne_pos_jl = program.add_instruction(XI_JGE((signed char)0));
      size_t A = program.nbytes;
      program.add_instruction(
          XI_MOV(XR_SCRATCH64, to_byte_vector((double)pow((double)2, 64))));
      program.add_instruction(XI_MOV(mem(XR_RSP, -16), XR_SCRATCH64));
      program.add_instruction(XI_FLD(mem(XR_RSP, -16, 8)));
      program.add_instruction(XI_FADDP());
      size_t B = program.nbytes;

      program[jne_pos_jl.index].immediate = to_byte_vector<signed char>(B - A);

    } else
      throw logic_error("unexpected sgned/width in assemble_fild");
  }

  virtual void assemble_fistp(X86Program& program, bool sgned) {
    if (!sgned && width == 8) {
      program.add_instruction(
          XI_MOV(XR_SCRATCH64, to_byte_vector(0x8000000000000000UL)));
      program.add_instruction(XI_MOV(mem(XR_RSP, -16), XR_SCRATCH64));
      program.add_instruction(XI_FILD(mem(XR_RSP, -16, 8)));
      program.add_instruction(XI_FADDP());
      program.add_instruction(XI_FISTP(mem(XR_RSP, -16, 8)));
      program.add_instruction(XI_SUB(mem(XR_RSP, -16, 8), XR_SCRATCH64));
    } else {
      program.add_instruction(XI_FISTP(mem(XR_RSP, -16, 8)));
    }
  }
};

struct CY86Register : CY86Operand {
  CY86Register(ECY86Register reg) : CY86Operand(YOT_REGISTER), reg(reg) {
    width = RegisterWidth(reg);
  }

  ECY86Register reg;

  virtual void adjust_width(size_t width) {
    if (RegisterWidth(reg) != width)
      throw logic_error("incorrect operand width");
  }

  virtual void write(ostream& o) const {
    o << "Register(" << CY86RegisterToString(reg) << ")";
  }

  virtual void assemble_load(X86Program& program, X86Register reg_out) {
    if (RegisterWidth(reg) != X86RegisterWidth(reg_out))
      throw logic_error("register width mismatch");

    program.add_instruction(XI_MOV(reg_out, CY86RegisterToX86(reg)));
  }

  virtual void assemble_store(X86Program& program, X86Register reg_in) {
    if (RegisterWidth(reg) != X86RegisterWidth(reg_in))
      throw logic_error("register width mismatch");

    program.add_instruction(XI_MOV(CY86RegisterToX86(reg), reg_in));
  }

  virtual void assemble_fpush(X86Program& program) {
    switch (width) {
      case 4:
      case 8:
      case 10:
        program.add_instruction(
            XI_MOV(mem(XR_RSP, -16), CY86RegisterToX86(reg)));
        program.add_instruction(XI_FLD(mem(XR_RSP, -16, width)));
        break;

      default:
        throw logic_error("invalid operand size for fpush");
    }
  }

  virtual void assemble_fpop(X86Program& program) {
    switch (width) {
      case 4:
      case 8:
      case 10:
        program.add_instruction(XI_FSTP(mem(XR_RSP, -16, width)));
        program.add_instruction(
            XI_MOV(CY86RegisterToX86(reg), mem(XR_RSP, -16)));
        break;

      default:
        throw logic_error("invalid operand size for fstore");
    }
  }

  virtual void assemble_fipush(X86Program& program, bool sgned) {
    program.add_instruction(XI_MOV(mem(XR_RSP, -16), CY86RegisterToX86(reg)));
    assemble_fild(program, sgned);
  }

  virtual void assemble_fipop(X86Program& program, bool sgned) {
    assemble_fistp(program, sgned);
    program.add_instruction(XI_MOV(CY86RegisterToX86(reg), mem(XR_RSP, -16)));
  }
};

struct CY86Immediate : CY86Operand {
  CY86Immediate() : CY86Operand(YOT_IMMEDIATE) {}
};

struct CY86AbsoluteImmediate : CY86Immediate {
  CY86AbsoluteImmediate(const CY86Literal& literal) : literal(literal) {
    width = literal.width();
  }

  CY86Literal literal;

  virtual void adjust_width(size_t width) {
    literal.adjust_width(width);
    this->width = literal.width();
  }

  virtual void write(ostream& o) const {
    o << "AbsoluteImmediate(";
    literal.write(o);
    o << ")";
  }

  virtual void assemble_load(X86Program& program, X86Register reg) {
    if (literal.width() != X86RegisterWidth(reg))
      throw logic_error("register width mismatch");

    program.add_instruction(XI_MOV(reg, literal.data));
  }

  virtual void assemble_store(X86Program&, X86Register) {
    throw logic_error("store to immediate?");
  }

  virtual void assemble_raw(X86Program& program) {
    program.add_instruction(XI_RAW(literal.data));
  }

  virtual void assemble_fpush(X86Program& program) {
    vector<byte> data = literal.data;

    switch (width) {
      case 4:
        program.add_instruction(XI_MOV(XR_SCRATCH32, data));
        program.add_instruction(XI_MOV(mem(XR_RSP, -16), XR_SCRATCH32));
        program.add_instruction(XI_FLD(mem(XR_RSP, -16, width)));
        break;

      case 8:
        program.add_instruction(XI_MOV(XR_SCRATCH64, data));
        program.add_instruction(XI_MOV(mem(XR_RSP, -16), XR_SCRATCH64));
        program.add_instruction(XI_FLD(mem(XR_RSP, -16, width)));
        break;

      case 10: {
        vector<byte> data0_8(data.begin(), data.begin() + 8);
        vector<byte> data8_10(data.begin() + 8, data.begin() + 10);

        program.add_instruction(XI_MOV(XR_SCRATCH64, data0_8));
        program.add_instruction(XI_MOV(mem(XR_RSP, -16), XR_SCRATCH64));
        program.add_instruction(XI_MOV(XR_SCRATCH16, data8_10));
        program.add_instruction(XI_MOV(mem(XR_RSP, -8), XR_SCRATCH16));
        program.add_instruction(XI_FLD(mem(XR_RSP, -16, width)));
        break;
      }

      default:
        throw logic_error("invalid operand size for fpush");
    }
  }

  virtual void assemble_fpop(X86Program&) {
    throw logic_error("fpop to immediate?");
  }

  virtual void assemble_fipush(X86Program& program, bool sgned) {
    program.add_instruction(XI_MOV(XR_SCRATCHW(width), literal.data));
    program.add_instruction(XI_MOV(mem(XR_RSP, -16), XR_SCRATCHW(width)));
    assemble_fild(program, sgned);
  }

  virtual void assemble_fipop(X86Program&, bool) {
    throw logic_error("fpop to immediate?");
  }
};

struct CY86LabelImmediate : CY86Immediate {
  CY86LabelImmediate(const string& label, signed long int delta)
      : label(label), delta(delta) {
    width = 8;
  }

  string label;
  signed long int delta;

  virtual void adjust_width(size_t width) { this->width = width; }

  virtual void write(ostream& o) const {
    o << "LabelImmediate(" << label << " " << delta;
    if (width != 8) o << " width=" << width;
    o << ")";
  }

  virtual void assemble_load(X86Program& program, X86Register reg) {
    if (width != X86RegisterWidth(reg))
      throw logic_error("invalid register width");

    program.add_instruction(XI_MOV(reg, vector<byte>(width, 0xAA)));
    program.add_immediate_fixup(label, delta);
  }

  virtual void assemble_store(X86Program&, X86Register) {
    throw logic_error("store to immediate?");
  }

  virtual void assemble_raw(X86Program& program) {
    program.add_instruction(XI_RAW(vector<byte>(width, 0xAA)));
    program.add_immediate_fixup(label, delta);
  }

  virtual void assemble_fpush(X86Program&) {
    throw logic_error("not implemented floating label immediate");
  }

  virtual void assemble_fpop(X86Program&) {
    throw logic_error("not implemented floating label immediate");
  }

  virtual void assemble_fipush(X86Program&, bool) {
    throw logic_error("not implemented floating label immediate");
  }

  virtual void assemble_fipop(X86Program&, bool) {
    throw logic_error("not implemented floating label immediate");
  }
};

struct CY86Memory : CY86Operand {
  CY86Memory() : CY86Operand(YOT_MEMORY) { width = 0; }

  virtual void adjust_width(size_t width) { this->width = width; }
};

struct CY86AbsoluteMemory : CY86Memory {
  CY86AbsoluteMemory(unsigned long int address) : address(address) {}

  unsigned long int address;

  virtual void write(ostream& o) const {
    o << "AbsoluteMemory(" << address << " " << width << ")";
  }

  virtual void assemble_load(X86Program& program, X86Register reg) {
    if (width != X86RegisterWidth(reg))
      throw logic_error("invalid register width");

    program.add_instruction(XI_MOV(XR_SCRATCH64, to_byte_vector(address)));
    program.add_instruction(XI_MOV(reg, mem(XR_SCRATCH64)));
  }

  virtual void assemble_store(X86Program& program, X86Register reg) {
    if (width != X86RegisterWidth(reg))
      throw logic_error("invalid register width");

    program.add_instruction(XI_MOV(XR_SCRATCH64, to_byte_vector(address)));
    program.add_instruction(XI_MOV(mem(XR_SCRATCH64), reg));
  }

  virtual void assemble_fpush(X86Program& program) {
    program.add_instruction(XI_MOV(XR_SCRATCH64, to_byte_vector(address)));
    program.add_instruction(XI_FLD(mem(XR_SCRATCH64, 0, width)));
  }

  virtual void assemble_fpop(X86Program& program) {
    program.add_instruction(XI_MOV(XR_SCRATCH64, to_byte_vector(address)));
    program.add_instruction(XI_FSTP(mem(XR_SCRATCH64, 0, width)));
  }

  virtual void assemble_fipush(X86Program& program, bool sgned) {
    program.add_instruction(XI_MOV(XR_SCRATCH64, to_byte_vector(address)));
    program.add_instruction(XI_MOV(XR_SCROTCHW(width), mem(XR_SCRATCH64, 0)));
    program.add_instruction(XI_MOV(mem(XR_RSP, -16), XR_SCROTCHW(width)));
    assemble_fild(program, sgned);
  }

  virtual void assemble_fipop(X86Program& program, bool sgned) {
    assemble_fistp(program, sgned);
    program.add_instruction(XI_MOV(XR_SCROTCHW(width), mem(XR_RSP, -16)));
    program.add_instruction(XI_MOV(XR_SCRATCH64, to_byte_vector(address)));
    program.add_instruction(
        XI_MOV(mem(XR_SCRATCH64, 0, width), XR_SCROTCHW(width)));
  }
};

struct CY86LabelMemory : CY86Memory {
  CY86LabelMemory(const string& label, signed long int delta)
      : label(label), delta(delta) {}

  string label;
  signed long int delta;

  virtual void write(ostream& o) const {
    o << "LabelMemory(" << label << " " << delta << " " << width << ")";
  }

  virtual void assemble_load(X86Program& program, X86Register reg) {
    if (width != X86RegisterWidth(reg))
      throw logic_error("invalid register width");

    program.add_instruction(XI_MOV(XR_SCRATCH64, vector<byte>(8, 0xAA)));
    program.add_immediate_fixup(label, delta);
    program.add_instruction(XI_MOV(reg, mem(XR_SCRATCH64)));
  }

  virtual void assemble_store(X86Program& program, X86Register reg) {
    if (width != X86RegisterWidth(reg))
      throw logic_error("invalid register width");

    program.add_instruction(XI_MOV(XR_SCRATCH64, vector<byte>(8, 0xAA)));
    program.add_immediate_fixup(label, delta);
    program.add_instruction(XI_MOV(mem(XR_SCRATCH64), reg));
  }

  virtual void assemble_fpush(X86Program& program) {
    program.add_instruction(XI_MOV(XR_SCRATCH64, vector<byte>(8, 0xAA)));
    program.add_immediate_fixup(label, delta);
    program.add_instruction(XI_FLD(mem(XR_SCRATCH64, 0, width)));
  }

  virtual void assemble_fpop(X86Program& program) {
    program.add_instruction(XI_MOV(XR_SCRATCH64, vector<byte>(8, 0xAA)));
    program.add_immediate_fixup(label, delta);
    program.add_instruction(XI_FSTP(mem(XR_SCRATCH64, 0, width)));
  }

  virtual void assemble_fipush(X86Program& program, bool sgned) {
    program.add_instruction(XI_MOV(XR_SCRATCH64, vector<byte>(8, 0xAA)));
    program.add_immediate_fixup(label, delta);
    program.add_instruction(XI_MOV(XR_SCROTCHW(width), mem(XR_SCRATCH64, 0)));
    program.add_instruction(XI_MOV(mem(XR_RSP, -16), XR_SCROTCHW(width)));
    assemble_fild(program, sgned);
  }

  virtual void assemble_fipop(X86Program& program, bool sgned) {
    assemble_fistp(program, sgned);
    program.add_instruction(XI_MOV(XR_SCROTCHW(width), mem(XR_RSP, -16)));
    program.add_instruction(XI_MOV(XR_SCRATCH64, vector<byte>(8, 0xAA)));
    program.add_immediate_fixup(label, delta);
    program.add_instruction(
        XI_MOV(mem(XR_SCRATCH64, 0, width), XR_SCROTCHW(width)));
  }
};

struct CY86RegisterMemory : CY86Memory {
  CY86RegisterMemory(ECY86Register reg, signed long int delta)
      : reg(reg), delta(delta) {}

  ECY86Register reg;
  signed long int delta;

  virtual void write(ostream& o) const {
    o << "RegisterMemory(" << CY86RegisterToString(reg) << " " << delta << " "
      << width << ")";
  }

  virtual void assemble_load(X86Program& program, X86Register reg_out) {
    if (width != X86RegisterWidth(reg_out))
      throw logic_error("invalid register width");

    program.add_instruction(XI_MOV(XR_SCRATCH64, to_byte_vector(delta)));
    program.add_instruction(XI_ADD(XR_SCRATCH64, CY86RegisterToX86(reg)));
    program.add_instruction(XI_MOV(reg_out, mem(XR_SCRATCH64)));
  }

  virtual void assemble_store(X86Program& program, X86Register reg_in) {
    if (width != X86RegisterWidth(reg_in))
      throw logic_error("invalid register width");

    program.add_instruction(XI_MOV(XR_SCRATCH64, to_byte_vector(delta)));
    program.add_instruction(XI_ADD(XR_SCRATCH64, CY86RegisterToX86(reg)));
    program.add_instruction(XI_MOV(mem(XR_SCRATCH64), reg_in));
  }

  virtual void assemble_fpush(X86Program& program) {
    program.add_instruction(XI_MOV(XR_SCRATCH64, to_byte_vector(delta)));
    program.add_instruction(XI_ADD(XR_SCRATCH64, CY86RegisterToX86(reg)));
    program.add_instruction(XI_FLD(mem(XR_SCRATCH64, 0, width)));
  }

  virtual void assemble_fpop(X86Program& program) {
    program.add_instruction(XI_MOV(XR_SCRATCH64, to_byte_vector(delta)));
    program.add_instruction(XI_ADD(XR_SCRATCH64, CY86RegisterToX86(reg)));
    program.add_instruction(XI_FSTP(mem(XR_SCRATCH64, 0, width)));
  }

  virtual void assemble_fipush(X86Program& program, bool sgned) {
    program.add_instruction(XI_MOV(XR_SCRATCH64, to_byte_vector(delta)));
    program.add_instruction(XI_ADD(XR_SCRATCH64, CY86RegisterToX86(reg)));
    program.add_instruction(XI_MOV(XR_SCROTCHW(width), mem(XR_SCRATCH64, 0)));
    program.add_instruction(XI_MOV(mem(XR_RSP, -16), XR_SCROTCHW(width)));
    assemble_fild(program, sgned);
  }

  virtual void assemble_fipop(X86Program& program, bool sgned) {
    assemble_fistp(program, sgned);
    program.add_instruction(XI_MOV(XR_SCROTCHW(width), mem(XR_RSP, -16)));
    program.add_instruction(XI_MOV(XR_SCRATCH64, to_byte_vector(delta)));
    program.add_instruction(XI_ADD(XR_SCRATCH64, CY86RegisterToX86(reg)));
    program.add_instruction(
        XI_MOV(mem(XR_SCRATCH64, 0, width), XR_SCROTCHW(width)));
  }
};

struct CY86Object : Pooled {
  virtual size_t assemble(X86Program& program) = 0;
};

struct CY86LiteralStatement : CY86Object {
  CY86LiteralStatement(const CY86Literal& literal) : literal(literal) {}

  CY86Literal literal;

  virtual void write(ostream& o) const {
    o << "LiteralStatement(";
    literal.write(o);
    o << ")";
  }

  size_t assemble(X86Program& program) {
    program.pad_to_alignment(literal.alignment);
    auto position = program.add_instruction(XI_RAW(literal.data));
    return position.address;
  }
};

const vector<CY86OperandConstraint>& CY86OperandConstraints(ECY86OpCode opcode);

struct CY86Instruction : CY86Object {
  CY86Instruction(ECY86OpCode opcode, const vector<CY86Operand*> operands)
      : opcode(opcode), operands(operands) {
    transform_operands();
  }

  void transform_operands() {
    const vector<CY86OperandConstraint>& constraints =
        CY86OperandConstraints(opcode);

    if (constraints.size() != operands.size())
      throw logic_error("incorrect number of operands");

    for (size_t i = 0; i < constraints.size(); i++)
      operands[i]->apply_constraint(constraints[i]);
  }

  ECY86OpCode opcode;
  vector<CY86Operand*> operands;

  virtual void write(ostream& o) const {
    o << "Instruction(" << CY86OpCodeToString(opcode);

    for (auto operand : operands) o << " " << operand;

    o << ")";
  }

  size_t assemble(X86Program& program) {
    size_t pos = program.nbytes;

    switch (opcode) {
      case YO_DATA8:
        operands[0]->assemble_raw(program);
        break;

      case YO_DATA16:
        program.pad_to_alignment(2);
        pos = program.nbytes;
        operands[0]->assemble_raw(program);
        break;
      case YO_DATA32:
        program.pad_to_alignment(4);
        pos = program.nbytes;
        operands[0]->assemble_raw(program);
        break;
      case YO_DATA64:
        program.pad_to_alignment(8);
        pos = program.nbytes;
        operands[0]->assemble_raw(program);
        break;

      case YO_MOVE8:
      case YO_MOVE16:
      case YO_MOVE32:
      case YO_MOVE64:
        operands[1]->assemble_load(
            program, X86RegisterTruncate(XR_RAX, operands[1]->width));
        operands[0]->assemble_store(
            program, X86RegisterTruncate(XR_RAX, operands[1]->width));
        break;

      case YO_MOVE80:
        operands[1]->assemble_fpush(program);
        operands[0]->assemble_fpop(program);
        break;

      case YO_JUMP:
        operands[0]->assemble_load(program, XR_RAX);
        program.add_instruction(XI_JMP(XR_RAX));
        break;

      case YO_JUMPIF: {
        operands[0]->assemble_load(program, XR_AL);
        program.add_instruction(XI_MOV(XR_CL, to_byte_vector((byte)0)));
        program.add_instruction(XI_CMP(XR_AL, XR_CL));
        X86Position jne_pos = program.add_instruction(XI_JE((signed char)0));
        size_t start = program.nbytes;
        operands[1]->assemble_load(program, XR_RBX);
        program.add_instruction(XI_JMP(XR_RBX));
        size_t end = program.nbytes;
        program[jne_pos.index].immediate =
            to_byte_vector<signed char>(end - start);
      } break;

      case YO_CALL:
        operands[0]->assemble_load(program, XR_RAX);
        program.add_instruction(XI_CALL(XR_RAX));
        break;

      case YO_RET:
        program.add_instruction(XI_RET());
        break;

      case YO_NOT8:
      case YO_NOT16:
      case YO_NOT32:
      case YO_NOT64: {
        X86Register acc = X86RegisterTruncate(XR_RAX, operands[1]->width);
        operands[1]->assemble_load(program, acc);
        program.add_instruction(XI_NOT(acc));
        operands[0]->assemble_store(program, acc);
        break;
      }

      case YO_AND8:
      case YO_AND16:
      case YO_AND32:
      case YO_AND64:
      case YO_OR8:
      case YO_OR16:
      case YO_OR32:
      case YO_OR64:
      case YO_XOR8:
      case YO_XOR16:
      case YO_XOR32:
      case YO_XOR64:
      case YO_IADD8:
      case YO_IADD16:
      case YO_IADD32:
      case YO_IADD64:
      case YO_ISUB8:
      case YO_ISUB16:
      case YO_ISUB32:
      case YO_ISUB64:

      {
        auto CY86SimpleBinaryOpToX86 = [](ECY86OpCode opcode) {
          switch (opcode) {
            case YO_AND8:
            case YO_AND16:
            case YO_AND32:
            case YO_AND64:
              return XI_AND;
            case YO_OR8:
            case YO_OR16:
            case YO_OR32:
            case YO_OR64:
              return XI_OR;
            case YO_XOR8:
            case YO_XOR16:
            case YO_XOR32:
            case YO_XOR64:
              return XI_XOR;
            case YO_IADD8:
            case YO_IADD16:
            case YO_IADD32:
            case YO_IADD64:
              return XI_ADD;
            case YO_ISUB8:
            case YO_ISUB16:
            case YO_ISUB32:
            case YO_ISUB64:
              return XI_SUB;
            default:
              throw logic_error("internal error");
          }
        };

        X86Register acc1 = X86RegisterTruncate(XR_RAX, operands[1]->width);
        X86Register acc2 = X86RegisterTruncate(XR_RBX, operands[2]->width);

        operands[1]->assemble_load(program, acc1);
        operands[2]->assemble_load(program, acc2);
        program.add_instruction(CY86SimpleBinaryOpToX86(opcode)(acc1, acc2));
        operands[0]->assemble_store(program, acc1);
        break;
      }

      case YO_LSHIFT8:
      case YO_LSHIFT16:
      case YO_LSHIFT32:
      case YO_LSHIFT64: {
        X86Register acc = X86RegisterTruncate(XR_RAX, operands[1]->width);

        operands[1]->assemble_load(program, acc);
        operands[2]->assemble_load(program, XR_CL);
        program.add_instruction(XI_SHL(acc));
        operands[0]->assemble_store(program, acc);
        break;
      }

      case YO_SRSHIFT8:
      case YO_SRSHIFT16:
      case YO_SRSHIFT32:
      case YO_SRSHIFT64: {
        X86Register acc = X86RegisterTruncate(XR_RAX, operands[1]->width);

        operands[1]->assemble_load(program, acc);
        operands[2]->assemble_load(program, XR_CL);
        program.add_instruction(XI_SAR(acc));
        operands[0]->assemble_store(program, acc);
        break;
      }

      case YO_URSHIFT8:
      case YO_URSHIFT16:
      case YO_URSHIFT32:
      case YO_URSHIFT64: {
        X86Register acc = X86RegisterTruncate(XR_RAX, operands[1]->width);

        operands[1]->assemble_load(program, acc);
        operands[2]->assemble_load(program, XR_CL);
        program.add_instruction(XI_SHR(acc));
        operands[0]->assemble_store(program, acc);
        break;
      }

      case YO_S8CONVF80:
      case YO_S16CONVF80:
      case YO_S32CONVF80:
      case YO_S64CONVF80: {
        operands[1]->assemble_fipush(program, true);
        operands[0]->assemble_fpop(program);
        break;
      }

      case YO_U8CONVF80:
      case YO_U16CONVF80:
      case YO_U32CONVF80:
      case YO_U64CONVF80: {
        operands[1]->assemble_fipush(program, false);
        operands[0]->assemble_fpop(program);
        break;
      }

      case YO_F80CONVS8:
      case YO_F80CONVS16:
      case YO_F80CONVS32:
      case YO_F80CONVS64: {
        operands[1]->assemble_fpush(program);
        operands[0]->assemble_fipop(program, true);
        break;
      }

      case YO_F80CONVU8:
      case YO_F80CONVU16:
      case YO_F80CONVU32:
      case YO_F80CONVU64: {
        operands[1]->assemble_fpush(program);
        operands[0]->assemble_fipop(program, false);
        break;
      }

      case YO_F32CONVF80:
      case YO_F64CONVF80:
      case YO_F80CONVF32:
      case YO_F80CONVF64: {
        operands[1]->assemble_fpush(program);
        operands[0]->assemble_fpop(program);
        break;
      }

      case YO_FADD32:
      case YO_FADD64:
      case YO_FADD80: {
        operands[1]->assemble_fpush(program);
        operands[2]->assemble_fpush(program);
        program.add_instruction(XI_FADDP());
        operands[0]->assemble_fpop(program);
        break;
      }

      case YO_FSUB32:
      case YO_FSUB64:
      case YO_FSUB80: {
        operands[1]->assemble_fpush(program);
        operands[2]->assemble_fpush(program);
        program.add_instruction(XI_FSUBP());
        operands[0]->assemble_fpop(program);
        break;
      }

      case YO_FMUL32:
      case YO_FMUL64:
      case YO_FMUL80: {
        operands[1]->assemble_fpush(program);
        operands[2]->assemble_fpush(program);
        program.add_instruction(XI_FMULP());
        operands[0]->assemble_fpop(program);
        break;
      }

      case YO_FDIV32:
      case YO_FDIV64:
      case YO_FDIV80: {
        operands[1]->assemble_fpush(program);
        operands[2]->assemble_fpush(program);
        program.add_instruction(XI_FDIVP());
        operands[0]->assemble_fpop(program);
        break;
      }

      case YO_SMUL8:
      case YO_SMUL16:
      case YO_SMUL32:
      case YO_SMUL64: {
        X86Register acc1 = X86RegisterTruncate(XR_RAX, operands[1]->width);
        X86Register acc2 = X86RegisterTruncate(XR_RBX, operands[2]->width);

        operands[1]->assemble_load(program, acc1);
        operands[2]->assemble_load(program, acc2);
        program.add_instruction(XI_IMUL(acc2));
        operands[0]->assemble_store(program, acc1);
        break;
      }

      case YO_UMUL8:
      case YO_UMUL16:
      case YO_UMUL32:
      case YO_UMUL64: {
        X86Register acc1 = X86RegisterTruncate(XR_RAX, operands[1]->width);
        X86Register acc2 = X86RegisterTruncate(XR_RBX, operands[2]->width);

        operands[1]->assemble_load(program, acc1);
        operands[2]->assemble_load(program, acc2);
        program.add_instruction(XI_MUL(acc2));
        operands[0]->assemble_store(program, acc1);
        break;
      }

      case YO_SDIV8:
      case YO_SDIV16:
      case YO_SDIV32:
      case YO_SDIV64: {
        size_t w = operands[1]->width;

        X86Register acc1 = X86RegisterTruncate(XR_RAX, w);
        X86Register acc2 = X86RegisterTruncate(XR_RBX, w);

        operands[1]->assemble_load(program, acc1);

        if (X86RegisterWidth(acc1) == 1) {
          program.add_instruction(XI_MOV(XR_CL, to_byte_vector(byte(8))));
          program.add_instruction(XI_SHL(XR_AX));
          program.add_instruction(XI_SAR(XR_AX));
        } else {
          X86Register d = X86RegisterTruncate(XR_RDX, operands[1]->width);

          program.add_instruction(XI_MOV(d, acc1));
          program.add_instruction(
              XI_MOV(XR_CL, to_byte_vector(byte(8 * w - 1))));
          program.add_instruction(XI_SAR(d));
        }

        operands[2]->assemble_load(program, acc2);
        program.add_instruction(XI_IDIV(acc2));
        operands[0]->assemble_store(program, acc1);
        break;
      }

      case YO_UDIV8:
      case YO_UDIV16:
      case YO_UDIV32:
      case YO_UDIV64: {
        size_t w = operands[1]->width;

        X86Register acc1 = X86RegisterTruncate(XR_RAX, w);
        X86Register acc2 = X86RegisterTruncate(XR_RBX, w);

        if (X86RegisterWidth(acc1) == 1) {
          program.add_instruction(XI_XOR(XR_AX, XR_AX));
        } else {
          X86Register d = X86RegisterTruncate(XR_RDX, operands[1]->width);
          program.add_instruction(XI_XOR(d, d));
        }

        operands[1]->assemble_load(program, acc1);
        operands[2]->assemble_load(program, acc2);
        program.add_instruction(XI_DIV(acc2));
        operands[0]->assemble_store(program, acc1);
        break;
      }

      case YO_SMOD8:
      case YO_SMOD16:
      case YO_SMOD32:
      case YO_SMOD64: {
        size_t w = operands[1]->width;

        X86Register acc1 = X86RegisterTruncate(XR_RAX, w);
        X86Register acc2 = X86RegisterTruncate(XR_RBX, w);
        X86Register d = X86RegisterTruncate(XR_RDX, operands[1]->width);

        operands[1]->assemble_load(program, acc1);

        if (X86RegisterWidth(acc1) == 1) {
          program.add_instruction(XI_MOV(XR_CL, to_byte_vector(byte(8))));
          program.add_instruction(XI_SHL(XR_AX));
          program.add_instruction(XI_SAR(XR_AX));
        } else {
          program.add_instruction(XI_MOV(d, acc1));
          program.add_instruction(
              XI_MOV(XR_CL, to_byte_vector(byte(8 * w - 1))));
          program.add_instruction(XI_SAR(d));
        }

        operands[2]->assemble_load(program, acc2);
        program.add_instruction(XI_IDIV(acc2));

        if (X86RegisterWidth(acc1) == 1) {
          program.add_instruction(XI_MOV(XR_CL, to_byte_vector(byte(8))));
          program.add_instruction(XI_SHR(XR_AX));
          operands[0]->assemble_store(program, acc1);
        } else {
          operands[0]->assemble_store(program, d);
        }

        break;
      }

      case YO_UMOD8:
      case YO_UMOD16:
      case YO_UMOD32:
      case YO_UMOD64: {
        size_t w = operands[1]->width;

        X86Register acc1 = X86RegisterTruncate(XR_RAX, w);
        X86Register acc2 = X86RegisterTruncate(XR_RBX, w);
        X86Register d = X86RegisterTruncate(XR_RDX, operands[1]->width);

        if (X86RegisterWidth(acc1) == 1) {
          program.add_instruction(XI_XOR(XR_AX, XR_AX));
        } else {
          program.add_instruction(XI_XOR(d, d));
        }

        operands[1]->assemble_load(program, acc1);
        operands[2]->assemble_load(program, acc2);
        program.add_instruction(XI_DIV(acc2));

        if (X86RegisterWidth(acc1) == 1) {
          program.add_instruction(XI_MOV(XR_CL, to_byte_vector(byte(8))));
          program.add_instruction(XI_SHR(XR_AX));
          operands[0]->assemble_store(program, acc1);
        } else {
          operands[0]->assemble_store(program, d);
        }

        break;
      }

      case YO_IEQ8:
      case YO_IEQ16:
      case YO_IEQ32:
      case YO_IEQ64:
      case YO_INE8:
      case YO_INE16:
      case YO_INE32:
      case YO_INE64:
      case YO_SLT8:
      case YO_SLT16:
      case YO_SLT32:
      case YO_SLT64:
      case YO_ULT8:
      case YO_ULT16:
      case YO_ULT32:
      case YO_ULT64:
      case YO_SGT8:
      case YO_SGT16:
      case YO_SGT32:
      case YO_SGT64:
      case YO_UGT8:
      case YO_UGT16:
      case YO_UGT32:
      case YO_UGT64:
      case YO_SLE8:
      case YO_SLE16:
      case YO_SLE32:
      case YO_SLE64:
      case YO_ULE8:
      case YO_ULE16:
      case YO_ULE32:
      case YO_ULE64:
      case YO_SGE8:
      case YO_SGE16:
      case YO_SGE32:
      case YO_SGE64:
      case YO_UGE8:
      case YO_UGE16:
      case YO_UGE32:
      case YO_UGE64: {
        auto CY86CompareToX86 = [](ECY86OpCode opcode) {
          switch (opcode) {
            case YO_IEQ8:
            case YO_IEQ16:
            case YO_IEQ32:
            case YO_IEQ64:
              return XI_SETE;

            case YO_INE8:
            case YO_INE16:
            case YO_INE32:
            case YO_INE64:
              return XI_SETNE;

            case YO_SLT8:
            case YO_SLT16:
            case YO_SLT32:
            case YO_SLT64:
              return XI_SETL;

            case YO_ULT8:
            case YO_ULT16:
            case YO_ULT32:
            case YO_ULT64:
              return XI_SETB;

            case YO_SGT8:
            case YO_SGT16:
            case YO_SGT32:
            case YO_SGT64:
              return XI_SETG;

            case YO_UGT8:
            case YO_UGT16:
            case YO_UGT32:
            case YO_UGT64:
              return XI_SETA;

            case YO_SLE8:
            case YO_SLE16:
            case YO_SLE32:
            case YO_SLE64:
              return XI_SETLE;

            case YO_ULE8:
            case YO_ULE16:
            case YO_ULE32:
            case YO_ULE64:
              return XI_SETBE;

            case YO_SGE8:
            case YO_SGE16:
            case YO_SGE32:
            case YO_SGE64:
              return XI_SETGE;

            case YO_UGE8:
            case YO_UGE16:
            case YO_UGE32:
            case YO_UGE64:
              return XI_SETAE;

            default:
              throw logic_error("internal error: unexpected cy86 comparitor");
          }
        };

        size_t w = operands[1]->width;
        X86Register acc1 = X86RegisterTruncate(XR_RAX, w);
        X86Register acc2 = X86RegisterTruncate(XR_RBX, w);
        operands[1]->assemble_load(program, acc1);
        operands[2]->assemble_load(program, acc2);
        program.add_instruction(XI_CMP(acc1, acc2));
        program.add_instruction(CY86CompareToX86(opcode)(XR_CL));
        operands[0]->assemble_store(program, XR_CL);
        break;
      }

      case YO_FEQ32:
      case YO_FEQ64:
      case YO_FEQ80:
      case YO_FNE32:
      case YO_FNE64:
      case YO_FNE80:
      case YO_FLT32:
      case YO_FLT64:
      case YO_FLT80:
      case YO_FGT32:
      case YO_FGT64:
      case YO_FGT80:
      case YO_FLE32:
      case YO_FLE64:
      case YO_FLE80:
      case YO_FGE32:
      case YO_FGE64:
      case YO_FGE80: {
        auto CY86CompareToX86 = [](ECY86OpCode opcode) {
          switch (opcode) {
            case YO_FEQ32:
            case YO_FEQ64:
            case YO_FEQ80:
              return XI_SETE;

            case YO_FNE32:
            case YO_FNE64:
            case YO_FNE80:
              return XI_SETNE;

            case YO_FLT32:
            case YO_FLT64:
            case YO_FLT80:
              return XI_SETB;

            case YO_FGT32:
            case YO_FGT64:
            case YO_FGT80:
              return XI_SETA;

            case YO_FLE32:
            case YO_FLE64:
            case YO_FLE80:
              return XI_SETBE;

            case YO_FGE32:
            case YO_FGE64:
            case YO_FGE80:
              return XI_SETAE;

            default:
              throw logic_error("internal error: unexpected cy86 comparitor");
          }
        };
        operands[2]->assemble_fpush(program);
        operands[1]->assemble_fpush(program);
        program.add_instruction(XI_FCOMIP());
        program.add_instruction(XI_FFREE(0));
        program.add_instruction(CY86CompareToX86(opcode)(XR_CL));
        operands[0]->assemble_store(program, XR_CL);
        break;
      }

      case YO_SYSCALL6:
        operands[7]->assemble_load(program, XR_R9);
      case YO_SYSCALL5:
        operands[6]->assemble_load(program, XR_R8);
      case YO_SYSCALL4:
        operands[5]->assemble_load(program, XR_R10);
      case YO_SYSCALL3:
        operands[4]->assemble_load(program, XR_RDX);
      case YO_SYSCALL2:
        operands[3]->assemble_load(program, XR_RSI);
      case YO_SYSCALL1:
      case YO_SYSCALL0:

        operands[1]->assemble_load(program, XR_RAX);

        if (opcode != YO_SYSCALL0)
          operands[2]->assemble_load(
              program, XR_RDI);  // load last because rdi is scratch

        program.add_instruction(XI_SYSCALL());
        operands[0]->assemble_store(program, XR_RAX);
        break;
      default:
        throw logic_error("not yet implemented");
    }

    return pos;
  }
};

struct CY86Program {
  vector<CY86Object*> objects;

  map<string, size_t> label_to_index;
  map<size_t, vector<string>> index_to_labels;

  void add_labels(const vector<string>& labels) {
    for (auto label : labels) {
      if (label_to_index.count(label))
        throw logic_error("duplicate label: " + label);

      size_t index = objects.size() - 1;
      label_to_index[label] = index;
      index_to_labels[index].push_back(label);
    }
  }

  void add_literal_statement(const CY86Literal& literal) {
    objects.push_back(new CY86LiteralStatement(literal));
  }

  void add_instruction(string opcode_str,
                       const vector<CY86Operand*>& operands) {
    ECY86OpCode opcode = StringToCY86OpCode(opcode_str);

    objects.push_back(new CY86Instruction(opcode, operands));
  }

  void dump() {
    cerr << objects.size() << " objects:" << endl;

    for (size_t i = 0; i < objects.size(); i++)
      cerr << "[" << i << "] " << objects[i] << endl;

    cerr << "labels:" << endl;
    for (auto x : label_to_index) cerr << x.first << " " << x.second << endl;
  }

  X86Program x86_program;
  unordered_map<string, size_t> label_addresses;

  vector<byte> assemble() {
    for (size_t i = 0; i < objects.size(); i++) {
      size_t x86_address = objects[i]->assemble(x86_program);
      for (auto label : index_to_labels[i])
        label_addresses[label] = x86_address;
    }

    x86_program.fixup_labels(label_addresses);

    return x86_program.output();
  }
};
