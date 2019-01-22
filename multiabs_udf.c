#include <stdlib.h>
#include <stdio.h>
#include <mysql.h>

typedef int LUDF_INT_TYPE;
#define MULTI_ABS_FUNC abs

/*
  This function is called by MySQL to initialize the "module". It is called once for every MULTI_ABS() usage.
  MULTI_ABS() does not need to allocate any memory on the heap to function properly, so this init function
  actually does nothing. If memory shoul be allocated (and initialized) initid->ptr should be used to store
  the result. If this is the case, the memory should be release in the _deinit function.
  UDF functions should not use static or global variables for writing.
*/
bool multi_abs_init(UDF_INIT * initid, const UDF_ARGS * const args, const char * const message)
{
	return 0;
}

/*
  Function to cleanup after the query which included MULTI_ABS().
*/
void multi_abs_deinit(const UDF_INIT * const initid)
{
}


/*
  Converts a UDF function argument of any type to an integer
*/
static inline LUDF_INT_TYPE ludf_arg_to_ll(const enum Item_result arg_type, const char * const arg_value, char * const error)
{
	LUDF_INT_TYPE op1 = 0;
	*error = 0;
	switch (arg_type) {
		case INT_RESULT:
			op1 = *((long long *) arg_value);
			break;
		case STRING_RESULT:
			op1 = atoll(arg_value);
			break;
		case REAL_RESULT:
			op1 = (long long) *((double *) arg_value);
			break;
		default:
			*error = 1;
			break;
	}
	return op1;
}

/*
  A function to make multiple abs() calls on pairs of arguments. The result of every call is added up and the cummulative result
  is returned by the function.
  The function expects that an even number of arguments are passed.
  - It doesn't fail on no arguments
  - It fails (returns NULL) when the argument count is an odd number. If UDF terminology, when an UDF fails it is not executed
    anymore for the rest of the query.
*/
long long multi_abs(const UDF_INIT * const initid, const UDF_ARGS * const args, char * const is_null, char * const error)
{
	unsigned int i= 0;
	unsigned long long sum= 0;
	if (args->arg_count % 2 == 1 ) {
		*is_null= 1;
		sprintf(error, "Wrong number of arguments. Must be an even number");
		return 0;
	}

	for (; i < args->arg_count; i+= 2) {
		char call1_error= 0, call2_error= 0;
		/*
		 * 	We should use long long here but the underlying data is actually an tinyint.
		 * 	long long is slower than an int (also the llabs() compared to abs() below.
		 * 	Thus, we are using int and abs() and get about 5% performance increase.
		 *
		*/
		const LUDF_INT_TYPE op1 = ludf_arg_to_ll(args->arg_type[i], args->args[i], &call1_error);
		const LUDF_INT_TYPE op2 = ludf_arg_to_ll(args->arg_type[i + 1], args->args[i + 1], &call2_error);

		if (call1_error || call2_error) {
			*is_null= 1;
			sprintf(error, "Error while converting arguments to long long. %d->error=%d %d->error=%d", i, call1_error, i + 1, call2_error);
		}
		sum+= MULTI_ABS_FUNC(op1 - op2);
	}

	return sum;
}


/*
  This function is called by MySQL if the MULTI_ABS() is called with a query which has aggregates.
  It is called for every row in the group.
*/
void multi_abs_add(UDF_INIT * initid, const UDF_ARGS * const arg, char *is_null, char * const error)
{
}


/*
  This function is called by MySQL if the MULTI_ABS() is called with a query which has aggregates.
  It is called at the beginning of every group.
*/
void multi_abs_clear(UDF_INIT * initid, char * const is_null, char * const error)
{
}
