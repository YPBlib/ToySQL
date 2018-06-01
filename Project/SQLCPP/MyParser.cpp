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

	
#define tokenLen 6

token curtoken[tokenLen];

void scrolltoken()
{
	for (int i = 0; i < tokenLen - 1; ++i)
		curtoken[i] = curtoken[i + 1];
	curtoken[tokenLen - 1] = gettok();
	
}

std::unique_ptr<ExprAST> ParseDoubleLiteralAST()
{
	auto Result = llvm::make_unique<DoubleLiteralAST>(curtoken[0].token_value);
	scrolltoken();
	return std::move(Result);
}

std::unique_ptr<ExprAST> ParseIntLiteralAST()
{
	auto Result = llvm::make_unique<IntLiteralAST>(curtoken[0].token_value);
	scrolltoken(); // consume the number
	return std::move(Result);
}

std::unique_ptr<ExprAST> ParseStringLiteralAST()
{
	auto Result = llvm::make_unique<StringLiteralAST>(curtoken[0].token_value);
	scrolltoken(); // consume the number
	return std::move(Result);
}

std::string callee;
std::vector<std::unique_ptr<ExprAST>> args;

std::unique_ptr<ExprAST> ParseCallAST()
{
	auto Result = llvm::make_unique<CallAST>(curtoken[0].token_value);
	scrolltoken(); // consume the number
	return std::move(Result);
}