#ifdef TEST
#include "test.h"
#endif

using namespace lisp;

int main(int argc, char const *argv[])
{
#ifdef TEST
	test();
	return 0;
#endif

	return 0;
}
