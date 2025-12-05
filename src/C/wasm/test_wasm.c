#include <emscripten.h>
#include <stdio.h>

// Use EMSCRIPTEN_KEEPALIVE to ensure this function is accessible from JavaScript
EMSCRIPTEN_KEEPALIVE
int run_test_calculation() {
    // A simple, verifiable calculation
    printf("WASM: Running test calculation 10 + 32...\n");
    return 10 + 32; 
}