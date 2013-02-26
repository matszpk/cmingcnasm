#include <stdarg.h>
#include <ulinux/compiler_types.h>
#include <ulinux/sysc.h>
#include <ulinux/types.h>
#include <ulinux/error.h>
#include <ulinux/args_env.h>
#include <ulinux/file.h>
#include <ulinux/fs.h>
#include <ulinux/dirent.h>

#include <ulinux/utils/ascii/string/vsprintf.h>

#define DPRINTF_BUF_SZ 1024
static k_u8 *dprintf_buf;

#define PERRC(s) {k_l rl;do{rl=sysc(write,3,2,s,sizeof(s));}\
while(rl==-K_EINTR||rl==-K_EAGAIN);}

#define PERR(f,...) u_a_dprintf(2,dprintf_buf,DPRINTF_BUF_SZ,(k_u8*)f,\
##__VA_ARGS__)

#define POUT(f,...) u_a_dprintf(1,dprintf_buf,DPRINTF_BUF_SZ,(k_u8*)f,\
##__VA_ARGS__)

#define POUTC(s) {k_l rl;do{rl=sysc(write,3,1,s,sizeof(s));}\
while(rl==-K_EINTR||rl==-K_EAGAIN);}

static k_ut is_current(k_u8 *n)
{
  if(n[0]=='.'&&n[1]==0) return 1;
  return 0;
}

static k_ut is_parent(k_u8 *n)
{
  if(n[0]=='.'&&n[1]=='.'&&n[2]==0) return 1;
  return 0;
}

static k_u depth=-1;

static void dout(struct k_dirent64 *d)
{
  POUT("%20lu %20ld %2u ",d->ino,d->off,d->type);
  k_u i=depth;
  while(i--) POUTC("  ");
  POUT("%s\n",d->name);
}

#define DIRENTS_BUF_SZ 8192
//XXX:carefull, the dentry type is not supported by all fs
static void dir_parse(k_i parent_fd)
{
  ++depth;
  k_u8 dirents[DIRENTS_BUF_SZ];
  while(1){
    k_l r=sysc(getdents64,3,parent_fd,dirents,DIRENTS_BUF_SZ);
    if(K_ISERR(r)){
      PERR("ERROR(%ld):getdents error\n",r);
      sysc(exit_group,1,-1);
    }
    if(!r) break;
    k_l i=0;
    while(i<r){
      struct k_dirent64 *d=(struct k_dirent64*)(dirents+i);

      dout(d);

      if(d->type==K_DT_DIR&&!is_current(d->name)&&!is_parent(d->name)){
        k_i dir_fd;
        do
          dir_fd=(k_i)sysc(openat,4,parent_fd,d->name,K_O_RDONLY|K_O_NONBLOCK,
                                    0);
        while(dir_fd==-K_EINTR);
        if(K_ISERR(dir_fd))
          PERR("ERROR(%d):unable to open subdir:%s\n",dir_fd,d->name);
        else{
          dir_parse(dir_fd);
          sysc(close,1,dir_fd);
        }
      }
      i+=d->rec_len;
    }
  }
  depth--;
}

void start(k_i argc,k_u8 **argv)
{
  k_u8 _dprintf_buf[DPRINTF_BUF_SZ];
  dprintf_buf=&_dprintf_buf[0];

  if(argc!=2){
    PERR("ERROR:wrong number of command arguments(%d)\n",argc);
    sysc(exit_group,1,-1);
  }

  //root fd
  k_i root_fd;
  do
    root_fd=(k_i)sysc(open,3,argv[1],K_O_RDONLY|K_O_NONBLOCK,0);
  while(root_fd==-K_EINTR);
  if(K_ISERR(root_fd)){
    PERR("ERROR(%d):unable to open root dir:%s\n",root_fd,argv[1]);
    sysc(exit_group,1,-1);
  }

  dir_parse(root_fd); 
  sysc(exit_group,1,0);
}
