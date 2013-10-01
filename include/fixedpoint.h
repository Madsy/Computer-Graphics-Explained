/*
Copyright (c) 2013, Mads Andreas Elvheim
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*/

#ifndef FIXEDPOINT_H_GUARD
#define FIXEDPOINT_H_GUARD

/* Assumes for bits for the fraction part */
inline int floorfp(int val)
{
    return val & ~((1<<16) - 1);
}

inline int ceilfp(int val)
{
    val += ((1<<16) - 1);
    val &= ~((1<<16) - 1);
    return val;
}


#endif
