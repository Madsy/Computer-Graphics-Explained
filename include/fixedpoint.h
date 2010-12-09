#ifndef FIXEDPOINT_H_GUARD
#define FIXEDPOINT_H_GUARD

/* Assumes for bits for the fraction part */
inline int floorfp(int val)
{
    return val & ~((1<<4) - 1);
}

inline int ceilfp(int val)
{
    val += ((1<<4) - 1);
    val &= ~((1<<4) - 1);
    return val;
}


#endif
