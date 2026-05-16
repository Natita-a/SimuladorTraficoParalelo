#include <assert.h>
#include <stdio.h>

void test_example(void) {
    assert(1 == 1);
    printf("test_example ... ok\n");
}

int main(void) {
    test_example();
    return 0;
}
