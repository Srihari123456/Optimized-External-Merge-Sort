#include <stdio.h>

#if 0
#include <assert.h>
#else
#include <stdlib.h>
#endif

#include "defs.h"

void Assert (bool const predicate,
		char const * const file, int const line)
{
	if (predicate)
		return;

	fflush (stdout);
	fprintf (stderr, "failed assertion at %s, %d\n", file, line);
	fflush (stderr);

#if 0
	assert (false);
#else
	exit (1);
#endif
} // Assert
