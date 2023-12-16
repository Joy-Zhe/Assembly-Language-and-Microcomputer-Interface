f:
        push    r15
        push    r14
        push    r13
        push    r12
        mov     r12d, edi
        push    rbp
        push    rbx
        sub     rsp, 104
        cmp     edi, 1
        jle     .L37
        lea     edx, [rdi-1]
        mov     ebx, edi
        xor     ebp, ebp
        mov     eax, edx
        and     eax, -2
        sub     ebx, eax
        mov     r12d, ebx
        mov     ebx, ebp
        mov     ebp, edi
        cmp     ebp, r12d
        je      .L48
.L3:
        sub     ebp, 2
        mov     esi, edx
        xor     r15d, r15d
        mov     r14d, ebx
        mov     eax, ebp
        mov     r13d, r12d
        and     eax, -2
        sub     esi, eax
        mov     eax, edx
        mov     DWORD PTR [rsp+56], esi
.L9:
        mov     esi, DWORD PTR [rsp+56]
        lea     edx, [rax-1]
        mov     ecx, edx
        cmp     eax, esi
        je      .L49
.L6:
        lea     r12d, [rax-2]
        mov     eax, ecx
        xor     ebx, ebx
        mov     DWORD PTR [rsp+40], r15d
        mov     edx, r12d
        mov     r15d, ebp
        mov     ebp, r13d
        mov     r13d, r14d
        and     edx, -2
        mov     r14d, ebx
        sub     eax, edx
        mov     DWORD PTR [rsp+60], eax
.L13:
        mov     eax, DWORD PTR [rsp+60]
        lea     edx, [rcx-1]
        mov     r8d, edx
        cmp     ecx, eax
        je      .L50
        sub     ecx, 2
        mov     eax, r8d
        xor     ebx, ebx
        mov     DWORD PTR [rsp+44], r14d
        mov     edx, ecx
        mov     DWORD PTR [rsp+48], ecx
        mov     r14d, ebx
        and     edx, -2
        sub     eax, edx
        mov     DWORD PTR [rsp+64], eax
        mov     eax, ebp
        mov     ebp, r12d
        mov     r12d, r15d
        mov     r15d, r13d
.L17:
        mov     esi, DWORD PTR [rsp+64]
        lea     edx, [r8-1]
        mov     r10d, edx
        cmp     r8d, esi
        je      .L51
        sub     r8d, 2
        mov     ebx, r10d
        mov     ecx, r14d
        xor     r13d, r13d
        mov     edx, r8d
        mov     DWORD PTR [rsp+52], r8d
        mov     esi, r15d
        mov     r14d, r10d
        and     edx, -2
        mov     r11d, eax
        sub     ebx, edx
        mov     edx, ebp
        mov     DWORD PTR [rsp+12], ebx
        mov     ebx, r12d
.L21:
        mov     r9d, DWORD PTR [rsp+12]
        lea     edi, [r14-1]
        mov     eax, edi
        cmp     r14d, r9d
        je      .L52
.L18:
        sub     r14d, 2
        mov     r10d, eax
        mov     DWORD PTR [rsp+4], r13d
        xor     r15d, r15d
        mov     edi, r14d
        and     edi, -2
        sub     r10d, edi
        mov     DWORD PTR [rsp+16], r10d
.L25:
        mov     r9d, DWORD PTR [rsp+16]
        lea     edi, [rax-1]
        mov     ebp, edi
        cmp     eax, r9d
        je      .L53
        lea     edi, [rax-2]
        lea     r9d, [rax-4]
        mov     r10d, ebp
        xor     r8d, r8d
        lea     r13d, [rax-3]
        lea     r12d, [rax-5]
        mov     eax, edi
        and     eax, -2
        sub     r10d, eax
        mov     eax, r12d
        mov     DWORD PTR [rsp+32], r10d
        mov     r10d, r9d
        mov     r9d, r13d
.L29:
        mov     r13d, DWORD PTR [rsp+32]
        cmp     ebp, r13d
        je      .L54
        mov     r12d, r9d
        lea     r13d, [rbp-4]
        mov     DWORD PTR [rsp+20], esi
        mov     esi, ebx
        and     r12d, -2
        mov     DWORD PTR [rsp+24], ecx
        mov     ebx, eax
        mov     ecx, edi
        sub     r13d, r12d
        mov     r12d, r10d
        mov     DWORD PTR [rsp+28], edx
        and     r12d, -2
        mov     DWORD PTR [rsp+8], r13d
        lea     r13d, [rbp-6]
        sub     r13d, r12d
        mov     r12d, eax
        mov     DWORD PTR [rsp], 0
        mov     DWORD PTR [rsp+36], r13d
        lea     edx, [r12+2]
        cmp     DWORD PTR [rsp+8], r12d
        je      .L55
.L35:
        xor     r13d, r13d
.L30:
        lea     edi, [rdx-1]
        mov     DWORD PTR [rsp+92], esi
        mov     DWORD PTR [rsp+88], ecx
        mov     DWORD PTR [rsp+84], r9d
        mov     DWORD PTR [rsp+80], r10d
        mov     DWORD PTR [rsp+76], r11d
        mov     DWORD PTR [rsp+72], r8d
        mov     DWORD PTR [rsp+68], edx
        call    f
        mov     edx, DWORD PTR [rsp+68]
        mov     r8d, DWORD PTR [rsp+72]
        add     r13d, eax
        mov     r11d, DWORD PTR [rsp+76]
        mov     r10d, DWORD PTR [rsp+80]
        sub     edx, 2
        mov     r9d, DWORD PTR [rsp+84]
        mov     ecx, DWORD PTR [rsp+88]
        cmp     edx, 1
        mov     esi, DWORD PTR [rsp+92]
        jg      .L30
        mov     eax, r12d
        and     eax, 1
        add     eax, r13d
        add     DWORD PTR [rsp], eax
        lea     eax, [r12-2]
        cmp     DWORD PTR [rsp+36], eax
        je      .L32
        mov     r12d, eax
        lea     edx, [r12+2]
        cmp     DWORD PTR [rsp+8], r12d
        jne     .L35
.L55:
        mov     r12d, DWORD PTR [rsp]
        mov     r13d, edx
        mov     eax, ebx
        mov     edi, ecx
        mov     ebx, esi
        mov     edx, DWORD PTR [rsp+28]
        mov     ecx, DWORD PTR [rsp+24]
        mov     esi, DWORD PTR [rsp+20]
        add     r13d, r12d
.L31:
        sub     ebp, 2
        add     r8d, r13d
        sub     r9d, 2
        sub     eax, 2
        sub     r10d, 2
        cmp     ebp, 1
        jne     .L29
        lea     r10d, [r8+1]
        mov     eax, edi
        add     r15d, r10d
        cmp     edi, 1
        jne     .L25
        mov     r13d, DWORD PTR [rsp+4]
        lea     edi, [r15+1]
.L58:
        add     r13d, edi
        cmp     r14d, 1
        jne     .L21
.L57:
        mov     r8d, DWORD PTR [rsp+52]
        mov     r15d, esi
        mov     r14d, ecx
        mov     eax, r11d
        mov     ebp, edx
        mov     r12d, ebx
        lea     edi, [r13+1]
.L19:
        add     r14d, edi
        cmp     r8d, 1
        jne     .L17
        mov     ebx, r14d
        mov     ecx, DWORD PTR [rsp+48]
        mov     r14d, DWORD PTR [rsp+44]
        mov     r13d, r15d
        lea     edx, [rbx+1]
        mov     r15d, r12d
        mov     r12d, ebp
        mov     ebp, eax
.L15:
        add     r14d, edx
        cmp     ecx, 1
        jne     .L13
        mov     ebx, r14d
        mov     r14d, r13d
        mov     r13d, ebp
        mov     ebp, r15d
        mov     r15d, DWORD PTR [rsp+40]
        lea     edx, [rbx+1]
        mov     eax, r12d
        add     r15d, edx
        cmp     r12d, 1
        jne     .L9
.L59:
        mov     ebx, r14d
        lea     eax, [r15+1]
        mov     r12d, r13d
        add     ebx, eax
        cmp     ebp, 1
        jne     .L56
.L38:
        lea     r12d, [rbx+1]
        jmp     .L37
.L53:
        mov     r13d, DWORD PTR [rsp+4]
        add     edi, r15d
        add     r13d, edi
        cmp     r14d, 1
        je      .L57
        mov     r9d, DWORD PTR [rsp+12]
        lea     edi, [r14-1]
        mov     eax, edi
        cmp     r14d, r9d
        jne     .L18
.L52:
        mov     r8d, DWORD PTR [rsp+52]
        mov     r15d, esi
        mov     r14d, ecx
        mov     eax, r11d
        mov     ebp, edx
        mov     r12d, ebx
        add     edi, r13d
        jmp     .L19
.L54:
        lea     r10d, [rbp-1+r8]
        mov     eax, edi
        add     r15d, r10d
        cmp     edi, 1
        jne     .L25
        mov     r13d, DWORD PTR [rsp+4]
        lea     edi, [r15+1]
        jmp     .L58
.L51:
        mov     ebx, r14d
        mov     r13d, r15d
        mov     r14d, DWORD PTR [rsp+44]
        mov     r15d, r12d
        mov     ecx, DWORD PTR [rsp+48]
        mov     r12d, ebp
        add     edx, ebx
        mov     ebp, eax
        jmp     .L15
.L32:
        mov     r13d, DWORD PTR [rsp]
        mov     eax, ebx
        mov     edi, ecx
        mov     ebx, esi
        mov     edx, DWORD PTR [rsp+28]
        mov     ecx, DWORD PTR [rsp+24]
        mov     esi, DWORD PTR [rsp+20]
        lea     r13d, [r13+1+r12]
        jmp     .L31
.L50:
        mov     ebx, r14d
        mov     r14d, r13d
        mov     r13d, ebp
        mov     ebp, r15d
        mov     r15d, DWORD PTR [rsp+40]
        add     edx, ebx
        mov     eax, r12d
        add     r15d, edx
        cmp     r12d, 1
        je      .L59
        mov     esi, DWORD PTR [rsp+56]
        lea     edx, [rax-1]
        mov     ecx, edx
        cmp     eax, esi
        jne     .L6
.L49:
        mov     ebx, r14d
        lea     eax, [rdx+r15]
        mov     r12d, r13d
        add     ebx, eax
        cmp     ebp, 1
        je      .L38
.L56:
        lea     edx, [rbp-1]
        cmp     ebp, r12d
        jne     .L3
.L48:
        lea     r12d, [rdx+rbx]
.L37:
        add     rsp, 104
        mov     eax, r12d
        pop     rbx
        pop     rbp
        pop     r12
        pop     r13
        pop     r14
        pop     r15
        ret
main:
        xor     eax, eax
        ret