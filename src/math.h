#ifndef __MATH_H__
#define __MATH_H__

static int divide(int num, int denom) {
    int quotient = 0;
    while (num >= denom) {
        num -= denom;
        quotient++;
    }
    return quotient;
}

static int mod(int num, int denom) {
    while (num >= denom) {
        num -= denom;
    }
    if (num != 0) {
        return denom - num;
    }
    return 0;
}

#endif