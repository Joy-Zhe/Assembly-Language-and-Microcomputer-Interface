#include <stdio.h>

void f(int a, int b);

void f2();

int main() {
    int i = 1;
    f(++i, ++i);
    f2();
    return 0;
}

void f(int a, int b) {
    printf("%d %d\n", a, b);
}

void f2() {
    int x = 1;
    int y = (x > 0) ? x++ : x--;
    printf("x:%d y:%d\n", x, y);
}