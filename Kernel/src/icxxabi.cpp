#include <icxxabi.h>



atexit_func_entry_t __atexit_funcs[ATEXIT_MAX_FUNCS];
uarch_t __atexit_func_count = 0;

extern "C" void *__dso_handle = nullptr; //Attention! Optimally, you should remove the '= 0' part and define this in your asm script.

extern "C" int __cxa_atexit(void(*f)(void *), void *objptr, void *dso)
{
	if (__atexit_func_count >= ATEXIT_MAX_FUNCS) { return -1; };
	__atexit_funcs[__atexit_func_count].destructor_func = f;
	__atexit_funcs[__atexit_func_count].obj_ptr = objptr;
	__atexit_funcs[__atexit_func_count].dso_handle = dso;
	__atexit_func_count++;
	return 0; /*I would prefer if functions returned 1 on success, but the ABI says...*/
};

// i would include the rest but im too lazy
// nothing references __cxa_finalize and my kernel never exits