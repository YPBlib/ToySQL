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

std::vector<std::unique_ptr<ExprAST>> LLstack;
std::vector<std::unique_ptr<ExprAST>> LRstack;
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
	args.push_back(ParseExprAST);

}
