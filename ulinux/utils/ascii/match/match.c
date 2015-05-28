/*******************************************************************************
This is a derived work based on GNU glibc implementation on 20130409.
Switch to GNU Lesser GPLv3 protection.
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/


/*
NOTE:do *NOT* even try to read that code without ksh extended glob
specifications, and keeping an eye on the abbreviations below

upstream code is hard to grasp
*/

#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <ulinux/sysc.h>
#include <ulinux/error.h>

#include <ulinux/utils/ascii/ascii.h>
#include <ulinux/utils/ascii/string/string.h>
#include <ulinux/utils/ascii/match/match.h>
#include <ulinux/mmap.h>
#include <ulinux/utils/mem.h>

/*----------------------------------------------------------------------------*/
/*ulinux namespace*/
#define loop while(1)
#define ANONYMOUS ULINUX_MAP_ANONYMOUS
#define CASEFOLD ULINUX_MATCH_CASEFOLD
#define ERR ULINUX_MATCH_ERR
#define ERR_NOMEM ULINUX_MATCH_ERR_NOMEM
#define EXTMATCH ULINUX_MATCH_EXTMATCH
#define is_alnum ulinux_is_alnum
#define is_alpha ulinux_is_alpha
#define is_blank ulinux_is_blank
#define is_cntrl ulinux_is_cntrl
#define is_digit ulinux_is_digit
#define is_graph ulinux_is_graph
#define is_lower ulinux_is_lower
#define is_print ulinux_is_print
#define is_punct ulinux_is_punct
#define is_space ulinux_is_space
#define is_upper ulinux_is_upper
#define is_xdigit ulinux_is_xdigit
#define ISERR ULINUX_ISERR
#define sl ulinux_sl
#define MATCH    ULINUX_MATCH_MATCH
#define memchr ulinux_memchr
#define memcpy ulinux_memcpy
#define mmap(a,b,c) ulinux_sysc(mmap,6,0,a,b,c,-1,0)
#define mremap(a,b,c) ulinux_sysc(mremap,4,a,b,c,0)
#define munmap(a,b) ulinux_sysc(munmap,2,a,b)
#define	NOMATCH  ULINUX_MATCH_NOMATCH
#define PRIVATE ULINUX_MAP_PRIVATE
#define RD ULINUX_PROT_READ
#define s64 ulinux_s64
#define s8 ulinux_s8
#define u16 ulinux_u16
#define u64 ulinux_u64
#define u8 ulinux_u8
#define strcat ulinux_strcat
#define strcmp ulinux_strcmp
#define strlen ulinux_strlen
#define WR ULINUX_PROT_WRITE
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
ABBREVIATIONS
addr    address
c       character
char(s) character(s)
cur     current
ep(s)   extended pattern(s)
err     error
nomem   no memory
of      offset
p(s)    pattern(s)
params  parameters
pc      pattern char
r       return value
rd      read
rem     remainder
sc      string char
str     string
sz      size
wr      write
------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#define CHAR_CLASS_MAX (sizeof("xdigit")-1)

#define STREQ(s1,s2) (strcmp(s1,s2)==0)
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*return values*/
/*#define ULINUX_MATCH_ERR_NOMEM -3*/
/*#define ULINUX_MATCH_ERR       -2*/
#define NO_VALID_P               -1
/*#define ULINUX_MATCH_MATCH      0*/
#define OK                        0
/*#define ULINUX_MATCH_NOMATCH    1*/
/*----------------------------------------------------------------------------*/

/*
In the context a match call, it's used to store the location of the
"ending" * following, at the same depth, a "starting" *.
The matcher is locked in the star_handler between 2 stars at the
same depth.
*/
struct match_params
{
  void *p;
  void *str;
};

static u8 fold(u8 c,u8 flgs)
{
  if(!(flgs&CASEFOLD)) return c;
  return ulinux_2lower(c);
}

//Recursively skip a p. Return a pointer right after it, or a pointer on the
//start of the p to skip if the 0 terminating char is encountered before the
//end of the p.
static u8 *end_p(u8 *p)
{
  u8 *pc=p;
  loop
    if(*++pc=='\0') return p;//this is an invalid p
    else if(*pc=='['){//handle brackets special
      //Skip the not sign. We have to recognize it because of a possibly
      //following ']'.
      if(*++pc=='!'||(*pc=='^')) ++pc;
      if(*pc==']') ++pc;//a leading ']' is recognized as such
      ///skip over all chars of the list
      while(1){
        if(*pc==']') break;
        if(*pc++=='\0') return p;//this is no valid p
      }
    }else if((*pc=='?'||*pc=='*'||*pc=='+'||*pc=='@'||*pc=='!')&&pc[1]=='(')
      pc=end_p(pc+1);
    else if(*pc==')') break;
  return pc+1;
}

static s8 extended_match_try(u8 type,u8 *p,u8 *str,u8 *str_end, u8 flgs);
static s8 match(u8 *p,u8 *str,u8 *str_end,u8 flgs,
                                                 struct match_params *end_star);

/*----------------------------------------------------------------------------*/
/*return value for the following handlers*/
#define BIT(x)               (1<<x)
#define R_GET(r)             ((s8)(r&0xff))
#define R_SET(r)             ((u16)(r&0xff))
#define RETURN_R             BIT(9)
#define MATCH_NORMAL         BIT(10)
#define NEXT_STR_CHAR        BIT(11)
#define NEXT_P_CHAR          BIT(12)
#define TEST_STR_END_REACHED BIT(13)
/*----------------------------------------------------------------------------*/

static u16 star_handler(u8 *c,u8 **pc,u8 **sc,u8 *str_end,u8 flgs,
                                                  struct match_params *end_star)
{
  if(flgs&EXTMATCH&&**pc=='('){
    s8 r=extended_match_try(*c,*pc,*sc,str_end,flgs);
    if(r!=NO_VALID_P) return RETURN_R|R_SET(r);
  }else if(end_star){/*this star will exit the current star context matching*/
    end_star->p=*pc-1;
    end_star->str=*sc;
    return RETURN_R|R_SET(MATCH);
  }

  loop{
    *c=*(*pc)++;

    /*do "absorb" in the current * the following sequence of ?,*,?()
      and *()*/
    if(*c!='?'&&*c!='*') break;

    /*?() and *() are "zero or one|more" occurences of matched patterns,
      then absorbed them in the **/
    if(**pc=='('&&(flgs&EXTMATCH)){
      u8 *endp=end_p(*pc);
      if(endp!=*pc){/*This is a p. Skip over it*/
        *pc=endp;
        continue;
      }
    }

    if(*c=='?'){/*match one any char from the string*/
      /*a ? needs to match one char*/
      if(*sc==str_end)/*there isn't another char; no match*/
        return RETURN_R|R_SET(NOMATCH);
      else
        /*one char of the str is consumed in matching this ? wildcard, so
          *??? won't match if there are less than three chars*/
        ++(*sc);
    }
  }

  /*the wildcard(s) is/are the last element of the p flag is set*/
  if(*c=='\0') return RETURN_R|R_SET(MATCH);

  struct match_params local_end_star;
  local_end_star.p=0;

  if(*c=='['||(flgs&EXTMATCH&&(*c=='@'||*c=='+'||*c=='!')&&**pc=='(')){
    /*the * is followed by a bracket or an "not absorbed" ep*/
    (*pc)--;
    /*See explanation of star context matching right below. Instead of a char
      like below, it's an ep.*/
    loop{
      if(*sc==str_end) break;
      s8 r=match(*pc,*sc,str_end,flgs,&local_end_star);
      if(r==MATCH){
        if(local_end_star.p==0) return RETURN_R|R_SET(MATCH);
        break;
      }
      ++(*sc);
    }
  }else{/*the * is followed by a "normal" char*/
    if(*c=='\\') *c=**pc;
    *c=fold(*c,flgs);
    (*pc)--;
    /*scan the str till we find a char which is the same than the first
      p char right after the *, then start to match in star context, namely
      till we reach the end, or we find another * will will end the matching
      in that context*/
    loop{
      if(*sc==str_end) break;
      if(fold(**sc,flgs)==*c){
        s8 r=match(*pc,*sc,str_end,flgs,&local_end_star);
        if(r==MATCH){
          if(local_end_star.p==0) return RETURN_R|R_SET(MATCH);
          break;
        }
      }
      ++(*sc);
    }
    if(local_end_star.p!=0){/*we are finish to match the str in star context*/
      *pc=local_end_star.p;
      *sc=local_end_star.str;
      return NEXT_P_CHAR;
    }
  }
  /*if we come here no match is possible with the wildcard*/
  return RETURN_R|R_SET(NOMATCH);
}

static u16 bracket_skip(u8 *c,u8 **pc)
{
  while(1){
ignore_next:
    *c=*(*pc)++;

    if(*c=='\0') return RETURN_R|R_SET(NOMATCH);/*[... (unterminated) loses*/

    if(*c=='\\'){
      if(**pc=='\0') return RETURN_R|R_SET(NOMATCH);
      /*XXX 1003.2d11 is unclear if this is right*/
      ++(*pc);
    }else if(*c=='['&&**pc==':'){
      u8 char_class_n=0;
      u8 *char_class_start_pc=*pc;

      loop{
        *c=*++(*pc);
        if(++char_class_n==CHAR_CLASS_MAX+1) return RETURN_R|R_SET(NOMATCH);

        if(**pc==':'&&(*pc)[1]==']') break;

        if(*c<'a'||*c>='z'){/*is_lower(c) && is_weaker_alpha(c)*/
          *pc=char_class_start_pc;
          goto ignore_next;
        }
      }
      (*pc)+=2;
      *c=*(*pc)++;
    }
    if(*c==']') break; 
  }
  return OK;
}

static u16 bracket_matched(u8 *c,u8 **pc,u8 not)
{
  u16 r=bracket_skip(c,pc);
  if(r!=OK) return r;

  if(not) return RETURN_R|R_SET(NOMATCH);
  return NEXT_STR_CHAR;
}

/*input:*c may not be folded*/
static u16 bracket_normal(u8 *c,u8 **pc,u8 not,u8 sc_folded)
{
  u8 is_range=(**pc=='-'&&(*pc)[1]!='\0'&&(*pc)[1]!=']');

  if(!is_range&&*c==sc_folded) return bracket_matched(c,pc,not);

  u8 c_old=*c;
  *c=*(*pc)++;

  if(*c=='-'&&**pc!=']'){
    u8 c_end=*(*pc)++;

    if(c_end=='\\') c_end=*(*pc)++;
    if(c_end=='\0') return RETURN_R|R_SET(NOMATCH);

    /*it is a range, ascii collation*/
    if(c_old<=sc_folded&&sc_folded<=c_end) return bracket_matched(c,pc,not);
    *c=*(*pc)++;
  }
  return OK;
}

static u16 bracket_char_class_get(u8 *c,u8 **pc,u8 not,u8 sc_folded,
                                                                 u8 *char_class)
{
  u8 char_class_n=0;
  u8 *char_class_start_pc=*pc;

  loop{
    if(char_class_n==CHAR_CLASS_MAX+1) return RETURN_R|R_SET(NOMATCH);

    *c=*++(*pc);
    if(*c==':'&&(*pc)[1]==']'){
      (*pc)+=2;
      break;
    }
    if(*c<'a'||*c>='z'){/*is_lower(c) && is_weaker_alpha(c)*/
      /*This cannot possibly be a char class name. Rewind and try to match
        it as a normal range.*/
      *pc=char_class_start_pc;
      *c='[';
      u16 r=bracket_normal(c,pc,not,sc_folded);
      if(r!=OK) return r;
    }
    char_class[char_class_n++]=*c;
  }
  char_class[char_class_n]='\0';
  return OK;
}

static u16 bracket_escape(u8 *c,u8 **pc,u8 flgs,u8 not,u8 sc_folded)
{
  if(**pc=='\0') return RETURN_R|R_SET(NOMATCH);
  *c=fold(**pc,flgs);
  ++(*pc);
  u16 r=bracket_normal(c,pc,not,sc_folded);
  if(r!=OK) return r;
  return OK;
} 

/*input: *c=='[' **pc==':'*/
static u16 bracket_class(u8 *c,u8 **pc,u8 **sc,u8 not,u8 sc_folded)
{
  u8 char_class[CHAR_CLASS_MAX+1];/*don't forget the 0 terminating char*/

  u16 r=bracket_char_class_get(c,pc,not,sc_folded,&char_class[0]);
  if(r!=OK) return r;

  if((STREQ(char_class,"alnum")&&is_alnum(**sc))
     ||(STREQ(char_class,"alpha")&&is_alpha(**sc))
     ||(STREQ(char_class,"blank")&&is_blank(**sc))
     ||(STREQ(char_class,"cntrl")&&is_cntrl(**sc))
     ||(STREQ(char_class,"digit")&&is_digit(**sc))
     ||(STREQ(char_class,"graph")&&is_graph(**sc))
     ||(STREQ(char_class,"lower")&&is_lower(**sc))
     ||(STREQ(char_class,"print")&&is_print(**sc))
     ||(STREQ(char_class,"punct")&&is_punct(**sc))
     ||(STREQ(char_class,"space")&&is_space(**sc))
     ||(STREQ(char_class,"upper")&&is_upper(**sc))
     ||(STREQ(char_class,"xdigit")&&is_xdigit(**sc)))
    return bracket_matched(c,pc,not);
  *c=*(*pc)++;
  return OK;
}

static u16 bracket_handler(u8 *c,u8 **pc,u8 **sc,void *str_end,u8 flgs)
{
  u8 *pc_init=*pc;
  if(*sc==str_end) return RETURN_R|R_SET(NOMATCH);

  u8 not=(**pc=='!'||**pc=='^');
  if(not) ++(*pc);

  u8 sc_folded=fold(**sc,flgs);

  *c=*(*pc)++;
  while(1){
    if(*c=='\\'){
      u16 r=bracket_escape(c,pc,flgs,not,sc_folded);
      if(r!=OK) return r;
    }else if(*c=='['&&**pc==':'){
      u16 r=bracket_class(c,pc,sc,not,sc_folded);
      if(r!=OK) return r;
    }else if(c=='\0'){/*[ unterminated, rewind and tell above to match normal*/
      *pc=pc_init;
      *c='[';
      return MATCH_NORMAL;
    }else{
      *c=fold(*c,flgs);
      u16 r=bracket_normal(c,pc,not,sc_folded);
      if(r!=0) return r; 
    }
    if(*c==']') break;
  }
  if(!not) return RETURN_R|R_SET(NOMATCH);
  return NEXT_STR_CHAR;
}

static u16 question_mark_handler(u8 c,u8 *pc,u8 *sc,void *str_end,u8 flgs)
{
  if(flgs&EXTMATCH&&*pc=='('){
    s8 r=extended_match_try(c,pc,sc,str_end,flgs);
    if(r!=NO_VALID_P) return RETURN_R|R_SET(r);
  }
  return TEST_STR_END_REACHED;
}

static u16 misc_handler(u8 c,u8 *pc,u8 *sc,void *str_end,u8 flgs)
{
  if(flgs&EXTMATCH&&*pc=='('){
    s8 r=extended_match_try(c,pc,sc,str_end,flgs);
    if(r!=NO_VALID_P) return RETURN_R|R_SET(r);
  }
  return MATCH_NORMAL;
}

static u16 escape_handler(u8 *c,u8 **pc,u8 flgs)
{
  u16 r=MATCH_NORMAL;
  *c=*(*pc)++;
  if(c=='\0') return r|RETURN_R|R_SET(NOMATCH);/*railing \ loses*/
  *c=fold(*c,flgs);
  return r;
}

static u16 special_char(u8 *c,u8 **pc,u8 **sc,void *str_end,u8 flgs,
                                                  struct match_params *end_star)
{
  u16 r=RETURN_R|R_SET(ERR);
  switch(*c){
  case '[':
    r=bracket_handler(c,pc,sc,str_end,flgs);
    break;
  case '*':
    r=star_handler(c,pc,sc,str_end,flgs,end_star);
    break;
  case '?':
    r=question_mark_handler(*c,*pc,*sc,str_end,flgs);
    break;
  case '+':
  case '@':
  case '!':
    r=misc_handler(*c,*pc,*sc,str_end,flgs);
    break;
  case '\\':
    r=escape_handler(c,pc,flgs);
    break;
  }
  return r;
}

/*str_end points right after the last char of str, the '\0' char*/
static s8 match(u8 *p,u8 *str,u8 *str_end,u8 flgs,struct match_params *end_star)
{
  u8 *pc=p;
  u8 *sc=str;
  u8 c;

  loop{
    c=*pc++;
    if(c=='\0') break;

    c=fold(c,flgs);

    u16 r;
    switch(c){
    case '[': case '*': case '?': case '+': case '@': case '!': case '\\':
      r=special_char(&c,&pc,&sc,str_end,flgs,end_star);
      break;
    default:
      r=MATCH_NORMAL;
    }

    if(!(r&NEXT_STR_CHAR)){
     if(r&RETURN_R) return R_GET(r);
     if(r&NEXT_P_CHAR) continue;
     if((r&MATCH_NORMAL)||(r&TEST_STR_END_REACHED))
       if(sc==str_end) return NOMATCH;
     if(r&MATCH_NORMAL)
       if(c!=fold(*sc,flgs)) return NOMATCH;
    }

    ++sc;
  }

  if(sc==str_end) return MATCH;
  return NOMATCH;
}

/*============================================================================*/
/*ep*/
struct p{
  u64 sz;/*not accounting the 0 terminating char*/
  u8 str[];
};

struct ep{
  u64 full_p_sz;/*the p sz from which the ep belongs to*/
  struct p *ps;
  s64 last;/*offset*/
  u64 sz;
  u8 *p_rem;/*remainder of the p (right and side of the ep)*/
  u8 type;
};

/*
based on the ep type, we may need to book enough room to put a worst case
scenario of a concatenated sub-p with the ep p rem
*/
static s8 ep_p_new(struct ep *ep,u8 *p_str,u64 p_sz)
{
  /*We will do some p concatenation for the following p types. Then book room
    for the worst case scenario.*/
  u64 str_sz=ep->type=='?'||ep->type=='@'?ep->full_p_sz:p_sz;

  sl addr;
  u64 map_sz;
  if(!ep->ps){/*first allocation*/
    map_sz=sizeof(*ep->ps)+str_sz+1;/*count the 0 terminating char*/
    addr=mmap(map_sz,RD|WR,PRIVATE|ANONYMOUS);
    if(!addr||ISERR(addr)) return ERR_NOMEM;

    ep->last=0;
  }else{
    u64 new_p_sz=sizeof(*ep->ps)+str_sz+1;/*count the 0 terminating char*/
    map_sz=ep->sz+new_p_sz;
    addr=mremap(ep->ps,ep->sz,map_sz);
    if(!addr||ISERR(addr)) return ERR_NOMEM;/*ep is unmapped elsewhere*/

    struct p *p_last=(struct p*)((u8*)(ep->ps)+ep->last);
    ep->last+=sizeof(*p_last)+p_last->sz+1;/*count the 0 terminating char*/
  }
  ep->ps=(struct p*)addr;
  ep->sz=map_sz;
  struct p *p=(struct p*)((u8*)(ep->ps)+ep->last);
  p->sz=str_sz;
  memcpy(&p->str[0],p_str,p_sz);/*copy only p_sz chars*/
  p->str[p_sz]=0;/*set the 0 terminating char*/
  return OK;
}

static struct p *ep_p_get(struct ep *ep,s64 of)
{
  return (struct p*)((u8*)(ep->ps)+of);
}

#define NO_MORE_P -1
static s64 ep_p_next(struct ep *ep,s64 p_of)
{
  struct p *p=(struct p*)((u8*)(ep->ps)+p_of);
  if(ep->sz==p_of+sizeof(*p)+p->sz+1) return NO_MORE_P;
  return p_of+sizeof(*p)+p->sz+1;
}
/*ep end*/
/*============================================================================*/

/*
This function splits the ep in sub-ps which use '|' char as a separator. Deal
with nested eps. Sub-ps are stored (copied) in a list (the ep struct)
input parameters:
  - p to point on the opening ( of the ep
  - ep is where the sub-patterns will be stored (copied)
output parameters:
  - p will point right after the closing ) fo the ep
  - ep will contain copies of all sub-ps of the ep
if successful ep  will contain at least one p
*/
static s8 ep_split(u8 *p,struct ep *ep)
{
  s8 r=OK;
  u8 *pc=p;
  ++pc;/*skip the opening (*/
  void *sub_p_start=pc;
  s64 depth=0;/*depth, if we have nested eps*/

  loop{
    if(depth<0) break;

    if(*pc=='\0'){
      r=NO_VALID_P;
      goto out;
    }else if(*pc=='['){/*handle brackets special*/
      /*Skip the not sign. We have to recognize it because of a possibly
        following ']'.*/
      ++pc;
      if(*pc=='!'||(*pc=='^')) ++pc;

      if(*pc==']') ++pc;/*a leading ']' is recognized as such*/

      loop{/*skip over all chars of the list*/
        if(*pc==']') break;
        if(*pc=='\0'){
          r=NO_VALID_P;
          goto out;
        }
        ++pc;
      }
    }else if((*pc=='?'||*pc=='*'||*pc=='+'||*pc=='@'||*pc=='!')&&pc[1]=='(')
      ++depth;/*remember the nesting depth of nested eps*/
    else if(*pc==')'){
      if(depth==0){/*this means we found the end of the ep*/
        r=ep_p_new(ep,sub_p_start,pc-(u8*)sub_p_start);
        if(r!=OK) goto out;
      }
      depth--;
    }else if(*pc=='|'){
      if(depth==0){
        r=ep_p_new(ep,sub_p_start,pc-(u8*)sub_p_start);
        if(r!=OK) goto out;
        sub_p_start=pc+1;
      }
    }
    ++pc;
  }
  /*store a pointer on the rem of the p, right after the closing )*/
  ep->p_rem=pc;
out:
  return r;
}

static s8 match_at_least_once(struct ep *ep,u8 *p,u8 *str,u8 *str_end, u8 flgs)
{
  s64 cur_p_of=0;
  loop{
    struct p *cur_p=ep_p_get(ep,cur_p_of);
    u8 *sc=str;
    loop{
      if(sc>(u8*)str_end) break;

      /*first match the prefix with the cur p*/
      s8 r=match(cur_p->str,str,sc,flgs,0);
      if(r==MATCH){
        /*This was successful. Now match the str rem with the p rem.*/
        r=match(ep->p_rem,sc,str_end,flgs,0);
        if(r==NOMATCH){
	  /*Unable to match the p rem with the str rem, then try to match
	    again the ep by rewinding the p from the start. We can because we
	    are to match the ep more than once*/
          if(sc!=str){
            r=match((u8*)p-1,sc,str_end,flgs,0);
            if(r==MATCH) return MATCH;
          }
        }else if(r==MATCH) return MATCH;
        else return r;/*oops!*/
      }
      ++sc;
    }
    cur_p_of=ep_p_next(ep,cur_p_of);
    if(cur_p_of==NO_MORE_P) break;
  }
  return NOMATCH;/*none of the ps lead to a match*/
}

static s8 match_only_once(struct ep *ep,u8 *str,u8 *str_end,u8 flgs)
{
  s64 cur_p_of=0;//offset
  loop{
    struct p *cur_p=ep_p_get(ep,cur_p_of);

    /*I cannot believe it but `strcat' is actually acceptable here. Match the
      entire str with the prefix from the ep and the rema of the p following
      the ep. We made room in the p str buffer in order to store the concatened
      p.*/
    s8 r=match(strcat(cur_p->str,ep->p_rem),str,str_end,flgs,0);
    if(r==MATCH) return MATCH;
        
    cur_p_of=ep_p_next(ep,cur_p_of);
    if(cur_p_of==NO_MORE_P) break;
  }
  return NOMATCH;/*none of the ps lead to a match*/
}

static s8 match_none(struct ep *ep,u8 *str,u8 *str_end,u8 flgs)
{
  u8 *sc=str;
  loop{
    if(sc>(u8*)str_end) break;

    s64 cur_p_of=0;
    loop{
      struct p *cur_p=ep_p_get(ep,cur_p_of);
      s8 r=match(cur_p->str,str,sc,flgs,0);
      if(r==MATCH) break;

      cur_p_of=ep_p_next(ep,cur_p_of);
      if(cur_p_of==NO_MORE_P) break;
    }

    /*if none of the ps matched see whether the p rem match the str rem*/
    if(cur_p_of==NO_MORE_P){
      s8 r=match(ep->p_rem,sc,str_end,flgs,0);
      if(r==MATCH) return MATCH;
    }
    ++sc;
  }
  /*none of the ps together with the rem of the p lead to a match*/
  return NOMATCH;
}

/*will return WATCH/NOMATCH/NO_VALID_P *and* misc errors*/
static s8 extended_match_try(u8 type,u8 *p,u8 *str,u8 *str_end,u8 flgs)
{
  struct ep ep;
  ep.type=type;
  ep.full_p_sz=strlen(p)+1;/*count the ep type char right befor the opening (*/
  ep.ps=0;
  ep.last=0;
  ep.sz=0;
  ep.p_rem=0;

  s8 r=ep_split(p,&ep);
  if(r!=OK) goto out;

  /*from here, p_rem points right after the closing ) of the ep*/

  r=MATCH;
  switch(type){
  case '*':
    if(match(ep.p_rem,str,str_end,flgs,0)==MATCH) break;
  /*FALLTHROUGH to at least once sub-p matched*/
  case '+':
    r=match_at_least_once(&ep,p,str,str_end,flgs);
    break;
  case '?':
    if(match(ep.p_rem,str,str_end,flgs,0)==MATCH) break;
  /*FALLTHROUGH to only once sub-p matched*/
  case '@':
    r=match_only_once(&ep,str,str_end,flgs);
    break;
  case '!':
    r=match_none(&ep,str,str_end,flgs);
    break;
  default:
    r=NO_VALID_P;
  }
out:
  if(ep.ps) munmap(ep.ps,ep.sz);
  return r;
}

s8 ulinux_match(u8 *pattern,u8 *str,u8 flgs)
{
  return match(pattern,str,(u8*)str+strlen(str),flgs,0);
}
