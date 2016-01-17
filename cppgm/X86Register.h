#pragma once

enum X86Register
{
	XR_RAX, XR_RBX, XR_RCX, XR_RDX,
	XR_RSI, XR_RDI, XR_RBP, XR_RSP,
	XR_R8,  XR_R9,  XR_R10, XR_R11,
	XR_R12, XR_R13, XR_R14, XR_R15,

	XR_EAX,  XR_EBX,  XR_ECX,  XR_EDX,
	XR_ESI,  XR_EDI,  XR_EBP,  XR_ESP,
	XR_R8D,  XR_R9D,  XR_R10D, XR_R11D,
	XR_R12D, XR_R13D, XR_R14D, XR_R15D,

	XR_AX,   XR_BX,   XR_CX,   XR_DX,
	XR_SI,   XR_DI,   XR_BP,   XR_SP,
	XR_R8W,  XR_R9W,  XR_R10W, XR_R11W,
	XR_R12W, XR_R13W, XR_R14W, XR_R15W,

	XR_AL,   XR_BL,   XR_CL,   XR_DL,
	XR_SIL,  XR_DIL,  XR_BPL,  XR_SPL,
	XR_R8L,  XR_R9L,  XR_R10L, XR_R11L,
	XR_R12L, XR_R13L, XR_R14L, XR_R15L
};

X86Register X86RegisterGroup(X86Register reg);

size_t X86RegisterWidth(X86Register reg);

int X86RegisterCode(X86Register reg);

X86Register StringToX86Register(const string& str);

string X86RegisterToString(X86Register);

X86Register X86RegisterTruncate(X86Register, size_t width);

