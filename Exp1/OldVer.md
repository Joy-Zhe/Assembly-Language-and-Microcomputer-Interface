+ 在大一时C小考试的复习中，遇到好多不会的题，拿题去搜索之后发现不少都是C语言的undefined behavior，但是当时忙于考试的复习，就潦草的记住了答案的普遍结果，想着应付过考试，就没有深究。如今有时间进行深入探索，我决定对C语言的一些undefined behaviour在不同编译器，或是相同编译器下不同优化级别下的不同表现进行探索与分析

+ 有时长表达式的运算顺序也是undefined behavior。
+ 似乎有些类型转换也是ubdefined behavior


+ 初步搜索[浅谈 C++ Undefined Behavior - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/391088391)
+ 这篇文章中提到 
+ **为什么 C++ 标准需要规定 undefined behavior？为什么不事无巨细地在标准和 C++ implementation 中规定好所有行为，直接消灭掉 unspecified behavior 和 undefined behavior？这个问题的答案是多方面的。首先，abstract machine 只是一个假想的模型，但实际上的硬件 / 软件环境太多太多，在某个平台上的 well-defined behavior 可能是另一个平台上的 undefined behavior；相应地，语言的语义也就不存在一个适用于现实世界中所有运行环境的设计。举例来说，在大部分 CPU 上，有符号整数的溢出是一个 perfectly well-defined behavior**

> 在查阅资料的过程中，我发现一个有意思的事
+ 在我的想法中，为了代码在不同编译器下的兼容性，undefined behavior应该被尽量避免，然而，却又有许多人利用ub，来简化一些操作，甚至用于CRC校验？
> 好像未定义行为也能为编译优化提供更大的空间和更大的可能性
+ 比如对数组下标越界的情况，如果编译器加入对此的检测，将会增加许多不必要的工作量，于是，将这种越界的判断交给了程序员判断而不是在编译过程中判断，许多程序在运行时才会报错。


+ 以下一段代码，在不同编译器中运行`f(++i, ++i)`会出现不同的结果：
``` C
#include <stdio.h>

void f(int a, int b);

int main() {
    int i = 1;
    f(++i, ++i);
    return 0;
}

void f(int a, int b) {
    printf("%d %d\n", a, b);
    return;
}
```
+ x86-64 clang 17.0.1
+ 输出结果：
```
2 3
```
+ 反汇编结果：
```asm
main:                                   # @main
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     dword ptr [rbp - 4], 0
        mov     dword ptr [rbp - 8], 1
        mov     edi, dword ptr [rbp - 8]
        add     edi, 1
        mov     dword ptr [rbp - 8], edi
        mov     esi, dword ptr [rbp - 8]
        add     esi, 1
        mov     dword ptr [rbp - 8], esi
        call    f
        xor     eax, eax
        add     rsp, 16
        pop     rbp
        ret
f:                                      # @f
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     dword ptr [rbp - 4], edi
        mov     dword ptr [rbp - 8], esi
        mov     esi, dword ptr [rbp - 4]
        mov     edx, dword ptr [rbp - 8]
        lea     rdi, [rip + .L.str]
        mov     al, 0
        call    printf@PLT
        add     rsp, 16
        pop     rbp
        ret
.L.str:
        .asciz  "%d %d\n"
```
+ x86-64 gcc 13.2
+ 输出结果：
```
3 3
```
+ 反汇编结果：
```asm
main:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     DWORD PTR [rbp-4], 1
        add     DWORD PTR [rbp-4], 1
        add     DWORD PTR [rbp-4], 1
        mov     edx, DWORD PTR [rbp-4]
        mov     eax, DWORD PTR [rbp-4]
        mov     esi, edx
        mov     edi, eax
        call    f
        mov     eax, 0
        leave
        ret
.LC0:
        .string "%d %d\n"
f:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     DWORD PTR [rbp-4], edi
        mov     DWORD PTR [rbp-8], esi
        mov     edx, DWORD PTR [rbp-8]
        mov     eax, DWORD PTR [rbp-4]
        mov     esi, eax
        mov     edi, OFFSET FLAT:.LC0
        mov     eax, 0
        call    printf
        nop
        leave
        ret
```


