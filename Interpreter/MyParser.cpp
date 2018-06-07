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



/// SQL end
token currtoken;
int delimiter = semicolon_mark;

using LR = std::vector<std::unique_ptr<ExprAST>> ;
using LL = std::vector<std::unique_ptr<ExprAST>> ;
void consumeit(std::vector<int> v, std::string s);

void consumeit(std::vector<int> v, std::string s)
{
	if (currtoken.token_kind == symbol)
	{
		int m = currtoken.token_value.symbol_mark; 
		if (std::find(v.cbegin(),v.cend(),m)!=v.cend())
		{
			currtoken = gettok();
			return;
		}
	}
	throw std::runtime_error(s);
}

void init_parser()
{
	currtoken = gettok();
}

bool iscompop(const int& i)
{
	return i == eq_mark || i == gteq_mark || i == gt_mark || i == lteq_mark || i == lt_mark || i == ltgt_mark || i == noteq_mark;
}

std::unique_ptr<ExprAST> ParseExprAST()
{
	auto lhs = ParseExpAST();
	std::unique_ptr<ExprAST> rhs;
	int op;
	if (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == and_mark ||
			currtoken.token_value.symbol_mark == andand_mark ||
			currtoken.token_value.symbol_mark == or_mark ||
			currtoken.token_value.symbol_mark == oror_mark)
		)
	{
		op = currtoken.token_value.symbol_mark;
		currtoken = gettok();	// consume op
		rhs = ParseExprAST();
		return llvm::make_unique<ExprAST>(std::move(lhs), op, std::move(rhs));
	}
	else
		return llvm::make_unique<ExprAST>(std::move(lhs),0,nullptr);
}

std::unique_ptr<ExpAST> ParseExpAST()
{
	std::unique_ptr<ExprAST> expr ;
	std::unique_ptr<BooleanPrimaryAST> bp ;
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
	return llvm::make_unique<ExpAST>(std::move(expr), std::move(bp));
}

std::unique_ptr<BooleanPrimaryAST> ParseBPAST()
{
	
	auto p = ParsePredicateAST();
	auto bp = llvm::make_unique<BooleanPrimaryAST>(nullptr, std::move(p), 0, 0, nullptr);
	while (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == tok_IS || iscompop(currtoken.token_value.symbol_mark))
		)
	{
		if (currtoken.token_value.symbol_mark == tok_IS)
		{
			currtoken = gettok();	// consume IS
			int flag = 0;
			if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == tok_NOT)
			{
				flag = tok_NOT;
				currtoken = gettok();	// consume NOT
				if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == tok_NULL)
				{
					currtoken = gettok();	// consume NULL
					flag = tok_NULL;
				}
				else
				{
					throw std::runtime_error(R"( parse error, do you mean "NOT NULL"?\n)");
				}
				bp=llvm::make_unique<BooleanPrimaryAST>(std::move(bp),nullptr, flag,0,nullptr);
			}
			else if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == tok_NULL)
			{
				flag = tok_NULL;
				currtoken = gettok();	// consume NULL
				bp = llvm::make_unique<BooleanPrimaryAST>(std::move(bp), nullptr, flag, 0, nullptr);
			}
			else
			{
				throw std::runtime_error("expect IS [NOT] NULL \n");
			}
		}
		else
		{
			int op = currtoken.token_value.symbol_mark;
			currtoken = gettok();	// consume op
			if ((currtoken.token_kind == symbol &&
				(currtoken.token_value.symbol_mark == tok_ALL || currtoken.token_value.symbol_mark == tok_ANY)
				))
			{
				int flag = currtoken.token_value.symbol_mark;
				currtoken = gettok();	// consume all/any
				if(currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == left_bracket_mark)
				{
					currtoken = gettok();	// consume '('
				}
				else
				{
					throw std::runtime_error("expext '( subquery)' pattern\n");
				}
				auto sub = ParseSubqueryAST();
				if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == right_bracket_mark)
				{
					currtoken = gettok();	// consume ')'
				}
				else
				{
					throw std::runtime_error("expext '( subquery)' pattern\n");
				}
				bp = llvm::make_unique<BooleanPrimaryAST>(std::move(bp), std::move(p), flag, op, std::move(sub));
			}
			else
			{
				p= ParsePredicateAST();
				bp = llvm::make_unique<BooleanPrimaryAST>(std::move(bp), std::move(p), 0, op, nullptr);
			}
		}
	}
	return std::move(bp);
	//return llvm::make_unique<BooleanPrimaryAST>(std::move(bp), nullptr, 0, 0, nullptr);
}

std::unique_ptr<PredicateAST> ParsePredicateAST()
{
	auto bitexpr = ParseBitExprAST();
	bool flag = true;
	std::unique_ptr<BitExprAST> rhs;
	std::unique_ptr<SubqueryAST> sub;
	std::vector<std::unique_ptr<ExprAST>> exprs;
	if (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == tok_NOT ||
			currtoken.token_value.symbol_mark == tok_REGEXP || currtoken.token_value.symbol_mark == tok_IN))
	{
		if (currtoken.token_value.symbol_mark == tok_NOT)
		{
			currtoken = gettok();	//consum NOT
			flag = false;
		}
		else if (currtoken.token_value.symbol_mark == tok_REGEXP)
		{
				currtoken = gettok();	//consume REGEXP
				rhs = ParseBitExprAST();
		}
		else if (currtoken.token_value.symbol_mark == tok_IN)
		{
			currtoken = gettok();	//consume IN
			consumeit({ left_bracket_mark }, "expect '('\n");
			if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == tok_SELECT)
			{
				sub = ParseSubqueryAST();
			}
			else
			{
				auto expr = ParseExprAST();
				exprs.push_back(std::move(expr));
				while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
				{
					currtoken = gettok();	//consume ','
					expr = ParseExprAST();
					exprs.push_back(std::move(expr));
				}
				consumeit({ right_bracket_mark }, "expect ')'\n");
			}
		}
	}
	return llvm::make_unique<PredicateAST>(std::move(bitexpr), flag, std::move(rhs), std::move(sub), std::move(exprs));
}

std::unique_ptr<BitExprAST> ParseBitExprAST()
{
	auto bitexp = ParseBitExpAST();
	int op = 0;
	std::unique_ptr<BitExprAST> bitexpr;
	while (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == plus_mark || currtoken.token_value.symbol_mark == minus_mark))
	{
		op = currtoken.token_value.symbol_mark;
		bitexpr = llvm::make_unique<BitExprAST>(std::move(bitexpr), op, std::move(bitexp));
		currtoken = gettok();	// consume '-' or  '+ '
		bitexp = ParseBitExpAST();
	}
	return llvm::make_unique<BitExprAST>(std::move(bitexpr),op, std::move(bitexp));
}

std::unique_ptr<BitExpAST> ParseBitExpAST()
{
	auto bitex = ParseBitExAST();
	int op = 0;
	std::unique_ptr<BitExpAST> bitexp;
	while (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == mult_mark || currtoken.token_value.symbol_mark == div_mark
			|| currtoken.token_value.symbol_mark == mod_mark))
	{
		op = currtoken.token_value.symbol_mark;
		bitexp = llvm::make_unique<BitExpAST>(std::move(bitexp), op, std::move(bitex));
		currtoken = gettok();	// consume '*' or  '/ ' or '%'
		bitex = ParseBitExAST();
	}
	return llvm::make_unique<BitExpAST>(std::move(bitexp),op, std::move(bitex));
}

std::unique_ptr<BitExAST> ParseBitExAST()
{
	int mark = 0;
	std::unique_ptr<BitExAST> bitex;
	std::unique_ptr<SimpleExprAST> SE;
	if (!(currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == plus_mark || currtoken.token_value.symbol_mark == minus_mark)))
	{
		SE = ParseSEAST();
		return llvm::make_unique<BitExAST>(mark, std::move(bitex), std::move(SE));
	}
	while (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == plus_mark || currtoken.token_value.symbol_mark == minus_mark))
	{
		mark = currtoken.token_value.symbol_mark;
		currtoken = gettok();	//consume mark
		bitex = ParseBitExAST();
		bitex = llvm::make_unique<BitExAST>(mark, std::move(bitex), std::move(SE));
	}
	return llvm::make_unique<BitExAST>(mark, std::move(bitex), std::move(SE));
}

std::unique_ptr<SimpleExprAST> ParseSEAST()
{
	if (currtoken.token_kind == literal_double || currtoken.token_kind == literal_string
		|| currtoken.token_kind == literal_int)
	{
		auto lit = ParseLiteralAST();
		return llvm::make_unique<SimpleExprAST>(std::move(lit));
	}
	else if (currtoken.token_kind == id)
	{
		auto idstr = ParseIdAST();
		if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == dot_mark)
		{
			currtoken = gettok();	//s=consume '.'
			auto colname = ParseIdAST();
			auto tbcol=llvm::make_unique<TablecolAST>(std::move(idstr->id),std::move(colname->id));
			return llvm::make_unique<SimpleExprAST>(std::move(tbcol));
		}
		else if ( currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == left_bracket_mark)
		{
			currtoken = gettok();	//s=consume '('
			auto call = ParseCallAST(std::move(idstr));
			return llvm::make_unique<SimpleExprAST>(std::move(call));
		}
		else
		{
			return llvm::make_unique<SimpleExprAST>(std::move(idstr));
		}
	}
	else if (currtoken.token_kind == symbol)
	{
		if (currtoken.token_value.symbol_mark == tok_EXISTS)
		{
			auto exsub = ParseExistsSubqueryAST();
			return llvm::make_unique<SimpleExprAST>(std::move(exsub));
		}
		else if(currtoken.token_value.symbol_mark == left_bracket_mark)
		{
			currtoken = gettok();	// consume '('
			auto sub = ParseSubqueryAST();
			currtoken = gettok();	// consume '('
			return llvm::make_unique<SimpleExprAST>(std::move(sub));
		}
		else
		{
			throw std::runtime_error("expect (subquery) or EXISTS (subquery)\n");
		}
	}
	else
	{
		throw std::runtime_error("expect simple expression");
	}
}

std::unique_ptr<ParenExprAST> ParseParenExprAST()
{
	currtoken = gettok();	// consume '('
	auto e = ParseExprAST();
	consumeit({ right_bracket_mark }, "expect ')'\n");
	return llvm::make_unique<ParenExprAST>(std::move(e));
}

std::unique_ptr<LiteralAST> ParseLiteralAST()
{
	if (currtoken.token_kind == literal_double)
		return llvm::make_unique<LiteralAST>(ParseDoubleLiteralAST());
	if (currtoken.token_kind == literal_int)
		return llvm::make_unique<LiteralAST>(ParseIntLiteralAST());
	if (currtoken.token_kind == literal_string)
		return llvm::make_unique<LiteralAST>(ParseStringLiteralAST());
}

std::unique_ptr<StringLiteralAST> ParseStringLiteralAST()
{
	std::string temps = currtoken.token_value.string_literal;
	currtoken = gettok(); // consume 1 string token
						  // wait,  this string may be concat
	while (currtoken.token_kind == literal_string)
	{
		temps += currtoken.token_value.string_literal;
		currtoken = gettok(); // consume 1 string token
	}
	auto s = llvm::make_unique<std::string>(temps);
	auto result = llvm::make_unique<StringLiteralAST>(std::move(s));
	return result;
}

std::unique_ptr<IntLiteralAST> ParseIntLiteralAST()
{
	auto result = llvm::make_unique<IntLiteralAST>(llvm::make_unique<int>(currtoken.token_value.int_literal));
	currtoken = gettok(); // consume 1 int token
	return result;
}

std::unique_ptr<DoubleLiteralAST> ParseDoubleLiteralAST()
{
	auto result = llvm::make_unique<DoubleLiteralAST>(llvm::make_unique<double>(currtoken.token_value.double_literal));
	currtoken = gettok(); // consume 1 double token
	return result;
}

std::unique_ptr<IdAST> ParseIdAST()
{
	if (currtoken.token_kind != id)
	{
		throw std::runtime_error("expect identifier \n)");
	}
	auto result = llvm::make_unique<IdAST>(llvm::make_unique<std::string>(currtoken.token_value.IdentifierStr));
	currtoken = gettok(); // consume 1 id
	return result;
};

std::unique_ptr<CallAST> ParseCallAST(std::unique_ptr<IdAST> callee)
{
	consumeit({ left_bracket_mark }, "expect '(' when parsing arg list");
	std::vector<std::unique_ptr<ExprAST>> args;
	args.push_back(ParseExprAST());
	while (currtoken.token_kind == symbol && currtoken.token_value.symbol_mark != comma_mark)
	{
		currtoken = gettok();    // consume 1 comma token
		args.push_back(ParseExprAST());
	}
	consumeit({ left_bracket_mark }, "expect '(' when parsing arg list");
	return llvm::make_unique<CallAST>(std::move(callee), std::move(args));
}

std::unique_ptr<CallAST> ParseCallAST()
{
	return ParseCallAST(ParseIdAST());
}

std::unique_ptr<TablecolAST> ParseTablecolAST()
{
	std::unique_ptr<std::string> table_name;
	std::unique_ptr<std::string> col_name;
	auto temp_name = std::move(ParseIdAST()->id);
	if (currtoken.token_kind == blank || currtoken.token_kind == dot_mark)
	{
		currtoken = gettok();	// consume '.'
		table_name = std::move(temp_name);
		col_name = std::move(ParseIdAST()->id);
	}
	return llvm::make_unique<TablecolAST>(std::move(table_name), std::move(col_name));
}

std::unique_ptr< ExistsSubqueryAST> ParseExistsSubqueryAST()
{
	currtoken = gettok();  // consume 'EXISTS' reserved word
	currtoken = gettok();  // consume '(' reserved word
	auto subquery = ParseSubqueryAST();
	currtoken = gettok();  // consume ')' reserved word
	return llvm::make_unique<ExistsSubqueryAST>(std::move(subquery));
};

std::unique_ptr<SubqueryAST> ParseSubqueryAST()
{
	currtoken = gettok();    // consume 'SELECT' reserved word
	bool distinct_flag=false, from_flag=false, where_flag=false, having_flag=false, group_flag=false, order_flag=false;
	std::vector<std::unique_ptr<SelectExprAST>> exprs;
	std::unique_ptr<TableRefsAST> tbrefs;
	std::unique_ptr<ExprAST> wherecond;
	std::vector<std::unique_ptr<ExprAST>> groupby_exprs;
	std::unique_ptr<ExprAST> havingcond;
	std::vector<std::unique_ptr<ExprAST>> orderby_exprs;
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_DISTINCT)
	{
		currtoken = gettok();	// consume DISTINCT
	}
	if (!(currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == mult_mark))
	{
		exprs.push_back(ParseSelectExprAST());
		while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
		{
			currtoken = gettok();	// consume ','
			exprs.push_back(ParseSelectExprAST());
		}
	}
	else
	{
		currtoken = gettok();	// consume '*'
		
	}
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_FROM)
	{
		from_flag = true;
		consumeit({ tok_FROM }, "expect FROM \n");
		tbrefs = ParseTableRefsAST();

		if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_WHERE)
		{
			where_flag = true;
			currtoken = gettok();	// consume WHERE
			wherecond = ParseExprAST();
		}

		if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_GROUP)
		{
			group_flag = true;
			currtoken = gettok();	// consume GROUP
			consumeit({ tok_BY }, "`BY` must follow `GROUP`\n");
			groupby_exprs.push_back(ParseExprAST());
			while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
			{
				currtoken = gettok();	// consume comma
				groupby_exprs.push_back(ParseExprAST());
			}
			if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_HAVING)
			{
				having_flag = true;
				currtoken = gettok();	// consume HAVING
				havingcond = ParseExprAST();
			}
		}

		if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_ORDER)
		{
			order_flag = true;
			currtoken = gettok();	// consume ORDER
			consumeit({ tok_BY }, "`BY` must follow `ORDER`\n");
			orderby_exprs.push_back(ParseExprAST());
			while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
			{
				currtoken = gettok();	// consume comma
				orderby_exprs.push_back(ParseExprAST());
			}
		}
	}
	return llvm::make_unique<SubqueryAST>(distinct_flag, std::move(exprs), from_flag, std::move(tbrefs),
		where_flag, std::move(wherecond), group_flag, std::move(groupby_exprs),
		having_flag, std::move(havingcond), order_flag, std::move(orderby_exprs) );
}

std::unique_ptr<OnJoinCondAST> ParseOnJoinCondAST()
{
	currtoken = gettok();	// consume `ON`
	return llvm::make_unique<OnJoinCondAST>(ParseExprAST());
}

std::unique_ptr<UsingJoinCondAST> ParseUsingJoinCondAST()
{
	currtoken = gettok();	//	consume `USING`
	currtoken = gettok();	//	consume `(`
	std::vector<std::unique_ptr<TablecolAST>> cols;
	cols.push_back(ParseTablecolAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		currtoken = gettok();	// consume ','
		cols.push_back(ParseTablecolAST());
	}
	consumeit({ right_bracket_mark }, "expect `)`");
	return llvm::make_unique<UsingJoinCondAST>(std::move(cols));
}

std::unique_ptr<SelectExprAST> ParseSelectExprAST()
{
	std::unique_ptr<ExprAST> expr;
	std::unique_ptr<IdAST> alias;
	expr = ParseExprAST();
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_AS)
	{
		currtoken = gettok();	//consume AS
		alias = ParseIdAST();
	}
	return llvm::make_unique<SelectExprAST>(std::move(expr), std::move(alias));
}

std::unique_ptr<TableRefsAST> ParseTableRefsAST()
{
	std::vector<std::unique_ptr<TableRefAST>> refs;
	refs.push_back(ParseTableRefAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		currtoken = gettok();	// consume comma
		refs.push_back(ParseTableRefAST());
	}
	return llvm::make_unique<TableRefsAST>(std::move(refs));
}

std::unique_ptr<TableRefAST> ParseTableRefAST()
{
	std::unique_ptr<TableFactorAST> tbfactor;
	std::unique_ptr<TRIJAST> trij;
	std::unique_ptr<TRLROJAST> trlroj;
	std::unique_ptr<TRNLROJAST> trnlroj;
	std::unique_ptr<TableRefAST> ref;
	tbfactor = ParseTableFactorAST();
	while (currtoken.token_kind == symbol&&(currtoken.token_value.symbol_mark == tok_INNER|| currtoken.token_value.symbol_mark == tok_CROSS
		||currtoken.token_value.symbol_mark == tok_JOIN|| currtoken.token_value.symbol_mark == tok_INNER
		|| currtoken.token_value.symbol_mark == tok_LEFT|| currtoken.token_value.symbol_mark == tok_RIGHT
		|| currtoken.token_value.symbol_mark == tok_NATURAL))
	{
		ref = llvm::make_unique<TableRefAST>(std::move(tbfactor), std::move(trij), std::move(trlroj), std::move(trnlroj));
		if (currtoken.token_value.symbol_mark == tok_NATURAL)
		{
			trnlroj = ParseTRNLROJAST(std::move(ref));
		}
		if (currtoken.token_value.symbol_mark == tok_LEFT || currtoken.token_value.symbol_mark == tok_RIGHT)
		{
			trlroj = ParseTRLROJAST(std::move(ref));
		}
		else
		{
			trij = ParseTRIJAST(std::move(ref));
		}
	}
	return llvm::make_unique<TableRefAST>(std::move(tbfactor), std::move(trij), std::move(trlroj), std::move(trnlroj));
}

std::unique_ptr<TRIJAST> ParseTRIJAST(std::unique_ptr<TableRefAST> ref)
{
	std::unique_ptr<TableFactorAST> factor;
	std::unique_ptr<JoinCondAST> cond;
	if (currtoken.token_kind == symbol && (currtoken.token_value.symbol_mark == tok_INNER
		|| currtoken.token_value.symbol_mark == tok_CROSS || currtoken.token_value.symbol_mark == tok_JOIN))
	{
		if (currtoken.token_value.symbol_mark == tok_INNER || currtoken.token_value.symbol_mark == tok_CROSS)
		{
			currtoken = gettok();	// consume INNER/CROSS
		}
		consumeit({ tok_JOIN }, "expect `JOIN`\n");
		factor = ParseTableFactorAST();
		if (currtoken.token_kind == symbol && (currtoken.token_value.symbol_mark == tok_ON
			|| currtoken.token_value.symbol_mark == tok_USING))
		{
			cond = ParseJoinCondAST();
		}
	}
	else
	{
		throw std::runtime_error("expect `INNER/CROSS/JOIN`\n");
	}
	return llvm::make_unique<TRIJAST>(std::move(ref), std::move(factor), std::move(cond));
}

std::unique_ptr<TRIJAST> ParseTRIJAST()
{
	return ParseTRIJAST(ParseTableRefAST());
}

std::unique_ptr<TRLROJAST> ParseTRLROJAST(std::unique_ptr<TableRefAST> lhs)
{
	std::unique_ptr<TableRefAST> rhs;
	std::unique_ptr<JoinCondAST> cond;
	int lr = 0;
	if (currtoken.token_kind == symbol && (currtoken.token_value.symbol_mark == tok_LEFT ||
		currtoken.token_value.symbol_mark == tok_RIGHT))
	{
		currtoken = gettok();	// consume  LEFT/RIGHT
		lr = currtoken.token_value.symbol_mark;
	}
	else
	{
		throw std::runtime_error("expect LEFT/RIGHT");
	}
	if (currtoken.token_kind == symbol && currtoken.token_value.symbol_mark == tok_OUTER)
	{
		currtoken = gettok();	// consume OUTER
	}
	consumeit({ tok_JOIN }, "expect `JOIN` \n");
	rhs = ParseTableRefAST();
	cond = ParseJoinCondAST();
	return llvm::make_unique<TRLROJAST>(std::move(lhs), std::move(rhs), std::move(cond), lr);
}

std::unique_ptr<TRLROJAST> ParseTRLROJAST()
{
	return ParseTRLROJAST(ParseTableRefAST());
}

std::unique_ptr<TRNLROJAST> ParseTRNLROJAST(std::unique_ptr<TableRefAST> ref)
{
	std::unique_ptr<TableFactorAST> factor;
	int lr = 0;
	consumeit({ tok_NATURAL }, "expect NATURAL\n");
	if (currtoken.token_kind == symbol && (currtoken.token_value.symbol_mark == tok_LEFT ||
		currtoken.token_value.symbol_mark == tok_RIGHT))
	{
		currtoken = gettok();	// consume  LEFT/RIGHT
		lr = currtoken.token_value.symbol_mark;
	}
	if (currtoken.token_kind == symbol && currtoken.token_value.symbol_mark == tok_OUTER)
	{
		currtoken = gettok();	// consume OUTER
	}
	consumeit({ tok_JOIN }, "expect `JOIN` \n");
	factor = ParseTableFactorAST();
	return llvm::make_unique<TRNLROJAST>(std::move(ref), std::move(factor),lr);

}

std::unique_ptr<TRNLROJAST> ParseTRNLROJAST()
{
	return  ParseTRNLROJAST(ParseTableRefAST());
}

std::unique_ptr<TableFactorAST> ParseTableFactorAST()
{
	std::unique_ptr<TableNameAST> tbname;
	std::unique_ptr<TableQueryAST> tbsub;
	std::unique_ptr<TableRefsAST> tbrefs; 
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_SELECT)
	{
		tbsub = ParseTableQueryAST();
	}
	else if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == left_bracket_mark)
	{
		currtoken = gettok();	// consume '('
		tbrefs = ParseTableRefsAST();
		consumeit({ right_bracket_mark }, "expect '(' \n");
	}
	else
	{
		tbname = ParseTableNameAST();
	}
	return llvm::make_unique<TableFactorAST>(std::move(tbname), std::move(tbsub), std::move(tbrefs));
}

std::unique_ptr<JoinCondAST> ParseJoinCondAST()
{
	std::unique_ptr<OnJoinCondAST> oncond;
	std::unique_ptr<UsingJoinCondAST> uselist;
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_ON)
	{
		oncond = ParseOnJoinCondAST();
	}
	else if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_USING)
	{
		uselist = ParseUsingJoinCondAST();
	}
	else
	{
		throw std::runtime_error("expect ON/USING\n");
	}
	return llvm::make_unique<JoinCondAST>(std::move(oncond), std::move(uselist));
}

std::unique_ptr<TableNameAST> ParseTableNameAST()
{
	std::unique_ptr<IdAST> tbname;
	std::unique_ptr<IdAST> alias;
	tbname = ParseIdAST();
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_AS)
	{
		currtoken = gettok();	// consume AS
		alias = ParseIdAST();
	}
	return llvm::make_unique<TableNameAST>(std::move(tbname), std::move(alias));
}

std::unique_ptr<TableQueryAST> ParseTableQueryAST()
{
	std::unique_ptr<SubqueryAST> subq;
	std::unique_ptr<IdAST> alias;
	subq = ParseSubqueryAST();
	consumeit({ tok_AS }, "expect AS");
	alias = ParseIdAST();
	return llvm::make_unique<TableQueryAST>(std::move(subq), std::move(alias));
}

std::unique_ptr<DatatypeAST> ParseDatatypeAST()
{
	int dtype = 0;
	int n = 0;
	if (currtoken.token_kind == symbol && (currtoken.token_value.symbol_mark == tok_INT ||
		currtoken.token_value.symbol_mark == tok_FLOAT || currtoken.token_value.symbol_mark == tok_DOUBLE ||
		currtoken.token_value.symbol_mark == tok_CHAR || currtoken.token_value.symbol_mark == tok_VARCHAR))
	{
		dtype = currtoken.token_value.symbol_mark;
		currtoken = gettok();	// consume INT/FLOAT/DOUBLE/CHAR/VARCHAR
	}
	if (currtoken.token_kind == symbol && currtoken.token_value.symbol_mark == left_bracket_mark)
	{
		consumeit({ left_bracket_mark }, "expect '('");
		if (currtoken.token_kind == literal_int)
		{
			n = currtoken.token_value.int_literal;
			currtoken = gettok();	// consume int
		}
		consumeit({ right_bracket_mark }, "expect ')'");
	}
	return llvm::make_unique<DatatypeAST>(dtype, n);
}

std::unique_ptr<RefdefAST> ParseRefdefAST()
{
	std::unique_ptr<IdAST> tbname;
	std::vector<std::unique_ptr<IdAST>> colname;
	int deleteop = 0;
	int updateop = 0; 
	consumeit({ tok_REFERENCES }, "expect REFERENCES \n");
	tbname = ParseIdAST();
	consumeit({ left_bracket_mark }, "expect '('\n");
	colname.push_back(ParseIdAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		currtoken = gettok();	// consume ','
		colname.push_back(ParseIdAST());
	}
	consumeit({ right_bracket_mark }, "expect ')'\n");
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_ON)
	{
		currtoken = gettok();	// consume 'ON'
		if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_DELETE)
		{
			currtoken = gettok();
			deleteop = currtoken.token_value.symbol_mark;
			currtoken = gettok();
		}
		else if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_UPDATE)
		{
			currtoken = gettok();
			updateop = currtoken.token_value.symbol_mark;
			currtoken = gettok();
		}
	}
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_ON)
	{
		currtoken = gettok();	// consume 'ON'
		if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_DELETE)
		{
			currtoken = gettok();
			deleteop = currtoken.token_value.symbol_mark;
			currtoken = gettok();
		}
		else if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_UPDATE)
		{
			currtoken = gettok();
			updateop = currtoken.token_value.symbol_mark;
			currtoken = gettok();
		}
	}
	return llvm::make_unique<RefdefAST>(std::move(tbname), std::move(colname),deleteop,updateop);
}

std::unique_ptr<PrimaryAST> ParsePrimaryAST()
{
	std::vector<std::unique_ptr<IdAST>> cols;
	consumeit({ tok_PRIMARY }, "expect keyword PRIMARY \n");
	consumeit({ tok_KEY }, "expect keyword KEY \n");
	consumeit({ left_bracket_mark }, "expect '(' \n");
	cols.push_back(ParseIdAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		consumeit({ comma_mark }, "expect ',' \n");
		cols.push_back(ParseIdAST());
	}
	return llvm::make_unique<PrimaryAST>(std::move(cols));
}

std::unique_ptr<UniqueAST> ParseUniqueAST()
{
	std::vector<std::unique_ptr<IdAST>> cols;
	consumeit({ tok_UNIQUE }, "expect keyword UNIQUE \n");
	consumeit({ left_bracket_mark }, "expect '(' \n");
	cols.push_back(ParseIdAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		consumeit({ comma_mark }, "expect ',' \n");
		cols.push_back(ParseIdAST());
	}
	return llvm::make_unique<UniqueAST>(std::move(cols));
}

std::unique_ptr<ForeignAST> ParseForeignAST()
{
	std::vector<std::unique_ptr<IdAST>> cols;
	std::unique_ptr<RefdefAST> refdef;
	consumeit({ tok_PRIMARY }, "expect keyword PRIMARY \n");
	consumeit({ tok_KEY }, "expect keyword KEY \n");
	consumeit({ left_bracket_mark }, "expect '(' \n");
	cols.push_back(ParseIdAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		consumeit({ comma_mark }, "expect ',' \n");
		cols.push_back(ParseIdAST());
	}
	refdef = ParseRefdefAST();
	return llvm::make_unique<ForeignAST>(std::move(cols), std::move(refdef));
	
}

std::unique_ptr<CreatedefAST> ParseCreatedefAST()
{
	std::unique_ptr<IdAST> colname;
	std::unique_ptr<ColdefAST> coldef;
	std::unique_ptr<PrimaryAST> prim;
	std::unique_ptr<UniqueAST> uni;
	std::unique_ptr<ForeignAST> forei;
	std::unique_ptr<ExprAST> check; 
	if (currtoken.token_kind == id)
	{
		colname = ParseIdAST();
		coldef = ParseColdefAST();
	}
	else if (currtoken.token_kind == symbol)
	{
		if (currtoken.token_value.symbol_mark == tok_PRIMARY)
		{
			prim = ParsePrimaryAST();
		}
		else if (currtoken.token_value.symbol_mark == tok_UNIQUE)
		{
			uni = ParseUniqueAST();
		}
		else if (currtoken.token_value.symbol_mark == tok_FOREIGN)
		{
			forei = ParseForeignAST();
		}
		else if (currtoken.token_value.symbol_mark == tok_CHECK)
		{
			check = ParseExprAST();
		}
		else
		{
			throw std::runtime_error("expect table-column-creation beginning with\
 column-name/PRIMARY/UNIQUE/FOREIGN/CHECK\n");
			return nullptr;
		}
	}
	else
	{
		throw std::runtime_error("expect table-column-creation beginning with\
 column-name/PRIMARY/UNIQUE/FOREIGN/CHECK\n");
		return nullptr;
	}
	return llvm::make_unique<CreatedefAST>(std::move(colname), std::move(coldef),
		std::move(prim), std::move(uni), std::move(forei), std::move(check));
}

std::unique_ptr<ColdefAST> ParseColdefAST()
{
	std::unique_ptr<DatatypeAST> dtype;
	bool null_flag = true;
	std::unique_ptr<ExprAST> default_value;
	bool unique_flag = false;
	bool primary_flag = false;
	std::unique_ptr<RefdefAST> refdef;
	dtype = ParseDatatypeAST();
	while (currtoken.token_kind == symbol)
	{
		if (currtoken.token_value.symbol_mark == tok_NOT)
		{
			consumeit({ tok_NOT }, "expect NOT\n");
			null_flag = true;
			consumeit({ tok_NULL }, "expect NULL\n");
		}
		else if (currtoken.token_value.symbol_mark == tok_NULL)
		{
			null_flag = false;
			consumeit({ tok_NULL }, "expect NULL\n");
		}
		if (currtoken.token_value.symbol_mark == tok_DEFAULT)
		{
			consumeit({ tok_DEFAULT }, "expect DEFAULT \n");
			default_value = ParseExprAST();
		}
		if (currtoken.token_value.symbol_mark == tok_UNIQUE)
		{
			consumeit({ tok_UNIQUE }, "expect UNIQUE \n");
			unique_flag = true;
			if (currtoken.token_value.symbol_mark == tok_KEY)
			{
				consumeit({ tok_KEY }, "expect KEY \n");
			}
		}
		if (currtoken.token_value.symbol_mark == tok_PRIMARY|| currtoken.token_value.symbol_mark == tok_KEY)
		{
			if (currtoken.token_value.symbol_mark == tok_PRIMARY)
			{
				consumeit({ tok_PRIMARY }, "expect PRIMARY \n");
			}
			consumeit({ tok_KEY }, "expect KEY \n");
		}
		if (currtoken.token_value.symbol_mark == tok_REFERENCES)
		{
			break;
		}
		else
			break;
	}
	if (currtoken.token_value.symbol_mark == tok_REFERENCES)
	{
		refdef = ParseRefdefAST();
	}
	return llvm::make_unique<ColdefAST>(std::move(dtype), 
		null_flag, std::move(default_value), unique_flag, primary_flag, std::move(refdef));
}

std::unique_ptr<CreateTableSimpleAST> ParseCreateTableSimpleAST()
{
	//consumeit({ tok_CREATE }, "expect `CREATE`\n");
	consumeit({ tok_TABLE }, "expect `TABLE`\n");
	auto Id = ParseIdAST();
	std::vector<std::unique_ptr<CreatedefAST>> create_defs;
	consumeit({ left_bracket_mark }, "expect `(`\n");
	create_defs.push_back(ParseCreatedefAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		currtoken = gettok();	// consuem ','
		create_defs.push_back(ParseCreatedefAST());
	}
	consumeit({ right_bracket_mark }, "expect `)`\n");
	consumeit({ delimiter }, "expect delimiter \n");
	return llvm::make_unique<CreateTableSimpleAST>(std::move(Id),std::move(create_defs));
}

std::unique_ptr<SelectAST> ParseSelectAST()
{
	auto subq = ParseSubqueryAST();
	consumeit({ delimiter }, "expect delimiter \n");
	return llvm::make_unique<SelectAST>(std::move(subq));
}

std::unique_ptr<StatementAST> ParseStatementAST()
{
	std::unique_ptr<CreateAST> create;
	std::unique_ptr<SelectAST> select;
	std::unique_ptr<DropAST> drop;
	std::unique_ptr<InsertAST> insert;
	std::unique_ptr<DeleteAST> dele;
	if (currtoken.token_kind == symbol && (currtoken.token_value.symbol_mark == tok_SET || 
	currtoken.token_value.symbol_mark == tok_INSERT ||currtoken.token_value.symbol_mark == tok_SELECT || 
		currtoken.token_value.symbol_mark == tok_CREATE ||currtoken.token_value.symbol_mark == tok_DROP || 
		currtoken.token_value.symbol_mark == tok_DELETE|| currtoken.token_value.symbol_mark == semicolon_mark))
	{
		if (currtoken.token_value.symbol_mark == tok_SET)
		{
			;
		} 
		else if (currtoken.token_value.symbol_mark == tok_INSERT)
		{
			insert = ParseInsertAST();
		}
		else if (currtoken.token_value.symbol_mark == tok_SELECT)
		{
			select = ParseSelectAST();
		}
		else if (currtoken.token_value.symbol_mark == tok_CREATE)
		{
			create = ParseCreateAST();
		}
		else if (currtoken.token_value.symbol_mark == tok_DROP)
		{
			drop = ParseDropAST();
		}
		else if (currtoken.token_value.symbol_mark == tok_DELETE)
		{
			dele = ParseDeleteAST();
		}
		else if (currtoken.token_value.symbol_mark == semicolon_mark)
		{
			;
		}
	}
	else
	{
		throw std::runtime_error("statement sentence can only begin with SET/INSERT/SELECT/CREATE/DROP/INSERT/DELETE/; \n");
		return nullptr;
	}
	return llvm::make_unique<StatementAST>(std::move(create), std::move(select),
		std::move(drop), std::move(insert), std::move(dele));
}

std::unique_ptr<CreateAST> ParseCreateAST()
{
	std::unique_ptr<CreateTableAST> ctable;
	std::unique_ptr<CreateIndexAST> cindex; 
	consumeit({ tok_CREATE }, "expect CREATE\n");
	if (currtoken.token_kind == symbol)
	{
		if (currtoken.token_value.symbol_mark == tok_TABLE)
		{
			ctable = ParseCreateTableAST();
		}
		else if (currtoken.token_value.symbol_mark == tok_INDEX)
		{
			cindex = ParseCreateIndexAST();
		}
		else
		{
			throw std::runtime_error("can only create TABLE/INDEX\n");
		}
	}
	else
	{
		throw std::runtime_error("can only create TABLE/INDEX\n");
	}
	return llvm::make_unique<CreateAST>(std::move(ctable), std::move(cindex));
}

std::unique_ptr<CreateTableAST> ParseCreateTableAST()
{
	std::unique_ptr< CreateTableSimpleAST> simplecreate;
	std::unique_ptr<CreateTableSelectAST> selectcreate;
	std::unique_ptr<CreateTableLikeAST> likecreate;
	simplecreate = ParseCreateTableSimpleAST();
	return llvm::make_unique<CreateTableAST>(std::move(simplecreate), std::move(selectcreate), std::move(likecreate));
}

std::unique_ptr<CreateIndexAST> ParseCreateIndexAST()
{
	std::unique_ptr<IdAST> index_name;
	std::unique_ptr<IdAST> table_name;
	std::vector<std::unique_ptr<IdAST>> col_names;
	consumeit({ tok_INDEX }, "expect INDEX\n");
	index_name = ParseIdAST();
	consumeit({ tok_ON }, "expect ON\n");
	table_name = ParseIdAST();
	consumeit({ left_bracket_mark }, "expect '('\n");
	col_names.push_back(ParseIdAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		consumeit({ comma_mark }, "expect ','");
		col_names.push_back(ParseIdAST());
	}
	consumeit({ right_bracket_mark }, "expect ')'\n");
	consumeit({ delimiter }, "expect delimiter \n");
	return llvm::make_unique<CreateIndexAST>(std::move(index_name),std::move( table_name), std::move(col_names));
}

std::unique_ptr<DropAST> ParseDropAST()
{
	std::unique_ptr<DropTableAST> droptb;
	std::unique_ptr<DropIndexAST> dropindex;
	consumeit({ tok_DROP }, "expect DROP \n");
	if (currtoken.token_kind == symbol)
	{
		if (currtoken.token_value.symbol_mark == tok_TABLE)
		{
			droptb = ParseDropTableAST();
		}
		else if (currtoken.token_value.symbol_mark == tok_INDEX)
		{
			dropindex = ParseDropIndexAST();
		}
		else
		{
			throw std::runtime_error("can only drop TABLE/INDEX \n");
			return nullptr;
		}
	}
	else
	{
		throw std::runtime_error("can only drop TABLE/INDEX \n");
		return nullptr;
	}
	return llvm::make_unique<DropAST>(std::move(droptb), std::move(dropindex));
}

std::unique_ptr<DropTableAST> ParseDropTableAST()
{
	consumeit({ tok_TABLE }, "expect TABLE \n");
	std::vector<std::unique_ptr<IdAST>> table_list;
	table_list.push_back(ParseIdAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		consumeit({ comma_mark }, "expect ','\n");
		table_list.push_back(ParseIdAST());
	}
	return llvm::make_unique<DropTableAST>(std::move(table_list));
}

std::unique_ptr<DropIndexAST> ParseDropIndexAST()
{
	consumeit({ tok_INDEX }, "expect INDEX \n");
	std::vector<std::unique_ptr<IdAST>> table_list;
	table_list.push_back(ParseIdAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		consumeit({ comma_mark }, "expect ','\n");
		table_list.push_back(ParseIdAST());
	}
	return llvm::make_unique<DropIndexAST>(std::move(table_list));
}

std::unique_ptr<InsertAST> ParseInsertAST()
{
	std::unique_ptr<IdAST> table_name;
	std::vector<std::unique_ptr<IdAST>> col_names;
	std::vector<std::unique_ptr<ExprAST>> value_list;
	consumeit({ tok_INSERT }, "expect INSERT\n");
	consumeit({ tok_INTO }, "expect INTO\n");
	table_name = ParseIdAST();
	consumeit({ tok_VALUES,tok_VALUE }, "expect VALUES/VALUE \n");
	consumeit({ left_bracket_mark }, "expect '(' \n");
	value_list.push_back(ParseExprAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		consumeit({ comma_mark }, "expect ',' \n");
		value_list.push_back(ParseExprAST());
	}
	return llvm::make_unique<InsertAST>(std::move(table_name), std::move(col_names),std::move( value_list));
}

std::unique_ptr<DeleteAST> ParseDeleteAST()
{
	std::unique_ptr<IdAST> table_name;
	std::unique_ptr< ExprAST> where_condition;
	consumeit({ tok_DELETE }, "expect DELETE\n");
	consumeit({ tok_FROM }, "expect FROM\n");
	table_name = ParseIdAST();
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_WHERE)
	{
		consumeit({ tok_FROM }, "expect WHERE\n");
		where_condition = ParseExprAST();
	}
	return llvm::make_unique<DeleteAST>(std::move(table_name), std::move(where_condition));
}

std::unique_ptr<CreateTableSelectAST> ParseCreateTableSelectAST();
std::unique_ptr<CreateTableLikeAST> ParseCreateTableLikeAST();
