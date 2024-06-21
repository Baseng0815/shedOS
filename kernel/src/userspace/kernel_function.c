#include "kernel_function.h"
#include "../libk/alloc.h"
#include "src/debug.h"
#include "src/libk/util.h"

struct kernel_function *kernel_functions[255] = { 0 };

void kernel_function_register(TAKE struct kernel_function *function)
{
        for (function_code code = 0; code < 255; code++) {
                if (kernel_functions[code] == NULL) {
                        kernel_functions[code] = function;
                        function->code = code;
                }
        }

        kernel_panic("Kernel function capacity exceeded", __FILE__, __LINE__);
}
