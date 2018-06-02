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
	auto result = llvm::make_unique<IntLiteralAST>(currtoken.token_value.int_literal);
	currtoken = gettok(); // consume 1 int token
	return std::move(result);
}

std::unique_ptr<DoubleLiteralAST> ParseDoubleLiteralAST()
{
	auto result = llvm::make_unique<DoubleLiteralAST>(currtoken.token_value.double_literal);
	currtoken = gettok(); // consume 1 double token
	return std::move(result);
}
/*
std::unique_ptr<ParenExprAST> ParseParenExprAST()
{
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
