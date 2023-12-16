+ 其实本来在与老师讨论过后，决定研究一下C语言中的undefined behavior，但是在测试不同编译器对整型溢出的未定义行为时，发现了编译器对于不等式的神奇优化。
+ 下面是一个例子
+ 对于以下这段代码：
```C
#include <stdio.h>
#include <limits.h>
int f(int x) {
	return x + 1 > x;
}

int main () {
	printf("%d\n", f(INT_MAX));
	return 0;
}
```
+ 在使用x86-64 clang 17.0.1编译器，不加编译优化时，出现如下输出：
```
0
```
+ 很显然，这不符合我们对于$x+1>x$这一不等式的认识，在我们的直觉中，这一不等式就相当于``
`1>0`，也即这是一条恒真的式子，那么我们来看看clang在这种情况下，进行了什么操作，才输出了0这种显然错误的结果：
```asm
f:                                      # @f
        push    rbp
        mov     rbp, rsp
        mov     dword ptr [rbp - 4], edi
        mov     eax, dword ptr [rbp - 4]
        add     eax, 1
        cmp     eax, dword ptr [rbp - 4]
        setg    al
        and     al, 1
        movzx   eax, al
        pop     rbp
        ret
```
+ 通过f函数的反汇编结果，我们可以了解到clang通过对传入参数+1，再与传入参数比较来实现这一函数，然而由于我们传入了INT_MAX，似乎导致这一结果溢出到负数，通过对INT_MAX+1的结果进行打印，我们可以看到输出结果为-INT_MAX，也即该有符号数发生了溢出。
+ 然而，编译器强大的优化解决了这一问题，当我们选择O2优化时，这段代码的反汇编结果如下：
```asm
f:                                      # @f
        mov     eax, 1
        ret
```
+ 神奇的事情发生了，f函数中，编译器直接将1赋给了eax寄存器并返回，直接跳过了原先对传入参数的计算，而将代码优化为了正确的结果，并且提升了代码运行的性能，不用再计算参数后比较。
+ 那么我便有一个问题，什么情况下，编译器会将一个不等式优化为常量，或者说不等式有两侧，什么情况下编译器会将一个不等式的一侧优化为常量以简化计算呢？

+ 这里就不得不提到一种编译器优化的技术——常量折叠，以上的例子中，就是使用了常量折叠，将一个显然成立的不等式折叠为了一个常数。

+ 接下来让我们看到这样一段代码：
``` C
#include<stdio.h>  

void test(unsigned int n){
    if (n > 12 - sizeof(int))
        printf("%d > 12 - %d\n", n, sizeof(int));
    else
        printf("%d <= 12 - %d\n", n, sizeof(int));
    if (n + sizeof(int) > 12)
        printf("%d + %d > 12\n", n, sizeof(int));
    else
        printf("%d + %d <= 12\n", n, sizeof(int));
}

int main() {
    test(10);
    return 0;
}
```
+ 在gcc 13.2下，一切正常
+ 无优化：
``` asm
.LC0:
        .string "%d > 12 - %d\n"
.LC1:
        .string "%d <= 12 - %d\n"
.LC2:
        .string "%d + %d > 12\n"
.LC3:
        .string "%d + %d <= 12\n"
test:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     DWORD PTR [rbp-4], edi
        mov     eax, DWORD PTR [rbp-4]
        cmp     eax, 8
        jbe     .L2
        mov     eax, DWORD PTR [rbp-4]
        mov     edx, 4
        mov     esi, eax
        mov     edi, OFFSET FLAT:.LC0
        mov     eax, 0
        call    printf
        jmp     .L3
.L2:
        mov     eax, DWORD PTR [rbp-4]
        mov     edx, 4
        mov     esi, eax
        mov     edi, OFFSET FLAT:.LC1
        mov     eax, 0
        call    printf
.L3:
        mov     eax, DWORD PTR [rbp-4]
        cdqe
        add     rax, 4
        cmp     rax, 12
        jbe     .L4
        mov     eax, DWORD PTR [rbp-4]
        mov     edx, 4
        mov     esi, eax
        mov     edi, OFFSET FLAT:.LC2
        mov     eax, 0
        call    printf
        jmp     .L6
.L4:
        mov     eax, DWORD PTR [rbp-4]
        mov     edx, 4
        mov     esi, eax
        mov     edi, OFFSET FLAT:.LC3
        mov     eax, 0
        call    printf
.L6:
        nop
        leave
        ret
main:
        push    rbp
        mov     rbp, rsp
        mov     edi, 10
        call    test
        mov     eax, 0
        pop     rbp
        ret
```
+ `n > 12 - sizeof(int)`对应的汇编代码为：
```asm
        mov     eax, DWORD PTR [rbp-4]
        cmp     eax, 8
        jbe     .L2
```
+ 而`n + sizeof(int) > 12`对应的汇编代码却为：
```asm
        mov     eax, DWORD PTR [rbp-4]
        cdqe
        add     rax, 4
        cmp     rax, 12
        jbe     .L4
```
+ 两者的差异一目了然，前者直接进行了常量折叠，右侧的sizeof(int)直接被作为一个常量，被编译器直接计算为`12-4=8`，继而eax寄存器直接与8比较即可；但是后者本质上也是同样的比较条件，为什么编译器还在规规矩矩的先加后比较呢，难道是因为现在n不是一个常量吗？那他为什么不把`+sizeof(int)`移动到不等式的另外一侧，也计算`12-sizeof(int)`的值呢？暂且先搁置一下，我再来试试其他编译器的结果。
+ 接下来我使用***x86-64 clang 17.0.1***，在优化级别为0时，表现为和gcc之下相同的结果，即全为常数的一侧被直接计算，以下是对应的汇编代码：
``` asm
test:                                   # @test
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     dword ptr [rbp - 4], edi
        movsxd  rax, dword ptr [rbp - 4]
        cmp     rax, 8
        jbe     .LBB0_2
        mov     esi, dword ptr [rbp - 4]
        lea     rdi, [rip + .L.str]
        mov     edx, 4
        mov     al, 0
        call    printf@PLT
        jmp     .LBB0_3
.LBB0_2:
        mov     esi, dword ptr [rbp - 4]
        lea     rdi, [rip + .L.str.1]
        mov     edx, 4
        mov     al, 0
        call    printf@PLT
.LBB0_3:
        movsxd  rax, dword ptr [rbp - 4]
        add     rax, 4
        cmp     rax, 12
        jbe     .LBB0_5
        mov     esi, dword ptr [rbp - 4]
        lea     rdi, [rip + .L.str.2]
        mov     edx, 4
        mov     al, 0
        call    printf@PLT
        jmp     .LBB0_6
.LBB0_5:
        mov     esi, dword ptr [rbp - 4]
        lea     rdi, [rip + .L.str.3]
        mov     edx, 4
        mov     al, 0
        call    printf@PLT
.LBB0_6:
        add     rsp, 16
        pop     rbp
        ret
main:                                   # @main
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     dword ptr [rbp - 4], 0
        mov     edi, 10
        call    test
        xor     eax, eax
        add     rsp, 16
        pop     rbp
        ret
.L.str:
        .asciz  "%d > 12 - %d\n"
.L.str.1:
        .asciz  "%d <= 12 - %d\n"
.L.str.2:
        .asciz  "%d + %d > 12\n"
.L.str.3:
        .asciz  "%d + %d <= 12\n"
```
+ `n > 12 - sizeof(int)`对应的汇编代码为：
```asm
        movsxd  rax, dword ptr [rbp - 4]
        cmp     rax, 8
        jbe     .LBB0_2
```
+ 而`n + sizeof(int) > 12`对应的汇编代码却为：
```asm
        movsxd  rax, dword ptr [rbp - 4]
        add     rax, 4
        cmp     rax, 12
        jbe     .LBB0_5
```
+ 然而，当我将优化级别改为O1时，抽象的事情发生了：
+ `n > 12 - sizeof(int)`对应的汇编代码为：
```asm
        movsxd  rbx, edi
        cmp     ebx, 9
        lea     rax, [rip + .L.str]
        lea     rdi, [rip + .L.str.1]
```
+ 而`n + sizeof(int) > 12`对应的汇编代码却为：
```asm
        mov     rax, rbx
        add     rax, -9
        cmp     rax, -13
        lea     rax, [rip + .L.str.2]
        lea     rdi, [rip + .L.str.3]
        cmovb   rdi, rax
```
+ 让我仔细思考一下clang在干嘛。。。
+ 这个问题可以追溯到课上提到的cmovcc指令，涉及到性能问题，这个我在后面再提（to be continued...）
+ 仔细思考一下，我猜测，这个问题似乎与之前那个`x+1>x`的式子有着异曲同工之妙，如果n是一个很大的数，比如极端一点，n就是之前我给x所取的值——`INT_MAX`，那么`n > 12 - sizeof(int)`与`n + sizeof(int) > 12`就出现了区别，假如先计算`n+sizeof(int)`，会导致左侧值溢出，但是如果将后者的式子优化为`n > 12 - sizeof(int)`，这个式子将会出现不同的判断结果，在《CSAPP》一书中提到了这种情景——不安全优化，即优化后的版本与未优化前的版本出现了不一致的行为。那么问题又来了，既然编译器拒绝优化这一不等式，那它又为何要将`x+1>x`优化为1呢。这个也之后再提。
+ 我们先来研究一下，为什么我们一眼就能看出可以构造出一侧常量的不等式，编译器却拒绝通过将+转移为-来进行优化
+ 让我们来看看LLVM的文档是怎么写的：
	+ nuw and nsw stand for “No Unsigned Wrap” and “No Signed Wrap”, respectively. If the [nuw](https://www.zhihu.com/search?q=nuw&search_source=Entity&hybrid_search_source=Entity&hybrid_search_extra=%7B%22sourceType%22%3A%22answer%22%2C%22sourceId%22%3A132543336%7D) and/or nsw [keywords](https://www.zhihu.com/search?q=keywords&search_source=Entity&hybrid_search_source=Entity&hybrid_search_extra=%7B%22sourceType%22%3A%22answer%22%2C%22sourceId%22%3A132543336%7D) are present, the result value of the add is a [poison value](https://link.zhihu.com/?target=http%3A//llvm.org/docs/LangRef.html%23poisonvalues)
	+ if unsigned and/or signed overflow, respectively, occurs.
![[Pasted image 20231107134330.png]]
+ 它有提到一个poison value，赶紧看看这又是什么意思，好在文档中贴心的提供了对应词条的跳转：![[Pasted image 20231107134535.png]]
> Poison Values
+ 这段话大意为Poison Value是一个错误操作的结果。为了方便预测执行（speculative execution），许多指令在遇到非法操作数时不会立即引发undefined behavior，而是返回一个poison value。
+ 官方文档中给出了一个例子，只不过这个例子是用LLVM IR表示的：
```LLVM

```
+ 来都来了，我决定上下翻一翻文档，果不其然，这个Poison Value上面和下面分别是Undefined Value和Well-defined Value，不禁让我联想到Undefined behavior和well-defined behavior，看看有啥关联：
> Undefined Value

> Well-defined Value