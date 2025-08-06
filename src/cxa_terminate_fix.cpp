#include <exception>

// Fix for missing __cxa_call_terminate symbol
// This provides the missing symbol that's expected by newer C++ runtimes

extern "C" {
// Provide a stub implementation if the symbol is missing
void __cxa_call_terminate_stub(void *thrown_exception) __attribute__((__noreturn__));
void __cxa_call_terminate_stub(void *thrown_exception)
{
    // Call terminate directly - this is what __cxa_call_terminate should do
    std::terminate();
}
}

// Use weak symbols to provide the missing function only if it's not already defined
extern "C" __attribute__((weak, alias("__cxa_call_terminate_stub"))) void
__cxa_call_terminate(void *);
