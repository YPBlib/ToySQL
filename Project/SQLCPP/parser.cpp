#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>


// 
class value
{
public:
	//value(const value& a):s(a.s),i(a.i),d(a.d){}
	virtual ~value(){}
};


class int_value :public value
{
	int i;
public:
	int_value(int i):i(i){}
};

class double_value :public value
{
	double d;
public:
	double_value(double d):d(d){}
};

class string_value :public value
{
	std::string s;
public:
	string_value(std::string s):s(s){}
};

struct token
{
	int token_kind;
	value token_value;

	token(const token& a):token_kind(a.token_kind),token_value(a.token_value){}
};
///////


class StatementAST
{
public:
	virtual ~StatementAST() {}
};

class ExprAST:public StatementAST
{
public:
	virtual ~ExprAST(){}
};



class NumberExprAST :public ExprAST
{
	double Val;

public:
	NumberExprAST(double Val):Val(Val){}
};

class VarExprAST :public ExprAST
{
	using string = std::string;
	string VarName;
	
public:
	VarExprAST(const string& VarName):VarName(VarName){}
};

class BinaryExprAST :public ExprAST
{
	token op;
	std::unique_ptr<ExprAST> LHS, RHS;

public:
	BinaryExprAST(token op,std::unique_ptr<ExprAST> LHS,std::unique_ptr<ExprAST> RHS):
		op(op),LHS(std::move(LHS)),RHS(std::move(RHS)){}
};

class CallExprAST :public ExprAST
{
	using string = std::string;
	template <typename T>
	using vector = std::vector<T>;

	string Callee;
	vector<std::unique_ptr<ExprAST>> args;

public:
	CallExprAST(const string& Callee, vector<std::unique_ptr<ExprAST>> args) :
		Callee(Callee),args(args){}

};

class PrototypeAST
{
	using string = std::string;
	template <typename T>
	using vector = std::vector<T>;

	string Name;
	vector<string> Args;

public:
	PrototypeAST(const string& name, vector<string> args):
		Name(name), Args(args) {}

	const string& getName()
	{
		return Name;
	}
};

class FunctionAST
{
	template <typename T>
	using unique = std::unique_ptr<T>;

	unique<PrototypeAST> Proto;
	unique<ExprAST> Body;

public:
	FunctionAST(unique<PrototypeAST> proto,unique<ExprAST> body):
		Proto(move(proto)),Body(move(body)){}
};




class Create_Expr_AST :public ExprAST
{
	;
};


class Create_Table_AST :public Create_Expr_AST
{
	;
};










int main()
{
	;
}