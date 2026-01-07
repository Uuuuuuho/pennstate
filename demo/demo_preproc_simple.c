/* 
 * Example C file to demonstrate Preprocessor Output (.i file)
 */

#define PI 3.14159
#define SQUARE(x) ((x) * (x))

// This comment will be stripped out
#ifdef ENABLE_FEATURE
    void feature_function(int val);
#else
    void default_function(void);
#endif

int main() {
    float radius = 5.0;
    // Macro expansion happens here
    float area = PI * SQUARE(radius);
    
    #ifdef ENABLE_FEATURE
        feature_function(10);
    #endif

    return 0;
}