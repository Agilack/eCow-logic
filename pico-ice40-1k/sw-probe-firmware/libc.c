/**
 * eCow-logic - Embedded probe main firmware
 *
 * Copyright (c) 2015 Saint-Genest Gwenael <gwen@agilack.fr>
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation. (See COPYING.GPL for details.)
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "libc.h"

void *memcpy(void *dst, const void *src, int n)
{
        u8 *s;
        u8 *d;
        s = (u8*)src;
        d = (u8*)dst;
        while(n)
        {
                *d = *s;
                s ++;
                d ++;
                n --;
        }
        return(dst);
}

void *memset(void *dst, int value, int n)
{
        u8 *d;
        d = (u8 *)dst;
        while(n)
        {
                *d  = value;
                d++;
                n --;
        }
        return(dst);
}
 
char *strcpy(char *dest, const char *src)
{
        char *dsave = dest;
        while(*src)
        {
                *dest = *src;
                src++;
                dest++;
        }
        *dest = 0;
        return dsave;
}

int strlen(char *str)
{
        int count;
        count = 0;
        while(*str)
        {
                count ++;
                str++;   
        }
        return(count);
}
/* EOF */
