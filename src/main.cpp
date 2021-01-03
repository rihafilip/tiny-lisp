#ifdef TEST
#include "test.h"
#endif

#include "compiler.h"
#include "runtime.h"
/*
TODO
-f file
if not applicated, arithmetics are pushed to stack
AP on arithmetics works
 */

using namespace lisp;
using namespace std;

pair<Enviroment, Compiler::EnvMap> evaluate ( const string & str, const Enviroment & env, const Compiler::EnvMap & envMap )
{
	auto [ code, outEnvMap ] = Compiler::CompileCode( Parser::Parse( Lexer::Scan( str ) ), envMap );
	return { 
		lisp::secd::Runtime::executeCode( Stack ( code ), env),
		outEnvMap
	};
}

void read ( const Enviroment & env = Enviroment(), const Compiler::EnvMap & envMap = Compiler::EnvMap() )
{
	if ( ! cin.good() )
	{
		cout << "> quit" << endl;
		return;
	}
	
	string line;
	cout << "> ";
	getline( cin, line );

	if ( line  == "quit" )
		return;

	auto [ nextEnv, nextEnvMap ] = evaluate( line, env, envMap );
	cout << endl;

	return read( nextEnv, nextEnvMap );
}

int main(int argc, char const *argv[])
{
#ifdef TEST
	test();
	return 0;
#endif

	GC::Start();
	read();
	GC::Stop();

	return 0;
}
