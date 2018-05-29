#pragma once

#ifndef llvmsql_h
#define llvmsql_h

class value;

class int_value;

class double_value;

class string_value;

class symbol_value;

class token;

void init_scanner();

namespace scan_nsp
{
	class value
	{
	public:
		//value(const value& a):s(a.s),i(a.i),d(a.d){}
		virtual ~value() {}
	};

	class int_value :public value
	{
		int i;
	public:
		int_value(int i) :i(i) {}
	};

	class double_value :public value
	{
		double d;
	public:
		double_value(double d) :d(d) {}
	};

	class string_value :public value
	{
		std::string s;
	public:
		string_value(std::string s) :s(s) {}
	};

	class reserved_value :public value
	{
		int t;
	public:
		reserved_value(int t) :t(t) {}
	};

	class id_value :public value
	{
		std::string s;
	public:
		id_value(std::string s) :s(s) {}
	};

	class eof_value :public value
	{
		const int v = 0;
	};

	class token
	{
	public:
		int token_kind;
		value token_value;

		token() = default;
		token(const token& a) :token_kind(a.token_kind), token_value(a.token_value) {}
		token& operator=(const token& b)
		{
			token_kind = b.token_kind;
			token_value = b.token_value;
		}
	};

	token gettok();
}
namespace parse_nsp {}
#endif // !llvmsql_h



