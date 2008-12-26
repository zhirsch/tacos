/*****************************************************************************
 * string.c
 *****************************************************************************/

#include <lib/string.h>
#include <tacos/types.h>

char *strncpy(char *dest, const char *src, size_t n)
{
   char *p = dest;

   while (n > 0 && *src != '\0')
   {
      *(dest++) = *(src++);
      n--;
   }

   while (n-- > 0)
      *(dest++) = '\0';

   return p;
}

void *memcpy(void *dest, const void *src, size_t n)
{
   for (size_t i = 0; i < n; i++)
      ((uint8_t *)dest)[i] = ((uint8_t *)src)[i];
   return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
   /* XXX: This isn't actually correct */
   return memcpy(dest, src, n);
}

void *memset(void *s, int c, size_t n)
{
   for (size_t i = 0; i < n; i++)
      ((uint8_t *)s)[i] = c;
   return s;
}

void *memsetw(void *s, int c, size_t n)
{
   for (size_t i = 0; i < n; i++)
      ((uint16_t *)s)[i] = c;
   return s;
}

size_t strlen(const char *str)
{
   int count = 0;
   while (*(str++) != '\0')
      count++;
   return count;
}
