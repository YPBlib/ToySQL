











class value
{
public:
	//value(const value& a):s(a.s),i(a.i),d(a.d){}
	virtual ~value(){}
};

class int_value :public value
{
	int i;
public:
	int_value(int i):i(i){}
};

class double_value :public value
{
	double d;
public:
	double_value(double d):d(d){}
};

class string_value :public value
{
	std::string s;
public:
	string_value(std::string s):s(s){}
};

class symbol_value :public value
{
	Token t;
public:
	symbol_value(Token t):t(t){}
};

class id_value :public value
{
	std::string s;
public:
	id_value(std::string s):s(s){}
};

class reserved_value :public value
{
	std::string s;
public:
	reserved_value(std::string s) :s(s) {}
};

class token
{
public:
	int token_kind;
	value token_value;

	token() = default;
	token(const token& a):token_kind(a.token_kind),token_value(a.token_value){}
	token& operator=(const token& b)
	{
		token_kind = b.token_kind;
		token_value = b.token_value;
	}
};

bool isidchar(int c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

void scroll_Char(int* ahead)
{
  // 处理字符流已经耗尽，出现EOF的corner case
  /*
      do something;
  */
  for(int i=0;i<LookAheadLen-1;++i)
  {
    ahead[i]=ahead[i+1];
  }
  ahead[LookAheadLen-1]=getchar();
}

token gettok() 
{
	static int scanner_status = blank;

	if(scanner_status!=blank)
  {
    fprintf(stderr,"scanner_status is not blank，but you simply call gettok() \n");
  }

  // Skip any whitespace.
  while (isspace(LookAhead[0]))
  {
    scroll_Char(LookAhead);
  }
  // # style comment
  if (LookAhead[0] == '#')
  {   
    scanner_status=comment;
    do
      scroll_Char(LookAhead);
    while (LookAhead[0] != EOF && LookAhead[0] != '\n' && LookAhead[0] != '\r');
    scanner_status=blank;
  }
  // -- style comment
  if(LookAhead[0] == '-'&& LookAhead[1] == '-')
  {
    scanner_status=comment;
    do
      scroll_Char(LookAhead);
    while (LookAhead[0] != EOF && LookAhead[0] != '\n' && LookAhead[0] != '\r');
    scanner_status=blank;
  }
  // /* */ style comment
  // 注意 /*/ 不是合法的注释
  if(LookAhead[0] == '/'&& LookAhead[1] == '*')
  {
    scanner_status=comment;
    scroll_Char(LookAhead);
    do
      scroll_Char(LookAhead);
    while (!(LookAhead[0] == '*' && LookAhead[1] == '/'));
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    scanner_status=blank;
  }


  // 字符串字面量 肥肠值得注意 \的转义
  // 注意 \的转义，对于静态字符串有效，对于交互输入无效
  // 字符串粘连还没做，后续填坑
  if(LookAhead[0]=='"'||LookAhead[0]=='\'')
  {
	  scanner_status = literal_string;
    auto match_char=LookAhead[0];
    scroll_Char(LookAhead);
    string_literal.clear();
    while(1)
    {
      if(LookAhead[0]!=match_char && LookAhead[0] != '\\')
      {
        string_literal+=static_cast<char>(LookAhead[0]);
        scroll_Char(LookAhead);
        continue;
      }

      if(LookAhead[0]=='\\')
      {
        if(LookAhead[1]=='0')
        {
          string_literal+='\0';
          scroll_Char(LookAhead);
          scroll_Char(LookAhead);
          continue;
        }
        if(LookAhead[1]=='\'')
        {
          string_literal+='\'';
          scroll_Char(LookAhead);
          scroll_Char(LookAhead);
          continue;
        }
        if(LookAhead[1]=='\"')
        {
          string_literal+='\"';
          scroll_Char(LookAhead);
          scroll_Char(LookAhead);
          continue;
        }
        if(LookAhead[1]=='b')
        {
          string_literal+='\b';
          scroll_Char(LookAhead);
          scroll_Char(LookAhead);
          continue;
        }
        if(LookAhead[1]=='n')
        {
          string_literal+='\n';
          scroll_Char(LookAhead);
          scroll_Char(LookAhead);
          continue;
        }
        if(LookAhead[1]=='r')
        {
          string_literal+='\r';
          scroll_Char(LookAhead);
          scroll_Char(LookAhead);
          continue;
        }
        if(LookAhead[1]=='t')
        {
          string_literal+='\t';
          scroll_Char(LookAhead);
          scroll_Char(LookAhead);
          continue;
        }
        if(LookAhead[1]=='Z')
        {
          string_literal+=static_cast<char>(26);
          scroll_Char(LookAhead);
          scroll_Char(LookAhead);
          continue;
        }
        if(LookAhead[1]=='\\')
        {
          string_literal+='\\';
          scroll_Char(LookAhead);
          scroll_Char(LookAhead);
          continue;
        }
        if(LookAhead[1]=='%')
        {
          string_literal+='%';
          scroll_Char(LookAhead);
          scroll_Char(LookAhead);
          continue;
        }
        if(LookAhead[1]=='_')
        {
          string_literal+='_';
          scroll_Char(LookAhead);
          scroll_Char(LookAhead);
          continue;
        }
        if(LookAhead[1]=='\n')
        {
          string_literal+='\n';
          scroll_Char(LookAhead);
          scroll_Char(LookAhead);
          continue;
        }
        throw; // "非法的转义序列， 本SQL方言仅支持R("\0 \' \" \b \n \r \t \Z \\ \% \_ \\n")"
      }

      if(LookAhead[0]==match_char)
      {
        auto t = token();
        t.token_kind=literal_string;
		t.token_value = string_value(string_literal);
        scroll_Char(LookAhead);
        scanner_status=blank;
        return t;
      }

      // can't get here
      throw "字符串少一半引号？";

    }
    

  }

  // true or false 字面量
  if(tolower(LookAhead[0])=='t' && tolower(LookAhead[1])=='r' && \
  tolower(LookAhead[2])=='u' && tolower(LookAhead[3])=='e')
  {
    scanner_status=literal_int;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = int_value(1);
    scanner_status=blank;
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    return t;
  }

  if(tolower(LookAhead[0])=='f' && tolower(LookAhead[1])=='a' && \
  tolower(LookAhead[2])=='l' && tolower(LookAhead[3])=='s' && tolower(LookAhead[4])=='e')
  {
    scanner_status=literal_int;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = int_value(0);
    scanner_status=blank;
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    return t;
  }

  // 数值字面量等会再写

  // 运算符
  // 务必先判断长的运算符比如<=>
  // 后判断短的运算符比如<=
  if(LookAhead[0]=='<' && LookAhead[1]=='=' && LookAhead[2]=='>')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(lteqgt_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='<' && LookAhead[1]=='<')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
    t.token_value=symbol_value(left_shift_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    return t;
  }

   if(LookAhead[0]=='>' && LookAhead[1]=='>')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
    t.token_value=symbol_value(right_shift_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    return t;
  }

   if(LookAhead[0]=='>' && LookAhead[1]=='=')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
    t.token_value=symbol_value(gteq_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    return t;
  }

   if(LookAhead[0]=='<' && LookAhead[1]=='=')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
    t.token_value=symbol_value(lteq_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    return t;
  }

   if(LookAhead[0]=='<' && LookAhead[1]=='>')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
    t.token_value=symbol_value(ltgt_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    return t;
  }

   if(LookAhead[0]=='!' && LookAhead[1]=='=')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
    t.token_value=symbol_value(noteq_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    return t;
  }

   if(LookAhead[0]=='&' && LookAhead[1]=='&')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(andand_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    return t;
  }

   if(LookAhead[0]=='|' && LookAhead[1]=='|')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(oror_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    return t;
  }

   if(LookAhead[0]==':' && LookAhead[1]=='=')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(assign_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    scroll_Char(LookAhead);
    return t;
  }

   if(LookAhead[0]=='!')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(not_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='-')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(minus_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='~')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(tilde_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='^')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(hat_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='*')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(mult_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='/')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(div_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='%')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(mod_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='+')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(plus_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='&')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(and_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='|')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(or_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='=')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(eq_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='>')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(gt_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='<')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(lt_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='#')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(number_sign_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='@')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(at_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='$')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(dollar_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]==',')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(comma_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='(')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(left_bracket_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]==')')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
    t.token_value = symbol_value(right_bracket_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='[')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(left_square_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]==']')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(right_square_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='{')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(left_curly_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='}')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(right_curly_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='.')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(dot_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]==';')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(semicolon_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='?')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(qusetion_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  if(LookAhead[0]=='`')
  {
    scanner_status=symbol_mark;
    auto t=token();
    t.token_kind=scanner_status;
	t.token_value = symbol_value(backquote_mark);
    scanner_status=blank;
    scroll_Char(LookAhead);
    return t;
  }

  // 保留字与变量名
  IdentifierStr.clear();
  do
  {
	  IdentifierStr +=LookAhead[0];
    scroll_Char(LookAhead);
  }
  while(isidchar(LookAhead[0]));
  //if(M)
  auto t=token();
  t.token_kind = id;
  t.token_value = id_value("");
  return t;
  //

    
  





  // Check for end of file.  Don't eat the EOF.
  if (LookAhead[0] == EOF)
  {
	  auto t = token();
	  return t;
  }


}
