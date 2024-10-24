/*
 * Copyright (c) 2015-2020 the fiat-crypto authors.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdint.h>

typedef unsigned char fiat_25519_uint1;
typedef signed char fiat_25519_int1;
#ifdef __GNUC__
#  define FIAT_25519_FIAT_INLINE __inline__
#else
#  define FIAT_25519_FIAT_INLINE
#endif

/* The type fiat_25519_loose_field_element is a field element with loose bounds. */
/* Bounds: [[0x0 ~> 0xc000000], [0x0 ~> 0x6000000], [0x0 ~> 0xc000000], [0x0 ~> 0x6000000], [0x0 ~> 0xc000000], [0x0 ~> 0x6000000], [0x0 ~> 0xc000000], [0x0 ~> 0x6000000], [0x0 ~> 0xc000000], [0x0 ~> 0x6000000]] */
typedef uint32_t fiat_25519_loose_field_element[10];

/* The type fiat_25519_tight_field_element is a field element with tight bounds. */
/* Bounds: [[0x0 ~> 0x4000000], [0x0 ~> 0x2000000], [0x0 ~> 0x4000000], [0x0 ~> 0x2000000], [0x0 ~> 0x4000000], [0x0 ~> 0x2000000], [0x0 ~> 0x4000000], [0x0 ~> 0x2000000], [0x0 ~> 0x4000000], [0x0 ~> 0x2000000]] */
typedef uint32_t fiat_25519_tight_field_element[10];

#if (-1 & 3) != 3
#error "This code only works on a two's complement system"
#endif

#if !defined(FIAT_25519_NO_ASM) && (defined(__GNUC__) || defined(__clang__))

static __inline__ uint32_t fiat_25519_value_barrier_u32(uint32_t a) {
    __asm__("" : "+r"(a) : /* no inputs */);
    return a;
}

#else
#  define fiat_25519_value_barrier_u32(x) (x)
#endif


/*
 * The function fiat_25519_addcarryx_u26 is an addition with carry.
 *
 * Postconditions:
 *   out1 = (arg1 + arg2 + arg3) mod 2^26
 *   out2 = ⌊(arg1 + arg2 + arg3) / 2^26⌋
 *
 * Input Bounds:
 *   arg1: [0x0 ~> 0x1]
 *   arg2: [0x0 ~> 0x3ffffff]
 *   arg3: [0x0 ~> 0x3ffffff]
 * Output Bounds:
 *   out1: [0x0 ~> 0x3ffffff]
 *   out2: [0x0 ~> 0x1]
 */
static FIAT_25519_FIAT_INLINE void
fiat_25519_addcarryx_u26(uint32_t *out1, fiat_25519_uint1 *out2, fiat_25519_uint1 arg1,
                         uint32_t arg2, uint32_t arg3) {
    uint32_t x1;
    uint32_t x2;
    fiat_25519_uint1 x3;
    x1 = ((arg1 + arg2) + arg3);
    x2 = (x1 & UINT32_C(0x3ffffff));
    x3 = (fiat_25519_uint1) (x1 >> 26);
    *out1 = x2;
    *out2 = x3;
}

/*
 * The function fiat_25519_subborrowx_u26 is a subtraction with borrow.
 *
 * Postconditions:
 *   out1 = (-arg1 + arg2 + -arg3) mod 2^26
 *   out2 = -⌊(-arg1 + arg2 + -arg3) / 2^26⌋
 *
 * Input Bounds:
 *   arg1: [0x0 ~> 0x1]
 *   arg2: [0x0 ~> 0x3ffffff]
 *   arg3: [0x0 ~> 0x3ffffff]
 * Output Bounds:
 *   out1: [0x0 ~> 0x3ffffff]
 *   out2: [0x0 ~> 0x1]
 */
static FIAT_25519_FIAT_INLINE void
fiat_25519_subborrowx_u26(uint32_t *out1, fiat_25519_uint1 *out2, fiat_25519_uint1 arg1,
                          uint32_t arg2, uint32_t arg3) {
    int32_t x1;
    fiat_25519_int1 x2;
    uint32_t x3;
    x1 = ((int32_t) (arg2 - arg1) - (int32_t) arg3);
    x2 = (fiat_25519_int1) (x1 >> 26);
    x3 = (x1 & UINT32_C(0x3ffffff));
    *out1 = x3;
    *out2 = (fiat_25519_uint1) (0x0 - x2);
}

/*
 * The function fiat_25519_addcarryx_u25 is an addition with carry.
 *
 * Postconditions:
 *   out1 = (arg1 + arg2 + arg3) mod 2^25
 *   out2 = ⌊(arg1 + arg2 + arg3) / 2^25⌋
 *
 * Input Bounds:
 *   arg1: [0x0 ~> 0x1]
 *   arg2: [0x0 ~> 0x1ffffff]
 *   arg3: [0x0 ~> 0x1ffffff]
 * Output Bounds:
 *   out1: [0x0 ~> 0x1ffffff]
 *   out2: [0x0 ~> 0x1]
 */
static FIAT_25519_FIAT_INLINE void
fiat_25519_addcarryx_u25(uint32_t *out1, fiat_25519_uint1 *out2, fiat_25519_uint1 arg1,
                         uint32_t arg2, uint32_t arg3) {
    uint32_t x1;
    uint32_t x2;
    fiat_25519_uint1 x3;
    x1 = ((arg1 + arg2) + arg3);
    x2 = (x1 & UINT32_C(0x1ffffff));
    x3 = (fiat_25519_uint1) (x1 >> 25);
    *out1 = x2;
    *out2 = x3;
}

/*
 * The function fiat_25519_subborrowx_u25 is a subtraction with borrow.
 *
 * Postconditions:
 *   out1 = (-arg1 + arg2 + -arg3) mod 2^25
 *   out2 = -⌊(-arg1 + arg2 + -arg3) / 2^25⌋
 *
 * Input Bounds:
 *   arg1: [0x0 ~> 0x1]
 *   arg2: [0x0 ~> 0x1ffffff]
 *   arg3: [0x0 ~> 0x1ffffff]
 * Output Bounds:
 *   out1: [0x0 ~> 0x1ffffff]
 *   out2: [0x0 ~> 0x1]
 */
static FIAT_25519_FIAT_INLINE void
fiat_25519_subborrowx_u25(uint32_t *out1, fiat_25519_uint1 *out2, fiat_25519_uint1 arg1,
                          uint32_t arg2, uint32_t arg3) {
    int32_t x1;
    fiat_25519_int1 x2;
    uint32_t x3;
    x1 = ((int32_t) (arg2 - arg1) - (int32_t) arg3);
    x2 = (fiat_25519_int1) (x1 >> 25);
    x3 = (x1 & UINT32_C(0x1ffffff));
    *out1 = x3;
    *out2 = (fiat_25519_uint1) (0x0 - x2);
}

/*
 * The function fiat_25519_cmovznz_u32 is a single-word conditional move.
 *
 * Postconditions:
 *   out1 = (if arg1 = 0 then arg2 else arg3)
 *
 * Input Bounds:
 *   arg1: [0x0 ~> 0x1]
 *   arg2: [0x0 ~> 0xffffffff]
 *   arg3: [0x0 ~> 0xffffffff]
 * Output Bounds:
 *   out1: [0x0 ~> 0xffffffff]
 */
static FIAT_25519_FIAT_INLINE void
fiat_25519_cmovznz_u32(uint32_t *out1, fiat_25519_uint1 arg1, uint32_t arg2, uint32_t arg3) {
    fiat_25519_uint1 x1;
    uint32_t x2;
    uint32_t x3;
    x1 = (!(!arg1));
    x2 = ((fiat_25519_int1) (0x0 - x1) & UINT32_C(0xffffffff));
    x3 = ((fiat_25519_value_barrier_u32(x2) & arg3) | (fiat_25519_value_barrier_u32((~x2)) & arg2));
    *out1 = x3;
}

/*
 * The function fiat_25519_carry_mul multiplies two field elements and reduces the result.
 *
 * Postconditions:
 *   eval out1 mod m = (eval arg1 * eval arg2) mod m
 *
 */
static void
fiat_25519_carry_mul(fiat_25519_tight_field_element out1, const fiat_25519_loose_field_element arg1,
                     const fiat_25519_loose_field_element arg2) {
    uint64_t x1;
    uint64_t x2;
    uint64_t x3;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
    uint64_t x8;
    uint64_t x9;
    uint64_t x10;
    uint64_t x11;
    uint64_t x12;
    uint64_t x13;
    uint64_t x14;
    uint64_t x15;
    uint64_t x16;
    uint64_t x17;
    uint64_t x18;
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t x29;
    uint64_t x30;
    uint64_t x31;
    uint64_t x32;
    uint64_t x33;
    uint64_t x34;
    uint64_t x35;
    uint64_t x36;
    uint64_t x37;
    uint64_t x38;
    uint64_t x39;
    uint64_t x40;
    uint64_t x41;
    uint64_t x42;
    uint64_t x43;
    uint64_t x44;
    uint64_t x45;
    uint64_t x46;
    uint64_t x47;
    uint64_t x48;
    uint64_t x49;
    uint64_t x50;
    uint64_t x51;
    uint64_t x52;
    uint64_t x53;
    uint64_t x54;
    uint64_t x55;
    uint64_t x56;
    uint64_t x57;
    uint64_t x58;
    uint64_t x59;
    uint64_t x60;
    uint64_t x61;
    uint64_t x62;
    uint64_t x63;
    uint64_t x64;
    uint64_t x65;
    uint64_t x66;
    uint64_t x67;
    uint64_t x68;
    uint64_t x69;
    uint64_t x70;
    uint64_t x71;
    uint64_t x72;
    uint64_t x73;
    uint64_t x74;
    uint64_t x75;
    uint64_t x76;
    uint64_t x77;
    uint64_t x78;
    uint64_t x79;
    uint64_t x80;
    uint64_t x81;
    uint64_t x82;
    uint64_t x83;
    uint64_t x84;
    uint64_t x85;
    uint64_t x86;
    uint64_t x87;
    uint64_t x88;
    uint64_t x89;
    uint64_t x90;
    uint64_t x91;
    uint64_t x92;
    uint64_t x93;
    uint64_t x94;
    uint64_t x95;
    uint64_t x96;
    uint64_t x97;
    uint64_t x98;
    uint64_t x99;
    uint64_t x100;
    uint64_t x101;
    uint64_t x102;
    uint32_t x103;
    uint64_t x104;
    uint64_t x105;
    uint64_t x106;
    uint64_t x107;
    uint64_t x108;
    uint64_t x109;
    uint64_t x110;
    uint64_t x111;
    uint64_t x112;
    uint64_t x113;
    uint64_t x114;
    uint32_t x115;
    uint64_t x116;
    uint64_t x117;
    uint32_t x118;
    uint64_t x119;
    uint64_t x120;
    uint32_t x121;
    uint64_t x122;
    uint64_t x123;
    uint32_t x124;
    uint64_t x125;
    uint64_t x126;
    uint32_t x127;
    uint64_t x128;
    uint64_t x129;
    uint32_t x130;
    uint64_t x131;
    uint64_t x132;
    uint32_t x133;
    uint64_t x134;
    uint64_t x135;
    uint32_t x136;
    uint64_t x137;
    uint64_t x138;
    uint32_t x139;
    uint64_t x140;
    uint64_t x141;
    uint32_t x142;
    uint32_t x143;
    uint32_t x144;
    fiat_25519_uint1 x145;
    uint32_t x146;
    uint32_t x147;
    x1 = ((uint64_t) (arg1[9]) * ((arg2[9]) * UINT8_C(0x26)));
    x2 = ((uint64_t) (arg1[9]) * ((arg2[8]) * UINT8_C(0x13)));
    x3 = ((uint64_t) (arg1[9]) * ((arg2[7]) * UINT8_C(0x26)));
    x4 = ((uint64_t) (arg1[9]) * ((arg2[6]) * UINT8_C(0x13)));
    x5 = ((uint64_t) (arg1[9]) * ((arg2[5]) * UINT8_C(0x26)));
    x6 = ((uint64_t) (arg1[9]) * ((arg2[4]) * UINT8_C(0x13)));
    x7 = ((uint64_t) (arg1[9]) * ((arg2[3]) * UINT8_C(0x26)));
    x8 = ((uint64_t) (arg1[9]) * ((arg2[2]) * UINT8_C(0x13)));
    x9 = ((uint64_t) (arg1[9]) * ((arg2[1]) * UINT8_C(0x26)));
    x10 = ((uint64_t) (arg1[8]) * ((arg2[9]) * UINT8_C(0x13)));
    x11 = ((uint64_t) (arg1[8]) * ((arg2[8]) * UINT8_C(0x13)));
    x12 = ((uint64_t) (arg1[8]) * ((arg2[7]) * UINT8_C(0x13)));
    x13 = ((uint64_t) (arg1[8]) * ((arg2[6]) * UINT8_C(0x13)));
    x14 = ((uint64_t) (arg1[8]) * ((arg2[5]) * UINT8_C(0x13)));
    x15 = ((uint64_t) (arg1[8]) * ((arg2[4]) * UINT8_C(0x13)));
    x16 = ((uint64_t) (arg1[8]) * ((arg2[3]) * UINT8_C(0x13)));
    x17 = ((uint64_t) (arg1[8]) * ((arg2[2]) * UINT8_C(0x13)));
    x18 = ((uint64_t) (arg1[7]) * ((arg2[9]) * UINT8_C(0x26)));
    x19 = ((uint64_t) (arg1[7]) * ((arg2[8]) * UINT8_C(0x13)));
    x20 = ((uint64_t) (arg1[7]) * ((arg2[7]) * UINT8_C(0x26)));
    x21 = ((uint64_t) (arg1[7]) * ((arg2[6]) * UINT8_C(0x13)));
    x22 = ((uint64_t) (arg1[7]) * ((arg2[5]) * UINT8_C(0x26)));
    x23 = ((uint64_t) (arg1[7]) * ((arg2[4]) * UINT8_C(0x13)));
    x24 = ((uint64_t) (arg1[7]) * ((arg2[3]) * UINT8_C(0x26)));
    x25 = ((uint64_t) (arg1[6]) * ((arg2[9]) * UINT8_C(0x13)));
    x26 = ((uint64_t) (arg1[6]) * ((arg2[8]) * UINT8_C(0x13)));
    x27 = ((uint64_t) (arg1[6]) * ((arg2[7]) * UINT8_C(0x13)));
    x28 = ((uint64_t) (arg1[6]) * ((arg2[6]) * UINT8_C(0x13)));
    x29 = ((uint64_t) (arg1[6]) * ((arg2[5]) * UINT8_C(0x13)));
    x30 = ((uint64_t) (arg1[6]) * ((arg2[4]) * UINT8_C(0x13)));
    x31 = ((uint64_t) (arg1[5]) * ((arg2[9]) * UINT8_C(0x26)));
    x32 = ((uint64_t) (arg1[5]) * ((arg2[8]) * UINT8_C(0x13)));
    x33 = ((uint64_t) (arg1[5]) * ((arg2[7]) * UINT8_C(0x26)));
    x34 = ((uint64_t) (arg1[5]) * ((arg2[6]) * UINT8_C(0x13)));
    x35 = ((uint64_t) (arg1[5]) * ((arg2[5]) * UINT8_C(0x26)));
    x36 = ((uint64_t) (arg1[4]) * ((arg2[9]) * UINT8_C(0x13)));
    x37 = ((uint64_t) (arg1[4]) * ((arg2[8]) * UINT8_C(0x13)));
    x38 = ((uint64_t) (arg1[4]) * ((arg2[7]) * UINT8_C(0x13)));
    x39 = ((uint64_t) (arg1[4]) * ((arg2[6]) * UINT8_C(0x13)));
    x40 = ((uint64_t) (arg1[3]) * ((arg2[9]) * UINT8_C(0x26)));
    x41 = ((uint64_t) (arg1[3]) * ((arg2[8]) * UINT8_C(0x13)));
    x42 = ((uint64_t) (arg1[3]) * ((arg2[7]) * UINT8_C(0x26)));
    x43 = ((uint64_t) (arg1[2]) * ((arg2[9]) * UINT8_C(0x13)));
    x44 = ((uint64_t) (arg1[2]) * ((arg2[8]) * UINT8_C(0x13)));
    x45 = ((uint64_t) (arg1[1]) * ((arg2[9]) * UINT8_C(0x26)));
    x46 = ((uint64_t) (arg1[9]) * (arg2[0]));
    x47 = ((uint64_t) (arg1[8]) * (arg2[1]));
    x48 = ((uint64_t) (arg1[8]) * (arg2[0]));
    x49 = ((uint64_t) (arg1[7]) * (arg2[2]));
    x50 = ((uint64_t) (arg1[7]) * ((arg2[1]) * 0x2));
    x51 = ((uint64_t) (arg1[7]) * (arg2[0]));
    x52 = ((uint64_t) (arg1[6]) * (arg2[3]));
    x53 = ((uint64_t) (arg1[6]) * (arg2[2]));
    x54 = ((uint64_t) (arg1[6]) * (arg2[1]));
    x55 = ((uint64_t) (arg1[6]) * (arg2[0]));
    x56 = ((uint64_t) (arg1[5]) * (arg2[4]));
    x57 = ((uint64_t) (arg1[5]) * ((arg2[3]) * 0x2));
    x58 = ((uint64_t) (arg1[5]) * (arg2[2]));
    x59 = ((uint64_t) (arg1[5]) * ((arg2[1]) * 0x2));
    x60 = ((uint64_t) (arg1[5]) * (arg2[0]));
    x61 = ((uint64_t) (arg1[4]) * (arg2[5]));
    x62 = ((uint64_t) (arg1[4]) * (arg2[4]));
    x63 = ((uint64_t) (arg1[4]) * (arg2[3]));
    x64 = ((uint64_t) (arg1[4]) * (arg2[2]));
    x65 = ((uint64_t) (arg1[4]) * (arg2[1]));
    x66 = ((uint64_t) (arg1[4]) * (arg2[0]));
    x67 = ((uint64_t) (arg1[3]) * (arg2[6]));
    x68 = ((uint64_t) (arg1[3]) * ((arg2[5]) * 0x2));
    x69 = ((uint64_t) (arg1[3]) * (arg2[4]));
    x70 = ((uint64_t) (arg1[3]) * ((arg2[3]) * 0x2));
    x71 = ((uint64_t) (arg1[3]) * (arg2[2]));
    x72 = ((uint64_t) (arg1[3]) * ((arg2[1]) * 0x2));
    x73 = ((uint64_t) (arg1[3]) * (arg2[0]));
    x74 = ((uint64_t) (arg1[2]) * (arg2[7]));
    x75 = ((uint64_t) (arg1[2]) * (arg2[6]));
    x76 = ((uint64_t) (arg1[2]) * (arg2[5]));
    x77 = ((uint64_t) (arg1[2]) * (arg2[4]));
    x78 = ((uint64_t) (arg1[2]) * (arg2[3]));
    x79 = ((uint64_t) (arg1[2]) * (arg2[2]));
    x80 = ((uint64_t) (arg1[2]) * (arg2[1]));
    x81 = ((uint64_t) (arg1[2]) * (arg2[0]));
    x82 = ((uint64_t) (arg1[1]) * (arg2[8]));
    x83 = ((uint64_t) (arg1[1]) * ((arg2[7]) * 0x2));
    x84 = ((uint64_t) (arg1[1]) * (arg2[6]));
    x85 = ((uint64_t) (arg1[1]) * ((arg2[5]) * 0x2));
    x86 = ((uint64_t) (arg1[1]) * (arg2[4]));
    x87 = ((uint64_t) (arg1[1]) * ((arg2[3]) * 0x2));
    x88 = ((uint64_t) (arg1[1]) * (arg2[2]));
    x89 = ((uint64_t) (arg1[1]) * ((arg2[1]) * 0x2));
    x90 = ((uint64_t) (arg1[1]) * (arg2[0]));
    x91 = ((uint64_t) (arg1[0]) * (arg2[9]));
    x92 = ((uint64_t) (arg1[0]) * (arg2[8]));
    x93 = ((uint64_t) (arg1[0]) * (arg2[7]));
    x94 = ((uint64_t) (arg1[0]) * (arg2[6]));
    x95 = ((uint64_t) (arg1[0]) * (arg2[5]));
    x96 = ((uint64_t) (arg1[0]) * (arg2[4]));
    x97 = ((uint64_t) (arg1[0]) * (arg2[3]));
    x98 = ((uint64_t) (arg1[0]) * (arg2[2]));
    x99 = ((uint64_t) (arg1[0]) * (arg2[1]));
    x100 = ((uint64_t) (arg1[0]) * (arg2[0]));
    x101 = (x100 + (x45 + (x44 + (x42 + (x39 + (x35 + (x30 + (x24 + (x17 + x9)))))))));
    x102 = (x101 >> 26);
    x103 = (uint32_t) (x101 & UINT32_C(0x3ffffff));
    x104 = (x91 + (x82 + (x74 + (x67 + (x61 + (x56 + (x52 + (x49 + (x47 + x46)))))))));
    x105 = (x92 + (x83 + (x75 + (x68 + (x62 + (x57 + (x53 + (x50 + (x48 + x1)))))))));
    x106 = (x93 + (x84 + (x76 + (x69 + (x63 + (x58 + (x54 + (x51 + (x10 + x2)))))))));
    x107 = (x94 + (x85 + (x77 + (x70 + (x64 + (x59 + (x55 + (x18 + (x11 + x3)))))))));
    x108 = (x95 + (x86 + (x78 + (x71 + (x65 + (x60 + (x25 + (x19 + (x12 + x4)))))))));
    x109 = (x96 + (x87 + (x79 + (x72 + (x66 + (x31 + (x26 + (x20 + (x13 + x5)))))))));
    x110 = (x97 + (x88 + (x80 + (x73 + (x36 + (x32 + (x27 + (x21 + (x14 + x6)))))))));
    x111 = (x98 + (x89 + (x81 + (x40 + (x37 + (x33 + (x28 + (x22 + (x15 + x7)))))))));
    x112 = (x99 + (x90 + (x43 + (x41 + (x38 + (x34 + (x29 + (x23 + (x16 + x8)))))))));
    x113 = (x102 + x112);
    x114 = (x113 >> 25);
    x115 = (uint32_t) (x113 & UINT32_C(0x1ffffff));
    x116 = (x114 + x111);
    x117 = (x116 >> 26);
    x118 = (uint32_t) (x116 & UINT32_C(0x3ffffff));
    x119 = (x117 + x110);
    x120 = (x119 >> 25);
    x121 = (uint32_t) (x119 & UINT32_C(0x1ffffff));
    x122 = (x120 + x109);
    x123 = (x122 >> 26);
    x124 = (uint32_t) (x122 & UINT32_C(0x3ffffff));
    x125 = (x123 + x108);
    x126 = (x125 >> 25);
    x127 = (uint32_t) (x125 & UINT32_C(0x1ffffff));
    x128 = (x126 + x107);
    x129 = (x128 >> 26);
    x130 = (uint32_t) (x128 & UINT32_C(0x3ffffff));
    x131 = (x129 + x106);
    x132 = (x131 >> 25);
    x133 = (uint32_t) (x131 & UINT32_C(0x1ffffff));
    x134 = (x132 + x105);
    x135 = (x134 >> 26);
    x136 = (uint32_t) (x134 & UINT32_C(0x3ffffff));
    x137 = (x135 + x104);
    x138 = (x137 >> 25);
    x139 = (uint32_t) (x137 & UINT32_C(0x1ffffff));
    x140 = (x138 * UINT8_C(0x13));
    x141 = (x103 + x140);
    x142 = (uint32_t) (x141 >> 26);
    x143 = (uint32_t) (x141 & UINT32_C(0x3ffffff));
    x144 = (x142 + x115);
    x145 = (fiat_25519_uint1) (x144 >> 25);
    x146 = (x144 & UINT32_C(0x1ffffff));
    x147 = (x145 + x118);
    out1[0] = x143;
    out1[1] = x146;
    out1[2] = x147;
    out1[3] = x121;
    out1[4] = x124;
    out1[5] = x127;
    out1[6] = x130;
    out1[7] = x133;
    out1[8] = x136;
    out1[9] = x139;
}

/*
 * The function fiat_25519_carry_square squares a field element and reduces the result.
 *
 * Postconditions:
 *   eval out1 mod m = (eval arg1 * eval arg1) mod m
 *
 */
static void fiat_25519_carry_square(fiat_25519_tight_field_element out1,
                                    const fiat_25519_loose_field_element arg1) {
    uint32_t x1;
    uint32_t x2;
    uint32_t x3;
    uint32_t x4;
    uint64_t x5;
    uint32_t x6;
    uint32_t x7;
    uint32_t x8;
    uint32_t x9;
    uint32_t x10;
    uint64_t x11;
    uint32_t x12;
    uint32_t x13;
    uint32_t x14;
    uint32_t x15;
    uint32_t x16;
    uint32_t x17;
    uint32_t x18;
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t x29;
    uint64_t x30;
    uint64_t x31;
    uint64_t x32;
    uint64_t x33;
    uint64_t x34;
    uint64_t x35;
    uint64_t x36;
    uint64_t x37;
    uint64_t x38;
    uint64_t x39;
    uint64_t x40;
    uint64_t x41;
    uint64_t x42;
    uint64_t x43;
    uint64_t x44;
    uint64_t x45;
    uint64_t x46;
    uint64_t x47;
    uint64_t x48;
    uint64_t x49;
    uint64_t x50;
    uint64_t x51;
    uint64_t x52;
    uint64_t x53;
    uint64_t x54;
    uint64_t x55;
    uint64_t x56;
    uint64_t x57;
    uint64_t x58;
    uint64_t x59;
    uint64_t x60;
    uint64_t x61;
    uint64_t x62;
    uint64_t x63;
    uint64_t x64;
    uint64_t x65;
    uint64_t x66;
    uint64_t x67;
    uint64_t x68;
    uint64_t x69;
    uint64_t x70;
    uint64_t x71;
    uint64_t x72;
    uint64_t x73;
    uint64_t x74;
    uint64_t x75;
    uint32_t x76;
    uint64_t x77;
    uint64_t x78;
    uint64_t x79;
    uint64_t x80;
    uint64_t x81;
    uint64_t x82;
    uint64_t x83;
    uint64_t x84;
    uint64_t x85;
    uint64_t x86;
    uint64_t x87;
    uint32_t x88;
    uint64_t x89;
    uint64_t x90;
    uint32_t x91;
    uint64_t x92;
    uint64_t x93;
    uint32_t x94;
    uint64_t x95;
    uint64_t x96;
    uint32_t x97;
    uint64_t x98;
    uint64_t x99;
    uint32_t x100;
    uint64_t x101;
    uint64_t x102;
    uint32_t x103;
    uint64_t x104;
    uint64_t x105;
    uint32_t x106;
    uint64_t x107;
    uint64_t x108;
    uint32_t x109;
    uint64_t x110;
    uint64_t x111;
    uint32_t x112;
    uint64_t x113;
    uint64_t x114;
    uint32_t x115;
    uint32_t x116;
    uint32_t x117;
    fiat_25519_uint1 x118;
    uint32_t x119;
    uint32_t x120;
    x1 = ((arg1[9]) * UINT8_C(0x13));
    x2 = (x1 * 0x2);
    x3 = ((arg1[9]) * 0x2);
    x4 = ((arg1[8]) * UINT8_C(0x13));
    x5 = ((uint64_t) x4 * 0x2);
    x6 = ((arg1[8]) * 0x2);
    x7 = ((arg1[7]) * UINT8_C(0x13));
    x8 = (x7 * 0x2);
    x9 = ((arg1[7]) * 0x2);
    x10 = ((arg1[6]) * UINT8_C(0x13));
    x11 = ((uint64_t) x10 * 0x2);
    x12 = ((arg1[6]) * 0x2);
    x13 = ((arg1[5]) * UINT8_C(0x13));
    x14 = ((arg1[5]) * 0x2);
    x15 = ((arg1[4]) * 0x2);
    x16 = ((arg1[3]) * 0x2);
    x17 = ((arg1[2]) * 0x2);
    x18 = ((arg1[1]) * 0x2);
    x19 = ((uint64_t) (arg1[9]) * (x1 * 0x2));
    x20 = ((uint64_t) (arg1[8]) * x2);
    x21 = ((uint64_t) (arg1[8]) * x4);
    x22 = ((arg1[7]) * ((uint64_t) x2 * 0x2));
    x23 = ((arg1[7]) * x5);
    x24 = ((uint64_t) (arg1[7]) * (x7 * 0x2));
    x25 = ((uint64_t) (arg1[6]) * x2);
    x26 = ((arg1[6]) * x5);
    x27 = ((uint64_t) (arg1[6]) * x8);
    x28 = ((uint64_t) (arg1[6]) * x10);
    x29 = ((arg1[5]) * ((uint64_t) x2 * 0x2));
    x30 = ((arg1[5]) * x5);
    x31 = ((arg1[5]) * ((uint64_t) x8 * 0x2));
    x32 = ((arg1[5]) * x11);
    x33 = ((uint64_t) (arg1[5]) * (x13 * 0x2));
    x34 = ((uint64_t) (arg1[4]) * x2);
    x35 = ((arg1[4]) * x5);
    x36 = ((uint64_t) (arg1[4]) * x8);
    x37 = ((arg1[4]) * x11);
    x38 = ((uint64_t) (arg1[4]) * x14);
    x39 = ((uint64_t) (arg1[4]) * (arg1[4]));
    x40 = ((arg1[3]) * ((uint64_t) x2 * 0x2));
    x41 = ((arg1[3]) * x5);
    x42 = ((arg1[3]) * ((uint64_t) x8 * 0x2));
    x43 = ((uint64_t) (arg1[3]) * x12);
    x44 = ((uint64_t) (arg1[3]) * (x14 * 0x2));
    x45 = ((uint64_t) (arg1[3]) * x15);
    x46 = ((uint64_t) (arg1[3]) * ((arg1[3]) * 0x2));
    x47 = ((uint64_t) (arg1[2]) * x2);
    x48 = ((arg1[2]) * x5);
    x49 = ((uint64_t) (arg1[2]) * x9);
    x50 = ((uint64_t) (arg1[2]) * x12);
    x51 = ((uint64_t) (arg1[2]) * x14);
    x52 = ((uint64_t) (arg1[2]) * x15);
    x53 = ((uint64_t) (arg1[2]) * x16);
    x54 = ((uint64_t) (arg1[2]) * (arg1[2]));
    x55 = ((arg1[1]) * ((uint64_t) x2 * 0x2));
    x56 = ((uint64_t) (arg1[1]) * x6);
    x57 = ((uint64_t) (arg1[1]) * (x9 * 0x2));
    x58 = ((uint64_t) (arg1[1]) * x12);
    x59 = ((uint64_t) (arg1[1]) * (x14 * 0x2));
    x60 = ((uint64_t) (arg1[1]) * x15);
    x61 = ((uint64_t) (arg1[1]) * (x16 * 0x2));
    x62 = ((uint64_t) (arg1[1]) * x17);
    x63 = ((uint64_t) (arg1[1]) * ((arg1[1]) * 0x2));
    x64 = ((uint64_t) (arg1[0]) * x3);
    x65 = ((uint64_t) (arg1[0]) * x6);
    x66 = ((uint64_t) (arg1[0]) * x9);
    x67 = ((uint64_t) (arg1[0]) * x12);
    x68 = ((uint64_t) (arg1[0]) * x14);
    x69 = ((uint64_t) (arg1[0]) * x15);
    x70 = ((uint64_t) (arg1[0]) * x16);
    x71 = ((uint64_t) (arg1[0]) * x17);
    x72 = ((uint64_t) (arg1[0]) * x18);
    x73 = ((uint64_t) (arg1[0]) * (arg1[0]));
    x74 = (x73 + (x55 + (x48 + (x42 + (x37 + x33)))));
    x75 = (x74 >> 26);
    x76 = (uint32_t) (x74 & UINT32_C(0x3ffffff));
    x77 = (x64 + (x56 + (x49 + (x43 + x38))));
    x78 = (x65 + (x57 + (x50 + (x44 + (x39 + x19)))));
    x79 = (x66 + (x58 + (x51 + (x45 + x20))));
    x80 = (x67 + (x59 + (x52 + (x46 + (x22 + x21)))));
    x81 = (x68 + (x60 + (x53 + (x25 + x23))));
    x82 = (x69 + (x61 + (x54 + (x29 + (x26 + x24)))));
    x83 = (x70 + (x62 + (x34 + (x30 + x27))));
    x84 = (x71 + (x63 + (x40 + (x35 + (x31 + x28)))));
    x85 = (x72 + (x47 + (x41 + (x36 + x32))));
    x86 = (x75 + x85);
    x87 = (x86 >> 25);
    x88 = (uint32_t) (x86 & UINT32_C(0x1ffffff));
    x89 = (x87 + x84);
    x90 = (x89 >> 26);
    x91 = (uint32_t) (x89 & UINT32_C(0x3ffffff));
    x92 = (x90 + x83);
    x93 = (x92 >> 25);
    x94 = (uint32_t) (x92 & UINT32_C(0x1ffffff));
    x95 = (x93 + x82);
    x96 = (x95 >> 26);
    x97 = (uint32_t) (x95 & UINT32_C(0x3ffffff));
    x98 = (x96 + x81);
    x99 = (x98 >> 25);
    x100 = (uint32_t) (x98 & UINT32_C(0x1ffffff));
    x101 = (x99 + x80);
    x102 = (x101 >> 26);
    x103 = (uint32_t) (x101 & UINT32_C(0x3ffffff));
    x104 = (x102 + x79);
    x105 = (x104 >> 25);
    x106 = (uint32_t) (x104 & UINT32_C(0x1ffffff));
    x107 = (x105 + x78);
    x108 = (x107 >> 26);
    x109 = (uint32_t) (x107 & UINT32_C(0x3ffffff));
    x110 = (x108 + x77);
    x111 = (x110 >> 25);
    x112 = (uint32_t) (x110 & UINT32_C(0x1ffffff));
    x113 = (x111 * UINT8_C(0x13));
    x114 = (x76 + x113);
    x115 = (uint32_t) (x114 >> 26);
    x116 = (uint32_t) (x114 & UINT32_C(0x3ffffff));
    x117 = (x115 + x88);
    x118 = (fiat_25519_uint1) (x117 >> 25);
    x119 = (x117 & UINT32_C(0x1ffffff));
    x120 = (x118 + x91);
    out1[0] = x116;
    out1[1] = x119;
    out1[2] = x120;
    out1[3] = x94;
    out1[4] = x97;
    out1[5] = x100;
    out1[6] = x103;
    out1[7] = x106;
    out1[8] = x109;
    out1[9] = x112;
}

/*
 * The function fiat_25519_carry reduces a field element.
 *
 * Postconditions:
 *   eval out1 mod m = eval arg1 mod m
 *
 */
static void
fiat_25519_carry(fiat_25519_tight_field_element out1, const fiat_25519_loose_field_element arg1) {
    uint32_t x1;
    uint32_t x2;
    uint32_t x3;
    uint32_t x4;
    uint32_t x5;
    uint32_t x6;
    uint32_t x7;
    uint32_t x8;
    uint32_t x9;
    uint32_t x10;
    uint32_t x11;
    uint32_t x12;
    uint32_t x13;
    uint32_t x14;
    uint32_t x15;
    uint32_t x16;
    uint32_t x17;
    uint32_t x18;
    uint32_t x19;
    uint32_t x20;
    uint32_t x21;
    uint32_t x22;
    x1 = (arg1[0]);
    x2 = ((x1 >> 26) + (arg1[1]));
    x3 = ((x2 >> 25) + (arg1[2]));
    x4 = ((x3 >> 26) + (arg1[3]));
    x5 = ((x4 >> 25) + (arg1[4]));
    x6 = ((x5 >> 26) + (arg1[5]));
    x7 = ((x6 >> 25) + (arg1[6]));
    x8 = ((x7 >> 26) + (arg1[7]));
    x9 = ((x8 >> 25) + (arg1[8]));
    x10 = ((x9 >> 26) + (arg1[9]));
    x11 = ((x1 & UINT32_C(0x3ffffff)) + ((x10 >> 25) * UINT8_C(0x13)));
    x12 = ((fiat_25519_uint1) (x11 >> 26) + (x2 & UINT32_C(0x1ffffff)));
    x13 = (x11 & UINT32_C(0x3ffffff));
    x14 = (x12 & UINT32_C(0x1ffffff));
    x15 = ((fiat_25519_uint1) (x12 >> 25) + (x3 & UINT32_C(0x3ffffff)));
    x16 = (x4 & UINT32_C(0x1ffffff));
    x17 = (x5 & UINT32_C(0x3ffffff));
    x18 = (x6 & UINT32_C(0x1ffffff));
    x19 = (x7 & UINT32_C(0x3ffffff));
    x20 = (x8 & UINT32_C(0x1ffffff));
    x21 = (x9 & UINT32_C(0x3ffffff));
    x22 = (x10 & UINT32_C(0x1ffffff));
    out1[0] = x13;
    out1[1] = x14;
    out1[2] = x15;
    out1[3] = x16;
    out1[4] = x17;
    out1[5] = x18;
    out1[6] = x19;
    out1[7] = x20;
    out1[8] = x21;
    out1[9] = x22;
}

/*
 * The function fiat_25519_add adds two field elements.
 *
 * Postconditions:
 *   eval out1 mod m = (eval arg1 + eval arg2) mod m
 *
 */
static void
fiat_25519_add(fiat_25519_loose_field_element out1, const fiat_25519_tight_field_element arg1,
               const fiat_25519_tight_field_element arg2) {
    uint32_t x1;
    uint32_t x2;
    uint32_t x3;
    uint32_t x4;
    uint32_t x5;
    uint32_t x6;
    uint32_t x7;
    uint32_t x8;
    uint32_t x9;
    uint32_t x10;
    x1 = ((arg1[0]) + (arg2[0]));
    x2 = ((arg1[1]) + (arg2[1]));
    x3 = ((arg1[2]) + (arg2[2]));
    x4 = ((arg1[3]) + (arg2[3]));
    x5 = ((arg1[4]) + (arg2[4]));
    x6 = ((arg1[5]) + (arg2[5]));
    x7 = ((arg1[6]) + (arg2[6]));
    x8 = ((arg1[7]) + (arg2[7]));
    x9 = ((arg1[8]) + (arg2[8]));
    x10 = ((arg1[9]) + (arg2[9]));
    out1[0] = x1;
    out1[1] = x2;
    out1[2] = x3;
    out1[3] = x4;
    out1[4] = x5;
    out1[5] = x6;
    out1[6] = x7;
    out1[7] = x8;
    out1[8] = x9;
    out1[9] = x10;
}

/*
 * The function fiat_25519_sub subtracts two field elements.
 *
 * Postconditions:
 *   eval out1 mod m = (eval arg1 - eval arg2) mod m
 *
 */
static void
fiat_25519_sub(fiat_25519_loose_field_element out1, const fiat_25519_tight_field_element arg1,
               const fiat_25519_tight_field_element arg2) {
    uint32_t x1;
    uint32_t x2;
    uint32_t x3;
    uint32_t x4;
    uint32_t x5;
    uint32_t x6;
    uint32_t x7;
    uint32_t x8;
    uint32_t x9;
    uint32_t x10;
    x1 = ((UINT32_C(0x7ffffda) + (arg1[0])) - (arg2[0]));
    x2 = ((UINT32_C(0x3fffffe) + (arg1[1])) - (arg2[1]));
    x3 = ((UINT32_C(0x7fffffe) + (arg1[2])) - (arg2[2]));
    x4 = ((UINT32_C(0x3fffffe) + (arg1[3])) - (arg2[3]));
    x5 = ((UINT32_C(0x7fffffe) + (arg1[4])) - (arg2[4]));
    x6 = ((UINT32_C(0x3fffffe) + (arg1[5])) - (arg2[5]));
    x7 = ((UINT32_C(0x7fffffe) + (arg1[6])) - (arg2[6]));
    x8 = ((UINT32_C(0x3fffffe) + (arg1[7])) - (arg2[7]));
    x9 = ((UINT32_C(0x7fffffe) + (arg1[8])) - (arg2[8]));
    x10 = ((UINT32_C(0x3fffffe) + (arg1[9])) - (arg2[9]));
    out1[0] = x1;
    out1[1] = x2;
    out1[2] = x3;
    out1[3] = x4;
    out1[4] = x5;
    out1[5] = x6;
    out1[6] = x7;
    out1[7] = x8;
    out1[8] = x9;
    out1[9] = x10;
}

/*
 * The function fiat_25519_opp negates a field element.
 *
 * Postconditions:
 *   eval out1 mod m = -eval arg1 mod m
 *
 */
static void
fiat_25519_opp(fiat_25519_loose_field_element out1, const fiat_25519_tight_field_element arg1) {
    uint32_t x1;
    uint32_t x2;
    uint32_t x3;
    uint32_t x4;
    uint32_t x5;
    uint32_t x6;
    uint32_t x7;
    uint32_t x8;
    uint32_t x9;
    uint32_t x10;
    x1 = (UINT32_C(0x7ffffda) - (arg1[0]));
    x2 = (UINT32_C(0x3fffffe) - (arg1[1]));
    x3 = (UINT32_C(0x7fffffe) - (arg1[2]));
    x4 = (UINT32_C(0x3fffffe) - (arg1[3]));
    x5 = (UINT32_C(0x7fffffe) - (arg1[4]));
    x6 = (UINT32_C(0x3fffffe) - (arg1[5]));
    x7 = (UINT32_C(0x7fffffe) - (arg1[6]));
    x8 = (UINT32_C(0x3fffffe) - (arg1[7]));
    x9 = (UINT32_C(0x7fffffe) - (arg1[8]));
    x10 = (UINT32_C(0x3fffffe) - (arg1[9]));
    out1[0] = x1;
    out1[1] = x2;
    out1[2] = x3;
    out1[3] = x4;
    out1[4] = x5;
    out1[5] = x6;
    out1[6] = x7;
    out1[7] = x8;
    out1[8] = x9;
    out1[9] = x10;
}

/*
 * The function fiat_25519_selectznz is a multi-limb conditional select.
 *
 * Postconditions:
 *   eval out1 = (if arg1 = 0 then eval arg2 else eval arg3)
 *
 * Input Bounds:
 *   arg1: [0x0 ~> 0x1]
 *   arg2: [[0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff]]
 *   arg3: [[0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff]]
 * Output Bounds:
 *   out1: [[0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff], [0x0 ~> 0xffffffff]]
 */
static void fiat_25519_selectznz(uint32_t out1[10], fiat_25519_uint1 arg1, const uint32_t arg2[10],
                                 const uint32_t arg3[10]) {
    uint32_t x1;
    uint32_t x2;
    uint32_t x3;
    uint32_t x4;
    uint32_t x5;
    uint32_t x6;
    uint32_t x7;
    uint32_t x8;
    uint32_t x9;
    uint32_t x10;
    fiat_25519_cmovznz_u32(&x1, arg1, (arg2[0]), (arg3[0]));
    fiat_25519_cmovznz_u32(&x2, arg1, (arg2[1]), (arg3[1]));
    fiat_25519_cmovznz_u32(&x3, arg1, (arg2[2]), (arg3[2]));
    fiat_25519_cmovznz_u32(&x4, arg1, (arg2[3]), (arg3[3]));
    fiat_25519_cmovznz_u32(&x5, arg1, (arg2[4]), (arg3[4]));
    fiat_25519_cmovznz_u32(&x6, arg1, (arg2[5]), (arg3[5]));
    fiat_25519_cmovznz_u32(&x7, arg1, (arg2[6]), (arg3[6]));
    fiat_25519_cmovznz_u32(&x8, arg1, (arg2[7]), (arg3[7]));
    fiat_25519_cmovznz_u32(&x9, arg1, (arg2[8]), (arg3[8]));
    fiat_25519_cmovznz_u32(&x10, arg1, (arg2[9]), (arg3[9]));
    out1[0] = x1;
    out1[1] = x2;
    out1[2] = x3;
    out1[3] = x4;
    out1[4] = x5;
    out1[5] = x6;
    out1[6] = x7;
    out1[7] = x8;
    out1[8] = x9;
    out1[9] = x10;
}

/*
 * The function fiat_25519_to_bytes serializes a field element to bytes in little-endian order.
 *
 * Postconditions:
 *   out1 = map (λ x, ⌊((eval arg1 mod m) mod 2^(8 * (x + 1))) / 2^(8 * x)⌋) [0..31]
 *
 * Output Bounds:
 *   out1: [[0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0x7f]]
 */
static void fiat_25519_to_bytes(uint8_t out1[32], const fiat_25519_tight_field_element arg1) {
    uint32_t x1;
    fiat_25519_uint1 x2;
    uint32_t x3;
    fiat_25519_uint1 x4;
    uint32_t x5;
    fiat_25519_uint1 x6;
    uint32_t x7;
    fiat_25519_uint1 x8;
    uint32_t x9;
    fiat_25519_uint1 x10;
    uint32_t x11;
    fiat_25519_uint1 x12;
    uint32_t x13;
    fiat_25519_uint1 x14;
    uint32_t x15;
    fiat_25519_uint1 x16;
    uint32_t x17;
    fiat_25519_uint1 x18;
    uint32_t x19;
    fiat_25519_uint1 x20;
    uint32_t x21;
    uint32_t x22;
    fiat_25519_uint1 x23;
    uint32_t x24;
    fiat_25519_uint1 x25;
    uint32_t x26;
    fiat_25519_uint1 x27;
    uint32_t x28;
    fiat_25519_uint1 x29;
    uint32_t x30;
    fiat_25519_uint1 x31;
    uint32_t x32;
    fiat_25519_uint1 x33;
    uint32_t x34;
    fiat_25519_uint1 x35;
    uint32_t x36;
    fiat_25519_uint1 x37;
    uint32_t x38;
    fiat_25519_uint1 x39;
    uint32_t x40;
    fiat_25519_uint1 x41;
    uint32_t x42;
    uint32_t x43;
    uint32_t x44;
    uint32_t x45;
    uint32_t x46;
    uint32_t x47;
    uint32_t x48;
    uint32_t x49;
    uint8_t x50;
    uint32_t x51;
    uint8_t x52;
    uint32_t x53;
    uint8_t x54;
    uint8_t x55;
    uint32_t x56;
    uint8_t x57;
    uint32_t x58;
    uint8_t x59;
    uint32_t x60;
    uint8_t x61;
    uint8_t x62;
    uint32_t x63;
    uint8_t x64;
    uint32_t x65;
    uint8_t x66;
    uint32_t x67;
    uint8_t x68;
    uint8_t x69;
    uint32_t x70;
    uint8_t x71;
    uint32_t x72;
    uint8_t x73;
    uint32_t x74;
    uint8_t x75;
    uint8_t x76;
    uint32_t x77;
    uint8_t x78;
    uint32_t x79;
    uint8_t x80;
    uint32_t x81;
    uint8_t x82;
    uint8_t x83;
    uint8_t x84;
    uint32_t x85;
    uint8_t x86;
    uint32_t x87;
    uint8_t x88;
    fiat_25519_uint1 x89;
    uint32_t x90;
    uint8_t x91;
    uint32_t x92;
    uint8_t x93;
    uint32_t x94;
    uint8_t x95;
    uint8_t x96;
    uint32_t x97;
    uint8_t x98;
    uint32_t x99;
    uint8_t x100;
    uint32_t x101;
    uint8_t x102;
    uint8_t x103;
    uint32_t x104;
    uint8_t x105;
    uint32_t x106;
    uint8_t x107;
    uint32_t x108;
    uint8_t x109;
    uint8_t x110;
    uint32_t x111;
    uint8_t x112;
    uint32_t x113;
    uint8_t x114;
    uint32_t x115;
    uint8_t x116;
    uint8_t x117;
    fiat_25519_subborrowx_u26(&x1, &x2, 0x0, (arg1[0]), UINT32_C(0x3ffffed));
    fiat_25519_subborrowx_u25(&x3, &x4, x2, (arg1[1]), UINT32_C(0x1ffffff));
    fiat_25519_subborrowx_u26(&x5, &x6, x4, (arg1[2]), UINT32_C(0x3ffffff));
    fiat_25519_subborrowx_u25(&x7, &x8, x6, (arg1[3]), UINT32_C(0x1ffffff));
    fiat_25519_subborrowx_u26(&x9, &x10, x8, (arg1[4]), UINT32_C(0x3ffffff));
    fiat_25519_subborrowx_u25(&x11, &x12, x10, (arg1[5]), UINT32_C(0x1ffffff));
    fiat_25519_subborrowx_u26(&x13, &x14, x12, (arg1[6]), UINT32_C(0x3ffffff));
    fiat_25519_subborrowx_u25(&x15, &x16, x14, (arg1[7]), UINT32_C(0x1ffffff));
    fiat_25519_subborrowx_u26(&x17, &x18, x16, (arg1[8]), UINT32_C(0x3ffffff));
    fiat_25519_subborrowx_u25(&x19, &x20, x18, (arg1[9]), UINT32_C(0x1ffffff));
    fiat_25519_cmovznz_u32(&x21, x20, 0x0, UINT32_C(0xffffffff));
    fiat_25519_addcarryx_u26(&x22, &x23, 0x0, x1, (x21 & UINT32_C(0x3ffffed)));
    fiat_25519_addcarryx_u25(&x24, &x25, x23, x3, (x21 & UINT32_C(0x1ffffff)));
    fiat_25519_addcarryx_u26(&x26, &x27, x25, x5, (x21 & UINT32_C(0x3ffffff)));
    fiat_25519_addcarryx_u25(&x28, &x29, x27, x7, (x21 & UINT32_C(0x1ffffff)));
    fiat_25519_addcarryx_u26(&x30, &x31, x29, x9, (x21 & UINT32_C(0x3ffffff)));
    fiat_25519_addcarryx_u25(&x32, &x33, x31, x11, (x21 & UINT32_C(0x1ffffff)));
    fiat_25519_addcarryx_u26(&x34, &x35, x33, x13, (x21 & UINT32_C(0x3ffffff)));
    fiat_25519_addcarryx_u25(&x36, &x37, x35, x15, (x21 & UINT32_C(0x1ffffff)));
    fiat_25519_addcarryx_u26(&x38, &x39, x37, x17, (x21 & UINT32_C(0x3ffffff)));
    fiat_25519_addcarryx_u25(&x40, &x41, x39, x19, (x21 & UINT32_C(0x1ffffff)));
    x42 = (x40 << 6);
    x43 = (x38 << 4);
    x44 = (x36 << 3);
    x45 = (x34 * (uint32_t) 0x2);
    x46 = (x30 << 6);
    x47 = (x28 << 5);
    x48 = (x26 << 3);
    x49 = (x24 << 2);
    x50 = (uint8_t) (x22 & UINT8_C(0xff));
    x51 = (x22 >> 8);
    x52 = (uint8_t) (x51 & UINT8_C(0xff));
    x53 = (x51 >> 8);
    x54 = (uint8_t) (x53 & UINT8_C(0xff));
    x55 = (uint8_t) (x53 >> 8);
    x56 = (x49 + (uint32_t) x55);
    x57 = (uint8_t) (x56 & UINT8_C(0xff));
    x58 = (x56 >> 8);
    x59 = (uint8_t) (x58 & UINT8_C(0xff));
    x60 = (x58 >> 8);
    x61 = (uint8_t) (x60 & UINT8_C(0xff));
    x62 = (uint8_t) (x60 >> 8);
    x63 = (x48 + (uint32_t) x62);
    x64 = (uint8_t) (x63 & UINT8_C(0xff));
    x65 = (x63 >> 8);
    x66 = (uint8_t) (x65 & UINT8_C(0xff));
    x67 = (x65 >> 8);
    x68 = (uint8_t) (x67 & UINT8_C(0xff));
    x69 = (uint8_t) (x67 >> 8);
    x70 = (x47 + (uint32_t) x69);
    x71 = (uint8_t) (x70 & UINT8_C(0xff));
    x72 = (x70 >> 8);
    x73 = (uint8_t) (x72 & UINT8_C(0xff));
    x74 = (x72 >> 8);
    x75 = (uint8_t) (x74 & UINT8_C(0xff));
    x76 = (uint8_t) (x74 >> 8);
    x77 = (x46 + (uint32_t) x76);
    x78 = (uint8_t) (x77 & UINT8_C(0xff));
    x79 = (x77 >> 8);
    x80 = (uint8_t) (x79 & UINT8_C(0xff));
    x81 = (x79 >> 8);
    x82 = (uint8_t) (x81 & UINT8_C(0xff));
    x83 = (uint8_t) (x81 >> 8);
    x84 = (uint8_t) (x32 & UINT8_C(0xff));
    x85 = (x32 >> 8);
    x86 = (uint8_t) (x85 & UINT8_C(0xff));
    x87 = (x85 >> 8);
    x88 = (uint8_t) (x87 & UINT8_C(0xff));
    x89 = (fiat_25519_uint1) (x87 >> 8);
    x90 = (x45 + (uint32_t) x89);
    x91 = (uint8_t) (x90 & UINT8_C(0xff));
    x92 = (x90 >> 8);
    x93 = (uint8_t) (x92 & UINT8_C(0xff));
    x94 = (x92 >> 8);
    x95 = (uint8_t) (x94 & UINT8_C(0xff));
    x96 = (uint8_t) (x94 >> 8);
    x97 = (x44 + (uint32_t) x96);
    x98 = (uint8_t) (x97 & UINT8_C(0xff));
    x99 = (x97 >> 8);
    x100 = (uint8_t) (x99 & UINT8_C(0xff));
    x101 = (x99 >> 8);
    x102 = (uint8_t) (x101 & UINT8_C(0xff));
    x103 = (uint8_t) (x101 >> 8);
    x104 = (x43 + (uint32_t) x103);
    x105 = (uint8_t) (x104 & UINT8_C(0xff));
    x106 = (x104 >> 8);
    x107 = (uint8_t) (x106 & UINT8_C(0xff));
    x108 = (x106 >> 8);
    x109 = (uint8_t) (x108 & UINT8_C(0xff));
    x110 = (uint8_t) (x108 >> 8);
    x111 = (x42 + (uint32_t) x110);
    x112 = (uint8_t) (x111 & UINT8_C(0xff));
    x113 = (x111 >> 8);
    x114 = (uint8_t) (x113 & UINT8_C(0xff));
    x115 = (x113 >> 8);
    x116 = (uint8_t) (x115 & UINT8_C(0xff));
    x117 = (uint8_t) (x115 >> 8);
    out1[0] = x50;
    out1[1] = x52;
    out1[2] = x54;
    out1[3] = x57;
    out1[4] = x59;
    out1[5] = x61;
    out1[6] = x64;
    out1[7] = x66;
    out1[8] = x68;
    out1[9] = x71;
    out1[10] = x73;
    out1[11] = x75;
    out1[12] = x78;
    out1[13] = x80;
    out1[14] = x82;
    out1[15] = x83;
    out1[16] = x84;
    out1[17] = x86;
    out1[18] = x88;
    out1[19] = x91;
    out1[20] = x93;
    out1[21] = x95;
    out1[22] = x98;
    out1[23] = x100;
    out1[24] = x102;
    out1[25] = x105;
    out1[26] = x107;
    out1[27] = x109;
    out1[28] = x112;
    out1[29] = x114;
    out1[30] = x116;
    out1[31] = x117;
}

/*
 * The function fiat_25519_from_bytes deserializes a field element from bytes in little-endian order.
 *
 * Postconditions:
 *   eval out1 mod m = bytes_eval arg1 mod m
 *
 * Input Bounds:
 *   arg1: [[0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0xff], [0x0 ~> 0x7f]]
 */
static void fiat_25519_from_bytes(fiat_25519_tight_field_element out1, const uint8_t arg1[32]) {
    uint32_t x1;
    uint32_t x2;
    uint32_t x3;
    uint32_t x4;
    uint32_t x5;
    uint32_t x6;
    uint32_t x7;
    uint32_t x8;
    uint32_t x9;
    uint32_t x10;
    uint32_t x11;
    uint32_t x12;
    uint32_t x13;
    uint32_t x14;
    uint32_t x15;
    uint8_t x16;
    uint32_t x17;
    uint32_t x18;
    uint32_t x19;
    uint32_t x20;
    uint32_t x21;
    uint32_t x22;
    uint32_t x23;
    uint32_t x24;
    uint32_t x25;
    uint32_t x26;
    uint32_t x27;
    uint32_t x28;
    uint32_t x29;
    uint32_t x30;
    uint32_t x31;
    uint8_t x32;
    uint32_t x33;
    uint32_t x34;
    uint32_t x35;
    uint32_t x36;
    uint8_t x37;
    uint32_t x38;
    uint32_t x39;
    uint32_t x40;
    uint32_t x41;
    uint8_t x42;
    uint32_t x43;
    uint32_t x44;
    uint32_t x45;
    uint32_t x46;
    uint8_t x47;
    uint32_t x48;
    uint32_t x49;
    uint32_t x50;
    uint32_t x51;
    uint8_t x52;
    uint32_t x53;
    uint32_t x54;
    uint32_t x55;
    uint32_t x56;
    uint32_t x57;
    uint32_t x58;
    uint32_t x59;
    uint8_t x60;
    uint32_t x61;
    uint32_t x62;
    uint32_t x63;
    uint32_t x64;
    uint8_t x65;
    uint32_t x66;
    uint32_t x67;
    uint32_t x68;
    uint32_t x69;
    uint8_t x70;
    uint32_t x71;
    uint32_t x72;
    uint32_t x73;
    uint32_t x74;
    uint8_t x75;
    uint32_t x76;
    uint32_t x77;
    uint32_t x78;
    x1 = ((uint32_t) (arg1[31]) << 18);
    x2 = ((uint32_t) (arg1[30]) << 10);
    x3 = ((uint32_t) (arg1[29]) << 2);
    x4 = ((uint32_t) (arg1[28]) << 20);
    x5 = ((uint32_t) (arg1[27]) << 12);
    x6 = ((uint32_t) (arg1[26]) << 4);
    x7 = ((uint32_t) (arg1[25]) << 21);
    x8 = ((uint32_t) (arg1[24]) << 13);
    x9 = ((uint32_t) (arg1[23]) << 5);
    x10 = ((uint32_t) (arg1[22]) << 23);
    x11 = ((uint32_t) (arg1[21]) << 15);
    x12 = ((uint32_t) (arg1[20]) << 7);
    x13 = ((uint32_t) (arg1[19]) << 24);
    x14 = ((uint32_t) (arg1[18]) << 16);
    x15 = ((uint32_t) (arg1[17]) << 8);
    x16 = (arg1[16]);
    x17 = ((uint32_t) (arg1[15]) << 18);
    x18 = ((uint32_t) (arg1[14]) << 10);
    x19 = ((uint32_t) (arg1[13]) << 2);
    x20 = ((uint32_t) (arg1[12]) << 19);
    x21 = ((uint32_t) (arg1[11]) << 11);
    x22 = ((uint32_t) (arg1[10]) << 3);
    x23 = ((uint32_t) (arg1[9]) << 21);
    x24 = ((uint32_t) (arg1[8]) << 13);
    x25 = ((uint32_t) (arg1[7]) << 5);
    x26 = ((uint32_t) (arg1[6]) << 22);
    x27 = ((uint32_t) (arg1[5]) << 14);
    x28 = ((uint32_t) (arg1[4]) << 6);
    x29 = ((uint32_t) (arg1[3]) << 24);
    x30 = ((uint32_t) (arg1[2]) << 16);
    x31 = ((uint32_t) (arg1[1]) << 8);
    x32 = (arg1[0]);
    x33 = (x31 + (uint32_t) x32);
    x34 = (x30 + x33);
    x35 = (x29 + x34);
    x36 = (x35 & UINT32_C(0x3ffffff));
    x37 = (uint8_t) (x35 >> 26);
    x38 = (x28 + (uint32_t) x37);
    x39 = (x27 + x38);
    x40 = (x26 + x39);
    x41 = (x40 & UINT32_C(0x1ffffff));
    x42 = (uint8_t) (x40 >> 25);
    x43 = (x25 + (uint32_t) x42);
    x44 = (x24 + x43);
    x45 = (x23 + x44);
    x46 = (x45 & UINT32_C(0x3ffffff));
    x47 = (uint8_t) (x45 >> 26);
    x48 = (x22 + (uint32_t) x47);
    x49 = (x21 + x48);
    x50 = (x20 + x49);
    x51 = (x50 & UINT32_C(0x1ffffff));
    x52 = (uint8_t) (x50 >> 25);
    x53 = (x19 + (uint32_t) x52);
    x54 = (x18 + x53);
    x55 = (x17 + x54);
    x56 = (x15 + (uint32_t) x16);
    x57 = (x14 + x56);
    x58 = (x13 + x57);
    x59 = (x58 & UINT32_C(0x1ffffff));
    x60 = (uint8_t) (x58 >> 25);
    x61 = (x12 + (uint32_t) x60);
    x62 = (x11 + x61);
    x63 = (x10 + x62);
    x64 = (x63 & UINT32_C(0x3ffffff));
    x65 = (uint8_t) (x63 >> 26);
    x66 = (x9 + (uint32_t) x65);
    x67 = (x8 + x66);
    x68 = (x7 + x67);
    x69 = (x68 & UINT32_C(0x1ffffff));
    x70 = (uint8_t) (x68 >> 25);
    x71 = (x6 + (uint32_t) x70);
    x72 = (x5 + x71);
    x73 = (x4 + x72);
    x74 = (x73 & UINT32_C(0x3ffffff));
    x75 = (uint8_t) (x73 >> 26);
    x76 = (x3 + (uint32_t) x75);
    x77 = (x2 + x76);
    x78 = (x1 + x77);
    out1[0] = x36;
    out1[1] = x41;
    out1[2] = x46;
    out1[3] = x51;
    out1[4] = x55;
    out1[5] = x59;
    out1[6] = x64;
    out1[7] = x69;
    out1[8] = x74;
    out1[9] = x78;
}

/*
 * The function fiat_25519_relax is the identity function converting from tight field elements to loose field elements.
 *
 * Postconditions:
 *   out1 = arg1
 *
 */
static void
fiat_25519_relax(fiat_25519_loose_field_element out1, const fiat_25519_tight_field_element arg1) {
    uint32_t x1;
    uint32_t x2;
    uint32_t x3;
    uint32_t x4;
    uint32_t x5;
    uint32_t x6;
    uint32_t x7;
    uint32_t x8;
    uint32_t x9;
    uint32_t x10;
    x1 = (arg1[0]);
    x2 = (arg1[1]);
    x3 = (arg1[2]);
    x4 = (arg1[3]);
    x5 = (arg1[4]);
    x6 = (arg1[5]);
    x7 = (arg1[6]);
    x8 = (arg1[7]);
    x9 = (arg1[8]);
    x10 = (arg1[9]);
    out1[0] = x1;
    out1[1] = x2;
    out1[2] = x3;
    out1[3] = x4;
    out1[4] = x5;
    out1[5] = x6;
    out1[6] = x7;
    out1[7] = x8;
    out1[8] = x9;
    out1[9] = x10;
}

/*
 * The function fiat_25519_carry_scmul_121666 multiplies a field element by 121666 and reduces the result.
 *
 * Postconditions:
 *   eval out1 mod m = (121666 * eval arg1) mod m
 *
 */
static void fiat_25519_carry_scmul_121666(fiat_25519_tight_field_element out1,
                                          const fiat_25519_loose_field_element arg1) {
    uint64_t x1;
    uint64_t x2;
    uint64_t x3;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
    uint64_t x8;
    uint64_t x9;
    uint64_t x10;
    uint32_t x11;
    uint32_t x12;
    uint64_t x13;
    uint32_t x14;
    uint32_t x15;
    uint64_t x16;
    uint32_t x17;
    uint32_t x18;
    uint64_t x19;
    uint32_t x20;
    uint32_t x21;
    uint64_t x22;
    uint32_t x23;
    uint32_t x24;
    uint64_t x25;
    uint32_t x26;
    uint32_t x27;
    uint64_t x28;
    uint32_t x29;
    uint32_t x30;
    uint64_t x31;
    uint32_t x32;
    uint32_t x33;
    uint64_t x34;
    uint32_t x35;
    uint32_t x36;
    uint64_t x37;
    uint32_t x38;
    uint32_t x39;
    uint32_t x40;
    uint32_t x41;
    fiat_25519_uint1 x42;
    uint32_t x43;
    uint32_t x44;
    fiat_25519_uint1 x45;
    uint32_t x46;
    uint32_t x47;
    x1 = ((uint64_t) UINT32_C(0x1db42) * (arg1[9]));
    x2 = ((uint64_t) UINT32_C(0x1db42) * (arg1[8]));
    x3 = ((uint64_t) UINT32_C(0x1db42) * (arg1[7]));
    x4 = ((uint64_t) UINT32_C(0x1db42) * (arg1[6]));
    x5 = ((uint64_t) UINT32_C(0x1db42) * (arg1[5]));
    x6 = ((uint64_t) UINT32_C(0x1db42) * (arg1[4]));
    x7 = ((uint64_t) UINT32_C(0x1db42) * (arg1[3]));
    x8 = ((uint64_t) UINT32_C(0x1db42) * (arg1[2]));
    x9 = ((uint64_t) UINT32_C(0x1db42) * (arg1[1]));
    x10 = ((uint64_t) UINT32_C(0x1db42) * (arg1[0]));
    x11 = (uint32_t) (x10 >> 26);
    x12 = (uint32_t) (x10 & UINT32_C(0x3ffffff));
    x13 = (x11 + x9);
    x14 = (uint32_t) (x13 >> 25);
    x15 = (uint32_t) (x13 & UINT32_C(0x1ffffff));
    x16 = (x14 + x8);
    x17 = (uint32_t) (x16 >> 26);
    x18 = (uint32_t) (x16 & UINT32_C(0x3ffffff));
    x19 = (x17 + x7);
    x20 = (uint32_t) (x19 >> 25);
    x21 = (uint32_t) (x19 & UINT32_C(0x1ffffff));
    x22 = (x20 + x6);
    x23 = (uint32_t) (x22 >> 26);
    x24 = (uint32_t) (x22 & UINT32_C(0x3ffffff));
    x25 = (x23 + x5);
    x26 = (uint32_t) (x25 >> 25);
    x27 = (uint32_t) (x25 & UINT32_C(0x1ffffff));
    x28 = (x26 + x4);
    x29 = (uint32_t) (x28 >> 26);
    x30 = (uint32_t) (x28 & UINT32_C(0x3ffffff));
    x31 = (x29 + x3);
    x32 = (uint32_t) (x31 >> 25);
    x33 = (uint32_t) (x31 & UINT32_C(0x1ffffff));
    x34 = (x32 + x2);
    x35 = (uint32_t) (x34 >> 26);
    x36 = (uint32_t) (x34 & UINT32_C(0x3ffffff));
    x37 = (x35 + x1);
    x38 = (uint32_t) (x37 >> 25);
    x39 = (uint32_t) (x37 & UINT32_C(0x1ffffff));
    x40 = (x38 * UINT8_C(0x13));
    x41 = (x12 + x40);
    x42 = (fiat_25519_uint1) (x41 >> 26);
    x43 = (x41 & UINT32_C(0x3ffffff));
    x44 = (x42 + x15);
    x45 = (fiat_25519_uint1) (x44 >> 25);
    x46 = (x44 & UINT32_C(0x1ffffff));
    x47 = (x45 + x18);
    out1[0] = x43;
    out1[1] = x46;
    out1[2] = x47;
    out1[3] = x21;
    out1[4] = x24;
    out1[5] = x27;
    out1[6] = x30;
    out1[7] = x33;
    out1[8] = x36;
    out1[9] = x39;
}
