/* 
 * Test file for InactiveCodeTracker
 * Contains conditional compilation blocks that will be inactive
 */

#include <stdio.h>

void print_debug(const char *msg) {
    printf("[DEBUG] %s\n", msg);
}

void print_release(const char *msg) {
    printf("[RELEASE] %s\n", msg);
}

int add(int a, int b) {
    int result = a + b;
    
#ifdef DEBUG_MODE
    print_debug("Debug mode enabled");
    printf("Adding %d + %d = %d\n", a, b, result);
#else
    print_release("Release mode");
#endif
    
    return result;
}

int multiply(int x, int y) {
    int product = x * y;
    
#ifdef VERBOSE
    printf("Multiplying %d * %d = %d\n", x, y, product);
#endif
    
#ifdef EXPERIMENTAL_OPT
    // This optimization is experimental
    if (x == 0 || y == 0) {
        return 0;
    }
    product = (x << 1) * y;  // Potential bug: intentional shift
#endif
    
    return product;
}

int main(void) {
    int a = 5;
    int b = 3;
    
    printf("Result: %d\n", add(a, b));
    printf("Product: %d\n", multiply(a, b));
    
#ifdef ENABLE_EXTENDED_TESTS
    printf("Running extended tests...\n");
    for (int i = 0; i < 10; i++) {
        printf("Test %d\n", i);
    }
#endif
    
    return 0;
}
__attribute__((annotate("inactive_block: /home/yqc5929/workspace/pennstate/preproc/test_output/test_inactive.c:54:1-59:7 code=ifdef_ENABLE_EXTENDED_TESTS_printf(Running_extended_testsn);_for_(int_i_=_0;_i_<_10;_i++)_{_printf(Test_dn,_i);_}_endif"), used)) static void __clang_inactive_marker_3(void) { }
__attribute__((annotate("inactive_block: /home/yqc5929/workspace/pennstate/preproc/test_output/test_inactive.c:36:1-42:7 code=ifdef_EXPERIMENTAL_OPT_//_This_optimization_is_experimental_if_(x_==_0_y_==_0)_{_return_0;_}_product_=_(x_<<_1)_*_y;_//_Potential_bug:_intentional_shift_endif"), used)) static void __clang_inactive_marker_2(void) { }
__attribute__((annotate("inactive_block: /home/yqc5929/workspace/pennstate/preproc/test_output/test_inactive.c:32:1-34:7 code=ifdef_VERBOSE_printf(Multiplying_d_*_d_=_dn,_x,_y,_product);_endif"), used)) static void __clang_inactive_marker_1(void) { }
__attribute__((annotate("inactive_block: /home/yqc5929/workspace/pennstate/preproc/test_output/test_inactive.c:19:1-22:6 code=ifdef_DEBUG_MODE_print_debug(Debug_mode_enabled);_printf(Adding_d_+_d_=_dn,_a,_b,_result);_else"), used)) static void __clang_inactive_marker_0(void) { }
