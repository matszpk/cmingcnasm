#include <stdarg.h>
#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <ulinux/sysc.h>
#include <ulinux/args_env.h>

#include <ulinux/utils/ascii/string/vsprintf.h>
#include <ulinux/utils/ascii/match/match.h>

struct test{
  void *string;
  void *pattern;
  ulinux_u8 flags;
};

//******************************************************************************
//Stolen glibc tests for the posix locale (ascii).
//Upgrade to a GNU Lesser GPLv3 protection.
//Copyrights, Sylvain BERTRAND (sylvain.bertrand@gmail.com).
//******************************************************************************
#define NOMATCH  0x01
#define EXTMATCH 0x02
struct test tests[]={
  {"!#%+,-./01234567889","!#%+,-./01234567889",0},
  {":;=@ABCDEFGHIJKLMNO",":;=@ABCDEFGHIJKLMNO",0},
  {"PQRSTUVWXYZ]abcdefg","PQRSTUVWXYZ]abcdefg",0},
  {"hijklmnopqrstuvwxyz","hijklmnopqrstuvwxyz",0},
  {"^_{}~","^_{}~",0},
  {"\"$&'()","\\\"\\$\\&\\'\\(\\)",0},
  {"*?[\\`|","\\*\\?\\[\\\\\\`\\|",0},
  {"<>","\\<\\>",0},
  {"?*[","[?*[][?*[][?*[]",0},
  {"a/b","?/b",0}, 
  {"a/b","a?b",0},
  {"a/b","a/?",0},
  {"aa/b","?/b",NOMATCH},
  {"aa/b","a?b",NOMATCH},
  {"a/bb","a/?",NOMATCH},
  {"abc","[abc]",NOMATCH},
  {"x","[abc]",NOMATCH},
  {"a","[abc]",0},
  {"[","[[abc]",0},
  {"a","[][abc]",0},
  {"a]","[]a]]",0},
  {"xyz","[!abc]",NOMATCH},
  {"x","[!abc]",0},
  {"a","[!abc]",NOMATCH},
  {"]","[][abc]",0},
  {"abc]","[][abc]",NOMATCH},
  {"[]abc","[][]abc",NOMATCH},
  {"]","[!]]",NOMATCH},
  {"aa]","[!]a]",NOMATCH},
  {"]","[!a]",0},
  {"]]","[!a]]",0},
  {"a","[[:alnum:]]",0},
  {"a","[![:alnum:]]",NOMATCH},
  {"-","[[:alnum:]]",NOMATCH},
  {"a]a","[[:alnum:]]a",NOMATCH},
  {"-","[[:alnum:]-]",0},
  {"aa","[[:alnum:]]a",0},
  {"-","[![:alnum:]]",0},
  {"]","[!][:alnum:]]",NOMATCH},
  {"[","[![:alnum:][]",NOMATCH},
  {"a","[[:alnum:]]",0},
  {"b","[[:alnum:]]",0},
  {"c","[[:alnum:]]",0},
  {"d","[[:alnum:]]",0},
  {"e","[[:alnum:]]",0},
  {"f","[[:alnum:]]",0},
  {"g","[[:alnum:]]",0},
  {"h","[[:alnum:]]",0},
  {"i","[[:alnum:]]",0},
  {"j","[[:alnum:]]",0},
  {"k","[[:alnum:]]",0},
  {"l","[[:alnum:]]",0},
  {"m","[[:alnum:]]",0},
  {"n","[[:alnum:]]",0},
  {"o","[[:alnum:]]",0},
  {"p","[[:alnum:]]",0},
  {"q","[[:alnum:]]",0},
  {"r","[[:alnum:]]",0},
  {"s","[[:alnum:]]",0},
  {"t","[[:alnum:]]",0},
  {"u","[[:alnum:]]",0},
  {"v","[[:alnum:]]",0},
  {"w","[[:alnum:]]",0},
  {"x","[[:alnum:]]",0},
  {"y","[[:alnum:]]",0},
  {"z","[[:alnum:]]",0},
  {"A","[[:alnum:]]",0},
  {"B","[[:alnum:]]",0},
  {"C","[[:alnum:]]",0},
  {"D","[[:alnum:]]",0},
  {"E","[[:alnum:]]",0},
  {"F","[[:alnum:]]",0},
  {"G","[[:alnum:]]",0},
  {"H","[[:alnum:]]",0},
  {"I","[[:alnum:]]",0},
  {"J","[[:alnum:]]",0},
  {"K","[[:alnum:]]",0},
  {"L","[[:alnum:]]",0},
  {"M","[[:alnum:]]",0},
  {"N","[[:alnum:]]",0},
  {"O","[[:alnum:]]",0},
  {"P","[[:alnum:]]",0},
  {"Q","[[:alnum:]]",0},
  {"R","[[:alnum:]]",0},
  {"S","[[:alnum:]]",0},
  {"T","[[:alnum:]]",0},
  {"U","[[:alnum:]]",0},
  {"V","[[:alnum:]]",0},
  {"W","[[:alnum:]]",0},
  {"X","[[:alnum:]]",0},
  {"Y","[[:alnum:]]",0},
  {"Z","[[:alnum:]]",0},
  {"0","[[:alnum:]]",0},
  {"1","[[:alnum:]]",0},
  {"2","[[:alnum:]]",0},
  {"3","[[:alnum:]]",0},
  {"4","[[:alnum:]]",0},
  {"5","[[:alnum:]]",0},
  {"6","[[:alnum:]]",0},
  {"7","[[:alnum:]]",0},
  {"8","[[:alnum:]]",0},
  {"9","[[:alnum:]]",0},
  {"!","[[:alnum:]]",NOMATCH},
  {"#","[[:alnum:]]",NOMATCH},
  {"%","[[:alnum:]]",NOMATCH},
  {"+","[[:alnum:]]",NOMATCH},
  {",","[[:alnum:]]",NOMATCH},
  {"-","[[:alnum:]]",NOMATCH},
  {".","[[:alnum:]]",NOMATCH},
  {"/","[[:alnum:]]",NOMATCH},
  {":","[[:alnum:]]",NOMATCH},
  {";","[[:alnum:]]",NOMATCH},
  {"=","[[:alnum:]]",NOMATCH},
  {"@","[[:alnum:]]",NOMATCH},
  {"[","[[:alnum:]]",NOMATCH},
  {"\\","[[:alnum:]]",NOMATCH},
  {"]","[[:alnum:]]",NOMATCH},
  {"^","[[:alnum:]]",NOMATCH},
  {"_","[[:alnum:]]",NOMATCH},
  {"{","[[:alnum:]]",NOMATCH},
  {"}","[[:alnum:]]",NOMATCH},
  {"~","[[:alnum:]]",NOMATCH},
  {"\"","[[:alnum:]]",NOMATCH},
  {"$","[[:alnum:]]",NOMATCH},
  {"&","[[:alnum:]]",NOMATCH},
  {"'","[[:alnum:]]",NOMATCH},
  {"(","[[:alnum:]]",NOMATCH},
  {")","[[:alnum:]]",NOMATCH},
  {"*","[[:alnum:]]",NOMATCH},
  {"?","[[:alnum:]]",NOMATCH},
  {"`","[[:alnum:]]",NOMATCH},
  {"|","[[:alnum:]]",NOMATCH},
  {"<","[[:alnum:]]",NOMATCH},
  {">","[[:alnum:]]",NOMATCH},
  {"\t","[[:cntrl:]]",0},
  {"t","[[:cntrl:]]",NOMATCH},
  {"t","[[:lower:]]",0},
  {"\t","[[:lower:]]",NOMATCH},
  {"T","[[:lower:]]",NOMATCH},
  {"\t","[[:space:]]",0},
  {"t","[[:space:]]",NOMATCH},
  {"t","[[:alpha:]]",0},
  {"\t","[[:alpha:]]",NOMATCH},
  {"0","[[:digit:]]",0},
  {"\t","[[:digit:]]",NOMATCH},
  {"t","[[:digit:]]",NOMATCH},
  {"\t","[[:print:]]",NOMATCH},
  {"t","[[:print:]]",0},
  {"T","[[:upper:]]",0},
  {"\t","[[:upper:]]",NOMATCH},
  {"t","[[:upper:]]",NOMATCH},
  {"\t","[[:blank:]]",0},
  {"t","[[:blank:]]",NOMATCH},
  {"\t","[[:graph:]]",NOMATCH},
  {"t","[[:graph:]]",0},
  {".","[[:punct:]]",0},
  {"t","[[:punct:]]",NOMATCH},
  {"\t","[[:punct:]]",NOMATCH},
  {"0","[[:xdigit:]]",0},
  {"\t","[[:xdigit:]]",NOMATCH},
  {"a","[[:xdigit:]]",0},
  {"A","[[:xdigit:]]",0},
  {"t","[[:xdigit:]]",NOMATCH},
  {"a","[[alpha]]",NOMATCH},
  {"a","[[alpha:]]",NOMATCH},
  {"a]","[[alpha]]",0},
  {"a]","[[alpha:]]",0},
  {"a","[[:alpha:][:digit:]]",0},
  {"a","[[:digit:][:alpha:]]",0},
  {"a","[a-c]",0},
  {"a","[b-c]",NOMATCH},
  {"d","[b-c]",NOMATCH},
  {"B","[a-c]",NOMATCH},
  {"b","[A-C]",NOMATCH},
  {"","[a-c]",NOMATCH},
  {"as","[a-ca-z]",NOMATCH},
  {"a","[a-c0-9]",0},
  {"d","[a-c0-9]",NOMATCH},
  {"B","[a-c0-9]",NOMATCH},
  {"-","[-a]",0},
  {"a","[-b]",NOMATCH},
  {"-","[!-a]",NOMATCH},
  {"a","[!-b]",0},
  {"-","[a-c-0-9]",0},
  {"b","[a-c-0-9]",0},
  {"a:","a[0-9-a]",NOMATCH},
  {"a:","a[09-a]",0},
  {"","*",0},
  {"asd/sdf","*",0},
  {"as","[a-c][a-z]",0},
  {"as","??",0},
  {"asd/sdf","as*df",0},
  {"asd/sdf","as*",0},
  {"asd/sdf","*df",0},
  {"asd/sdf","as*dg",NOMATCH},
  {"asdf","as*df",0},
  {"asdf","as*df?",NOMATCH},
  {"asdf","as*??",0},
  {"asdf","a*???",0},
  {"asdf","*????",0},
  {"asdf","????*",0},
  {"asdf","??*?",0},
  {"/","/", 0},
  {"/","/*",0},
  {"/","*/",0},
  {"/","/?",NOMATCH},
  {"/","?/",NOMATCH},
  {"/","?",0},
  {".","?",0},
  {"/.","??",0},
  {"/","[!a-c]",0},
  {".","[!a-c]",0},
  {"/$","\\/\\$",0},
  {"/[","\\/\\[",0},
  {"/[","\\/[",0},
  {"/[]","\\/\\[]",0},
  {"foobar","foo*[abc]z",NOMATCH},
  {"foobaz","foo*[abc][xyz]",0},
  {"foobaz","foo?*[abc][xyz]",0},
  {"foobaz","foo?*[abc][x/yz]",0},
  {"az","[a-]z",0},
  {"bz","[ab-]z",0},
  {"cz","[ab-]z",NOMATCH},
  {"-z","[ab-]z",0},
  {"az","[-a]z",0},
  {"bz","[-ab]z",0},
  {"cz","[-ab]z",NOMATCH},
  {"-z","[-ab]z",0},
  {"\\","[\\\\-a]",0},
  {"_","[\\\\-a]",0},
  {"a","[\\\\-a]",0},
  {"-","[\\\\-a]",NOMATCH},
  {"\\","[\\]-a]",NOMATCH},
  {"_","[\\]-a]",0},
  {"a","[\\]-a]",0},
  {"]","[\\]-a]",0},
  {"-","[\\]-a]",NOMATCH},
  {"\\","[!\\\\-a]",NOMATCH},
  {"_","[!\\\\-a]",NOMATCH},
  {"a","[!\\\\-a]",NOMATCH},
  {"-","[!\\\\-a]",0},
  {"!","[\\!-]",0},
  {"-","[\\!-]",0},
  {"\\","[\\!-]",NOMATCH},
  {"Z","[Z-\\\\]",0},
  {"[","[Z-\\\\]",0},
  {"\\","[Z-\\\\]",0},
  {"-","[Z-\\\\]",NOMATCH},
  {"Z","[Z-\\]]",0},
  {"[","[Z-\\]]",0},
  {"\\","[Z-\\]]",0},
  {"]","[Z-\\]]",0},
  {"-","[Z-\\]]",NOMATCH},
  {"abcd","?@(a|b)*@(c)d",EXTMATCH},
  {"12","[1-9]*([0-9])",EXTMATCH},
  {"12abc","[1-9]*([0-9])",NOMATCH|EXTMATCH},
  {"1","[1-9]*([0-9])",EXTMATCH},
  {"07","+([0-7])",EXTMATCH},
  {"0377","+([0-7])",EXTMATCH},
  {"09","+([0-7])",NOMATCH|EXTMATCH},
  {"paragraph","para@(chute|graph)",EXTMATCH},
  {"paramour","para@(chute|graph)",NOMATCH|EXTMATCH},
  {"para991","para?([345]|99)1",EXTMATCH},
  {"para381","para?([345]|99)1",NOMATCH|EXTMATCH},
  {"paragraph","para*([0-9])",NOMATCH|EXTMATCH},
  {"para","para*([0-9])",EXTMATCH},
  {"para13829383746592","para*([0-9])",EXTMATCH},
  {"paragraph","para+([0-9])",NOMATCH|EXTMATCH},
  {"para","para+([0-9])",NOMATCH|EXTMATCH},
  {"para987346523","para+([0-9])",EXTMATCH},
  {"paragraph","para!(*.[0-9])",EXTMATCH},
  {"para.38","para!(*.[0-9])",EXTMATCH},
  {"para.graph","para!(*.[0-9])",EXTMATCH},
  {"para39","para!(*.[0-9])",EXTMATCH},
  {"","*(0|1|3|5|7|9)",EXTMATCH},
  {"137577991","*(0|1|3|5|7|9)",EXTMATCH},
  {"2468","*(0|1|3|5|7|9)",NOMATCH|EXTMATCH},
  {"1358","*(0|1|3|5|7|9)",NOMATCH|EXTMATCH},
  {"file.c","*.c?(c)",EXTMATCH},
  {"file.C","*.c?(c)",NOMATCH|EXTMATCH},
  {"file.cc","*.c?(c)",EXTMATCH},
  {"file.ccc","*.c?(c)",NOMATCH|EXTMATCH},
  {"parse.y","!(*.c|*.h|Makefile.in|config*|README)",EXTMATCH},
  {"shell.c","!(*.c|*.h|Makefile.in|config*|README)",NOMATCH|EXTMATCH},
  {"Makefile","!(*.c|*.h|Makefile.in|config*|README)",EXTMATCH},
  {"abcfefg","ab**(e|f)",EXTMATCH},
  {"abcfefg","ab**(e|f)g",EXTMATCH},
  {"ab","ab*+(e|f)",NOMATCH|EXTMATCH},
  {"abef","ab***ef",EXTMATCH},
  {"abef","ab**",EXTMATCH},
  {"fofo","*(f*(o))",EXTMATCH},
  {"ffo","*(f*(o))",EXTMATCH},
  {"foooofo","*(f*(o))",EXTMATCH},
  {"foooofof","*(f*(o))",EXTMATCH},
  {"fooofoofofooo","*(f*(o))",EXTMATCH},
  {"foooofof","*(f+(o))",NOMATCH|EXTMATCH},
  {"xfoooofof","*(f*(o))",NOMATCH|EXTMATCH},
  {"foooofofx","*(f*(o))",NOMATCH|EXTMATCH},
  {"ofxoofxo","*(*(of*(o)x)o)",EXTMATCH},
  {"ofooofoofofooo","*(f*(o))",NOMATCH|EXTMATCH},
  {"foooxfooxfoxfooox","*(f*(o)x)",EXTMATCH},
  {"foooxfooxofoxfooox","*(f*(o)x)",NOMATCH|EXTMATCH},
  {"foooxfooxfxfooox","*(f*(o)x)",EXTMATCH},
  {"ofxoofxo","*(*(of*(o)x)o)",EXTMATCH},
  {"ofoooxoofxo","*(*(of*(o)x)o)",EXTMATCH},
  {"ofoooxoofxoofoooxoofxo","*(*(of*(o)x)o)",EXTMATCH},
  {"ofoooxoofxoofoooxoofxoo","*(*(of*(o)x)o)",EXTMATCH},
  {"ofoooxoofxoofoooxoofxofo","*(*(of*(o)x)o)",NOMATCH|EXTMATCH},
  {"ofoooxoofxoofoooxoofxooofxofxo","*(*(of*(o)x)o)",EXTMATCH},
  {"aac","*(@(a))a@(c)",EXTMATCH},
  {"ac","*(@(a))a@(c)",EXTMATCH},
  {"c","*(@(a))a@(c)",NOMATCH|EXTMATCH},
  {"aaac","*(@(a))a@(c)",EXTMATCH},
  {"baaac","*(@(a))a@(c)",NOMATCH|EXTMATCH},
  {"abcd","?@(a|b)*@(c)d",EXTMATCH},
  {"abcd","@(ab|a*@(b))*(c)d",EXTMATCH},
  {"acd","@(ab|a*(b))*(c)d",EXTMATCH},
  {"abbcd","@(ab|a*(b))*(c)d",EXTMATCH},
  {"effgz","@(b+(c)d|e*(f)g?|?(h)i@(j|k))",EXTMATCH},
  {"efgz","@(b+(c)d|e*(f)g?|?(h)i@(j|k))",EXTMATCH},
  {"egz","@(b+(c)d|e*(f)g?|?(h)i@(j|k))",EXTMATCH},
  {"egzefffgzbcdij","*(b+(c)d|e*(f)g?|?(h)i@(j|k))",EXTMATCH},
  {"egz","@(b+(c)d|e+(f)g?|?(h)i@(j|k))",NOMATCH|EXTMATCH},
  {"ofoofo","*(of+(o))",EXTMATCH},
  {"oxfoxoxfox","*(oxf+(ox))",EXTMATCH},
  {"oxfoxfox","*(oxf+(ox))",NOMATCH|EXTMATCH},
  {"ofoofo","*(of+(o)|f)",EXTMATCH},
  {"foofoofo","@(foo|f|fo)*(f|of+(o))",EXTMATCH},
  {"oofooofo","*(of|oof+(o))",EXTMATCH},
  {"fffooofoooooffoofffooofff","*(*(f)*(o))",EXTMATCH},
  {"fofoofoofofoo","*(fo|foo)",EXTMATCH},
  {"foo","!(x)",EXTMATCH},
  {"foo","!(x)*",EXTMATCH},
  {"foo","!(foo)",NOMATCH|EXTMATCH},
  {"foo","!(foo)*",EXTMATCH},
  {"foobar","!(foo)",EXTMATCH},
  {"foobar","!(foo)*",EXTMATCH},
  {"moo.cow","!(*.*).!(*.*)",EXTMATCH},
  {"mad.moo.cow","!(*.*).!(*.*)",NOMATCH|EXTMATCH},
  {"mucca.pazza","mu!(*(c))?.pa!(*(z))?",NOMATCH|EXTMATCH},
  {"fff","!(f)",EXTMATCH},
  {"fff","*(!(f))",EXTMATCH},
  {"fff","+(!(f))",EXTMATCH},
  {"ooo","!(f)",EXTMATCH},
  {"ooo","*(!(f))",EXTMATCH},
  {"ooo","+(!(f))",EXTMATCH},
  {"foo","!(f)",EXTMATCH},
  {"foo","*(!(f))",EXTMATCH},
  {"foo","+(!(f))",EXTMATCH},
  {"f","!(f)",NOMATCH|EXTMATCH},
  {"f","*(!(f))",NOMATCH|EXTMATCH},
  {"f","+(!(f))",NOMATCH|EXTMATCH},
  {"foot","@(!(z*)|*x)",EXTMATCH},
  {"zoot","@(!(z*)|*x)",NOMATCH|EXTMATCH},
  {"foox","@(!(z*)|*x)",EXTMATCH},
  {"zoox","@(!(z*)|*x)",EXTMATCH},
  {"foo" ,"*(!(foo))",EXTMATCH},
  {"foob","!(foo)b*",NOMATCH|EXTMATCH},
  {"foobb","!(foo)b*",EXTMATCH},
  {"[","*([a[])",EXTMATCH},
  {"]","*([]a[])",EXTMATCH},
  {"a","*([]a[])",EXTMATCH},
  {"b","*([!]a[])",EXTMATCH},
  {"[","*([!]a[]|[[])",EXTMATCH},
  {"]","*([!]a[]|[]])",EXTMATCH},
  {"[","!([!]a[])",EXTMATCH},
  {"]","!([!]a[])",EXTMATCH},
  {")","*([)])",EXTMATCH},
  {"*","*([*(])",EXTMATCH},
  {"abcd","*!(|a)cd",EXTMATCH},
  {"","",0},
  {"","",EXTMATCH},
  {"","*([abc])",EXTMATCH},
  {"","?([abc])",EXTMATCH},
  {0,0,0}
};


#define BUF_SZ 2048
void _start(void) 
{
  ulinux_u8 buf[BUF_SZ];
  struct test *test=&tests[0];
  ulinux_u16 n=0;
  while(1){
    if(test->pattern==0) break;

    ulinux_u8 flgs=0;
    if(test->flags&EXTMATCH) flgs|=ULINUX_MATCH_EXTMATCH;

    ulinux_s8 expected=ULINUX_MATCH_MATCH;
    void *expected_str="match";
    if(test->flags&NOMATCH){
      expected=ULINUX_MATCH_NOMATCH;
      expected_str="nomatch";
    }

    ulinux_dprintf(1,buf,BUF_SZ,"%u:%s %s 0x%02x ",n,test->string,test->pattern,
                                                                          flgs);
    ulinux_s8 r=ulinux_match(test->pattern,test->string,flgs);

    if(r==expected){
      ulinux_dprintf(1,buf,BUF_SZ,"OK(%s)\n",expected_str);
    }else {
      void *str;
      if(expected==ULINUX_MATCH_MATCH){
        str="nomatch";
        if(r!=ULINUX_MATCH_NOMATCH) str="error code";
      }else{
        str="match";
        if(r!=ULINUX_MATCH_MATCH) str="error code";
      } 
      ulinux_dprintf(1,buf,BUF_SZ,"ERROR(expected:%s but got %s(%d))\n",expected_str,str,r);
    }
    ++test;++n;
  }
  ulinux_sysc(exit_group,1,0);
}