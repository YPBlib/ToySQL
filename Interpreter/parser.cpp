#include"../Catalog/catalog.h"
#include"../Recorder/recorder.h"

token currtoken;
int delimiter = semicolon_mark;
int selecttimes;
using LR = std::vector<std::shared_ptr<ExprAST>>;
using LL = std::vector<std::shared_ptr<ExprAST>>;
void consumeit(std::deque<int>&& v, std::string s);

void consumeit(std::deque<int>&& v, std::string s)
{
	int& m = currtoken.token_value.symbol_mark;
	if (v.size() == 2 && v[0] == tok_QUIT && v[1] == delimiter)
	{
		if (currtoken.token_kind == symbol && m == tok_QUIT)
		{
			currtoken = gettok();
			if (currtoken.token_kind == symbol && m == delimiter)return;
			else throw std::runtime_error(s);
		}
		else throw std::runtime_error(s);
	}

	while (v.size())
	{
		if (currtoken.token_kind == symbol&& v[0] == m)
		{
			v.pop_front();
			currtoken = gettok();
		}
		else throw std::runtime_error(s);
	}
}

void init_parser()
{
	currtoken = gettok();
}

bool iscompop(const int& i)
{
	return i == eq_mark || i == gteq_mark || i == gt_mark || i == lteq_mark || i == lt_mark || i == ltgt_mark || i == noteq_mark;
}

std::shared_ptr<ExprAST> ParseExprAST()
{
	auto lhs = ParseExpAST();
	std::shared_ptr<ExprAST> rhs;
	int op;
	if (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == and_mark ||
			currtoken.token_value.symbol_mark == andand_mark ||
			currtoken.token_value.symbol_mark == or_mark ||
			currtoken.token_value.symbol_mark == oror_mark||
			currtoken.token_value.symbol_mark==tok_AND||
			currtoken.token_value.symbol_mark==tok_OR)
		)
	{
		op = currtoken.token_value.symbol_mark;
		currtoken = gettok();	// consume op
		rhs = ParseExprAST();
		return std::make_shared<ExprAST>(std::move(lhs), op, std::move(rhs));
	}
	else
		return std::make_shared<ExprAST>(std::move(lhs), 0, nullptr);
}

std::shared_ptr<ExpAST> ParseExpAST()
{
	std::shared_ptr<ExprAST> expr;
	std::shared_ptr<BooleanPrimaryAST> bp;
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
	return std::make_shared<ExpAST>(std::move(expr), std::move(bp));
}

std::shared_ptr<BooleanPrimaryAST> ParseBPAST()
{

	auto p = ParsePredicateAST();
	auto bp = std::make_shared<BooleanPrimaryAST>(nullptr, std::move(p), 0, 0, nullptr);
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
				bp = std::make_shared<BooleanPrimaryAST>(std::move(bp), nullptr, flag, 0, nullptr);
			}
			else if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == tok_NULL)
			{
				flag = tok_NULL;
				currtoken = gettok();	// consume NULL
				bp = std::make_shared<BooleanPrimaryAST>(std::move(bp), nullptr, flag, 0, nullptr);
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
				if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == left_bracket_mark)
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
				bp = std::make_shared<BooleanPrimaryAST>(std::move(bp), std::move(p), flag, op, std::move(sub));
			}
			else
			{
				p = ParsePredicateAST();
				bp = std::make_shared<BooleanPrimaryAST>(std::move(bp), std::move(p), 0, op, nullptr);
			}
		}
	}
	return std::move(bp);
	//return std::make_shared<BooleanPrimaryAST>(std::move(bp), nullptr, 0, 0, nullptr);
}

std::shared_ptr<PredicateAST> ParsePredicateAST()
{
	auto bitexpr = ParseBitExprAST();
	bool flag = true;
	std::shared_ptr<BitExprAST> rhs;
	std::shared_ptr<PredicateAST> p;
	std::shared_ptr<SubqueryAST> sub;
	std::vector<std::shared_ptr<ExprAST>> exprs;
	if (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == tok_NOT ||
			currtoken.token_value.symbol_mark == tok_BETWEEN || currtoken.token_value.symbol_mark == tok_IN))
	{
		if (currtoken.token_value.symbol_mark == tok_NOT)
		{
			flag = false;
			currtoken = gettok();	//consum NOT
			if (!(currtoken.token_kind == symbol &&
				(currtoken.token_value.symbol_mark == tok_IN || currtoken.token_value.symbol_mark == tok_BETWEEN)))
			{
				throw std::runtime_error("IN/BETWEEN should follow BOT when parsing predicate \n");
			}

		}
		if (currtoken.token_value.symbol_mark == tok_BETWEEN)
		{
			currtoken = gettok();	//consume BETWEEN
			rhs = ParseBitExprAST();
			consumeit({ tok_AND }, "BETWEEN AND mismatches \n");
			p = ParsePredicateAST();
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
	return std::make_shared<PredicateAST>(std::move(bitexpr), flag, std::move(rhs),
		std::move(p), std::move(sub), std::move(exprs));
}

std::shared_ptr<BitExprAST> ParseBitExprAST()
{
	auto bitexp = ParseBitExpAST();
	int op = 0;
	std::shared_ptr<BitExprAST> bitexpr;
	while (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == plus_mark || currtoken.token_value.symbol_mark == minus_mark))
	{
		op = currtoken.token_value.symbol_mark;
		bitexpr = std::make_shared<BitExprAST>(std::move(bitexpr), op, std::move(bitexp));
		currtoken = gettok();	// consume '-' or  '+ '
		bitexp = ParseBitExpAST();
	}
	return std::make_shared<BitExprAST>(std::move(bitexpr), op, std::move(bitexp));
}

std::shared_ptr<BitExpAST> ParseBitExpAST()
{
	auto bitex = ParseBitExAST();
	int op = 0;
	std::shared_ptr<BitExpAST> bitexp;
	while (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == mult_mark || currtoken.token_value.symbol_mark == div_mark
			|| currtoken.token_value.symbol_mark == mod_mark))
	{
		op = currtoken.token_value.symbol_mark;
		bitexp = std::make_shared<BitExpAST>(std::move(bitexp), op, std::move(bitex));
		currtoken = gettok();	// consume '*' or  '/ ' or '%'
		bitex = ParseBitExAST();
	}
	return std::make_shared<BitExpAST>(std::move(bitexp), op, std::move(bitex));
}

std::shared_ptr<BitExAST> ParseBitExAST()
{
	int mark = 0;
	std::shared_ptr<BitExAST> bitex;
	std::shared_ptr<SimpleExprAST> SE;
	if (!(currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == plus_mark || currtoken.token_value.symbol_mark == minus_mark)))
	{
		SE = ParseSEAST();
		return std::make_shared<BitExAST>(mark, std::move(bitex), std::move(SE));
	}
	while (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == plus_mark || currtoken.token_value.symbol_mark == minus_mark))
	{
		mark = currtoken.token_value.symbol_mark;
		currtoken = gettok();	//consume mark
		bitex = ParseBitExAST();
		bitex = std::make_shared<BitExAST>(mark, std::move(bitex), std::move(SE));
	}
	return std::make_shared<BitExAST>(mark, std::move(bitex), std::move(SE));
}

std::shared_ptr<SimpleExprAST> ParseSEAST()
{
	if (currtoken.token_kind == literal_double || currtoken.token_kind == literal_string
		|| currtoken.token_kind == literal_int)
	{
		auto lit = ParseLiteralAST();
		return std::make_shared<SimpleExprAST>(std::move(lit));
	}
	else if (currtoken.token_kind == id)
	{
		auto idstr = ParseIdAST();
		if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == dot_mark)
		{
			currtoken = gettok();	//s=consume '.'
			auto colname = ParseIdAST();
			auto tbcol = std::make_shared<TablecolAST>(std::move(idstr->id), std::move(colname->id));
			return std::make_shared<SimpleExprAST>(std::move(tbcol));
		}
		else if (currtoken.token_kind == symbol &&currtoken.token_value.symbol_mark == left_bracket_mark)
		{
			currtoken = gettok();	//s=consume '('
			auto call = ParseCallAST(std::move(idstr));
			return std::make_shared<SimpleExprAST>(std::move(call));
		}
		else
		{
			return std::make_shared<SimpleExprAST>(std::move(idstr));
		}
	}
	else if (currtoken.token_kind == symbol)
	{
		if (currtoken.token_value.symbol_mark == tok_EXISTS)
		{
			auto exsub = ParseExistsSubqueryAST();
			return std::make_shared<SimpleExprAST>(std::move(exsub));
		}
		else if (currtoken.token_value.symbol_mark == left_bracket_mark)
		{
			currtoken = gettok();	// consume '('
			auto sub = ParseSubqueryAST();
			currtoken = gettok();	// consume '('
			return std::make_shared<SimpleExprAST>(std::move(sub));
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

std::shared_ptr<ParenExprAST> ParseParenExprAST()
{
	consumeit({ left_bracket_mark }, "expect '('\n");
	auto e = ParseExprAST();
	consumeit({ right_bracket_mark }, "expect ')'\n");
	return std::make_shared<ParenExprAST>(std::move(e));
}

std::shared_ptr<LiteralAST> ParseLiteralAST()
{
	if (currtoken.token_kind == literal_double)
		return std::make_shared<LiteralAST>(ParseDoubleLiteralAST());
	if (currtoken.token_kind == literal_int)
		return std::make_shared<LiteralAST>(ParseIntLiteralAST());
	if (currtoken.token_kind == literal_string)
		return std::make_shared<LiteralAST>(ParseStringLiteralAST());
}

std::shared_ptr<StringLiteralAST> ParseStringLiteralAST()
{
	std::string temps = currtoken.token_value.string_literal;
	currtoken = gettok(); // consume 1 string token
						  // wait,  this string may be concat
	while (currtoken.token_kind == literal_string)
	{
		temps += currtoken.token_value.string_literal;
		currtoken = gettok(); // consume 1 string token
	}
	auto s = std::make_shared<std::string>(temps);
	auto result = std::make_shared<StringLiteralAST>(std::move(s));
	return result;
}

std::shared_ptr<IntLiteralAST> ParseIntLiteralAST()
{
	auto result = std::make_shared<IntLiteralAST>(std::make_shared<int>(currtoken.token_value.int_literal));
	currtoken = gettok(); // consume 1 int token
	return result;
}

std::shared_ptr<DoubleLiteralAST> ParseDoubleLiteralAST()
{
	auto result = std::make_shared<DoubleLiteralAST>(std::make_shared<double>(currtoken.token_value.double_literal));
	currtoken = gettok(); // consume 1 double token
	return result;
}

std::shared_ptr<IdAST> ParseIdAST()
{
	if (currtoken.token_kind != id)
	{
		throw std::runtime_error("expect identifier \n)");
	}
	auto result = std::make_shared<IdAST>(std::make_shared<std::string>(currtoken.token_value.IdentifierStr));
	currtoken = gettok(); // consume 1 id
	return result;
};

std::shared_ptr<CallAST> ParseCallAST(std::shared_ptr<IdAST> callee)
{
	consumeit({ left_bracket_mark }, "expect '(' when parsing arg list");
	std::vector<std::shared_ptr<ExprAST>> args;
	args.push_back(ParseExprAST());
	while (currtoken.token_kind == symbol && currtoken.token_value.symbol_mark != comma_mark)
	{
		currtoken = gettok();    // consume 1 comma token
		args.push_back(ParseExprAST());
	}
	consumeit({ left_bracket_mark }, "expect '(' when parsing arg list");
	return std::make_shared<CallAST>(std::move(callee), std::move(args));
}

std::shared_ptr<CallAST> ParseCallAST()
{
	return ParseCallAST(ParseIdAST());
}

std::shared_ptr<TablecolAST> ParseTablecolAST()
{
	std::shared_ptr<std::string> table_name;
	std::shared_ptr<std::string> col_name;
	auto temp_name = std::move(ParseIdAST()->id);
	if (currtoken.token_kind == blank || currtoken.token_kind == dot_mark)
	{
		currtoken = gettok();	// consume '.'
		table_name = std::move(temp_name);
		col_name = std::move(ParseIdAST()->id);
	}
	return std::make_shared<TablecolAST>(std::move(table_name), std::move(col_name));
}

std::shared_ptr< ExistsSubqueryAST> ParseExistsSubqueryAST()
{
	currtoken = gettok();  // consume 'EXISTS' reserved word
	currtoken = gettok();  // consume '(' reserved word
	auto subquery = ParseSubqueryAST();
	currtoken = gettok();  // consume ')' reserved word
	return std::make_shared<ExistsSubqueryAST>(std::move(subquery));
};

std::shared_ptr<SubqueryAST> ParseSubqueryAST()
{
	currtoken = gettok();    // consume 'SELECT' reserved word
	bool distinct_flag = false, from_flag = false, where_flag = false, having_flag = false, group_flag = false, order_flag = false;
	std::vector<std::shared_ptr<SelectExprAST>> exprs;
	std::shared_ptr<TableRefsAST> tbrefs;
	std::shared_ptr<ExprAST> wherecond;
	std::vector<std::shared_ptr<ExprAST>> groupby_exprs;
	std::shared_ptr<ExprAST> havingcond;
	std::vector<std::shared_ptr<ExprAST>> orderby_exprs;
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
	return std::make_shared<SubqueryAST>(distinct_flag, std::move(exprs), from_flag, std::move(tbrefs),
		where_flag, std::move(wherecond), group_flag, std::move(groupby_exprs),
		having_flag, std::move(havingcond), order_flag, std::move(orderby_exprs));
}

std::shared_ptr<OnJoinCondAST> ParseOnJoinCondAST()
{
	currtoken = gettok();	// consume `ON`
	return std::make_shared<OnJoinCondAST>(ParseExprAST());
}

std::shared_ptr<UsingJoinCondAST> ParseUsingJoinCondAST()
{
	currtoken = gettok();	//	consume `USING`
	currtoken = gettok();	//	consume `(`
	std::vector<std::shared_ptr<TablecolAST>> cols;
	cols.push_back(ParseTablecolAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		currtoken = gettok();	// consume ','
		cols.push_back(ParseTablecolAST());
	}
	consumeit({ right_bracket_mark }, "expect `)`");
	return std::make_shared<UsingJoinCondAST>(std::move(cols));
}

std::shared_ptr<SelectExprAST> ParseSelectExprAST()
{
	std::shared_ptr<ExprAST> expr;
	std::shared_ptr<IdAST> alias;
	expr = ParseExprAST();
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_AS)
	{
		currtoken = gettok();	//consume AS
		alias = ParseIdAST();
	}
	return std::make_shared<SelectExprAST>(std::move(expr), std::move(alias));
}

std::shared_ptr<TableRefsAST> ParseTableRefsAST()
{
	std::vector<std::shared_ptr<TableRefAST>> refs;
	refs.push_back(ParseTableRefAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		currtoken = gettok();	// consume comma
		refs.push_back(ParseTableRefAST());
	}
	return std::make_shared<TableRefsAST>(std::move(refs));
}

std::shared_ptr<TableRefAST> ParseTableRefAST()
{
	std::shared_ptr<TableFactorAST> tbfactor;
	std::shared_ptr<TRIJAST> trij;
	std::shared_ptr<TRLROJAST> trlroj;
	std::shared_ptr<TRNLROJAST> trnlroj;
	std::shared_ptr<TableRefAST> ref;
	tbfactor = ParseTableFactorAST();
	ref = std::make_shared<TableRefAST>(std::move(tbfactor), std::move(trij), std::move(trlroj), std::move(trnlroj));
	while (currtoken.token_kind == symbol &&
		(currtoken.token_value.symbol_mark == tok_INNER || currtoken.token_value.symbol_mark == tok_CROSS
			|| currtoken.token_value.symbol_mark == tok_JOIN
			|| currtoken.token_value.symbol_mark == tok_LEFT || currtoken.token_value.symbol_mark == tok_RIGHT
			|| currtoken.token_value.symbol_mark == tok_NATURAL))
	{
		if (currtoken.token_value.symbol_mark == tok_NATURAL)
		{
			trnlroj = ParseTRNLROJAST(std::move(ref));
			ref = std::make_shared<TableRefAST>(std::move(tbfactor), std::move(trij), std::move(trlroj), std::move(trnlroj));
		}
		else if (currtoken.token_value.symbol_mark == tok_LEFT || currtoken.token_value.symbol_mark == tok_RIGHT)
		{
			trlroj = ParseTRLROJAST(std::move(ref));
			ref = std::make_shared<TableRefAST>(std::move(tbfactor), std::move(trij), std::move(trlroj), std::move(trnlroj));
		}
		else
		{
			trij = ParseTRIJAST(std::move(ref));
			ref = std::make_shared<TableRefAST>(std::move(tbfactor), std::move(trij), std::move(trlroj), std::move(trnlroj));
		}
	}
	return std::move(ref);
}

std::shared_ptr<TRIJAST> ParseTRIJAST(std::shared_ptr<TableRefAST> ref)
{
	std::shared_ptr<TableFactorAST> factor;
	std::shared_ptr<JoinCondAST> cond;
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
	return std::make_shared<TRIJAST>(std::move(ref), std::move(factor), std::move(cond));
}

std::shared_ptr<TRIJAST> ParseTRIJAST()
{
	return ParseTRIJAST(ParseTableRefAST());
}

std::shared_ptr<TRLROJAST> ParseTRLROJAST(std::shared_ptr<TableRefAST> lhs)
{
	std::shared_ptr<TableRefAST> rhs;
	std::shared_ptr<JoinCondAST> cond;
	int lr = 0;
	if (currtoken.token_kind == symbol && (currtoken.token_value.symbol_mark == tok_LEFT ||
		currtoken.token_value.symbol_mark == tok_RIGHT))
	{
		lr = currtoken.token_value.symbol_mark;
		currtoken = gettok();	// consume  LEFT/RIGHT
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
	return std::make_shared<TRLROJAST>(std::move(lhs), std::move(rhs), std::move(cond), lr);
}

std::shared_ptr<TRLROJAST> ParseTRLROJAST()
{
	return ParseTRLROJAST(ParseTableRefAST());
}

std::shared_ptr<TRNLROJAST> ParseTRNLROJAST(std::shared_ptr<TableRefAST> ref)
{
	std::shared_ptr<TableFactorAST> factor;
	int lr = 0;
	consumeit({ tok_NATURAL }, "expect NATURAL\n");
	if (currtoken.token_kind == symbol && (currtoken.token_value.symbol_mark == tok_LEFT ||
		currtoken.token_value.symbol_mark == tok_RIGHT))
	{
		lr = currtoken.token_value.symbol_mark;
		currtoken = gettok();	// consume  LEFT/RIGHT
	}

	if (currtoken.token_kind == symbol && currtoken.token_value.symbol_mark == tok_OUTER)
	{
		currtoken = gettok();	// consume OUTER
	}
	consumeit({ tok_JOIN }, "expect `JOIN` \n");
	factor = ParseTableFactorAST();
	return std::make_shared<TRNLROJAST>(std::move(ref), std::move(factor), lr);

}

std::shared_ptr<TRNLROJAST> ParseTRNLROJAST()
{
	return  ParseTRNLROJAST(ParseTableRefAST());
}

std::shared_ptr<TableFactorAST> ParseTableFactorAST()
{
	std::shared_ptr<TableNameAST> tbname;
	std::shared_ptr<TableQueryAST> tbsub;
	std::shared_ptr<TableRefsAST> tbrefs;
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
	return std::make_shared<TableFactorAST>(std::move(tbname), std::move(tbsub), std::move(tbrefs));
}

std::shared_ptr<JoinCondAST> ParseJoinCondAST()
{
	std::shared_ptr<OnJoinCondAST> oncond;
	std::shared_ptr<UsingJoinCondAST> uselist;
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
	return std::make_shared<JoinCondAST>(std::move(oncond), std::move(uselist));
}

std::shared_ptr<TableNameAST> ParseTableNameAST()
{
	std::shared_ptr<IdAST> tbname;
	std::shared_ptr<IdAST> alias;
	tbname = ParseIdAST();
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_AS)
	{
		currtoken = gettok();	// consume AS
		alias = ParseIdAST();
	}
	return std::make_shared<TableNameAST>(std::move(tbname), std::move(alias));
}

std::shared_ptr<TableQueryAST> ParseTableQueryAST()
{
	std::shared_ptr<SubqueryAST> subq;
	std::shared_ptr<IdAST> alias;
	subq = ParseSubqueryAST();
	consumeit({ tok_AS }, "expect AS");
	alias = ParseIdAST();
	return std::make_shared<TableQueryAST>(std::move(subq), std::move(alias));
}

std::shared_ptr<DatatypeAST> ParseDatatypeAST()
{
	int dtype = 0;
	int n = 1;	//default length is 1 , not 0
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
	return std::make_shared<DatatypeAST>(dtype, n);
}

std::shared_ptr<RefdefAST> ParseRefdefAST()
{
	std::shared_ptr<IdAST> tbname;
	std::vector<std::shared_ptr<IdAST>> colname;
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
	return std::make_shared<RefdefAST>(std::move(tbname), std::move(colname), deleteop, updateop);
}

std::shared_ptr<PrimaryAST> ParsePrimaryAST()
{
	std::vector<std::shared_ptr<IdAST>> cols;
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_PRIMARY)
	{
		consumeit({ tok_PRIMARY }, "expect `PRIMARY` \n");
	}
	//consumeit({ tok_PRIMARY }, "expect keyword PRIMARY \n");
	consumeit({ tok_KEY }, "expect keyword `KEY` \n");
	consumeit({ left_bracket_mark }, "expect '(' \n");
	cols.push_back(ParseIdAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		consumeit({ comma_mark }, "expect ',' \n");
		cols.push_back(ParseIdAST());
	}
	consumeit({ right_bracket_mark }, "expect ')' \n");
	return std::make_shared<PrimaryAST>(std::move(cols));
}

std::shared_ptr<unicAST> ParseunicAST()
{
	std::vector<std::shared_ptr<IdAST>> cols;
	consumeit({ tok_UNIQUE }, "expect keyword shared \n");
	consumeit({ left_bracket_mark }, "expect '(' \n");
	cols.push_back(ParseIdAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		consumeit({ comma_mark }, "expect ',' \n");
		cols.push_back(ParseIdAST());
	}
	return std::make_shared<unicAST>(std::move(cols));
}

std::shared_ptr<ForeignAST> ParseForeignAST()
{
	std::vector<std::shared_ptr<IdAST>> cols;
	std::shared_ptr<RefdefAST> refdef;
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
	return std::make_shared<ForeignAST>(std::move(cols), std::move(refdef));

}

std::shared_ptr<CreatedefAST> ParseCreatedefAST()
{
	std::shared_ptr<IdAST> colname;
	std::shared_ptr<ColdefAST> coldef;
	std::shared_ptr<PrimaryAST> prim;
	std::shared_ptr<unicAST> uni;
	std::shared_ptr<ForeignAST> forei;
	std::shared_ptr<ExprAST> check;
	if (currtoken.token_kind == id)
	{
		colname = ParseIdAST();
		coldef = ParseColdefAST();
	}
	else if (currtoken.token_kind == symbol)
	{
		if (currtoken.token_value.symbol_mark == tok_PRIMARY || currtoken.token_value.symbol_mark == tok_KEY)
		{
			prim = ParsePrimaryAST();
		}
		else if (currtoken.token_value.symbol_mark == tok_UNIQUE)
		{
			uni = ParseunicAST();
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
 column-name/PRIMARY/shared/FOREIGN/CHECK\n");
			return nullptr;
		}
	}
	else
	{
		throw std::runtime_error("expect table-column-creation beginning with\
 column-name/PRIMARY/shared/FOREIGN/CHECK\n");
		return nullptr;
	}
	return std::make_shared<CreatedefAST>(std::move(colname), std::move(coldef),
		std::move(prim), std::move(uni), std::move(forei), std::move(check));
}

std::shared_ptr<ColdefAST> ParseColdefAST()
{
	std::shared_ptr<DatatypeAST> dtype;
	bool null_flag = true;
	std::shared_ptr<ExprAST> default_value;
	bool unic_flag = false;
	bool primary_flag = false;
	std::shared_ptr<RefdefAST> refdef;
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
			consumeit({ tok_UNIQUE }, "expect shared \n");
			unic_flag = true;
			if (currtoken.token_value.symbol_mark == tok_KEY)
			{
				consumeit({ tok_KEY }, "expect KEY \n");
			}
		}
		if (currtoken.token_value.symbol_mark == tok_PRIMARY || currtoken.token_value.symbol_mark == tok_KEY)
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
	return std::make_shared<ColdefAST>(std::move(dtype),
		null_flag, std::move(default_value), unic_flag, primary_flag, std::move(refdef));
}

std::shared_ptr<CreateTableSimpleAST> ParseCreateTableSimpleAST()
{
	//consumeit({ tok_CREATE }, "expect `CREATE`\n");
	consumeit({ tok_TABLE }, "expect `TABLE`\n");
	auto Id = ParseIdAST();
	std::vector<std::shared_ptr<CreatedefAST>> create_defs;
	consumeit({ left_bracket_mark }, "expect `(`\n");
	create_defs.push_back(ParseCreatedefAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		currtoken = gettok();	// consuem ','
		create_defs.push_back(ParseCreatedefAST());
	}
	consumeit({ right_bracket_mark }, "expect `)`\n");
	return std::make_shared<CreateTableSimpleAST>(std::move(Id), std::move(create_defs));
}

std::shared_ptr<SelectAST> ParseSelectAST()
{
	auto subq = ParseSubqueryAST();
	//consumeit({ delimiter }, "expect delimiter \n");
	return std::make_shared<SelectAST>(std::move(subq));
}

std::shared_ptr<StatementAST> ParseStatementAST()
{
	std::shared_ptr<CreateAST> create;
	std::shared_ptr<SelectAST> select;
	std::shared_ptr<DropAST> drop;
	std::shared_ptr<InsertAST> insert;
	std::shared_ptr<DeleteAST> dele;
	std::shared_ptr<SetAST> setvar;
	if (currtoken.token_kind == symbol && (currtoken.token_value.symbol_mark == tok_SET ||
		currtoken.token_value.symbol_mark == tok_INSERT || currtoken.token_value.symbol_mark == tok_SELECT ||
		currtoken.token_value.symbol_mark == tok_CREATE || currtoken.token_value.symbol_mark == tok_DROP ||
		currtoken.token_value.symbol_mark == tok_DELETE || currtoken.token_value.symbol_mark == semicolon_mark||
		currtoken.token_value.symbol_mark==tok_QUIT))
	{
		if (currtoken.token_value.symbol_mark == tok_SET)
		{
			setvar = ParseSetAST();
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
			if (create->ctable)
			{
				auto tb_cata = create->ctable->simplecreate;
				make_cata(tb_cata);
			}
		}
		else if (currtoken.token_value.symbol_mark == tok_DROP)
		{
			drop = ParseDropAST();
		}
		else if (currtoken.token_value.symbol_mark == tok_DELETE)
		{
			dele = ParseDeleteAST();
		}
		else if (currtoken.token_value.symbol_mark == tok_QUIT)
		{
			consumeit({ tok_QUIT,delimiter }, "expect `QUIT;` \n");
			return nullptr;
		}
		else if (currtoken.token_value.symbol_mark == semicolon_mark)
		{
			;
		}
		consumeit({ delimiter }, "expect delimiter\n");
		if (insert != nullptr)
		{
			// ����catalog::tabelbase  // ��blk�һ��� vctor<record>,��ÿ��Ը�����ѹ��1�У����Ϻ���
			string tbname = *(insert->table_name->id.get());
			string db_flie = toysql::record_path + std::to_string(catalog::catamap[tbname]) + ".db";
			ifstream rsx(db_flie, ifstream::ate|ifstream::binary);
			int dbsize = rsx.tellg();
			rsx.close();
			vector<int> pgs;
			vector<record> tbrecoxd;
			vector<record>tbrecord;
			vector<shared_ptr<DataValue>> vals;
			catalog::SQLtable tbinfo;
			// ��ȡ��������Ϣ  // ȡ��insertʽ���и����е�ֵ
			auto exprs = insert->value_list;
			for (auto e : exprs)
			{
				auto lite = e->lhs->bp->p->bitexpr->bitexp->bitex->SE->lit;
				if (lite->doublevalue)
				{
					vals.push_back(std::make_shared<DataDouble>(*(lite->doublevalue->value.get())));
				}
				if (lite->intvalue)
				{
					vals.push_back(std::make_shared<DataInt>(*(lite->intvalue->value.get())));
				}
				if (lite->stringvalue)
				{
					vals.push_back(std::make_shared<DataString>(*(lite->stringvalue->value.get())));
				}
			}
			// ����ǰ�ļ��: primary��unic
			// check prim
			if (!tbinfo.primcols.empty())
			{
				string prim_key = tbinfo.primcols[0];
				//�����prim�ǵڼ���
				int iter1 = 0;
				for (iter1 = 0; iter1 < vals.size(); ++iter1)
				{
					if (prim_key == tbinfo.cols[iter1].colname)
						break;
				}
				// ���� prim
				// �õ�vals[iter1]
				shared_ptr<DataValue> insert_prim = vals[iter1];
				for (auto iter2 : tbrecord)
				{
					if (iter2.data[iter1]->getValue() == insert_prim->getValue())
					{
						std::cout << "duplicate primary key on`"
							<< tbinfo.cols[iter1].colname << "` value" << insert_prim->getValue() << std::endl;
						return std::make_shared<StatementAST>(std::move(create), std::move(select),
							std::move(drop), std::move(insert), std::move(dele), std::move(setvar));
					}
				}
			}

			// check unic
			if (!tbinfo.uniccols.empty())
			{
				for (string uni_key : tbinfo.uniccols)
				{
					//�����uni�ǵڼ���
					int iter1 = 0;
					for (iter1 = 0; iter1 < vals.size(); ++iter1)
					{
						if (uni_key == tbinfo.cols[iter1].colname)
							break;
					}
					// ���� uni
					// �õ�vals[iter1]
					shared_ptr<DataValue> insert_uni = vals[iter1];
					for (auto iter2 : tbrecord)
					{
						if (iter2.data[iter1]->getValue() == insert_uni->getValue())
						{
							std::cout << "duplicate unique key on`"
								<< tbinfo.cols[iter1].colname << "` value" << insert_uni->getValue() << std::endl;
							return std::make_shared<StatementAST>(std::move(create), std::move(select),
								std::move(drop), std::move(insert), std::move(dele), std::move(setvar));
						}
					}
				}

			}

			// insert�������㣬����catalog::tabelbase
			int tb_index = catalog::catamap[tbname];
			catalog::tablebase[tb_index].isinmemory = true;
			catalog::tablebase[tb_index].pages = pgs;
			// ����λ��
			// 5. д��delete recordλ�ã���¼��ҳ
			bool find_dirty = false;
			for (auto& r : tbrecoxd)
			{
				if (r.isdeleted)
				{
					find_dirty = true;
					// �ڴ˴�����
					r = record(r.pos, r.series,tbinfo.record_size, vals);
					catalog::tablebase[tb_index].record_num++;
					catalog::tablebase[tb_index].isinmemory = true;
				}
			}
			// 6. ���û�ҵ�����blkβ��д�룬��¼��ҳ
			std::sort(pgs.begin(), pgs.end());
			int mo = -1;
			bool find_bottom = false;
			if (find_dirty)
			{
				mo = *(pgs.end() - 1);
				 
				// β���Ƿ���ã�
				find_bottom = (BLOCK_8k - BufferManager[mo].bytes*BufferManager[mo].recordnum) > BufferManager[mo].bytes;
				if (find_bottom)
				{
					// �ڴ˴�����
					BufferManager[mo].isdirty = true;
					BufferManager[mo].recordnum++;
					catalog::tablebase[tb_index].record_num++;
					catalog::tablebase[tb_index].isinmemory = true;
					// record ���� blk
					trans2block(nullptr,vals,true);
				}
			}
		    // 7. �����û�ҵ�������ҳ����¼��ҳ������catalog::tablebase
			if (!find_dirty)
			{
				catalog::tablebase[catalog::catamap[tbname]].record_num++;
				char t8[8192]{0};
				trans2block(t8,vals, true);
				ofstream ofs(db_flie, ofstream::binary | ofstream::app);
				int k = catalog::tablebase[catalog::catamap[tbname]].record_size;
				for (int i = 0; i != k; ++i)
					ofs << t8[i];
				ofs.close();
			}
			// 8. ����AST����������record // �Ѿ��������if�б������������
		}
		else if(drop&&drop->droptb)
		{
			string tbname = *(drop->droptb->table_list[0]->id.get());
			// num.log
			int n = 0;
			string nlog= catalog::cata_path + "num.log";
			ifstream numlog(nlog);
			numlog >> n;
			numlog.close();
			// map.log
			vector<string> tn(n); vector<int> in(n);
			string mlog = catalog::cata_path + "map.log";
			ifstream maplog(mlog);
			for (int i = 0; i != n; ++i)
			{
				maplog >> tn[i] >> in[i];
			}
			int findi = 0;
			for (int i = 0; i != n; ++i)
			{
				if (tn[i] == tbname)
					findi = i;
			}
			maplog.close();
			ofstream omaplog(mlog);
			for (int i = 0; i < findi; ++i)
			{
				omaplog << tn[i] << "\t" << in[i];
			}
			for (int i = findi+1; i < n; ++i)
			{
				omaplog << tn[i] << "\t" << in[i];
			}
			omaplog.close();
			// #.log
			string signlog = catalog::cata_path + std::to_string(n) + ".log";
			FILE* w1 = fopen(signlog.c_str(), "wb");
			fclose(w1);
			// #.db
			string signdb = toysql::record_path + std::to_string(n) + ".db";
			FILE* w2 = fopen(signdb.c_str(), "wb");
			fclose(w2);
		}
		else if(select)
		{
			string outfile =  std::to_string(selecttimes++) + ".output";
			int n = select->subquery->exprs.size();
			string tbname = *(select->subquery->tbrefs->refs[0]->tbfactor->tbname->tbname->id.get());
			string infile = toysql::record_path + std::to_string(catalog::catamap[tbname]) + ".db";
			int recordsize = catalog::tablebase[catalog::catamap[tbname]].record_size;
			int recordnum = catalog::tablebase[catalog::catamap[tbname]].record_num;
			ofstream ofs(outfile);
			vector<string> colnames; 
			for (int i = 0; i != catalog::tablebase[catalog::catamap[tbname]].col_num; ++i)
			{
				colnames.push_back(catalog::tablebase[catalog::catamap[tbname]].cols[i].colname);
				ofs << colnames[i] << "\t\t";
			}
			
			
			int col_num = catalog::tablebase[catalog::catamap[tbname]].col_num;
			auto cs = catalog::tablebase[catalog::catamap[tbname]].cols;
			int bytes = catalog::tablebase[catalog::catamap[tbname]].record_size;
			vector<int> cstype;
			for (auto col : cs)
			{
				cstype.push_back(col.coltype);
			}
			vector<record> result;
			FILE* ifs = fopen(infile.c_str(), "rb");
			for (int i = 0; i != recordnum; ++i)
			{
				char t8[8192]{ 0 };
				fread(t8, sizeof(char), bytes, ifs);
//  vector<shared_ptr<DataValue>> trans2record(vector<int> cstype,const int& bytes, int curch, char* ptr)				
				auto dv = trans2record(cstype, bytes, 0, buff[i]);
				for (auto item : dv)
					ofs << item << "\t";
				ofs << std::endl;
			}
			
		}
		else
		{
			;
		}
	}
	else
	{
		throw std::runtime_error("statement sentence can only begin with SET/INSERT/SELECT/CREATE/DROP/INSERT/DELETE/QUIT/; \n");
		return nullptr;
	}
	return std::make_shared<StatementAST>(std::move(create), std::move(select),
		std::move(drop), std::move(insert), std::move(dele), std::move(setvar));
}

std::shared_ptr<CreateAST> ParseCreateAST()
{
	std::shared_ptr<CreateTableAST> ctable;
	std::shared_ptr<CreateIndexAST> cindex;
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
	return std::make_shared<CreateAST>(std::move(ctable), std::move(cindex));
}

std::shared_ptr<CreateTableAST> ParseCreateTableAST()
{
	std::shared_ptr< CreateTableSimpleAST> simplecreate;
	std::shared_ptr<CreateTableSelectAST> selectcreate;
	std::shared_ptr<CreateTableLikeAST> likecreate;
	simplecreate = ParseCreateTableSimpleAST();
	return std::make_shared<CreateTableAST>(std::move(simplecreate), std::move(selectcreate), std::move(likecreate));
}

std::shared_ptr<CreateIndexAST> ParseCreateIndexAST()
{
	std::shared_ptr<IdAST> index_name;
	std::shared_ptr<IdAST> table_name;
	std::vector<std::shared_ptr<IdAST>> col_names;
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
	return std::make_shared<CreateIndexAST>(std::move(index_name), std::move(table_name), std::move(col_names));
}

std::shared_ptr<DropAST> ParseDropAST()
{
	std::shared_ptr<DropTableAST> droptb;
	std::shared_ptr<DropIndexAST> dropindex;
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
	return std::make_shared<DropAST>(std::move(droptb), std::move(dropindex));
}

std::shared_ptr<DropTableAST> ParseDropTableAST()
{
	consumeit({ tok_TABLE }, "expect TABLE \n");
	std::vector<std::shared_ptr<IdAST>> table_list;
	table_list.push_back(ParseIdAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		consumeit({ comma_mark }, "expect ','\n");
		table_list.push_back(ParseIdAST());
	}
	return std::make_shared<DropTableAST>(std::move(table_list));
}

std::shared_ptr<DropIndexAST> ParseDropIndexAST()
{
	consumeit({ tok_INDEX }, "expect INDEX \n");
	std::vector<std::shared_ptr<IdAST>> index_list;
	index_list.push_back(ParseIdAST());
	while (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == comma_mark)
	{
		consumeit({ comma_mark }, "expect ','\n");
		index_list.push_back(ParseIdAST());
	}
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_ON)
	{
		consumeit({ tok_ON }, "expect `ON` \n");
		auto id = ParseIdAST();
	}
	return std::make_shared<DropIndexAST>(std::move(index_list));
}

std::shared_ptr<InsertAST> ParseInsertAST()
{
	std::shared_ptr<IdAST> table_name;
	std::vector<std::shared_ptr<IdAST>> col_names;
	std::vector<std::shared_ptr<ExprAST>> value_list;
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
	consumeit({ right_bracket_mark }, "expect ')' \n");
	return std::make_shared<InsertAST>(std::move(table_name), std::move(col_names), std::move(value_list));
}

std::shared_ptr<DeleteAST> ParseDeleteAST()
{
	std::shared_ptr<IdAST> table_name;
	std::shared_ptr< ExprAST> where_condition;
	consumeit({ tok_DELETE }, "expect DELETE\n");
	consumeit({ tok_FROM }, "expect FROM\n");
	table_name = ParseIdAST();
	if (currtoken.token_kind == symbol&&currtoken.token_value.symbol_mark == tok_WHERE)
	{
		consumeit({ tok_WHERE }, "expect WHERE\n");
		where_condition = ParseExprAST();
	}
	return std::make_shared<DeleteAST>(std::move(table_name), std::move(where_condition));
}

std::shared_ptr<SetAST> ParseSetAST()
{
	consumeit({ at_mark }, "expect `@` \n");
	shared_ptr<IdAST> id;
	shared_ptr<ExprAST> expr;
	id = ParseIdAST();
	expr = ParseExprAST();
	return std::make_shared<SetAST>(std::move(id), std::move(expr));
}

std::shared_ptr<CreateTableSelectAST> ParseCreateTableSelectAST();
std::shared_ptr<CreateTableLikeAST> ParseCreateTableLikeAST();

std::shared_ptr<SimpleExprAST> negative_SE(std::shared_ptr<SimpleExprAST> se);
std::shared_ptr<SimpleExprAST> plus_SE(std::shared_ptr<SimpleExprAST> lhs, std::shared_ptr<SimpleExprAST> rhs);
std::shared_ptr<SimpleExprAST> minus_SE(std::shared_ptr<SimpleExprAST> lhs, std::shared_ptr<SimpleExprAST> rhs);
std::shared_ptr<SimpleExprAST> mul_SE(std::shared_ptr<SimpleExprAST> lhs, std::shared_ptr<SimpleExprAST> rhs);
std::shared_ptr<SimpleExprAST> div_SE(std::shared_ptr<SimpleExprAST> lhs, std::shared_ptr<SimpleExprAST> rhs);
std::shared_ptr<SimpleExprAST> mod_SE(std::shared_ptr<SimpleExprAST> lhs, std::shared_ptr<SimpleExprAST> rhs);
std::shared_ptr<SimpleExprAST> in_SE(std::shared_ptr<SimpleExprAST> lhs, std::shared_ptr<SimpleExprAST> rhs);
std::shared_ptr<SimpleExprAST> eq_SE(std::shared_ptr<SimpleExprAST> lhs, std::shared_ptr<SimpleExprAST> rhs);
std::shared_ptr<SimpleExprAST> gt_SE(std::shared_ptr<SimpleExprAST> lhs, std::shared_ptr<SimpleExprAST> rhs);
std::shared_ptr<SimpleExprAST> lt_SE(std::shared_ptr<SimpleExprAST> lhs, std::shared_ptr<SimpleExprAST> rhs);
std::shared_ptr<SimpleExprAST> gteq_SE(std::shared_ptr<SimpleExprAST> lhs, std::shared_ptr<SimpleExprAST> rhs);
std::shared_ptr<SimpleExprAST> lteq_SE(std::shared_ptr<SimpleExprAST> lhs, std::shared_ptr<SimpleExprAST> rhs);
std::shared_ptr<SimpleExprAST> gtlt_SE(std::shared_ptr<SimpleExprAST> lhs, std::shared_ptr<SimpleExprAST> rhs);
std::shared_ptr<SimpleExprAST> noteq_SE(std::shared_ptr<SimpleExprAST> lhs, std::shared_ptr<SimpleExprAST> rhs);
std::shared_ptr<SimpleExprAST> not_SE(std::shared_ptr<SimpleExprAST> se);
std::shared_ptr<SimpleExprAST> and_SE(std::shared_ptr<SimpleExprAST> lhs, std::shared_ptr<SimpleExprAST> rhs);
std::shared_ptr<SimpleExprAST> or_SE(std::shared_ptr<SimpleExprAST> lhs, std::shared_ptr<SimpleExprAST> rhs);


/*
std::shared_ptr<SimpleExprAST> SimpleExprAST::traitValue()
{
	if (id) return id->traitValue();
	if (call) return call->traitValue();
	if (tablecol) return tablecol->traitValue();
	//if(expr)return expr->
	//if(sub)return sub->
	//if(exists)return exists->
	if (lit) return lit->traitValue();
	return nullptr;
}

std::shared_ptr<SimpleExprAST> LiteralAST::traitValue()
{
	if (intvalue) return intvalue->traitValue();
	if (doublevalue) return doublevalue->traitValue();
	if (stringvalue) return stringvalue->traitValue();
	return nullptr;
}
*/

std::shared_ptr<SimpleExprAST> aux_binary_op(std::shared_ptr<SimpleExprAST>, std::shared_ptr<SimpleExprAST>)
{
	return nullptr;
}

std::shared_ptr<SimpleExprAST> aux_unit_op(std::shared_ptr<SimpleExprAST>)
{
	return nullptr;
}

std::shared_ptr<SimpleExprAST> getValue(std::shared_ptr<SimpleExprAST> SE, decltype(aux_unit_op) op)
{
	return op(SE);
}

std::shared_ptr<SimpleExprAST> getValue(std::shared_ptr<SimpleExprAST> lhs,
	std::shared_ptr<SimpleExprAST> rhs, decltype(aux_binary_op) op)
{
	return op(lhs, rhs);
}