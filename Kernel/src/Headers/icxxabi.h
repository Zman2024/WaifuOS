// haha i want global objects

typedef unsigned uarch_t;

constexpr uarch_t ATEXIT_MAX_FUNCS = 0x100;

struct atexit_func_entry_t
{
	/*
	* Each member is 8 bytes large. Such that each entry is 24bytes.
	* 256 * 24 = 0x1800B exact.
	**/
	void(*destructor_func)(void *);
	void *obj_ptr;
	void *dso_handle;
};

extern "C" atexit_func_entry_t __atexit_funcs[];

extern "C" int __cxa_atexit(void(*f)(void *), void *objptr, void *dso);
//void __cxa_finalize(void *f);
