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



bool iscompop(const int& i)
{
	return i == eq_mark || i == gteq_mark || i == gt_mark || i == lteq_mark || i == lt_mark || i == ltgt_mark || i == noteq_mark;
}



std::unique_ptr<ExprAST> ParseExprAST()
{
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	auto lhs = ParseExpAST();
	std::unique_ptr<ExprAST> rhs = nullptr;
	std::unique_ptr<int> op = nullptr;
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	if (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == and_mark ||
			currtoken.token_value.symbol_mark == andand_mark ||
			currtoken.token_value.symbol_mark == or_mark ||
			currtoken.token_value.symbol_mark == oror_mark)
		)
	{
		op = llvm::make_unique<int>(currtoken.token_value.symbol_mark);
		while (currtoken.token_kind == blank || currtoken.token_kind == comment)
		{
			currtoken = gettok();
		}
		rhs = ParseExprAST();
	}
		
	return llvm::make_unique<ExprAST>(lhs, op, rhs);
}

std::unique_ptr<ExpAST> ParseExpAST()
{
	std::unique_ptr<ExprAST> expr = nullptr;
	std::unique_ptr<BooleanPrimaryAST> bp = nullptr;
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	if (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == tok_NOT || currtoken.token_value.symbol_mark == not_mark))
	{
		currtoken = gettok();	// consume ! or NOT
		expr = ParseExprAST();
	}
	else
	{
		bp = ParseBPAST();
	}
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	return llvm::make_unique<ExpAST>(expr, bp);
}

std::unique_ptr<BooleanPrimaryAST> ParseBPAST()
{
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	auto p = ParsePredicateAST();
	if (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == tok_IS || iscompop(currtoken.token_value.symbol_mark))
		)
	{
		if (currtoken.token_value.symbol_mark == tok_IS)
		{
			currtoken = gettok();	// consume IS
			std::unique_ptr<bool> flag = nullptr;
			if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == tok_NOT)
			{
				flag = llvm::make_unique<bool>(false);
				currtoken = gettok();	// consume NOT
				currtoken = gettok();	// consume NULL
				return  llvm::make_unique<BooleanPrimaryAST>(llvm::make_unique<BooleanPrimaryAST>(p), flag);
			}
			else if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == tok_NULL)
			{
				flag = llvm::make_unique<bool>(true);
				currtoken = gettok();	// consume NULL
				return  llvm::make_unique<BooleanPrimaryAST>(llvm::make_unique<BooleanPrimaryAST>(p), flag);
			}
			else
			{
				throw std::runtime_error("expect IS [NOT] NULL \n");
			}
		}
		else
		{
			auto op = llvm::make_unique<int>(currtoken.token_value.symbol_mark);
			currtoken = gettok();	// consume op
			if ((currtoken.token_kind == symbol &&
				(currtoken.token_value.symbol_mark == tok_ALL || currtoken.token_value.symbol_mark == tok_ANY)
				))
			{
				currtoken = gettok();	// consume all/any
				currtoken = gettok();	// consume '('
				auto sub = ParseSubqueryAST();
				auto bp = llvm::make_unique<BooleanPrimaryAST>(p);
				return llvm::make_unique<BooleanPrimaryAST>(bp, sub, op);
			}
			else
			{
				auto bp = llvm::make_unique<BooleanPrimaryAST>(p);
				auto p2= ParsePredicateAST();
				return llvm::make_unique<BooleanPrimaryAST>(bp, p2, op);
			}
		}
		
	}
	else
	{
		while (currtoken.token_kind == blank || currtoken.token_kind == comment)
		{
			currtoken = gettok();
		}
		return llvm::make_unique<BooleanPrimaryAST>(p);
	}
}

std::unique_ptr<PredicateAST> ParsePredicateAST()
{
	auto bitexpr = ParseBitExprAST();
	if (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == tok_NOT ||
			currtoken.token_value.symbol_mark == tok_REGEXP || currtoken.token_value.symbol_mark == tok_IN))
	{
		if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == tok_NOT)
		{
			currtoken = gettok();	//consum NOT
			auto flag = llvm::make_unique<bool>(false);
			if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == tok_REGEXP)
			{
				currtoken = gettok();	//consume REGEXP
				auto rhs = ParseBitExprAST();
				return llvm::make_unique<PredicateAST>(bitexpr, rhs, flag);
			}
			else if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == tok_IN)
			{
				currtoken = gettok();	//consume IN
				if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == left_bracket_mark)
				{
					currtoken = gettok();	//consume '('
				}
				else
				{
					throw std::runtime_error("expect '(' \n");
				}
				if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == tok_SELECT)
				{
					auto sub = ParseSubqueryAST();
					return llvm::make_unique<PredicateAST>(bitexpr, sub, flag);
				}
				else
				{
					std::vector<std::unique_ptr<ExprAST>> exprs;
					auto expr = ParseExprAST();
					exprs.push_back(std::move(expr));
					while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
					{
						currtoken = gettok();	//consume ','
						expr = ParseExprAST();
						exprs.push_back(std::move(expr));
					}
					if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == right_bracket_mark)
					{
						currtoken = gettok();	//consume ')'
						return llvm::make_unique<PredicateAST>(bitexpr, exprs, flag);
					}
					else
					{
						throw std::runtime_error("expect ')' \n");
					}

				}
				
			}
		}
	}
	else
	{
		return llvm::make_unique<PredicateAST>(bitexpr);
	}
}

std::unique_ptr<BitExprAST> ParseBitExprAST()
{
	auto bitexp = ParseBitExpAST();
	if (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == plus_mark || currtoken.token_value.symbol_mark == minus_mark))
	{
		auto op = llvm::make_unique<int>(currtoken.token_value.symbol_mark);
		currtoken = gettok();	// consume '-' or  '+ '
		auto bitexpr = ParseBitExprAST();
		return llvm::make_unique<BitExprAST>(bitexp, op, bitexpr);
	}
	else
	{
		return llvm::make_unique<BitExprAST>(bitexp);
	}
}

std::unique_ptr<BitExpAST> ParseBitExpAST()
{
	auto bitex = ParseBitExAST();
	if (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == mult_mark || currtoken.token_value.symbol_mark == div_mark
			|| currtoken.token_value.symbol_mark == mod_mark))
	{
		auto op = llvm::make_unique<int>(currtoken.token_value.symbol_mark);
		currtoken = gettok();	// consume '*' or  '/ ' or '%'
		auto bitexp = ParseBitExpAST();
		return llvm::make_unique<BitExprAST>(bitex, op, bitexp);
	}
	else
	{
		return llvm::make_unique<BitExpAST>(bitex);
	}
}

std::unique_ptr<BitExAST> ParseBitExAST()
{
	if (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == plus_mark || currtoken.token_value.symbol_mark == minus_mark))
	{
		auto mark = llvm::make_unique<int>(currtoken.token_value.symbol_mark);
		currtoken = gettok();	//consume mark
		auto bitex = ParseBitExAST();
		return llvm::make_unique<BitExAST>(mark, bitex);
	}
	else
	{
		auto SE = ParseSEAST();
		return llvm::make_unique<BitExAST>(SE);
	}
}

std::unique_ptr<SimpleExprAST> ParseSEAST()
{
	if (currtoken.token_kind == literal_double || currtoken.token_kind == literal_string
		|| currtoken.token_kind == literal_int)
	{
		auto lit = ParseLiteralAST();
		return llvm::make_unique<SimpleExprAST>(lit);
	}
	else if (currtoken.token_kind == id)
	{
		;
	}
	else if (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == left_bracket_mark || currtoken.token_value.symbol_mark == tok_EXISTS))
	{
		;
	}
	else
	{
		throw std::runtime_error("expect simple expression")
	}
}

class SimpleExprAST :public BitExAST
{
public:
	std::unique_ptr<IdAST> id = nullptr;
	std::unique_ptr<CallAST> call = nullptr;
	std::unique_ptr<TablecolAST> tablecol = nullptr;
	std::unique_ptr<ExprAST> expr = nullptr;
	std::unique_ptr<SubqueryAST> sub = nullptr;
	std::unique_ptr<ExistsSubqueryAST> exists = nullptr;
	std::unique_ptr<LiteralAST> lit = nullptr;

	SimpleExprAST() = default;
	SimpleExprAST(std::unique_ptr<IdAST> id) :id(std::move(id)) {}
	SimpleExprAST(std::unique_ptr<CallAST> call) :call(std::move(call)) {}
	SimpleExprAST(std::unique_ptr<TablecolAST> tablecol) :tablecol(std::move(tablecol)) {}
	SimpleExprAST(std::unique_ptr<ExprAST> expr) :expr(std::move(expr)) {}
	SimpleExprAST(std::unique_ptr<SubqueryAST> sub) :sub(std::move(sub)) {}
	SimpleExprAST(std::unique_ptr<ExistsSubqueryAST> exists) :exists(std::move(exists)) {}
	SimpleExprAST(std::unique_ptr<LiteralAST> lit) :lit(std::move(lit)) {}
};



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
	return llvm::make_unique<CreateTableSimpleAST>(table_name, std::move(cols));
}

std::unique_ptr<OnJoinCondAST> ParseOnJoinCondAST()
{
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	currtoken = gettok();	// consume `ON`
	//auto cond = ParseExprAST();
	auto cond = ParseLiteralAST();
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	return llvm::make_unique<OnJoinCondAST>(cond);
}

std::unique_ptr<UsingJoinCondAST> ParseUsingJoinCondAST()
{
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	//	consume `USING (`
	currtoken = gettok(); currtoken = gettok();
	std::vector<std::unique_ptr<TablecolAST>> cols;
	std::string colname = ParseIdAST()->getvalue()->IdentifierStr;
	cols.push_back(std::move(llvm::make_unique<TablecolAST>(colname)));
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		currtoken = gettok();	// consume ','
		std::string colname = ParseIdAST()->getvalue()->IdentifierStr;
		cols.push_back(std::move(llvm::make_unique<TablecolAST>(colname)));
	}
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == right_bracket_mark)
	{
		currtoken = gettok();	// consume '('
		while (currtoken.token_kind == blank || currtoken.token_kind == comment)
		{
			currtoken = gettok();
		}
		return llvm::make_unique<UsingJoinCondAST>(cols);
	}
	else
	{
		throw std::runtime_error("using condition miss ')' mark");
	}
	
	
}

std::unique_ptr<TablecolAST> ParseTablecolAST()
{
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	std::string table_name = ParseIdAST()->getvalue()->IdentifierStr;
	currtoken = gettok();	// consume '.' mark
	std::string col_name = ParseIdAST()->getvalue()->IdentifierStr;
	while (currtoken.token_kind == blank || currtoken.token_kind == comment)
	{
		currtoken = gettok();
	}
	return llvm::make_unique<TablecolAST>(table_name, col_name);
}



