#include "std.pch"

#include "X86Register.h"

X86Register X86RegisterTruncate(X86Register reg, size_t width)
{
	int pos = int(reg);

	if (pos < 0 || pos >= 16)
		throw logic_error("invalid X86RegisterTruncate input");

	switch (width)
	{
	case 8: return X86Register(pos);
	case 4: return X86Register(pos+16);
	case 2: return X86Register(pos+32);
	case 1: return X86Register(pos+48);
	default: throw logic_error("X86RegisterTruncate: invalid register width");
	}

}


X86Register X86RegisterGroup(X86Register reg)
{
	switch (reg)
	{
	case XR_RAX:
	case XR_EAX:
	case XR_AX:
	case XR_AL:
		return XR_RAX;
	case XR_RBX:
	case XR_EBX:
	case XR_BX:
	case XR_BL:
		return XR_RBX;
	case XR_RCX:
	case XR_ECX:
	case XR_CX:
	case XR_CL:
		return XR_RCX;
	case XR_RDX:
	case XR_EDX:
	case XR_DX:
	case XR_DL:
		return XR_RDX;
	case XR_RSI:
	case XR_ESI:
	case XR_SI:
	case XR_SIL:
		return XR_RSI;
	case XR_RDI:
	case XR_EDI:
	case XR_DI:
	case XR_DIL:
		return XR_RDI;
	case XR_RSP:
	case XR_ESP:
	case XR_SP:
	case XR_SPL:
		return XR_RSP;
	case XR_RBP:
	case XR_EBP:
	case XR_BP:
	case XR_BPL:
		return XR_RBP;
	case XR_R8:
	case XR_R8D:
	case XR_R8W:
	case XR_R8L:
		return XR_R8;
	case XR_R9:
	case XR_R9D:
	case XR_R9W:
	case XR_R9L:
		return XR_R9;
	case XR_R10:
	case XR_R10D:
	case XR_R10W:
	case XR_R10L:
		return XR_R10;
	case XR_R11:
	case XR_R11D:
	case XR_R11W:
	case XR_R11L:
		return XR_R11;
	case XR_R12:
	case XR_R12D:
	case XR_R12W:
	case XR_R12L:
		return XR_R12;
	case XR_R13:
	case XR_R13D:
	case XR_R13W:
	case XR_R13L:
		return XR_R13;
	case XR_R14:
	case XR_R14D:
	case XR_R14W:
	case XR_R14L:
		return XR_R14;
	case XR_R15:
	case XR_R15D:
	case XR_R15W:
	case XR_R15L:
		return XR_R15;
	default:
		throw logic_error("internal error: RegGroup RegGroup");
	}
}

size_t X86RegisterWidth(X86Register reg)
{
	switch (reg)
	{
	case XR_RAX:
	case XR_RBX:
	case XR_RCX:
	case XR_RDX:
	case XR_RSI:
	case XR_RDI:
	case XR_RBP:
	case XR_RSP:
	case XR_R8:
	case XR_R9:
	case XR_R10:
	case XR_R11:
	case XR_R12:
	case XR_R13:
	case XR_R14:
	case XR_R15:
		return 8;

	case XR_EAX:
	case XR_EBX:
	case XR_ECX:
	case XR_EDX:
	case XR_ESI:
	case XR_EDI:
	case XR_EBP:
	case XR_ESP:
	case XR_R8D:
	case XR_R9D:
	case XR_R10D:
	case XR_R11D:
	case XR_R12D:
	case XR_R13D:
	case XR_R14D:
	case XR_R15D:
		return 4;

	case XR_AX:
	case XR_BX:
	case XR_CX:
	case XR_DX:
	case XR_SI:
	case XR_DI:
	case XR_BP:
	case XR_SP:
	case XR_R8W:
	case XR_R9W:
	case XR_R10W:
	case XR_R11W:
	case XR_R12W:
	case XR_R13W:
	case XR_R14W:
	case XR_R15W:
		return 2;

	case XR_AL:
	case XR_BL:
	case XR_CL:
	case XR_DL:
	case XR_SIL:
	case XR_DIL:
	case XR_BPL:
	case XR_SPL:
	case XR_R8L:
	case XR_R9L:
	case XR_R10L:
	case XR_R11L:
	case XR_R12L:
	case XR_R13L:
	case XR_R14L:
	case XR_R15L:
		return 1;
	};

	throw logic_error("internal error: unknown register");
}

int X86RegisterCode(X86Register reg)
{
	switch (X86RegisterGroup(reg))
	{
	case XR_RAX: return 0;
	case XR_RBX: return 3;
	case XR_RCX: return 1;
	case XR_RDX: return 2;
	case XR_RSI: return 6;
	case XR_RDI: return 7;
	case XR_RBP: return 5;
	case XR_RSP: return 4;
	case XR_R8:  return 8;
	case XR_R9:  return 9;
	case XR_R10: return 10;
	case XR_R11: return 11;
	case XR_R12: return 12;
	case XR_R13: return 13;
	case XR_R14: return 14;
	case XR_R15: return 15;
	default:
		throw logic_error("internal error: X86RegisterCode");
	}
}

struct X86RegisterName
{
	X86Register reg;
	const char* name;
};

X86RegisterName X86RegisterNames[] =
{
	{ XR_RAX,  "rax"  }, { XR_RBX,  "rbx"  },
	{ XR_RCX,  "rcx"  }, { XR_RDX,  "rdx"  },
	{ XR_RSI,  "rsi"  }, { XR_RDI,  "rdi"  },
	{ XR_RBP,  "rbp"  }, { XR_RSP,  "rsp"  },
	{ XR_R8,   "r8"   }, { XR_R9,   "r9"   },
	{ XR_R10,  "r10"  }, { XR_R11,  "r11"  },
	{ XR_R12,  "r12"  }, { XR_R13,  "r13"  },
	{ XR_R14,  "r14"  }, { XR_R15,  "r15"  },

	{ XR_EAX,  "eax"  }, { XR_EBX,  "ebx"  },
	{ XR_ECX,  "ecx"  }, { XR_EDX,  "edx"  },
	{ XR_ESI,  "esi"  }, { XR_EDI,  "edi"  },
	{ XR_EBP,  "ebp"  }, { XR_ESP,  "esp"  },
	{ XR_R8D,  "r8d"  }, { XR_R9D,  "r9d"  },
	{ XR_R10D, "r10d" }, { XR_R11D, "r11d" },
	{ XR_R12D, "r12d" }, { XR_R13D, "r13d" },
	{ XR_R14D, "r14d" }, { XR_R15D, "r15d" },

	{ XR_AX,   "ax"   }, { XR_BX,   "bx"   },
	{ XR_CX,   "cx"   }, { XR_DX,   "dx"   },
	{ XR_SI,   "si"   }, { XR_DI,   "di"   },
	{ XR_BP,   "bp"   }, { XR_SP,   "sp"   },
	{ XR_R8W,  "r8w"  }, { XR_R9W,  "r9w"  },
	{ XR_R10W, "r10w" }, { XR_R11W, "r11w" },
	{ XR_R12W, "r12w" }, { XR_R13W, "r13w" },
	{ XR_R14W, "r14w" }, { XR_R15W, "r15w" },

	{ XR_AL,   "al"   }, { XR_BL,   "bl"   },
	{ XR_CL,   "cl"   }, { XR_DL,   "dl"   },
	{ XR_SIL,  "sil"  }, { XR_DIL,  "dil"  },
	{ XR_BPL,  "bpl"  }, { XR_SPL,  "spl"  },
	{ XR_R8L,  "r8l"  }, { XR_R9L,  "r9l"  },
	{ XR_R10L, "r10l" }, { XR_R11L, "r11l" },
	{ XR_R12L, "r12l" }, { XR_R13L, "r13l" },
	{ XR_R14L, "r14l" }, { XR_R15L, "r15l" }
};

X86Register StringToX86Register(const string& str)
{
	static map<string, X86Register> m = []
	{
		map<string, X86Register> y;

		for (auto x : X86RegisterNames)
			y[x.name] = x.reg;

		return y;
	}();

	auto it = m.find(str);

	if (it == m.end())
		throw logic_error("internal error: unknown x86 register");
	else
		return it->second;
}

string X86RegisterToString(X86Register reg)
{
	static map<X86Register, string> m = []
	{
		map<X86Register, string> y;

		for (auto x : X86RegisterNames)
			y[x.reg] = x.name;

		return y;
	}();

	auto it = m.find(reg);

	if (it == m.end())
		throw logic_error("internal error: unknown x86 register");
	else
		return it->second;
}
