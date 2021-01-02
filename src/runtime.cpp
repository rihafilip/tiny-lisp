#include "runtime.h"
#include <functional> 

using namespace lisp::secd;

Enviroment Runtime::executeCode ( const Stack & code, const Enviroment & env )
{
	if ( code . empty() )
		return env;

	// new enviroment or, in case of corrupted code, previous enviroment
	return executeCode( code.pop(), executeSource( Stack(), env, Stack( code.top() ), Stack() ) . value_or( env ) );
}

std::optional<Enviroment> Runtime::executeSource ( const Stack & _s, const Enviroment & _e, const Stack & _c, const Stack & _d )
{
	if ( _c . empty () )
	{
		print ( _s );
		return _e;
	}

	if ( ! _c . top() . isIns() )
	{
		std::cerr << "Corrupted code, found '" << _c.top() << "'." << std::endl;
		return std::nullopt;
	}

	std::optional<Registers> out = executeInstruction( _c . top() . ins(), _s, _e, _c . pop(), _d );

	if ( ! out )
		return std::nullopt;
	
	return executeSource ( out -> _s, out -> _e, out -> _c, out -> _d );
}

std::optional<Runtime::Registers> Runtime::executeInstruction ( Instruction ins, const Stack & _s, const Enviroment & _e, const Stack & _c, const Stack & _d )
{
	switch ( ins )
	{
		// load constant from code
		case LDC:
			return Registers( _s.push ( _c.top() ), _e, _c.pop(), _d );

		// push nil on stack
		case NIL:
			return Registers( _s.push( Value::Null() ), _e, _c, _d );

		// arithmetic and relation operators
		// pops two numbers from stack and pushes result on stack
		case ADD:
		case SUB:
		case MUL:
		case DIV:
		case LESS:
		case MORE:
			return binaryOperator( ins, _s, _e, _c, _d );

		// pops two values form stack and pushes 1 if they are equal, 0 otherwise
		case EQ:
			return equals ( _s, _e, _c, _d );

		// pops two values from stack and pushes cons cell made form them on stack
		case CONS:
		{
			Value rhs = _s.top();
			Value lhs = _s.pop().top();
			return Registers( _s.pop().pop().push( Value::Cons( rhs, lhs ) ) , _e, _c, _d );
		}

		// pop cons cell form stack, pushes car of that cell onto stack
		case CAR:
			return consAccess( CAR, _s, _e, _c, _d );

		// pop cons cell form stack, pushes cdr of that cell onto stack
		case CDR:
			return consAccess( CDR, _s, _e, _c, _d );

		// pops element from stack
		// on stack pushes 1 if it is cons cell, 0 otherwise
		case CONSP:
		{
			Value out = Value::Null();
			if ( _s.top().isCons() )
				out = Value::Integer( 1 );
			else
				out = Value::Integer( 0 );

			return Registers( _s.pop().push( out ), _e, _c, _d );
		}

		// pops element from stack
		// and two elements from code ( true case and false case )
		// if from stack is 1, true case is executed
		// otherwise false case is executed
		// pushes rest of code onto dump
		case SEL:
			return select( _s, _e, _c, _d );

		// pops code from dump
		case JOIN:
			return Registers( _s, _e, Stack (_d . top() ), _d . pop() );

		// pops cons cell ( depth, index ) from code
		// finds value in enviroment on given indexes
		// pushes that value on stack
		case LD:
		{
			std::optional<Value> out = _e.onIndex( _c.top() );
			if ( ! out )
			{
				std::cerr << "Accessing non-existent enviroment variable" << std::endl;
				return std::nullopt;				
			}

			return Registers( _s.push( *out ), _e, _c.pop(), _d );
		}

		// takes enviroment and pops body from code
		// pushes on stack closure = ( body . enviroment )
		case LDF:
		{
			Value enviroment = _e.data();
			Value code = _c.top();

			return Registers( _s.push( Value::Closure( code, enviroment ) ), _e, _c.pop(), _d );
		}

		// takes enviroment and pops body from code
		// adds closure into enviroment
		case DEFUN:
		{
			Value enviroment = _e.data();
			Value code = _c.top();

			return Registers( _s, _e.add ( Value::Closure( code, enviroment ) ), _c.pop(), _d );
		}

		// pops code and arguments from stack
		// applies them
		case AP:
			return apply( _s, _e, _c, _d );

		// pops return value from stack
		// returns state of stack, enviroment and code from dump
		// pushes return value onto stack
		case RTN:
			return returns( _s, _e, _c, _d);

		// pops form stack list of arguments, prints them
		case PRINT:
			print ( Stack(_s.top()) );
			return Registers( _s.pop(), _e, _c, _d );

		// reads from cin number or string symbol
		case READ:
			return Registers( _s . push( read() ), _e, _c, _d );
	}

	// shouln't occurr
	std::cerr << "Unknown instruction." << std::endl;
	return std::nullopt;
}

std::optional<Runtime::Registers> Runtime::binaryOperator ( Instruction ins, const Stack & _s, const Enviroment & _e, const Stack & _c, const Stack & _d )
{
	Value rhsV = _s.top();
	Value lhsV = _s.pop().top();

	if ( ! ( rhsV.isNum() && lhsV.isNum() ) )
	{
		std::cerr << "Arithmetic operator on non-number." << std::endl;
		return std::nullopt;
	}

	int rhs = rhsV.num();
	int lhs = lhsV.num();
	int out;

	switch( ins )
	{
		case ADD:
			out = rhs + lhs;
			break;
		case SUB:
			out = rhs - lhs;
			break;
		case MUL:
			out = rhs * lhs;
			break;
		case DIV:
			out = rhs / lhs;
			break;

		case LESS:
			out = rhs < lhs ? 1 : 0;
			break;
		case MORE:
			out = rhs > lhs ? 1 : 0;
			break;
		default:
			break;
	}

	return Registers( _s.pop().pop().push(Value::Integer(out)), _e, _c, _d );
}

std::optional<Runtime::Registers> Runtime::equals ( const Stack & _s, const Enviroment & _e, const Stack & _c, const Stack & _d )
{
	Value rhs = _s.top();
	Value lhs = _s.pop().top();
	int output;

	if ( rhs.equals(lhs) )
		output = 1;
	else
		output = 0;
	
	return Registers( _s.pop().pop().push( Value::Integer(output) ) , _e, _c, _d );
}

std::optional<Runtime::Registers> Runtime::consAccess ( Instruction ins, const Stack & _s, const Enviroment & _e, const Stack & _c, const Stack & _d )
{
	Value lst = _s.top();
	if ( ! lst.isCons()  )
	{
		std::cerr << "Cons access in non-cons element." << std::endl;
		return std::nullopt;		
	}

	Value out = Value::Null();

	if ( ins == CAR )
		out = lst.car();
	else /*( ins == CDR )*/
		out = lst.cdr();
	
	return Registers( _s.pop().push( out ), _e, _c, _d );
}

// if
// 0 means false, everything else means true
std::optional<Runtime::Registers> Runtime::select ( const Stack & _s, const Enviroment & _e, const Stack & _c, const Stack & _d )
{
	Value condition = _s . top();
	Value truecase = _c.top();
	Value falsecase = _c.pop().top();
	Value out = Value::Null();

	if ( ! condition.isNum() || ( condition.num() == 0 ) )
		out = falsecase;
	else
		out = truecase;

	Stack outStack = Stack(out);

	return Registers( _s.pop(), _e, outStack , _d.push( _c.pop().pop().data() ) );
}

// stack = empty
// enviroment = closure.cdr + args
// code = closure.car
// to dump is pushed ( stack enviroment . code )
std::optional<Runtime::Registers> Runtime::apply ( const Stack & _s, const Enviroment & _e, const Stack & _c, const Stack & _d )
{
	Value clos = _s.top();
	if ( ! clos.isClos()  )
	{
		std::cerr << "Apply on non-closure." << std::endl;
		return std::nullopt;
	}

	Value args = _s.pop().top();
	Value toDump = Value::Cons( _s.pop().pop().data(),
		Value::Cons( _e.data(), _c.data() ) );

	return Registers (
		Stack(),
		Enviroment( clos . cdr() ) . shifted() . setZeroDepth ( args ) ,
		Stack( clos.car() ),
		_d.push( toDump )
	);
}
std::optional<Runtime::Registers> Runtime::returns ( const Stack & _s, const Enviroment & _e, const Stack & _c, const Stack & _d )
{
	Value retVal = _s.top();
	// ( stack enviroment . code )
	Value prev = _d.top();

	Stack st = Stack ( prev.car() ) . push( retVal );
	Enviroment env = Enviroment ( prev.cdr().car() );
	Stack code = Stack ( prev.cdr().cdr() );
	return Registers( st, env, code, _d.pop() );
}

void Runtime::print ( const Stack & s )
{
	if ( s.top().isNull()  )
		return;

	std::cout << s.top() << " ";	
	return print( s.pop() );
}

Value Runtime::read ()
{
	std::string input;
	std::cin >> input;

	if ( ! input.empty()
		&& ( isdigit(input[0]) || input[0] == '-' )
		&& input.find_first_not_of( "0123456789" ) == std::string::npos
	)
		return Value::Integer( stoi( input ) );

	return Value::Symbol(input);
}