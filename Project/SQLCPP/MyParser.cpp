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
#include<exception>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include<exception>
#include"llvmsql.h"
using namespace llvm;




/// AST end

using LR = std::vector<std::unique_ptr<ExprAST>> ;
using LL = std::vector<std::unique_ptr<ExprAST>> ;
token currtoken;

std::unique_ptr<StringLiteralAST> ParseStringLiteralAST()
{
	while (currtoken.token_kind == blank|| currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	if (currtoken.token_kind != literal_string)
	{
		throw std::runtime_error("expect string literal\n");
	}
	std::string temps = currtoken.token_value.string_literal;
	currtoken=gettok(); // consume 1 string token
				   // wait,  this string may be concat
	while (currtoken.token_kind == literal_string)
	{
		temps += currtoken.token_value.string_literal;
		currtoken=gettok(); // consume 1 string token
	}
	auto result = llvm::make_unique<StringLiteralAST>(temps);
	return std::move(result);
}

std::unique_ptr<IntLiteralAST> ParseIntLiteralAST()
{
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	if (currtoken.token_kind != literal_int)
	{
		throw std::runtime_error("expect int literal\n");
	}
	auto result = llvm::make_unique<IntLiteralAST>(currtoken.token_value.int_literal);
	currtoken = gettok(); // consume 1 int token
	return std::move(result);
}

std::unique_ptr<DoubleLiteralAST> ParseDoubleLiteralAST()
{
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	if (currtoken.token_kind != literal_double)
	{
		throw std::runtime_error("expect double literal\n");
	}
	auto result = llvm::make_unique<DoubleLiteralAST>(currtoken.token_value.double_literal);
	currtoken = gettok(); // consume 1 double token
	return std::move(result);
}

/*
std::unique_ptr<ParenExprAST> ParseParenExprAST()
{
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == left_bracket_mark)
	{
		currtoken = gettok();
		auto e = ParseExprAST();
		if (!(currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == right_bracket_mark))
			throw std::runtime_error("expected ')' ");
		else
			return llvm::make_unique<ParenExprAST>(e);
	}
	throw std::runtime_error("expected '(' ");
}
*/

std::unique_ptr<LiteralAST> ParseLiteralAST()
{
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	if (currtoken.token_kind == literal_double)
	{
		return ParseDoubleLiteralAST();
	}
	if (currtoken.token_kind == literal_int)
	{
		return ParseIntLiteralAST();
	}
	if (currtoken.token_kind == literal_string)
	{
		return ParseStringLiteralAST();
	}
	else
		throw std::runtime_error("expect literal(int,float or string\n)");
}

std::unique_ptr<IdAST> ParseIdAST()
{
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	if (currtoken.token_kind != id)
	{
		throw std::runtime_error("expect identifier \n)");
	}
	auto result = llvm::make_unique<IdAST>(currtoken.token_value.IdentifierStr);
	currtoken = gettok(); // consume 1 id
	return std::move(result);
};

/*
std::unique_ptr<CallAST> ParseCallAST()
{
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	decltype(ParseIdAST()) x = nullptr;
	try
	{
		ParseIdAST();
	}
	catch (std::runtime_error(s))
	{
		throw std::runtime_error(s);
	}
	std::string callee = x->getvalue()->IdentifierStr;
	if (!(currtoken.token_kind == symbol && currtoken.token_value.symbol_mark != left_bracket_mark))
	{
		throw std::runtime_error("expect '(' \n");
	}
	std::vector<std::unique_ptr<ExprAST>> args;
	args.push_back(ParseExprAST());
	while (currtoken.token_kind == symbol && currtoken.token_value.symbol_mark != comma_mark)
	{
		currtoken = gettok();    // consume 1 comma token
		args.push_back(ParseExprAST());
	}
	if (currtoken.token_kind == symbol && currtoken.token_value.symbol_mark != right_bracket_mark)
	{
		return llvm::make_unique<CallAST>(callee, args);
	}
	else
		throw std::runtime_error("expect ')'");

}
*/

std::unique_ptr<TablecolAST> ParseTablecolAST()
{
	auto n1 = ParseIdAST();
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == dot_mark)
	{
		currtoken = gettok();
		auto n2 = ParseIdAST();
		return llvm::make_unique<TablecolAST>(n1->getvalue()->IdentifierStr, n2->getvalue()->IdentifierStr);
	}
	else
	{
		return llvm::make_unique<TablecolAST>(n1->getvalue()->IdentifierStr);
	}
}

std::unique_ptr< ExistsSubqueryAST> ParseExistsSubqueryAST()
{
	currtoken = gettok();  // consume 'EXISTS' reserved word
	currtoken = gettok();  // consume '(' reserved word
	auto subquery = ParseSubqueryAST();
	currtoken = gettok();  // consume ')' reserved word
	return llvm::make_unique<ExistsSubqueryAST>(subquery);
};

std::unique_ptr<SubqueryAST> ParseSubqueryAST()
{
	currtoken = gettok();    // consume 'SELECT' reserved word
	bool distinct_flag, from_flag, where_flag, having_flag, group_flag, order_flag;
	bool group_ASC, order_ASC;
	LR lr;
}

class SubqueryAST final :public SimpleExprAST
{
	bool distinct_flag = false;
	std::vector<std::unique_ptr<ExprAST>> exprs;

	bool from_flag = false;
	std::vector<std::unique_ptr<TableRefsAST>> tbrefs;

	bool where_flag = false;
	std::unique_ptr<ExprAST> wherecond;

	bool having_flag = false;
	std::unique_ptr<ExprAST> havingcond;

	bool group_flag = false;
	std::vector<table_col> groupby_col_name;

	bool order_flag = false;
	std::vector<table_col> orderby_col_name;

public:
	SubqueryAST(bool distinct_flag, std::vector<std::unique_ptr<ExprAST>> exprs,
		bool from_flag, std::vector<std::unique_ptr<TableRefsAST>> tbrefs,
		bool where_flag, std::unique_ptr<ExprAST> wherecond,
		bool having_flag, std::unique_ptr<ExprAST> havingcond,
		bool group_flag, std::vector<table_col> groupby_col_name,
		bool order_flag, std::vector<table_col> orderby_col_name
	) :
		distinct_flag(distinct_flag), exprs(std::move(exprs)),
		from_flag(from_flag), tbrefs(std::move(tbrefs)),
		where_flag(where_flag), wherecond(std::move(wherecond)),
		having_flag(having_flag), havingcond(std::move(havingcond)),
		group_flag(group_flag), groupby_col_name(std::move(groupby_col_name)),
		order_flag(order_flag), orderby_col_name(std::move(orderby_col_name))
	{}
};
