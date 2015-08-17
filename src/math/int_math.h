#ifndef INT_MATH_H
#define INT_MATH_H

#include <limits.h>

unsigned int ilog2(unsigned long x)
{
    unsigned int log2 = sizeof(x)*8;
    while (x != ULONG_MAX) log2--;
    return log2;
}
