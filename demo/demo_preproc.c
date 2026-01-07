/* 
 * Example C file to demonstrate Preprocessor Output (.i file)
 * Shows macro expansion with control flow and conditional compilation
 */

#include <stdio.h>

#define PI 3.14159
#define SQUARE(x) ((x) * (x))

// Macro with control flow - multi-statement macro using do-while(0)
#define LOG_ERROR(msg, code) do { \
    fprintf(stderr, "Error: %s (code=%d)\n", msg, code); \
    return code; \
} while(0)

// Conditional macro - expands differently based on another macro
#ifdef DEBUG_MODE
    #define DEBUG_PRINT(fmt, ...) fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) ((void)0)  // Does nothing in release mode
#endif

// Macro with if-else logic inside
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(val, min, max) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

// Function-like macro with nested conditionals
#define VALIDATE_AND_PROCESS(x) do { \
    if ((x) < 0) { \
        LOG_ERROR("Negative value not allowed", -1); \
    } \
    if ((x) > 100) { \
        DEBUG_PRINT("Value %d exceeds threshold", x); \
        x = 100; \
    } \
} while(0)

#ifdef ENABLE_FEATURE
    void feature_function(int val);
#else
    void default_function(void);
#endif

int calculate_area(int radius) {
    DEBUG_PRINT("Calculating area for radius: %d", radius);
    
    if (radius < 0) {
        LOG_ERROR("Invalid radius", -1);
    }
    
    int safe_radius = CLAMP(radius, 1, 10);
    return PI * SQUARE(safe_radius);
}

int main() {
    int value = 150;
    
    DEBUG_PRINT("Starting program with value=%d", value);
    
    VALIDATE_AND_PROCESS(value);
    
    int max_val = MAX(value, 50);
    
    #ifdef ENABLE_FEATURE
        feature_function(max_val);
    #else
        default_function();
    #endif
    
    int area = calculate_area(5);
    printf("Area: %d\n", area);

    return 0;
}
