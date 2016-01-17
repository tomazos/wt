#pragma once

struct Declaration;
struct RawExpression;
struct Condition;

struct Statement {};

struct DeclarationStatement : Statement
{
	Declaration* declaration;

	DeclarationStatement(Declaration* declaration)
		: declaration(declaration)
	{}
};

struct LabeledStatement : Statement
{
	string label;
	Statement* statement;

	LabeledStatement(const string& label, Statement* statement)
		: label(label)
		, statement(statement)
	{}
};

struct CaseStatement : Statement
{
	RawExpression* case_expression;
	Statement* statement;

	CaseStatement(RawExpression* case_expression, Statement* statement)
		: case_expression(case_expression)
		, statement(statement)
	{}
};

struct DefaultStatement : Statement
{
	Statement* statement;

	DefaultStatement(Statement* statement)
		: statement(statement)
	{}
};

struct EmptyStatement : Statement {};

struct CompoundStatement : Statement
{
	vector<Statement*> statements;

	void push_statement(Statement* statement)
	{
		statements.push_back(statement);
	}
};

struct ExpressionStatement : Statement
{
	RawExpression* expression;

	ExpressionStatement(RawExpression* expression)
		: expression(expression)
	{}
};

struct SwitchStatement : Statement
{
	Condition* condition;
	Statement* statement;

	SwitchStatement(Condition* condition, Statement* statement)
		: condition(condition)
		, statement(statement)
	{}
};

struct IfStatement : Statement
{
	Condition* condition;
	Statement *ontrue, *onfalse;

	IfStatement(Condition* condition, Statement* ontrue)
		: condition(condition)
		, ontrue(ontrue)
		, onfalse(nullptr)
	{}

	IfStatement(Condition* condition, Statement* ontrue, Statement* onfalse)
		: condition(condition)
		, ontrue(ontrue)
		, onfalse(onfalse)
	{}
};

struct WhileStatement : Statement
{
	Condition* condition;
	Statement* statement;

	WhileStatement(Condition* condition, Statement* statement)
		: condition(condition)
		, statement(statement)
	{}
};

struct DoStatement : Statement
{
	Statement* statement;
	RawExpression* expression;

	DoStatement(Statement* statement, RawExpression* expression)
		: statement(statement)
		, expression(expression)
	{}
};

struct ForStatement : Statement
{
	Statement* init_statement;
	Condition* condition;
	RawExpression* expression;
	Statement* statement;

	ForStatement(Statement* init_statement, Condition* condition, RawExpression* expression, Statement* statement)
		: init_statement(init_statement)
		, condition(condition)
		, expression(expression)
		, statement(statement)
	{}
};

struct BreakStatement : Statement {};

struct ContinueStatement : Statement {};

struct ReturnStatement : Statement
{
	RawExpression* expression;

	ReturnStatement(RawExpression* expression)
		: expression(expression)
	{}
};

struct GotoStatement : Statement
{
	string label;

	GotoStatement(const string& label)
		: label(label)
	{}
};
