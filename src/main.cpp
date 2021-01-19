#include "test.h"
#include "compiler.h"
#include "runtime.h"

#include <sstream>
#include <fstream>
#include <cstring>

using namespace lisp;
using namespace std;

pair<Environment, Compiler::EnvMap> evaluate ( const string & str, const Environment & env, const Compiler::EnvMap & envMap )
{
	auto [ code, outEnvMap ] = Compiler::CompileCode( Parser::Parse( Lexer::Scan( str ) ), envMap );
	return { 
		lisp::secd::Runtime::executeCode( Stack ( code ), env),
		outEnvMap
	};
}

void read ( const Environment & env = Environment(), const Compiler::EnvMap & envMap = Compiler::EnvMap() )
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

pair<Environment, Compiler::EnvMap> parseArgs( int argc, char const *argv[], const Environment & env = Environment(), const Compiler::EnvMap & envMap = Compiler::EnvMap() )
{
	if ( argc == 0 )
		return { env, envMap };

	// load file
	if ( strcmp( argv[0], "-f" ) == 0 )
	{
		if ( argc < 2 )
		{
			cout << "Usage of '-f': TinyLISP -f 'filename'" << endl;
			return parseArgs( --argc, ++argv, env, envMap );
		}

		--argc;
		++argv;
		const char * filename = argv[0];

		ifstream fs = ifstream( filename );
		if ( ! fs.good() )
		{
			cout << "File " << filename << " is not processable." << endl;
			return parseArgs( --argc, ++argv, env, envMap );
		}

		// load file into string buffer
		stringstream ss;
		ss << fs . rdbuf();

		auto [nextEnv, nextEnvMap] = evaluate( ss.str(), env, envMap );
		return parseArgs( --argc, ++argv, nextEnv, nextEnvMap );
	}

	else if ( strcmp( argv[0], "-h" ) == 0 || strcmp( argv[0], "--help" ) == 0 )
	{
		cout
		<< "TinyLisp interpreter:\n\n"
		<< "Usage: TinyLISP [options] \n\n"
		<< "Options:\n"
		<< "\t -f [file] \t loads TinyLisp expressions file\n"
		<< "\t -h, --help \t shows this help"
		<< endl;
	}

	else
		cout << "Unknown option '" << argv[0] << "', for help use flag --help." << endl;
	
	return parseArgs( --argc, ++argv, env, envMap );
}

int main(int argc, char const *argv[])
{
#ifdef TEST
	test();
	return 0;
#endif

	GC::Start();

	if ( argc > 1 )
	{
		auto [env, map] = parseArgs( --argc, ++argv );
		read ( env, map );
	}
	else
		read();

	GC::Stop();

	return 0;
}
