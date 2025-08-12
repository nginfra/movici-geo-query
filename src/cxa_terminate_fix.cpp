#include <exception>

// Fix for missing __cxa_call_terminate symbol
// This provides the missing symbol that's expected by newer C++ runtimes

// Platform-specific attributes
#ifdef _MSC_VER
// MSVC doesn't need __cxa_call_terminate (it's a GCC/Clang ABI thing)
// But we provide a stub just in case for cross-platform compatibility
#define WEAK_SYMBOL
#define NORETURN __declspec(noreturn)
#elif defined(__APPLE__)
#define WEAK_SYMBOL __attribute__((weak))
#define NORETURN __attribute__((__noreturn__))
#else
#define WEAK_SYMBOL __attribute__((weak))
#define NORETURN __attribute__((__noreturn__))
#endif

extern "C" {

#ifdef _MSC_VER
// On Windows/MSVC, __cxa_call_terminate is not needed (different ABI)
// We provide an empty stub for compatibility
#elif defined(__APPLE__)
// On macOS/Darwin, we can't use alias attribute, so provide the function directly
WEAK_SYMBOL void __cxa_call_terminate(void *) NORETURN;
WEAK_SYMBOL void __cxa_call_terminate(void *)
{
    // Call terminate directly - this is what __cxa_call_terminate should do
    std::terminate();
}
#else
// On Linux and other platforms, use alias for better optimization
void __cxa_call_terminate_stub(void *thrown_exception) NORETURN;
void __cxa_call_terminate_stub(void *thrown_exception)
{
    (void)thrown_exception;  // Suppress unused parameter warning
    // Call terminate directly - this is what __cxa_call_terminate should do
    std::terminate();
}

// Use weak symbols to provide the missing function only if it's not already defined
__attribute__((weak, alias("__cxa_call_terminate_stub"))) void __cxa_call_terminate(void *);
#endif
}
