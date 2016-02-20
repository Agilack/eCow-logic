/**
 * eCow-logic - Bootloader
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

void msleep(u32 milliseconds)
{
  int i;
  
  i = 400 * milliseconds;
  for ( ; i > 0; i--)
    __asm volatile ("nop");
}
 
char * strcat (char *dest, const char *src)
{
        char *s1 = dest;
	const char *s2 = src;
	char c;
	/* Find the end of the string. */
	do
		c = *s1++;
	while (c != '\0');
	/* Make S1 point before the next character, so we can increment
	it while memory is read (wins on pipelined cpus). */
	s1 -= 2;
	do
	{
		c = *s2++;
		*++s1 = c;
	}
	while (c != '\0');
	return dest;
}

char *strchr(const char *s, int c)
{
        char *ptr;
        
        for (ptr = (char *)s; ptr != 0; ptr++)
        {
                if (*ptr == 0)
                        break;
                if (*ptr == c)
                        return ptr;
        }
        return 0;
}

int   strcmp(const char *p1, const char *p2)
{
	register const unsigned char *s1 = (const unsigned char *) p1;
	register const unsigned char *s2 = (const unsigned char *) p2;
	unsigned char c1, c2;

	do
	{
		c1 = (unsigned char) *s1++;
		c2 = (unsigned char) *s2++;
		if (c1 == '\0')
			return c1 - c2;
	}
	while (c1 == c2);
	return c1 - c2;
}
 
int  strncmp(const char *p1, const char *p2, int len)
{
	register const unsigned char *s1 = (const unsigned char *) p1;
	register const unsigned char *s2 = (const unsigned char *) p2;
	unsigned char c1, c2;

	do
	{
	        if (len == 0)
	                return 0;
                
		c1 = (unsigned char) *s1++;
		c2 = (unsigned char) *s2++;
		if (c1 == '\0')
			return c1 - c2;
                len --;
	}
	while (c1 == c2);
	return c1 - c2;
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

int strlen(const char *str)
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
