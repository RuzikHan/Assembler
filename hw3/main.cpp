#include <cstdio>
#include <cstdint>
#include <iostream>
#include <cstring>
#include <emmintrin.h>
#include <vector>

__m128i space, mask[16], inv_mask[16];

void print128_num(__m128i var) {
    uint16_t *val = (uint16_t*) &var;
    printf("Numerical: %i %i %i %i %i %i %i %i \n",
    val[0], val[1], val[2], val[3], val[4], val[5],
    val[6], val[7]);
}

void fillreg() {
    space = _mm_set1_epi8(' ');
    char n = static_cast<char>(0x00);
    char z = static_cast<char>(0xff);
    mask[0] = _mm_setr_epi8(z, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    mask[1] = _mm_setr_epi8(n, z, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    mask[2] = _mm_setr_epi8(n, n, z, n, n, n, n, n, n, n, n, n, n, n, n, n);
    mask[3] = _mm_setr_epi8(n, n, n, z, n, n, n, n, n, n, n, n, n, n, n, n);
    mask[4] = _mm_setr_epi8(n, n, n, n, z, n, n, n, n, n, n, n, n, n, n, n);
    mask[5] = _mm_setr_epi8(n, n, n, n, n, z, n, n, n, n, n, n, n, n, n, n);
    mask[6] = _mm_setr_epi8(n, n, n, n, n, n, z, n, n, n, n, n, n, n, n, n);
    mask[7] = _mm_setr_epi8(n, n, n, n, n, n, n, z, n, n, n, n, n, n, n, n);
    mask[8] = _mm_setr_epi8(n, n, n, n, n, n, n, n, z, n, n, n, n, n, n, n);
    mask[9] = _mm_setr_epi8(n, n, n, n, n, n, n, n, n, z, n, n, n, n, n, n);
    mask[10] = _mm_setr_epi8(n, n, n, n, n, n, n, n, n, n, z, n, n, n, n, n);
    mask[11] = _mm_setr_epi8(n, n, n, n, n, n, n, n, n, n, n, z, n, n, n, n);
    mask[12] = _mm_setr_epi8(n, n, n, n, n, n, n, n, n, n, n, n, z, n, n, n);
    mask[13] = _mm_setr_epi8(n, n, n, n, n, n, n, n, n, n, n, n, n, z, n, n);
    mask[14] = _mm_setr_epi8(n, n, n, n, n, n, n, n, n, n, n, n, n, n, z, n);
    mask[15] = _mm_setr_epi8(n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, z);
    inv_mask[0] = _mm_setr_epi8(n, z, z, z, z, z, z, z, z, z, z, z, z, z, z, z);
    inv_mask[1] = _mm_setr_epi8(z, n, z, z, z, z, z, z, z, z, z, z, z, z, z, z);
    inv_mask[2] = _mm_setr_epi8(z, z, n, z, z, z, z, z, z, z, z, z, z, z, z, z);
    inv_mask[3] = _mm_setr_epi8(z, z, z, n, z, z, z, z, z, z, z, z, z, z, z, z);
    inv_mask[4] = _mm_setr_epi8(z, z, z, z, n, z, z, z, z, z, z, z, z, z, z, z);
    inv_mask[5] = _mm_setr_epi8(z, z, z, z, z, n, z, z, z, z, z, z, z, z, z, z);
    inv_mask[6] = _mm_setr_epi8(z, z, z, z, z, z, n, z, z, z, z, z, z, z, z, z);
    inv_mask[7] = _mm_setr_epi8(z, z, z, z, z, z, z, n, z, z, z, z, z, z, z, z);
    inv_mask[8] = _mm_setr_epi8(z, z, z, z, z, z, z, z, n, z, z, z, z, z, z, z);
    inv_mask[9] = _mm_setr_epi8(z, z, z, z, z, z, z, z, z, n, z, z, z, z, z, z);
    inv_mask[10] = _mm_setr_epi8(z, z, z, z, z, z, z, z, z, z, n, z, z, z, z, z);
    inv_mask[11] = _mm_setr_epi8(z, z, z, z, z, z, z, z, z, z, z, n, z, z, z, z);
    inv_mask[12] = _mm_setr_epi8(z, z, z, z, z, z, z, z, z, z, z, z, n, z, z, z);
    inv_mask[13] = _mm_setr_epi8(z, z, z, z, z, z, z, z, z, z, z, z, z, n, z, z);
    inv_mask[14] = _mm_setr_epi8(z, z, z, z, z, z, z, z, z, z, z, z, z, z, n, z);
    inv_mask[15] = _mm_setr_epi8(z, z, z, z, z, z, z, z, z, z, z, z, z, z, z, n);

}

bool check(__m128i x) {
    uint16_t *val = (uint16_t*) &x;
    for (int i = 0; i < 8; i++) {
        if (val[i] != 0xffff) {
            return false;
        }
    }
    return true;
}

int simple_check(std::string line) {
    char* source = (char*) line.c_str();
    size_t tmp = line.length();
    char nwdest[tmp + 1];
    int ans = 0;
    asm volatile("push %2\n\t"
                 "push %3\n\t"
                 "inc %3\n\t"
                 ".blankloop:\n\t"
                 "movb $32, (%2)\n\t"
                 "lea 1(%2), %2\n\t"
                 "dec %3\n\t"
                 "jnz .blankloop\n\t"
                 "pop %3\n\t"
                 "pop %2\n\t"

                 "push %1\n\t"
                 "push %2\n\t"
                 "push %3\n\t"
                 "lea 1(%2), %2\n\t"
                 ".loop:\n\t"
                 "movb (%1), %%al\n\t"
                 "movb %%al, (%2)\n\t"
                 "lea 1(%1), %1\n\t"
                 "lea 1(%2), %2\n\t"
                 "dec %3\n\t"
                 "jnz .loop\n\t"
                 "pop %3\n\t"
                 "pop %2\n\t"
                 "pop %1\n\t"

                 "push %3\n\t"
                 "push %1\n\t"
                 "push %2\n\t"
                 "xor %%al, %%al\n\t"
                 "inc %%al\n\t"
                 "movb %%al, (%2)\n\t"
                 "dec %%al\n\t"
                 "lea 1(%2), %2\n\t"
                 ".nloop:\n\t"
                 "movb (%1), %%bl\n\t"
                 "cmp $32, %%bl\n\t"
                 "je .getone\n\t"
                 "movb %%al, (%1)\n\t"
                 "jmp .afterset\n\t"
                 ".getone:\n\t"
                 "inc %%al\n\t"
                 "movb %%al, (%1)\n\t"
                 "dec %%al\n\t"
                 ".afterset:\n\t"
                 "movb (%2), %%bl\n\t"
                 "cmp $32, %%bl\n\t"
                 "je .getone_1\n\t"
                 "movb %%al, (%2)\n\t"
                 "jmp .afterset_1\n\t"
                 ".getone_1:\n\t"
                 "inc %%al\n\t"
                 "movb %%al, (%2)\n\t"
                 "dec %%al\n\t"
                 ".afterset_1:\n\t"
                 "lea 1(%1), %1\n\t"
                 "lea 1(%2), %2\n\t"
                 "dec %3\n\t"
                 "jnz .nloop\n\t"
                 "pop %2\n\t"
                 "pop %1\n\t"
                 "pop %3\n\t"

                 "push %1\n\t"
                 "push %3\n\t"
                 ".xorloop:\n\t"
                 "movb (%2), %%al\n\t"
                 "movb (%1), %%bl\n\t"
                 "xorb %%bl, %%al\n\t"
                 "movb %%al, (%1)\n\t"
                 "lea 1(%1), %1\n\t"
                 "lea 1(%2), %2\n\t"
                 "dec %3\n\t"
                 "jnz .xorloop\n\t"
                 "pop %3\n\t"
                 "pop %1\n\t"
                    
                 "xor %%ecx, %%ecx\n\t"
                 ".countloop:\n\t"
                 "movb (%1), %%al\n\t"
                 "cmp $0, %%al\n\t"
                 "je .nocount\n\t"
                 "inc %%ecx\n\t"
                 ".nocount:\n\t"
                 "lea 1(%1), %1\n\t"
                 "dec %3\n\t"
                 "jnz .countloop\n\t"
                 "inc %%ecx\n\t"

                 "movl %%ecx, %0\n\t"

        :"=r"(ans)
        :"r"(source), "r"(nwdest), "r"(tmp)
        :"%al", "%bl", "%eax", "%ecx");
    return ans / 2;
}

int find_ans(int tmp, char l1, char l2, char r1, char r2) {
    if (l1 == ' ' && l2 == ' ') {
        if (r1 == ' ' && r2 == ' ') return tmp / 2;
        if (r1 == ' ' && r2 != ' ') return (tmp - 1) / 2;
        if (r1 != ' ' && r2 == ' ') return tmp / 2;
        if (r1 != ' ' && r2 != ' ') return (tmp - 1) / 2;
    } else if (l1 == ' ' && l2 != ' ') {
        if (r1 == ' ' && r2 == ' ') return tmp / 2;
        if (r1 == ' ' && r2 != ' ') return (tmp + 1) / 2;   
        if (r1 != ' ' && r2 == ' ') return tmp / 2;
        if (r1 != ' ' && r2 != ' ') return (tmp + 1) / 2;
    } else if (l1 != ' ' && l2 == ' ') {
        if (r1 == ' ' && r2 == ' ') return (tmp + 1) / 2;
        if (r1 == ' ' && r2 != ' ') return tmp / 2;
        if (r1 != ' ' && r2 == ' ') return (tmp + 1) / 2;
        if (r1 != ' ' && r2 != ' ') return tmp / 2;
    } else if (l1 != ' ' && l2 != ' ') {
        if (r1 == ' ' && r2 == ' ') return (tmp + 1) / 2;
        if (r1 == ' ' && r2 != ' ') return tmp / 2 + 1;
        if (r1 != ' ' && r2 == ' ') return (tmp + 1) / 2;
        if (r1 != ' ' && r2 != ' ') return tmp / 2 + 1;
    }
}


std::string test1() {
    std::string line;
    for (int i = 0; i < 15768; i++) {
        line = line + "a ";
    }
    return line;
}

std::string test2() {
    std::string line = "thtrh gsdf ikik asdfd thth asdfsadf";
    for (int i = 0; i < 12345; i++) {
        line = line + " rttyyu ";
        line = line + "  a";
    }
    return line;
}

std::string test3() {
    std::string line = "1 2 3 4 5 6 7 8 9 10 11";
    for (int i = 0; i < 6577; i++) {
        line = line + " dddddddd dddddd ddddddddddddd tyr   ";
    }
    return line;
}

int main() {
    fillreg();
	std::string line;
    //getline(std::cin, line);
    std::vector<std::string>tests;
    tests.resize(3);
    tests[0] = test1();
    tests[1] = test2();
    tests[2] = test3();
	for (int q = 0; q < 3; q++) {
	    line = tests[q];
	    if (line.length() == 0) {
	        std::cout << 0 << "\n";
	        exit(0);
	    }
	    int tmp = line.length() - 1;
	    while (tmp >= 0 && line[tmp] == ' ') tmp--;
	    if (tmp < 0) {
	        std::cout << 0 << "\n";
	        exit(0);
	    } else {
	        line.erase(tmp + 1, line.length() - tmp - 1);
	    }
	    for (int i = 0; i < line.length() % 16; i++) {
	        line = ' ' + line;
	    }
	    std::cout << simple_check(line) << "\n";
	    const char* source = line.c_str();
	    int count = line.length() / 16;
	    int val = 0;
	    if (count > 1) {
	        for (int i = 0; i < count - 1; i++) {
	            if (line[16 * i + 15] != ' ' && line[16 * i + 16] != ' ') val++;
	        }
	    }
	    int ans = 0;
	    for (int k = 0; k < count - 1; k++) {
	        int i = 16 * k;
	        __m128i a, b;
	        a = _mm_setr_epi8(source[i + 0], source[i + 1], source[i + 2], source[i + 3], source[i + 4], source[i + 5], 
	            source[i + 6], source[i + 7], source[i + 8], source[i + 9], source[i + 10], source[i + 11], source[i + 12],
	             source[i + 13], source[i + 14], source[i + 15]);
	        b = _mm_setr_epi8(source[i + 1], source[i + 2], source[i + 3], source[i + 4], source[i + 5], source[i + 6], 
	            source[i + 7], source[i + 8], source[i + 9], source[i + 10], source[i + 11], source[i + 12], source[i + 13],
	             source[i + 14], source[i + 15], source[i + 16]);
	        a = _mm_xor_si128(a, space);
	        b = _mm_xor_si128(b, space);
	        for (int j = 0; j < 16; j++) {
	            __m128i c = a;
	            __m128i d = _mm_and_si128(a, inv_mask[j]);
	            if (!check(_mm_cmpeq_epi32(c, d))) {
	                a = _mm_or_si128(a, mask[j]);
	            }
	            c = b;
	            d = _mm_and_si128(b, inv_mask[j]);
	            if (!check(_mm_cmpeq_epi32(c, d))) {
	                b = _mm_or_si128(b, mask[j]);
	            }
	        }
	        a = _mm_xor_si128(a, b);
	        int ans_tmp = 0;
	        for (int j = 0; j < 16; j++) {
	            __m128i c = a;
	            a = _mm_or_si128(a, inv_mask[j]);
	            if (check(a)) {
	                ans_tmp++;
	            }
	            a = c;
	        }
	        if (ans_tmp != 0) {
	            ans_tmp = find_ans(ans_tmp, source[i + 0], source[i + 15], source[i + 1], source[i + 16]);
	        } else {
	            if (source[i + 1] != ' ' && source[i + 16] != ' ') ans_tmp = 1;
	        }
	        ans += ans_tmp;
	    }
	    ans += simple_check(line.substr((count - 1) * 16));
	    std::cout << ans - val << "\n";
	}
    return 0;
}
