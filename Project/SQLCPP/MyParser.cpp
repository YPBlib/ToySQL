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



class ExprAST
{
public:
	virtual ~ExprAST() = default;
};

class BooleanPrimaryAST :public ExprAST {};
class PredicateAST :public BooleanPrimaryAST {};
class BitExprAST :public PredicateAST {};
class UnitSEAST:public BitExprAST{};
class SimpleExprAST :public UnitSEAST {};
class LiteralAST :public SimpleExprAST {};
class IdAST :public SimpleExprAST {};
class ParenExprAST:public SimpleExprAST{};


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

class IntLiteralAST final:public LiteralAST
{
	int value;
public:
	IntLiteralAST(int value) :value(value) {}
};

class DoubleLiteralAST final: public LiteralAST
{
	double value;
public:
	DoubleLiteralAST(double value) : value(value) {}
};

class StringLiteralAST final:public LiteralAST
{
	std::string value;
public:
	StringLiteralAST(const std::string& value) :value(value) {}
};

class VarAST final:public IdAST
{
	std::string var_name;
public:
	VarAST(const std::string& var_name):var_name(var_name){}
};

class ArgAST final :public IdAST
{
	std::string arg_name;
public:
	ArgAST(const std::string& arg_name) :arg_name(arg_name) {}
};

class CalleeAST final :public IdAST
{
	std::string callee_name;
public:
	CalleeAST(const std::string& callee_name) :callee_name(callee_name) {}
};

class TablenameAST final :public IdAST
{
	std::string table_name;
public:
	TablenameAST(const std::string& table_name) :table_name(table_name) {}
};

class ColnameAST final :public IdAST
{
	std::string table_name; 
	std::string col_name;
public:
	ColnameAST(const std::string& col_name) :table_name(""),col_name(col_name){}
	ColnameAST(const std::string& table_name, const std::string& col_name) :table_name(table_name), col_name(col_name){}
};



class SubqueryAST final:public SimpleExprAST
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
		from_flag(from_flag),tbrefs(std::move(tbrefs)),
		where_flag(where_flag), wherecond(std::move(wherecond)),
		having_flag(having_flag), havingcond(std::move(havingcond)),
		group_flag(group_flag), groupby_col_name(std::move(groupby_col_name)),
		order_flag(order_flag), orderby_col_name(std::move(orderby_col_name))
		 {}
};

class ExistsSubqueryAST final:public ParenExprAST
{
	std::unique_ptr<SubqueryAST> subquery;
public:
	ExistsSubqueryAST(std::unique_ptr<SubqueryAST> subquery) :subquery(std::move(subquery)) {}
};


class TableRefsAST
{
	std::vector<std::unique_ptr<TableRefAST>> refs;
public:
	TableRefsAST(std::vector<std::unique_ptr<TableRefAST>> refs) :refs(std::move(refs)) {}
};
class JoinCondAST {};

class TableRefAST
{
public:
	std::string topalias;
};

class TableFactorAST :public TableRefAST {};
class JoinTableAST :public TableRefAST {};

class TableNameAST final :public TableFactorAST
{
	std::string name;
public:
	TableNameAST(const std::string& name) :name(name) {}
};

class TableQueryAST final :public TableFactorAST
{
	std::string alias;
	std::unique_ptr<SubqueryAST> subq;
public:
	TableQueryAST(const std::string& alias, std::unique_ptr<SubqueryAST> subq) :
		alias(alias), subq(std::move(subq)) {}

};

class ParenTableRefAST final :public TableFactorAST
{
	std::unique_ptr<TableRefsAST> refs;
public:
	ParenTableRefAST(std::unique_ptr<TableRefsAST> refs) :
		refs(std::move(refs)) {}
};

class OnJoinCondAST final :public JoinCondAST
{
	std::unique_ptr<ExprAST> cond;
public:
	OnJoinCondAST(std::unique_ptr<ExprAST> cond):
		cond(std::move(cond)){}
};

class UsingJoinCondAST final :public JoinCondAST
{
	std::vector<std::unique_ptr<ColnameAST>> cols;
	UsingJoinCondAST(std::vector<std::unique_ptr<ColnameAST>> cols):
		cols(std::move(cols)){}
};

class TRIJAST final :public JoinTableAST
{
	std::unique_ptr<TableRefAST> ref;
	std::unique_ptr<TableFactorAST> factor;
	std::unique_ptr<JoinCondAST> cond;
public:
	bool innerflag = false;
	bool crossflag = false;
	TRIJAST(std::unique_ptr<TableRefAST> ref, std::unique_ptr<TableFactorAST> factor) :
		ref(std::move(ref)), factor(std::move(factor)), cond(nullptr) {}
	TRIJAST(std::unique_ptr<TableRefAST> ref, std::unique_ptr<TableFactorAST> factor, std::unique_ptr<JoinCondAST> cond) :
		ref(std::move(ref)), factor(std::move(factor)), cond(std::move(cond)) {}
};

class TRLROJAST final :public JoinTableAST
{
	std::unique_ptr<TableRefAST> lhs;
	std::unique_ptr<TableRefAST> rhs;
	std::unique_ptr<JoinCondAST> cond;
public:
	bool leftflag = false;
	bool rightflag = false;
	bool outerflag = false;
	TRLROJAST(std::unique_ptr<TableRefAST> lhs, std::unique_ptr<TableRefAST> rhs, std::unique_ptr<JoinCondAST> cond) :
		lhs(std::move(lhs)), rhs(std::move(rhs)), cond(std::move(cond)) {}
};

class TRNLROJAST final :public JoinTableAST
{
	std::unique_ptr<TableRefAST> ref;
	std::unique_ptr<TableFactorAST> factor;
public:
	bool leftflag = false;
	bool rightflag = false;
	bool outerflag = false;
	TRNLROJAST(std::unique_ptr<TableRefAST> ref, std::unique_ptr<TableFactorAST> factor) :
		ref(std::move(ref)), factor(std::move(factor)) {}
};



class CallAST final :public SimpleExprAST
{
	std::string callee;
	std::vector<std::unique_ptr<ExprAST>> args;
public:
	CallAST(const std::string &callee, std::vector<std::unique_ptr<ExprAST>> args)
		: callee(callee), args(std::move(args)) {}
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
	
class CreateAST :public StatementAST {};

class CreateTableAST :public CreateAST {};
	
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

token curtoken[LL_LRLen];

void token_init()
{
	token t;
	t.token_kind = blank;
	t.token_value = blank_value(' ');
	for (int i = 0; i < LL_LRLen; ++i)
		curtoken[i] = t;
}

void scrolltoken()
{
	for (int i = 0; i < LL_LRLen - 1; ++i)
		curtoken[i] = curtoken[i + 1];
	curtoken[LL_LRLen - 1] = gettok();
}


std::unique_ptr<StringLiteralAST> ParseStringLiteralAST()
{
	std::string temps = curtoken[0].token_value.string_literal;
	scrolltoken(); // consume 1 string token
	// wait,  this string may be concat
	while (curtoken[0].token_kind == literal_string)
	{
		temps += curtoken[0].token_value.string_literal;
		scrolltoken(); // consume 1 string token
	}
	auto result = llvm::make_unique<StringLiteralAST>(temps);
	return std::move(result);
}

std::unique_ptr<IntLiteralAST> ParseIntLiteralAST()
{
	auto result = llvm::make_unique<IntLiteralAST>(curtoken[0].token_value.int_literal);
	scrolltoken(); // consume 1 int token
	return std::move(result);
}

std::unique_ptr<DoubleLiteralAST> ParseDoubleLiteralAST()
{
	auto result = llvm::make_unique<DoubleLiteralAST>(curtoken[0].token_value.double_literal);
	scrolltoken(); // consume 1 double token
	return std::move(result);
}

std::unique_ptr<IdAST> ParseIdAST()
{
	auto result = llvm::make_unique<IdAST>(curtoken[0].token_value);
	scrolltoken(); // consume 1 id token
	return std::move(result);
}

std::unique_ptr<CallAST> ParseCallAST()
{
	std::string callee = (curtoken[0]).token_value.string_literal;
	scrolltoken(); // consume 1 callee name;
	if (!(curtoken[0].token_kind == symbol&&curtoken[0].token_value.symbol_mark == left_bracket_mark))
	{
		throw std::runtime_error("missing '(' when parsing function call");
	}
	auto args = ParseBracketExprseqAST();
	return llvm::make_unique<CallAST>(callee,args);
}

std::unique_ptr<SEOrormarkSEAST> ParseSEOrormarkSEAST()
{
	auto LHS = ParseSimpleExprAST();
}

std::unique_ptr<BracketExprseqAST> ParseBracketExprseqAST()
{
	scrolltoken(); // consume 1 '(';
	std::vector<std::unique_ptr<SimpleExprAST>> exprs;
	auto se = ParseSimpleExprAST();
	exprs.push_back(se);
	while (!(curtoken[0].token_kind == symbol &&curtoken[0].token_value.symbol_mark == right_bracket_mark))
	{
		if (!(curtoken[0].token_kind == symbol &&curtoken[0].token_value.symbol_mark == comma_mark))
		{
			throw std::runtime_error("missing  ',' in args list");
		}	
		scrolltoken(); // consume 1 ',';
		auto se = ParseSimpleExprAST();
		exprs.push_back(se);
	}
	if (!(curtoken[0].token_kind == symbol &&curtoken[0].token_value.symbol_mark == right_bracket_mark))
	{
		throw std::runtime_error("missing  ')' when args list ends");
	}
	scrolltoken(); // consume 1 ')';
	return llvm::make_unique<BracketExprseqAST>(std::move(exprs));
}

std::unique_ptr<SimpleExprAST> ParseSimpleExprAST()
{
	;
}