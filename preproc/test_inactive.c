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
