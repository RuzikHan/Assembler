#include <cstdio>
#include <cstdint>
#include <iostream>
#include <cstring>

void *memcpy1(void *dest, const void *src, std::size_t count) {
    asm volatile ("xor %%rax, %%rax\n\t"
    			  "cmp %%rax, %2\n\t"
    			  "je .finish1\n\t"
    			  ".loop1:\n\t"
                  "movb (%0), %%al\n\t"
                  "movb %%al, (%1)\n\t"
                  "lea 1(%0), %0\n\t"
                  "lea 1(%1), %1\n\t"
                  "dec %2\n\t"
                  "jnz .loop1\n\t"
                  ".finish1:\n\t"
    :
    : "r" (src), "r" (dest), "r" (count)
    : "%al", "%rax"
    );
}

void *memcpy4(void *dest, const void *src, std::size_t count) {
    std::size_t mod_count = count % 4;
    count /= 4;
    asm volatile ("xor %%rax, %%rax\n\t"
    			  "cmp %%rax, %2\n\t"
    			  "je .afterloop4\n\t"
    			  ".loop4:\n\t"
                  "movl (%0), %%eax\n\t"
                  "movl %%eax, (%1)\n\t"
                  "lea 4(%0), %0\n\t"
                  "lea 4(%1), %1\n\t"
                  "dec %2\n\t"
                  "jnz .loop4\n\t"
                  ".afterloop4:\n\t"
                  "xor %%rax, %%rax\n\t"
                  "cmp %%rax, %3\n\t"
                  "je .finish4\n\t"
                  ".loop4_1:\n\t"
                  "movb (%0), %%al\n\t"
                  "movb %%al, (%1)\n\t"
                  "lea 1(%0), %0\n\t"
                  "lea 1(%1), %1\n\t"
                  "dec %3\n\t"
                  "jnz .loop4_1\n\t"
                  ".finish4:\n\t"
    :
    : "r" (src), "r" (dest), "r" (count), "r"(mod_count)
    : "%eax", "%al", "%rax"
    );
}

void *memcpy8(void *dest, const void *src, std::size_t count) {
    std::size_t mod_count = count % 8;
    count /= 8;
    std::size_t mod4_count = 0;
    if (mod_count > 3) {
        mod4_count = mod_count - 4;
        mod_count = 1;
    } else {
        mod4_count = mod_count;
        mod_count = 0;
    }
    asm volatile ("xor %%rax, %%rax\n\t"
    			  "cmp %%rax, %2\n\t"
    			  "je .afterloop8\n\t"
    			  ".loop8:\n\t"
                  "movq (%0), %%rax\n\t"
                  "movq %%rax, (%1)\n\t"
                  "lea 8(%0), %0\n\t"
                  "lea 8(%1), %1\n\t"
                  "dec %2\n\t"
                  "jnz .loop8\n\t"
                  ".afterloop8:\n\t"
                  "xor %%rax, %%rax\n\t"
                  "cmp %%rax, %3\n\t"
                  "je .afterloop8_4\n\t"
                  "movl (%0), %%eax\n\t"
                  "movl %%eax, (%1)\n\t"
                  "lea 4(%0), %0\n\t"
                  "lea 4(%1), %1\n\t"
                  ".afterloop8_4:\n\t"
                  "xor %%rax, %%rax\n\t"
                  "cmp %%rax, %4\n\t"
                  "je .finish8\n\t"
                  ".loop8_1:\n\t"
                  "movb (%0), %%al\n\t"
                  "movb %%al, (%1)\n\t"
                  "lea 1(%0), %0\n\t"
                  "lea 1(%1), %1\n\t"
                  "dec %4\n\t"
                  "jnz .loop8_1\n\t"
                  ".finish8:\n\t"
    :
    : "r" (src), "r" (dest), "r" (count), "r"(mod_count), "r"(mod4_count)
    : "%eax", "%al", "%rax"
    );
}

void *memcpy16_align(void *dest, const void *src, std::size_t count) {
	void *tmp_src = (void *) (src);
	std::size_t sd = (16 - (size_t) tmp_src % 16) % 16;
	//printf("%d\n", sd);
	//sd = 8;
	//printf("%d\n", count);
	count -= sd;
	//printf("%d\n", count);
	std::size_t sd8 = 0;
	if (sd > 7) {
		sd8 = 1;
		sd -= 8;
	} else {
		sd8 = 0;
	}
	std::size_t sd4 = 0;
	if (sd > 3) {
		sd4 = 1;
		sd -= 4;
	} else {
		sd4 = 0;
	}
    std::size_t mod_count = count % 16;
    count /= 16;
    std::size_t mod8_count = 0;
    if (mod_count > 7) {
        mod8_count = 1;
        mod_count -= 8;
    } else {
        mod8_count = 0;
    }
    std::size_t mod4_count = 0;
    if (mod_count > 3) {
        mod4_count = mod_count - 4;
        mod_count = 1;
    } else {
        mod4_count = mod_count;
        mod_count = 0;
    }
    /*printf("%d\n", count);
    printf("%d\n", mod8_count);
    printf("%d\n", mod_count);
    printf("%d\n", mod4_count);
    printf("%d\n", sd8);
    printf("%d\n", sd4);
    printf("%d\n", sd);*/
    asm volatile(""
    			 "xor %%rax, %%rax\n\t"
    			 "cmp %%rax, %6\n\t"
    			 "je .sdloop8\n\t"
    			 "movq (%0), %%rax\n\t"
    			 "movq %%rax, (%1)\n\t"
    			 "lea 8(%0), %0\n\t"
    			 "lea 8(%1), %1\n\t"
    			 ".sdloop8:\n\t"
    			 "xor %%rax, %%rax\n\t"
    			 "cmp %%rax, %7\n\t"
    			 "je .sdloop4\n\t"
    			 "movl (%0), %%eax\n\t"
    			 "movl %%eax, (%1)\n\t"
    			 "lea 4(%0), %0\n\t"
    			 "lea 4(%1), %1\n\t"
    			 ".sdloop4:\n\t"
    			 "xor %%rax, %%rax\n\t"
    			 "cmp %%rax, %8\n\t"
    			 "je .sdloop1_al\n\t"
    			 ".sdloop1:"
    			 "movb (%0), %%al\n\t"
    			 "movb %%al, (%1)\n\t"
    			 "lea 1(%0), %0\n\t"
    			 "lea 1(%1), %1\n\t"
    			 "dec %8\n\t"
    			 "jnz .sdloop1\n\t"
    			 ".sdloop1_al:\n\t"
    			 "xor %%rax, %%rax\n\t"
    			 "cmp %%rax, %2\n\t"
    			 "je .afterloop16_al\n\t"
    			 ".loop16_al:\n\t"
    			 "movdqa (%0), %%xmm0\n\t"
    			 "movdqu %%xmm0, (%1)\n\t"
    			 "lea 16(%0), %0\n\t"
    			 "lea 16(%1), %1\n\t"
    			 "dec %2\n\t"
    			 "jnz .loop16_al\n\t"
    			 ".afterloop16_al:\n\t"
    			 "xor %%rax, %%rax\n\t"
    			 "cmp %%rax, %3\n\t"
    			 "je .afterloop16_8_al\n\t"
    			 "movq (%0), %%rax\n\t"
    			 "movq %%rax, (%1)\n\t"
    			 "lea 8(%0), %0\n\t"
    			 "lea 8(%1), %1\n\t"
    			 ".afterloop16_8_al:\n\t"
    			 "xor %%rax, %%rax\n\t"
    			 "cmp %%rax, %4\n\t"
    			 "je .afterloop16_4_al\n\t"
    			 "movl (%0), %%eax\n\t"
    			 "movl %%eax, (%1)\n\t"
    			 "lea 4(%0), %0\n\t"
    			 "lea 4(%1), %1\n\t"
    			 ".afterloop16_4_al:\n\t"
    			 "xor %%rax, %%rax\n\t"
    			 "cmp %%rax, %5\n\t"
    			 "je .finish16_al\n\t"
    			 ".loop16_1_al:\n\t"
    			 "movb (%0), %%al\n\t"
    			 "movb %%al, (%1)\n\t"
    			 "lea 1(%0), %0\n\t"
    			 "lea 1(%1), %1\n\t"
    			 "dec %5\n\t"
    			 "jnz .loop16_1_al\n\t"
    			 ".finish16_al:\n\t"
    :
    :"r"(src), "r"(dest), "r"(count), "r"(mod8_count), "r"(mod_count), "r"(mod4_count), "r"(sd8), "r"(sd4), "r"(sd)
    :"%al", "%eax", "%rax", "%xmm0");
}

void *memcpy16_unalign(void *dest, const void *src, std::size_t count) {
    std::size_t mod_count = count % 16;
    count /= 16;
    std::size_t mod8_count = 0;
    if (mod_count > 7) {
        mod8_count = 1;
        mod_count -= 8;
    } else {
        mod8_count = 0;
    }
    std::size_t mod4_count = 0;
    if (mod_count > 3) {
        mod4_count = mod_count - 4;
        mod_count = 1;
    } else {
        mod4_count = mod_count;
        mod_count = 0;
    }
    asm volatile("xor %%rax, %%rax\n\t"
    			 "cmp %%rax, %2\n\t"
    			 "je .afterloop16\n\t"
    			 ".loop16:\n\t"
    			 "movdqu (%0), %%xmm0\n\t"
    			 "movdqu %%xmm0, (%1)\n\t"
    			 "lea 16(%0), %0\n\t"
    			 "lea 16(%1), %1\n\t"
    			 "dec %2\n\t"
    			 "jnz .loop16\n\t"
    			 ".afterloop16:\n\t"
    			 "xor %%rax, %%rax\n\t"
    			 "cmp %%rax, %3\n\t"
    			 "je .afterloop16_8\n\t"
    			 "movq (%0), %%rax\n\t"
    			 "movq %%rax, (%1)\n\t"
    			 "lea 8(%0), %0\n\t"
    			 "lea 8(%1), %1\n\t"
    			 ".afterloop16_8:\n\t"
    			 "xor %%rax, %%rax\n\t"
    			 "cmp %%rax, %4\n\t"
    			 "je .afterloop16_4\n\t"
    			 "movl (%0), %%eax\n\t"
    			 "movl %%eax, (%1)\n\t"
    			 "lea 4(%0), %0\n\t"
    			 "lea 4(%1), %1\n\t"
    			 ".afterloop16_4:\n\t"
    			 "xor %%rax, %%rax\n\t"
    			 "cmp %%rax, %5\n\t"
    			 "je .finish16_un\n\t"
    			 ".loop16_1:\n\t"
    			 "movb (%0), %%al\n\t"
    			 "movb %%al, (%1)\n\t"
    			 "lea 1(%0), %0\n\t"
    			 "lea 1(%1), %1\n\t"
    			 "dec %5\n\t"
    			 "jnz .loop16_1\n\t"
    			 ".finish16_un:\n\t"
    :
    :"r"(src), "r"(dest), "r"(count), "r"(mod8_count), "r"(mod_count), "r"(mod4_count)
    :"%al", "%eax", "%rax", "%xmm0");
}

int main() {
	int sz = 50;
    char source[] = "QWERTYUIOPASDFGHJKL+-*/=!@qwertyui";
	//char source[40];
	//scanf("%s", source);
    char dest[sz], dest1[sz], dest4[sz], dest8[sz], dest16_un[sz], dest16_al[sz];
    /*memcpy(dest, source, sizeof dest);
    for (char c : dest)
        std::cout << c << " ";
    std::cout << "\n";*/
    memcpy1(dest1, source, sizeof dest1);
    for (char c : dest1)
        std::cout << c << "";
    std::cout << "\n";
    memcpy4(dest4, source, sizeof dest4);
    for (char c : dest4)
        std::cout << c << "";
    std::cout << "\n";
    memcpy8(dest8, source, sizeof dest8);
    for (char c : dest8)
        std::cout << c << "";
    std::cout << "\n";
    memcpy16_unalign(dest16_un, source, sizeof dest16_un);
    for (char c : dest16_un)
        std::cout << c << "";
    std::cout << "\n";
    memcpy16_align(dest16_al, source, sizeof dest16_al);
    for (char c : dest16_al)
        std::cout << c << "";
    std::cout << "\n";
    return 0;
}