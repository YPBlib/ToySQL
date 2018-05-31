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
	class ExprAST
	{
	public:
		virtual ~ExprAST() = default;
	};

	class BooleanPrimaryAST :public ExprAST {};
	class PredicateAST :public BooleanPrimaryAST {};
	class BitExprAST :public PredicateAST {};
	class SimpleExprAST :public BitExprAST {};
	class LiteralAST :public SimpleExprAST {};

	class table_col
	{
		using pair = std::pair<std::string, std::string>;
		pair p;
	public:
		typename pair::first_type& table_name = p.first;
		typename pair::second_type& col_name = p.second;
		table_col() = default;
		table_col(const std::string& table_name, const std::string& col_name) :p(table_name, col_name) {}
	};

	class IntLiteralAST :public LiteralAST
	{
		int value;
	public:
		IntLiteralAST(int value) :value(value) {}
	};

	class DoubleLiteralAST : public LiteralAST
	{
		double value;
	public:
		DoubleLiteralAST(double value) : value(value) {}
	};

	class StringLiteralAST :public LiteralAST
	{
		std::string value;
	public:
		StringLiteralAST(const std::string& value) :value(value) {}
	};

	class IdAST :public SimpleExprAST
	{
		std::string var_name;
		IdAST(const std::string& var_name) :var_name(var_name) {}
	};

	class CallAST :public SimpleExprAST
	{
		std::string callee;
		std::vector<std::unique_ptr<ExprAST>> args;
	public:
		CallAST(const std::string &callee, std::vector<std::unique_ptr<ExprAST>> args)
			: callee(callee), args(std::move(args)) {}
	};

	class VarAST :public SimpleExprAST
	{
		std::string var_name;
		VarAST(const std::string& var_name) :var_name(var_name) {}
	};

	class SEOrormarkSEAST :public SimpleExprAST
	{
		std::unique_ptr<SimpleExprAST> LHS;
		std::unique_ptr<SimpleExprAST> RHS;
		SEOrormarkSEAST(std::unique_ptr<SimpleExprAST> LHS, std::unique_ptr<SimpleExprAST> RHS) :
			LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	};

	class PlusmarkSEAST :public SimpleExprAST
	{
		std::unique_ptr<SimpleExprAST> se;
		PlusmarkSEAST(std::unique_ptr<SimpleExprAST> se) :se(std::move(se)) {}
	};

	class MinusmarkSEAST :public SimpleExprAST
	{
		std::unique_ptr<SimpleExprAST> se;
		MinusmarkSEAST(std::unique_ptr<SimpleExprAST> se) :se(std::move(se)) {}
	};

	class TildemarkSEAST :public SimpleExprAST
	{
		std::unique_ptr<SimpleExprAST> se;
		TildemarkSEAST(std::unique_ptr<SimpleExprAST> se) :se(std::move(se)) {}
	};

	class NotmarkSEAST :public SimpleExprAST
	{
		std::unique_ptr<SimpleExprAST> se;
		NotmarkSEAST(std::unique_ptr<SimpleExprAST> se) :se(std::move(se)) {}
	};

	class BracketExprseqAST :public SimpleExprAST
	{
		std::vector<std::unique_ptr<SimpleExprAST>> exprs;
		BracketExprseqAST(std::vector<std::unique_ptr<SimpleExprAST>> exprs) :exprs(std::move(exprs)) {}
	};

	class SubqueryAST :public SimpleExprAST
	{
		bool distinct_flag = false;
		bool where_flag = false;
		std::unique_ptr<ExprAST> wherecond;
		bool having_flag = false;
		std::unique_ptr<ExprAST> havingcond;
		bool group_flag = false;
		std::vector<table_col> groupby_col_name;
		bool order_flag = false;
		std::vector<table_col> orderby_col_name;
		bool into_flag = false;
		std::string into_var;
		std::vector<std::unique_ptr<ExprAST>> exprs;
		std::vector<std::string> table_name;
	public:
		SubqueryAST(bool distinct_flag, bool where_flag, std::unique_ptr<ExprAST> wherecond,
			bool having_flag, std::unique_ptr<ExprAST> havingcond,
			bool group_flag, std::vector<table_col> groupby_col_name,
			bool order_flag, std::vector<table_col> orderby_col_name,
			bool into_flag, const std::string& into_var,
			std::vector<std::unique_ptr<ExprAST>> exprs, std::vector<std::string> table_name) :
			distinct_flag(distinct_flag), where_flag(where_flag), wherecond(std::move(wherecond)),
			having_flag(having_flag), havingcond(std::move(havingcond)),
			group_flag(group_flag), groupby_col_name(std::move(groupby_col_name)),
			order_flag(order_flag), orderby_col_name(std::move(orderby_col_name)),
			into_flag(into_flag), into_var(into_var) {}
	};

	

	class ExistsSubqueryAST :public SimpleExprAST
	{
		std::unique_ptr<SubqueryAST> subquery;
	public:
		ExistsSubqueryAST(std::unique_ptr<SubqueryAST> subquery) :subquery(std::move(subquery)) {}
	};

	

	class BEOrmarkBE :public BitExprAST
	{
		std::unique_ptr<BitExprAST> LHS;
		std::unique_ptr<BitExprAST> RHS;
	public:
		BEOrmarkBE(std::unique_ptr<BitExprAST> LHS, std::unique_ptr<BitExprAST> RHS) :
			LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	};

	class BEAndmarkBE :public BitExprAST
	{
		std::unique_ptr<BitExprAST> LHS;
		std::unique_ptr<BitExprAST> RHS;
	public:
		BEAndmarkBE(std::unique_ptr<BitExprAST> LHS, std::unique_ptr<BitExprAST> RHS) :
			LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	};

	class BELshiftmarkBE :public BitExprAST
	{
		std::unique_ptr<BitExprAST> LHS;
		std::unique_ptr<BitExprAST> RHS;
	public:
		BELshiftmarkBE(std::unique_ptr<BitExprAST> LHS, std::unique_ptr<BitExprAST> RHS) :
			LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	};

	class BERshiftmarkBE :public BitExprAST
	{
		std::unique_ptr<BitExprAST> LHS;
		std::unique_ptr<BitExprAST> RHS;
	public:
		BERshiftmarkBE(std::unique_ptr<BitExprAST> LHS, std::unique_ptr<BitExprAST> RHS) :
			LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	};

	class BEPlusmarkBE :public BitExprAST
	{
		std::unique_ptr<BitExprAST> LHS;
		std::unique_ptr<BitExprAST> RHS;
	public:
		BEPlusmarkBE(std::unique_ptr<BitExprAST> LHS, std::unique_ptr<BitExprAST> RHS) :
			LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	};

	class BEMinusmarkBE :public BitExprAST
	{
		std::unique_ptr<BitExprAST> LHS;
		std::unique_ptr<BitExprAST> RHS;
	public:
		BEMinusmarkBE(std::unique_ptr<BitExprAST> LHS, std::unique_ptr<BitExprAST> RHS) :
			LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	};

	class BEMultmarkBE :public BitExprAST
	{
		std::unique_ptr<BitExprAST> LHS;
		std::unique_ptr<BitExprAST> RHS;
	public:
		BEMultmarkBE(std::unique_ptr<BitExprAST> LHS, std::unique_ptr<BitExprAST> RHS) :
			LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	};

	class BEDivmarkBE :public BitExprAST
	{
		std::unique_ptr<BitExprAST> LHS;
		std::unique_ptr<BitExprAST> RHS;
	public:
		BEDivmarkBE(std::unique_ptr<BitExprAST> LHS, std::unique_ptr<BitExprAST> RHS) :
			LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	};

	class BEDivBE :public BitExprAST
	{
		std::unique_ptr<BitExprAST> LHS;
		std::unique_ptr<BitExprAST> RHS;
	public:
		BEDivBE(std::unique_ptr<BitExprAST> LHS, std::unique_ptr<BitExprAST> RHS) :
			LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	};

	class BEModBE :public BitExprAST
	{
		std::unique_ptr<BitExprAST> LHS;
		std::unique_ptr<BitExprAST> RHS;
	public:
		BEModBE(std::unique_ptr<BitExprAST> LHS, std::unique_ptr<BitExprAST> RHS) :
			LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	};

	class BEModmarkBE :public BitExprAST
	{
		std::unique_ptr<BitExprAST> LHS;
		std::unique_ptr<BitExprAST> RHS;
	public:
		BEModmarkBE(std::unique_ptr<BitExprAST> LHS, std::unique_ptr<BitExprAST> RHS) :
			LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	};

	class BEHatmarkBE :public BitExprAST
	{
		std::unique_ptr<BitExprAST> LHS;
		std::unique_ptr<BitExprAST> RHS;
	public:
		BEHatmarkBE(std::unique_ptr<BitExprAST> LHS, std::unique_ptr<BitExprAST> RHS) :
			LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	};

	

	

	class BEInSubqueryAST :public PredicateAST
	{
		bool flag;
		std::unique_ptr<BitExprAST> be;
		std::unique_ptr<SubqueryAST> subquery;
	public:
		BEInSubqueryAST(bool flag, std::unique_ptr<BitExprAST> be, std::unique_ptr<SubqueryAST> subquery) :
			flag(flag), be(std::move(be)), subquery(std::move(subquery)) {}
	};

	class BEInExprseqAST :public PredicateAST
	{
		bool flag;
		std::unique_ptr<BitExprAST> be;
		std::vector<ExprAST> exprs;
	public:
		BEInExprseqAST(bool flag, std::unique_ptr<BitExprAST> be, std::vector<ExprAST> exprs) :
			flag(flag), be(std::move(be)), exprs(std::move(exprs)) {}
	};

	class BEBetweenAndAST :public PredicateAST
	{
		bool flag;
		std::unique_ptr<BitExprAST> LHS;
		std::unique_ptr<BitExprAST> RHS;
		std::unique_ptr<PredicateAST> P;
	public:
		BEBetweenAndAST(bool flag, std::unique_ptr<BitExprAST> LHS, std::unique_ptr<BitExprAST> RHS, std::unique_ptr<PredicateAST> P) :
			flag(flag), LHS(std::move(LHS)), RHS(std::move(RHS)) ,P(std::move(P)) {}
	};

	class BERegexpAST :public PredicateAST
	{
		bool flag;
		std::unique_ptr<BitExprAST> LHS;
		std::unique_ptr<BitExprAST> RHS;
	public:
		BERegexpAST(bool flag, std::unique_ptr<BitExprAST> LHS, std::unique_ptr<BitExprAST> RHS) :
			flag(flag), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	};
	
	

	class ExprOrExprAST :public ExprAST
	{
		std::unique_ptr<ExprAST> lhs;
		std::unique_ptr<ExprAST> rhs;
	public:
		ExprOrExprAST(std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs) :
			lhs(std::move(lhs)), rhs(std::move(rhs)) {}
	};

	class ExprOrormarkExprAST :public ExprAST
	{
		std::unique_ptr<ExprAST> lhs;
		std::unique_ptr<ExprAST> rhs;
	public:
		ExprOrormarkExprAST(std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs) :
			lhs(std::move(lhs)), rhs(std::move(rhs)) {}
	};

	class ExprXorExprAST :public ExprAST
	{
		std::unique_ptr<ExprAST> lhs;
		std::unique_ptr<ExprAST> rhs;
	public:
		ExprXorExprAST(std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs) :
			lhs(std::move(lhs)), rhs(std::move(rhs)) {}
	};

	class ExprAndExprAST :public ExprAST
	{
		std::unique_ptr<ExprAST> lhs;
		std::unique_ptr<ExprAST> rhs;
	public:
		ExprAndExprAST(std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs) :
			lhs(std::move(lhs)), rhs(std::move(rhs)) {}
	};

	class ExprAndandmarkExprAST :public ExprAST
	{
		std::unique_ptr<ExprAST> lhs;
		std::unique_ptr<ExprAST> rhs;
	public:
		ExprAndandmarkExprAST(std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs) :
			lhs(std::move(lhs)), rhs(std::move(rhs)) {}
	};

	class NotExprAST :public ExprAST
	{
		std::unique_ptr<ExprAST> expr;
	public:
		NotExprAST(std::unique_ptr<ExprAST> expr) :expr(std::move(expr)) {}
	};

	class NotmarkExprAST :public ExprAST
	{
		std::unique_ptr<ExprAST> expr;
	public:
		NotmarkExprAST(std::unique_ptr<ExprAST> expr) :expr(std::move(expr)) {}
	};

	

	class BPNULLAST :public BooleanPrimaryAST
	{
		bool flag;
		std::unique_ptr<BitExprAST> bp;
	public:
		BPNULLAST(bool flag, std::unique_ptr<BitExprAST> bp) :
			flag(flag), bp(std::move(bp)) {}
	};

	class BPCoPredicateAST :public BooleanPrimaryAST
	{
		bool flag;
		std::unique_ptr<BitExprAST> bp;
		std::unique_ptr<PredicateAST> predicate;
	public:
		BPCoPredicateAST(bool flag, std::unique_ptr<BitExprAST> bp, std::unique_ptr<PredicateAST> predicate) :
			flag(flag), bp(std::move(bp)), predicate(std::move(predicate)) {}
	};

	class BPCoSubqueryAST :public BooleanPrimaryAST
	{
		bool flag;
		int op;
		std::unique_ptr<BitExprAST> bp;
		std::unique_ptr<SubqueryAST> subquery;
	public:
		BPCoSubqueryAST(bool falg, int op, std::unique_ptr<BitExprAST> bp, std::unique_ptr<SubqueryAST> subquery) :
			flag(flag), op(op), bp(std::move(bp)), subquery(std::move(subquery)) {}
	};

	

	

	
	
	class StatementAST
	{
	public:
		virtual ~StatementAST()= default;
	};

	class datatype
	{
		int i;
		double d;
		std::string s;
	public:
		datatype(int i,double d,const std::string& s):i(i),d(d),s(s){}
		~datatype() = default;
	};

	class CreateAST:public StatementAST{};

	class CreateTableAST :public CreateAST{};
	class create_def;
	
	
	class col_def
	{
		int dtype;
		std::unique_ptr<datatype> defaultvalue;
		bool nullable = true;
		bool unique = false;
		bool primary = false;
	public:
		col_def(int dtype, std::unique_ptr<datatype> defaultvalue, bool nullable, bool unique, bool primary) :
			dtype(dtype), defaultvalue(std::move(defaultvalue)),
			nullable(nullable), unique(unique), primary(primary) {}

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

	class CreateTableSimpleAST :public CreateTableAST
	{
		std::vector<std::unique_ptr<create_def>> create_defs;
	public:
		CreateTableSimpleAST(std::vector<std::unique_ptr<create_def>> create_defs) :
			create_defs(std::move(create_defs)){}
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
		InsertAST(const std::string& table_name, std::vector<std::string>col_name, std::vector<std::unique_ptr<ExprAST>> value_list) :
			table_name(table_name), col_name(std::move(col_name)), value_list(std::move(value_list)){}
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
		std::unique_ptr<SubqueryAST> subquery;
	};
	
	

}

  /// CurTok/getNextToken - Provide a simple token buffer.  CurTok is the current
  /// token the parser is looking at.  getNextToken reads another token from the
  /// lexer and updates CurTok with its results.
/*
static scan_nsp::token& CurTok;
static scan_nsp::token& getNextToken()
{
	return CurTok = std::move(scan_nsp::gettok());
}
*/
/// BinopPrecedence - This holds the precedence for each binary operator that is
/// defined.
//  static std::map<char, int> BinopPrecedence;

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
/**
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

	return llvm::make_unique<parser_nsp::CallAST>(IdName, std::move(Args));
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


*/

