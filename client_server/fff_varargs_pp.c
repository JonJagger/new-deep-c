//# 1 "varargs.c"
//# 1 "<built-in>" 1
//# 1 "<built-in>" 3
//# 170 "<built-in>" 3
//# 1 "<command line>" 1
//# 1 "<built-in>" 2
//# 1 "varargs.c" 2
//# 1 "./fff.h" 1
//# 103 "./fff.h"

// FAKE_VALUE_FUNC(int, printf, const char *, ...)

/*JJ*/ #include <assert.h> 	// assert()
/*JJ*/ #include <stdarg.h>	// va_list
/*JJ*/ #include <stdio.h>		// fputs()
/*JJ*/ #include <string.h> 	// memcpy(), strcmp()

typedef struct 
{
    void * call_history[(50u)];
    unsigned int call_history_idx;
} fff_globals_t;

fff_globals_t fff;

//# 5 "varargs.c" 2
typedef struct printf_Fake 
{ 
	const char * arg0_val; 
	const char * arg0_history[(50u)]; 
	//... arg1_val; 
	//... arg1_history[(50u)]; 
	unsigned int call_count; 
	unsigned int arg_history_len; 
	unsigned int arg_histories_dropped; 
	int return_val; 
	int return_val_seq_len; 
	int return_val_seq_idx; 
	int * return_val_seq; 
	//int(*custom_fake)(const char * arg0, ... arg1);
	/*JJ*/ int(*custom_fake)(const char * arg0, va_list rest);
} printf_Fake; 

extern printf_Fake printf_fake; 

void printf_reset(); 

printf_Fake printf_fake; 

//int printf(const char * arg0, ... arg1)
/**JJ*/ int printf(const char * arg0, ...)
{ 
	memcpy((void*)&printf_fake.arg0_val, (void*)&arg0, sizeof(arg0));
	//memcpy((void*)&printf_fake.arg1_val, (void*)&arg1, sizeof(arg1));
	
	if (printf_fake.call_count < (50u))
	{ 
		memcpy((void*)&printf_fake.arg0_history[printf_fake.call_count], (void*)&arg0, sizeof(arg0));
		//memcpy((void*)&printf_fake.arg1_history[printf_fake.call_count], (void*)&arg1, sizeof(arg1)); 
	} 
	else
	{ 
		printf_fake.arg_histories_dropped++;
	} 
	printf_fake.call_count++; 
	if (fff.call_history_idx < (50u)) 
		fff.call_history[fff.call_history_idx++] = (void *)printf;
	if (printf_fake.custom_fake) 
	{
		//return printf_fake.custom_fake(arg0, arg1);
		/*JJ*/ va_list rest;
		/*JJ*/ va_start(rest, arg0);
		/*JJ*/ int rv = printf_fake.custom_fake(arg0, rest);
		/*JJ*/ va_end(rest);
		/*JJ*/ return rv;
	}
	if (printf_fake.return_val_seq_len)
	{ 
		if (printf_fake.return_val_seq_idx < printf_fake.return_val_seq_len)
		{ 
			return printf_fake.return_val_seq[printf_fake.return_val_seq_idx++];
		} 
		return printf_fake.return_val_seq[printf_fake.return_val_seq_len-1];
	} 
	return printf_fake.return_val;
} 

void printf_reset()
{ 
	memset(&printf_fake, 0, sizeof(printf_fake));
	printf_fake.arg_history_len = (50u);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static char printf_output[1024];

static int custom_printf(const char * format, va_list args)
{
	return vsnprintf(printf_output, sizeof printf_output, format, args);
}

static void test_variadic_function(void)
{
	printf_reset();
	printf_fake.custom_fake = custom_printf;
	printf("Hello %s\n", "fred");
	assert(printf_fake.call_count == 1);	
	assert(strcmp("Hello %s\n", printf_fake.arg0_history[0]) == 0);
	assert(strcmp(printf_output, "Hello fred\n") == 0);
}

int main(void)
{
	test_variadic_function();
	fputs("All tests passed\n", stdout);
}



