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
#include<exception>
#include"llvmsql.h"


using namespace llvm;


namespace parser_nsp
{

	class StatementAST
	{
	public:
		virtual ~StatementAST()= default;
		
	};

	union datatype
	{
		int i;
		double d;
		std::string s;
	public:
		datatype() = default;
	};

	class CreateAST:public StatementAST{};

	class CreateTableAST :public CreateAST{};

	class CreateTableSimpleAST :public CreateTableAST
	{
		std::vector<std::unique_ptr<create_def>> create_defs;
	};

	class create_def
	{
		std::string col_name;
		std::unique_ptr<col_def> col_defs;
		bool primary;
		bool unique;
	public:
		create_def(bool primary,bool unique,const std::string& col_name,std::unique_ptr<col_def> col_defs):
			primary(primary),unique(unique),col_name(col_name),col_defs(std::move(col_defs)){}
	};

	class col_def
	{
		int dtype;
		std::unique_ptr<datatype> defaultvalue;
		bool nullable = true;
		bool unique = false;
		bool primary = false;
	public:
		col_def(int dtype, std::unique_ptr<parser_nsp::datatype> defaultvalue, bool nullable, bool unique, bool primary) :
			dtype(dtype), defaultvalue(std::move(defaultvalue)),
			nullable(nullable), unique(unique), primary(primary){}
		
	};

	class CreateTableSelectAST :public CreateTableAST
	{
		std::string table_name;
	public:
		CreateTableSelectAST(const std::string& table_name):
			table_name(table_name){}
	};

	class CreateTableLikeAST :public CreateTableAST
	{
		std::string table_name;
		std::string old_name;
	public:
		CreateTableLikeAST(const std::string& table_name, const std::string& old_name):
			table_name(table_name), old_name(old_name) {}
	};

	class CreateIndexAST :public CreateAST
	{
		std::string index_name;
		std::string table_name;
		std::string col_name;
	public:
		CreateIndexAST(const std::string& index_name, const std::string& table_name,const std::string& col_name):
			index_name(index_name),table_name(table_name),col_name(col_name){}
	};

	class DropAST :public StatementAST {};

	class DropTableAST :public DropAST
	{
		std::vector<std::string> table_list;

	public:
		DropTableAST(std::vector<std::string> table_list): table_list(std::move(table_list)){}
	};

	class DropIndexAST :public DropAST
	{
		std::string index_name;
		std::string table_name;
	public:
		DropIndexAST(const std::string& index_name,const std::string& table_name):
			index_name(std::move(index_name)),table_name(std::move(table_name)){}
	};

	class InsertAST :public StatementAST
	{
		std::string table_name;
		std::vector<std::string> col_name;
		std::vector<std::unique_ptr<ExprAST>> value_list;

	public:
		InsertAST(const std::string& table_name, std::vector<std::string>col_name, std::vector<std::unique_ptr<ExprAST>>) :
			table_name(table_name), col_name(col_name), value_list(value_list){}
	};
	
	class DeleteAST :public StatementAST
	{
		std::string table_name;
		std::unique_ptr< ExprAST> where_condition;

	public:

		DeleteAST(const std::string& table_name,std::unique_ptr<ExprAST>where_condition):
			table_name(table_name),where_condition(std::move(where_condition) ){}
	};

	class SelectAST :public StatementAST
	{
		bool distinct = false;
		std::vector<std::string> table_names;
		std::unique_ptr<ExprAST> where_cond;
		std::unique_ptr<ExprAST> having;
		std::unique_ptr<ExprAST> stm;
		std::string var;
	public:
		SelectAST(std::
	};
	
	class ExprAST
	{
	public:
		virtual ~ExprAST() = default;

	};

	class ExprOrExprAST:public ExprAST {};

	class ExprOrormarkExprAST :public ExprAST {};

	class ExprXorExprAST :public ExprAST {};
	
	class ExprAndExprAST :public ExprAST {};
	
	class ExprAndandmarkExprAST :public ExprAST {};
	
	class NotExprAST :public ExprAST {};
	
	class NotmarkExprAST :public ExprAST {};
	
	class BooleanPrimaryAST :public ExprAST{};

	class BPIsAST :public BooleanPrimaryAST {};

	class BPCoPredicateAST :public BooleanPrimaryAST {};

	class BPCoSubqueryAST :public BooleanPrimaryAST {};

	class PredicateAST:public BooleanPrimaryAST{};

	class BEInSubqueryAST :public PredicateAST {};

	class BEInExprseqAST :public PredicateAST {};

	class BEBetweenAndAST :public PredicateAST {};

	class BERegexpAST :public PredicateAST {};

	class BitExprAST :public PredicateAST {};

	class BEOrmarkBE :public BitExprAST {};

	class BEAndmarkBE :public BitExprAST {};

	class BELshiftmarkBE :public BitExprAST {};

	class BERshiftmarkBE :public BitExprAST {};

	class BEPlusmarkBE :public BitExprAST {};

	class BEMinusmarkBE :public BitExprAST {};

	class BEMultmarkBE :public BitExprAST {};

	class BEDivmarkBE :public BitExprAST {};

	class BEDivBE :public BitExprAST {};

	class BEModBE :public BitExprAST {};

	class BEModmarkBE :public BitExprAST {};

	class BEHatmarkBE :public BitExprAST {};

	class BEPlusmarkIE :public BitExprAST {};

	class BEMinusmarkIE :public BitExprAST {};

	class SimpleExprAST:public BitExprAST{};

	class LiteralAST:public SimpleExprAST{};

	class IdAST :public SimpleExprAST {};

	class CallAST :public SimpleExprAST {};

	class VarAST :public SimpleExprAST {};

	class SEOrormarkSEAST :public SimpleExprAST {};

	class PlusmarkSEAST :public SimpleExprAST {};

	class MinusmarkSEAST :public SimpleExprAST {};

	class TildemarkSEAST :public SimpleExprAST {};

	class NotmarkSEAST :public SimpleExprAST {};

	class BracketExprseqAST :public SimpleExprAST {};

	class SubqueryAST :public SimpleExprAST
	{
		std::unique_ptr<SelectAST> selectstm;
		SubqueryAST(std::unique_ptr<SelectAST> selectstm):selectstm(std::move(selectstm)){}
	};

	class ExistsSubqueryAST :public SimpleExprAST {};



	/// DoubleLiteralAST - Expression class for numeric literals like "1.0".
	class DoubleLiteralAST : public LiteralAST
	{
		double Val;

	public:
		DoubleLiteralAST(double Val) : Val(Val) {}

	};

	/// VariableExprAST - Expression class for referencing a variable, like "a".
	class VariableExprAST : public ExprAST
	{
		std::string Name;

	public:
		VariableExprAST(const std::string &Name) : Name(Name) {}

	};

	/// BinaryExprAST - Expression class for a binary operator.
	class BinaryExprAST : public ExprAST
	{
		char Op;
		std::unique_ptr<ExprAST> LHS, RHS;

	public:
		BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
			std::unique_ptr<ExprAST> RHS)
			: Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

	};

	/// CallExprAST - Expression class for function calls.
	class CallExprAST : public ExprAST
	{
		std::string Callee;
		std::vector<std::unique_ptr<ExprAST>> Args;

	public:
		CallExprAST(const std::string &Callee, std::vector<std::unique_ptr<ExprAST>> Args)
			: Callee(Callee), Args(std::move(Args)) {}

	};

	/// PrototypeAST - This class represents the "prototype" for a function,
	/// which captures its name, and its argument names (thus implicitly the number
	/// of arguments the function takes).
	class PrototypeAST
	{
		std::string Name;
		std::vector<std::string> Args;

	public:
		PrototypeAST(const std::string &Name, std::vector<std::string> Args)
			: Name(Name), Args(std::move(Args)) {}

		const std::string &getName() const { return Name; }
	};

	/// FunctionAST - This class represents a function definition itself.
	class FunctionAST
	{
		std::unique_ptr<PrototypeAST> Proto;
		std::unique_ptr<ExprAST> Body;

	public:
		FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body)
			: Proto(std::move(Proto)), Body(std::move(Body)) {}

	};

}

  /// CurTok/getNextToken - Provide a simple token buffer.  CurTok is the current
  /// token the parser is looking at.  getNextToken reads another token from the
  /// lexer and updates CurTok with its results.
static scan_nsp::token& CurTok;
static scan_nsp::token& getNextToken()
{
	return CurTok = std::move(scan_nsp::gettok());
}

/// BinopPrecedence - This holds the precedence for each binary operator that is
/// defined.
//static std::map<char, int> BinopPrecedence;

/*
/// GetTokPrecedence - Get the precedence of the pending binary operator token.
static int GetTokPrecedence()
{
	if (!isascii(CurTok))
		return -1;

	// Make sure it's a declared binop.
	int TokPrec = BinopPrecedence[CurTok];
	if (TokPrec <= 0)
		return -1;
	return TokPrec;
}
*/

/// LogError* - These are little helper functions for error handling.
std::unique_ptr<parser_nsp::ExprAST> LogError(const char *Str)
{
	fprintf(stderr, "Error: %s\n", Str);
	return nullptr;
}

std::unique_ptr<parser_nsp::PrototypeAST> LogErrorP(const char *Str)
{
	LogError(Str);
	return nullptr;
}

static std::unique_ptr<parser_nsp::ExprAST> ParseExpression();

/// numberexpr ::= number
static std::unique_ptr<parser_nsp::ExprAST> ParseDouble()
{
	auto Result = llvm::make_unique<parser_nsp::DoubleLiteralAST>(scan_nsp::double_literal);
	getNextToken(); // consume the number
	return std::move(Result);
}

/// parenexpr ::= '(' expression ')'
static std::unique_ptr<parser_nsp::ExprAST> ParseParenExpr()
{
	getNextToken(); // eat (.
	auto V = ParseExpression();
	if (!V)
		return nullptr;

	if (CurTok->token_value != scan_nsp::reserved_value( right_bracket_mark) )
		return LogError("expected ')'");
	getNextToken(); // eat ).
	return V;
}

/// identifierexpr
///   ::= identifier
///   ::= identifier '(' expression* ')'
static std::unique_ptr<parser_nsp::ExprAST> ParseIdentifierExpr()
{
	std::string IdName = scan_nsp::IdentifierStr;

	getNextToken(); // eat identifier.

	if (CurTok != '(') // Simple variable ref.
		return llvm::make_unique<parser_nsp::VariableExprAST>(IdName);

	// Call.
	getNextToken(); // eat (
	std::vector<std::unique_ptr<parser_nsp::ExprAST>> Args;
	if (CurTok != ')')
	{
		while (true)
		{
			if (auto Arg = ParseExpression())
				Args.push_back(std::move(Arg));
			else
				return nullptr;

			if (CurTok == ')')
				break;

			if (CurTok != ',')
				return LogError("Expected ')' or ',' in argument list");
			getNextToken();
		}
	}

	// Eat the ')'.
	getNextToken();

	return llvm::make_unique<parser_nsp::CallExprAST>(IdName, std::move(Args));
}

/// primary
///   ::= identifierexpr
///   ::= numberexpr
///   ::= parenexpr
static std::unique_ptr<parser_nsp::ExprAST> ParsePrimary()
{
	switch (CurTok.token_kind)
	{
	default:
		return LogError("unknown token when expecting an expression");
	case tok_identifier:
		return ParseIdentifierExpr();
	case tok_number:
		return ParseNumberExpr();
	case '(':
		return ParseParenExpr();
	}
}

/// binoprhs
///   ::= ('+' primary)*
static std::unique_ptr<parser_nsp::ExprAST> ParseBinOpRHS(int ExprPrec, std::unique_ptr<parser_nsp::ExprAST> LHS)
{
	// If this is a binop, find its precedence.
	while (true)
	{
		int TokPrec = GetTokPrecedence();

		// If this is a binop that binds at least as tightly as the current binop,
		// consume it, otherwise we are done.
		if (TokPrec < ExprPrec)
			return LHS;

		// Okay, we know this is a binop.
		int BinOp = CurTok;
		getNextToken(); // eat binop

						// Parse the primary expression after the binary operator.
		auto RHS = ParsePrimary();
		if (!RHS)
			return nullptr;

		// If BinOp binds less tightly with RHS than the operator after RHS, let
		// the pending operator take RHS as its LHS.
		int NextPrec = GetTokPrecedence();
		if (TokPrec < NextPrec)
		{
			RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
			if (!RHS)
				return nullptr;
		}

		// Merge LHS/RHS.
		LHS =
			llvm::make_unique<parser_nsp::BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
	}
}

/// expression
///   ::= primary binoprhs
///
static std::unique_ptr<parser_nsp::ExprAST> ParseExpression()
{
	auto LHS = ParsePrimary();
	if (!LHS)
		return nullptr;

	return ParseBinOpRHS(0, std::move(LHS));
}

/// prototype
///   ::= id '(' id* ')'
static std::unique_ptr<parser_nsp::PrototypeAST> ParsePrototype()
{
	if (CurTok != scan_nsp::tok_identifier)
		return LogErrorP("Expected function name in prototype");

	std::string FnName = scan_nsp::IdentifierStr;
	getNextToken();

	if (CurTok != '(')
		return LogErrorP("Expected '(' in prototype");

	std::vector<std::string> ArgNames;
	while (getNextToken() == tok_identifier)
		ArgNames.push_back(IdentifierStr);
	if (CurTok != ')')
		return LogErrorP("Expected ')' in prototype");

	// success.
	getNextToken(); // eat ')'.

	return llvm::make_unique<parser_nsp::PrototypeAST>(FnName, std::move(ArgNames));
}

/// definition ::= 'def' prototype expression
static std::unique_ptr<parser_nsp::FunctionAST> ParseDefinition()
{
	getNextToken(); // eat def.
	auto Proto = ParsePrototype();
	if (!Proto)
		return nullptr;

	if (auto E = ParseExpression())
		return llvm::make_unique<parser_nsp::FunctionAST>(std::move(Proto), std::move(E));
	return nullptr;
}

/// toplevelexpr ::= expression
static std::unique_ptr<parser_nsp::FunctionAST> ParseTopLevelExpr()
{
	if (auto E = ParseExpression())
	{
		// Make an anonymous proto.
		auto Proto = llvm::make_unique<parser_nsp::PrototypeAST>("__anon_expr",
			std::vector<std::string>());
		return llvm::make_unique<parser_nsp::FunctionAST>(std::move(Proto), std::move(E));
	}
	return nullptr;
}

/// external ::= 'extern' prototype
static std::unique_ptr<parser_nsp::PrototypeAST> ParseExtern()
{
	getNextToken(); // eat extern.
	return ParsePrototype();
}