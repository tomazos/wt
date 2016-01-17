# cppgm starter makefile

apps = \
	pptoken \
	posttoken \
	ctrlexpr \
	macro \
	preproc \
	recog \
	nsdecl \
	nsinit \
	cy86
	
units = \
	AnnotatedExpressions \
	AST \
	CppParser \
	CtrlExprParser \
	CY86AsmParser \
	CY86Instruction \
	Declarations \
	DebugPostTokenOutputStream \
	Declarations \
	EFundamentalType \
	Entities \
	ETokenType \
	MacroReplacement \
	Names \
	Pooled \
	PPTokenize \
	PreprocessingToken \
	Preprocessor \
	RawExpressions \
	SemC \
	SourcePositionTracker \
	Statements \
	Text \
	Tokenizer \
	Types \
	SemC \
	Statements \
	SymbolTable \
	X86Instruction \
	X86Program \
	X86Register

all: $(apps)

CXXFLAGS = -MD -g -O2 -std=gnu++11

clean:
	-rm $(apps) *.o *.d

$(apps): %: %-ref.o $(units:=.o)
	g++ -g -O2 -std=gnu++11 $^ -o $@

-include $(units:=.d) $(apps:=.d)

