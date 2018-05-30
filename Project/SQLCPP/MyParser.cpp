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
#include"llvmsql.h"


using namespace llvm;


namespace parser_nsp
{

	/// ExprAST - Base class for all expression nodes.
	class ExprAST
	{
	public:
		virtual ~ExprAST() = default;

		virtual Value *codegen() = 0;
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

	class PredicateAST :public BooleanPrimaryAST {};

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

	class ParamAST :public SimpleExprAST {};

	class VarAST :public SimpleExprAST {};

	class SEOrormarkSEAST :public SimpleExprAST {};

	class PlusmarkSEAST :public SimpleExprAST {};

	class MinusmarkSEAST :public SimpleExprAST {};

	class TildemarkSEAST :public SimpleExprAST {};

	class NotmarkSEAST :public SimpleExprAST {};

	class BinarySEAST :public SimpleExprAST {};

	class BracketExprseqAST :public SimpleExprAST {};

	class SubqueryAST :public SimpleExprAST {};

	class ExistsSubqueryAST :public SimpleExprAST {};

	class MatchExprAST :public SimpleExprAST {};

	class CaseExprAST :public SimpleExprAST {};

	class IntervalExprAST :public SimpleExprAST {};


	/// DoubleLiteralAST - Expression class for numeric literals like "1.0".
	class DoubleLiteralAST : public LiteralAST
	{
		double Val;

	public:
		DoubleLiteralAST(double Val) : Val(Val) {}

		Value *codegen() override;
	};

	/// VariableExprAST - Expression class for referencing a variable, like "a".
	class VariableExprAST : public ExprAST
	{
		std::string Name;

	public:
		VariableExprAST(const std::string &Name) : Name(Name) {}

		Value *codegen() override;
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

		Value *codegen() override;
	};

	/// CallExprAST - Expression class for function calls.
	class CallExprAST : public ExprAST
	{
		std::string Callee;
		std::vector<std::unique_ptr<ExprAST>> Args;

	public:
		CallExprAST(const std::string &Callee, std::vector<std::unique_ptr<ExprAST>> Args)
			: Callee(Callee), Args(std::move(Args)) {}

		Value *codegen() override;
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

		Function *codegen();
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

		Function *codegen();
	};

}

  /// CurTok/getNextToken - Provide a simple token buffer.  CurTok is the current
  /// token the parser is looking at.  getNextToken reads another token from the
  /// lexer and updates CurTok with its results.
static scan_nsp::token* CurTok;
static scan_nsp::token getNextToken()
{
	return *CurTok = std::move(scan_nsp::gettok());
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
	switch (CurTok)
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
	if (CurTok != tok_identifier)
		return LogErrorP("Expected function name in prototype");

	std::string FnName = IdentifierStr;
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