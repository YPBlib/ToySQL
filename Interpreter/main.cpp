#include "parser.h"
int main()
{
	init_cata(); 
	initbuff();
	//init_scanner();
	//init_parser();
	
	setvbuf(stdout,NULL,_IONBF,0);

	while (1)
	{
		try
		{
			auto z = ParseStatementAST();
			toysql::exec<decltype(z)>(z);
			if (z == nullptr)
				break;
		}
		catch (std::runtime_error& s)
		{
			std::cout << s.what() << std::endl;
			skip_exp();
		}
	}
	return 0;
}