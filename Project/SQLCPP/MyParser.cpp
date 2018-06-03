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
#include"catalog.h"
using namespace llvm;

/// SQL begin

std::map<table_col, SQLcol> name_entity_map;

/// SQL end

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
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
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
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
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
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
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
		while (currtoken.token_kind == blank || currtoken.token_kind == comment)
		{
			currtoken = gettok();
		}
		return ParseDoubleLiteralAST();
	}
	if (currtoken.token_kind == literal_int)
	{
		while (currtoken.token_kind == blank || currtoken.token_kind == comment)
		{
			currtoken = gettok();
		}
		return ParseIntLiteralAST();
	}
	if (currtoken.token_kind == literal_string)
	{
		while (currtoken.token_kind == blank || currtoken.token_kind == comment)
		{
			currtoken = gettok();
		}
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
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
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


/*
std::unique_ptr< ExistsSubqueryAST> ParseExistsSubqueryAST()
{
	currtoken = gettok();  // consume 'EXISTS' reserved word
	currtoken = gettok();  // consume '(' reserved word
	auto subquery = ParseSubqueryAST();
	currtoken = gettok();  // consume ')' reserved word
	return llvm::make_unique<ExistsSubqueryAST>(subquery);
};
*/

/*
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
*/






std::unique_ptr<ColdefAST> ParseColdefAST()
{
	std::string colname = ParseIdAST()->getvalue()->IdentifierStr;
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_INT)
	{
		while (currtoken.token_kind == blank || currtoken.token_kind == comment)
		{
			currtoken = gettok();
		}
		currtoken = gettok();    // consume `INT`
		return llvm::make_unique<ColdefAST>(colname, literal_int, true, false, false);
	}
	if (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == tok_FLOAT || currtoken.token_value.symbol_mark == tok_DOUBLE))
	{
		while (currtoken.token_kind == blank || currtoken.token_kind == comment)
		{
			currtoken = gettok();
		}
		currtoken = gettok();    // consume `DOUBLE`
		return llvm::make_unique<ColdefAST>(colname, literal_double, true, false, false);
	}
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_CHAR)
	{
		currtoken = gettok();   // consume `CHAR`
		currtoken = gettok();   // consume `(`
		if (currtoken.token_kind == literal_int)
		{
			int n = currtoken.token_value.int_literal;
			currtoken = gettok();    // consume int literal
			currtoken = gettok();    // consume `)`
			while (currtoken.token_kind == blank || currtoken.token_kind == comment)
			{
				currtoken = gettok();
			}
			return llvm::make_unique<ColdefAST>(colname, literal_string, true, false, false, n);
		}
		throw std::runtime_error("char size must be int \n");
		
	}
	return nullptr;
}


std::unique_ptr<CreateTableSimpleAST> ParseCreateTableSimpleAST()
{
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	// consume ` CREATE TABLE `
	currtoken = gettok();
	currtoken = gettok();
	std::string table_name = ParseIdAST()->getvalue()->IdentifierStr;
	// consume `(`
	currtoken = gettok();
	std::vector<std::unique_ptr<ColdefAST>> cols;
	auto col = ParseColdefAST();
	col->tbname = table_name;
	cols.push_back(std::move(col));
	while (currtoken.token_kind == symbol&& currtoken.token_value.symbol_mark == comma_mark)
	{
		// consume `,`
		currtoken = gettok();
		auto col = ParseColdefAST();
		col->tbname = table_name;
		cols.push_back(std::move(col));
	}
	// consume `)`
	currtoken = gettok();
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	//return nullptr;
	return llvm::make_unique<CreateTableSimpleAST>(table_name, std::move(cols));
}
