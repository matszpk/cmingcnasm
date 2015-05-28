#ifndef ULINUX_UTILS_ASCII_STRING_STRING_H
#define ULINUX_UTILS_ASCII_STRING_STRING_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/

#define ulinux_loop while(1)

/*
strncmp - Compare two length-limited strings
@cs: One string
@ct: Another string
@n: The maximum number of bytes to compare
*/
static inline ulinux_s8 ulinux_strncmp(ulinux_u8 *cs,ulinux_u8 *ct,ulinux_u64 n)
{
  ulinux_u8 c1,c2;
  ulinux_loop{
    if(n==0) break;

    c1=*cs++;
    c2=*ct++;
    if(c1!=c2) return c1<c2?-1:1;
    if(!c1) break;
    n--;
  }
  return 0;
}

/*use carefully*/
static inline ulinux_s8 ulinux_strcmp(ulinux_u8 *cs,ulinux_u8 *ct)
{
  ulinux_u8 c1,c2;
  ulinux_loop{
    c1=*cs++;
    c2=*ct++;
    if(c1!=c2) return c1<c2?-1:1;
    if(!c1) break;
  }
  return 0;
}

/*use carefully*/
static inline void ulinux_strcpy(ulinux_u8 *dest,ulinux_u8 *src)
{
  ulinux_loop{
    *dest++=*src;
    if(*src++==0) break;
  }
}

static inline void ulinux_strncpy(ulinux_u8 *d,ulinux_u8 *src,ulinux_u64 n)
{
  ulinux_loop{
    if(n&&*src==0) break;

    *d++=*src++;
    n--;
  }
  ulinux_loop{
    if(n==0) break;

    *d++=0;
    n--;
  }
}

static inline ulinux_u8 ulinux_strnlen(ulinux_u64 *sz,ulinux_u8 *start,
                                                                 ulinux_u64 max)
{
  ulinux_u8 *end;

  end=start;
  ulinux_loop{
    if(max--&&*end==0) break;
    ++end;
  }
  if(*end==0){
    *sz=end-start;
    return 1;
  }
  return 0;
}

/*use carefully*/
static inline ulinux_u64 ulinux_strlen(ulinux_u8 *start)
{
  ulinux_u8 *end;

  end=start;
  ulinux_loop{
    if(*end==0) break;
    ++end;
  }
  return end-start;
}

/*use carefully*/
static inline ulinux_u8 *ulinux_strcat(ulinux_u8 *dest,ulinux_u8 *src)
{
  ulinux_u8 *tmp=dest;

  ulinux_loop{
    if(*dest==0) break;
    ++dest;
  }
  ulinux_loop{
    if((*dest++=*src++)=='\0') break;
  }
  return tmp;
}

/*use carefully*/
static inline ulinux_u8 *ulinux_strstr(ulinux_u8 *haystack,ulinux_u8 *needle)
{
  ulinux_u64 haystack_rem_len,needle_len;

  needle_len = ulinux_strlen(needle);
  if(!needle_len) return haystack;
  haystack_rem_len = ulinux_strlen(haystack);
  ulinux_loop{
    if(haystack_rem_len<needle_len) break;
    haystack_rem_len--;
    if(!ulinux_memcmp(haystack,needle,needle_len)) return haystack;
    ++haystack;
  }
  return 0;
}
#undef ulinux_loop
#endif
