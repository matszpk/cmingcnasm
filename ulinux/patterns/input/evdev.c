#include <stdarg.h>
#include <ulinux/compiler_types.h>
#include <ulinux/sysc.h>
#include <ulinux/types.h>
#include <ulinux/error.h>
#include <ulinux/file.h>
#include <ulinux/fs.h>
#include <ulinux/dirent.h>
#include <ulinux/ioctl.h>
#include <ulinux/time.h>
#include <ulinux/input/evdev.h>
#include <ulinux/epoll.h>

#include <ulinux/utils/ascii/string/string.h>
#include <ulinux/utils/ascii/string/vsprintf.h>
#include <ulinux/utils/mem.h>

#include "ulinux_namespace.h"

#define DPRINTF_BUF_SZ 1024
static u8 *dprintf_buf;

#define PERRC(str) {l rl;do{rl=ulinux_sysc(write,3,2,str,sizeof(str));}\
while(rl==-EINTR||rl==-EAGAIN);}

#define PERR(fmt,...) ulinux_dprintf(2,dprintf_buf,DPRINTF_BUF_SZ,fmt,\
##__VA_ARGS__)

#define EVDEVS_MAX 32//see linux/Documentation/input/input.txt:#3.2.4
//bits field sizes are long size modulos
#define LONG_BITS (sizeof(l)<<3)

struct evdev{
  i fd;
  struct input_event ev;
  u8 ev_read_bytes;
  l ev_bits[((EV_MAX/LONG_BITS)+1)];
  l key_bits[((KEY_MAX/LONG_BITS)+1)];
  l rel_bits[((REL_MAX/LONG_BITS)+1)];
  l abs_bits[((ABS_MAX/LONG_BITS)+1)];
  l msc_bits[((MSC_MAX/LONG_BITS)+1)];
  l sw_bits[((SW_MAX/LONG_BITS)+1)];
  l led_bits[((LED_MAX/LONG_BITS)+1)];
  l snd_bits[((SND_MAX/LONG_BITS)+1)];
  l ff_bits[((FF_MAX/LONG_BITS)+1)];
};
static u32 evdevs_n=0;
static struct evdev evdevs[EVDEVS_MAX];

static void bits_dump(void *hdr,l *p,u8 bytes)
{
  PERR("%s=0x",hdr);
  u8 *a=(u8*)p;
  for(u8 j=0;j<bytes;++j) PERR("%1x%1x",a[j]&0x0f,a[j]>>4);
  PERRC("\n");
}

static u8 test_bit(l *p,u16 bit_off)
{
  u16 byte_off=bit_off>>3;
  u8 byte_bit_off=bit_off%8;
  u8 *a=(u8*)p;
  return (a[byte_off]>>byte_bit_off)&1;
}

static s8 ev_bits(void)
{
  l r=ioctl(evdevs[evdevs_n].fd,EVIOCGBIT(0,sizeof(evdevs[evdevs_n].ev_bits)),
                                               (l)&evdevs[evdevs_n].ev_bits[0]);
  if(ISERR(r)){
    PERR("error(%ld):unable to get the evdev ev bits\n",r);
    return -1;
  }
  bits_dump("ev_bits",&evdevs[evdevs_n].ev_bits[0],
                                              sizeof(evdevs[evdevs_n].ev_bits));
  return 0;
}

static s8 xxx_bits(u16 type,l *xxx_bits_array,l xxx_bits_array_sz,void *desc)
{
  if(test_bit(evdevs[evdevs_n].ev_bits,type)){
    l r=ioctl(evdevs[evdevs_n].fd,EVIOCGBIT(type,xxx_bits_array_sz),
                                                                xxx_bits_array);
    if(ISERR(r)){
      PERR("error(%ld):unable to get the evdev %s bits\n",r,desc);
      return -1;
    }
    bits_dump(desc,xxx_bits_array,xxx_bits_array_sz);
  }else PERR("no %s bits\n",desc);
  return 0;
}

static void dentry_process(i dev_input_fd,struct dirent64 *d)
{
  if(evdevs_n==EVDEVS_MAX||strncmp(d->name,"event",5)) goto exit;

  PERR("\nstart processing %u:/dev/input/%s...\n",evdevs_n,d->name);

  l r;
  do r=openat(dev_input_fd,d->name,RDWR|NONBLOCK,0); while(r==-EINTR);
  if(ISERR(r)){
    PERR("error(%ld):unable to open the evdev node\n",r);
    goto exit;
  }

  evdevs[evdevs_n].fd=(i)r;

  i driver_version;
  r=ioctl(evdevs[evdevs_n].fd,EVIOCGVERSION,(l)&driver_version);
  if(ISERR(r)){
    PERR("error(%ld):unable to get the driver version\n",r);
    goto close_evdev_fd;   
  }
  PERR("driver version=0x%08x\n",driver_version);

  static u8 buf[256];
  r=ioctl(evdevs[evdevs_n].fd,EVIOCGNAME(sizeof(buf)),(l)&buf[0]);
  if(ISERR(r)){
    PERR("error(%ld):unable to get the evdev name\n",r);
    goto close_evdev_fd;   
  }
  PERR("evdev name=%s\n",buf);

  r=ioctl(evdevs[evdevs_n].fd,EVIOCGPHYS(sizeof(buf)),(l)&buf[0]);
  if(ISERR(r)){
    PERR("error(%ld):unable to get the evdev physical location\n",r);
    goto close_evdev_fd;   
  }
  PERR("evdev physical location=%s\n",buf);

  if(ev_bits()!=0) goto close_evdev_fd;
  if(xxx_bits(EV_KEY,&evdevs[evdevs_n].key_bits[0],
                                    sizeof(evdevs[evdevs_n].key_bits),"key")!=0)
    goto close_evdev_fd;
  if(xxx_bits(EV_REL,&evdevs[evdevs_n].rel_bits[0],
                                    sizeof(evdevs[evdevs_n].rel_bits),"rel")!=0)
    goto close_evdev_fd;
  if(xxx_bits(EV_ABS,&evdevs[evdevs_n].abs_bits[0],
                                    sizeof(evdevs[evdevs_n].abs_bits),"abs")!=0)
    goto close_evdev_fd;
  if(xxx_bits(EV_MSC,&evdevs[evdevs_n].msc_bits[0],
                                    sizeof(evdevs[evdevs_n].msc_bits),"msc")!=0)
    goto close_evdev_fd;
  if(xxx_bits(EV_SW,&evdevs[evdevs_n].sw_bits[0],
                                      sizeof(evdevs[evdevs_n].sw_bits),"sw")!=0)
    goto close_evdev_fd;
  if(xxx_bits(EV_LED,&evdevs[evdevs_n].led_bits[0],
                                    sizeof(evdevs[evdevs_n].led_bits),"led")!=0)
    goto close_evdev_fd;
  if(xxx_bits(EV_SND,&evdevs[evdevs_n].snd_bits[0],
                                    sizeof(evdevs[evdevs_n].snd_bits),"snd")!=0)
    goto close_evdev_fd;
  if(xxx_bits(EV_FF,&evdevs[evdevs_n].ff_bits[0],
                                      sizeof(evdevs[evdevs_n].ff_bits),"ff")!=0)
    goto close_evdev_fd;
  ++evdevs_n;
  goto exit;

close_evdev_fd:
  do r=close(evdevs[evdevs_n].fd); while(r==-EINTR);

exit:
  return;
}

#define DIRENTS_BUF_SZ 8192
static void dev_input_parse(i dev_input_fd)
{
  u8 dirents[DIRENTS_BUF_SZ];
  while(1){
    l r=getdents64(dev_input_fd,dirents,DIRENTS_BUF_SZ);
    if(ISERR(r)){
      PERR("ERROR(%ld):getdents error\n",r);
      exit(-1);
    }
    if(!r) break;
    l j=0;
    while(j<r){
      struct dirent64 *d=(struct dirent64*)(dirents+j);

      dentry_process(dev_input_fd,d);
      j+=d->rec_len;
    }
  }
}

static void *syn_code_str(u16 code)
{
  switch(code){
  case SYN_REPORT:
    return "report";
  case SYN_CONFIG:
    return "config";
  case SYN_MT_REPORT:
    return "mt_report";
  case SYN_DROPPED:
    return "dropped";
  default:
    return "unknown syn code";
  }
}

static void *rel_code_str(u16 code)
{
  switch(code){
  case REL_X:
    return "x";
  case REL_Y:
    return "y";
  case REL_Z:
    return "z";
  case REL_RX:
    return "rx";
  case REL_RY:
    return "ry";
  case REL_HWHEEL:
    return "hwheel";
  case REL_DIAL:
    return "dial";
  case REL_WHEEL:
    return "wheel";
  case REL_MISC:
    return "misc";
  default:
    return "unknown rel code";
  }
}

static void *msc_code_str(u16 code)
{
  switch(code){
  case MSC_SERIAL:
    return "serial";
  case MSC_PULSELED:
    return "pulseled";
  case MSC_GESTURE:
    return "gesture";
  case MSC_RAW:
    return "raw";
  case MSC_SCAN:
    return "scan";
  default:
    return "unknown msc code";
  }
}

static void *sw_code_str(u16 code)
{
  switch(code){
  case SW_LID:
    return "lid";
  case SW_TABLET_MODE:
    return "tablet mode";
  case SW_HEADPHONE_INSERT:
    return "headphone insert";
  case SW_RFKILL_ALL:
    return "rf kill all";
  case SW_MICROPHONE_INSERT:
    return "microphone insert";
  case SW_DOCK:
    return "dock";
  case SW_LINEOUT_INSERT:
    return "line out insert";
  case SW_JACK_PHYSICAL_INSERT:
    return "jack physical insert";
  case SW_VIDEOOUT_INSERT:
    return "video out insert";
  case SW_CAMERA_LENS_COVER:
    return "camera lens cover";
  case SW_KEYPAD_SLIDE:
    return "keypad slide";
  case SW_FRONT_PROXIMITY:
    return "front proximity";
  case SW_ROTATE_LOCK:
    return "rotate lock";
  case SW_LINEIN_INSERT:
    return "line in insert";
  default:
    return "unknown sw code";
  }
}

static void *snd_code_str(u16 code)
{
  switch(code){
  case SND_CLICK:
    return "click";
  case SND_BELL:
    return "bell";
  case SND_TONE:
    return "tone";
  default:
    return "unknown snd code";
  }
}

static void device_event_process(u8 j)
{
  struct input_event *ev=&evdevs[j].ev;
  void *type_str;
  void *code_str;
  switch(ev->type){
  case EV_SYN:
    type_str="syn";
    code_str=syn_code_str(ev->code);
    break;
  case EV_KEY:
    type_str="key";
    code_str="see key table in ulinux/input/evdev.h";
    break;
  case EV_REL:
    type_str="rel";
    code_str=rel_code_str(ev->code);
    break;
  case EV_ABS:
    type_str="abs";
    code_str="see abs table in ulinux/input/evdev.h";
    break;
  case EV_MSC:
    type_str="msc";
    code_str=msc_code_str(ev->code);
    break;
  case EV_SW:
    type_str="sw";
    code_str=sw_code_str(ev->code);
    break;
  case EV_LED:
    type_str="led";
    code_str="LED IN ONLY OUPUT, NOT INPUT!!!";
    break;
  case EV_SND:
    type_str="snd";
    code_str=snd_code_str(ev->code);
    break;
  case EV_REP:
    type_str="rep";
    code_str="support is not implemented!";
    break;
  case EV_FF:
    type_str="ff";
    code_str="see ff table in ulinux/input/evdev.h";
    break;
  case EV_PWR:
    type_str="pwr";
    code_str="support is not implemented!";
    break;
  case EV_FF_STATUS:
    type_str="ff_status";
    code_str="see ff status table in ulinux_input/evdev.h";
    break;
  default:
    type_str="unknown";
    code_str="unknown type";
  }

  PERR("\ngot event from device %u:\n"
       "   time=%ld.%ld\n"
       "   type=0x%04x(%s)\n"
       "   code=0x%04x(%s)\n"
       "   value=0x%08x\n",
       j,ev->time.sec,ev->time.usec,ev->type,type_str,ev->code,code_str,
       ev->value);
}

static void monitor(void)
{
  l r0=epoll_create1(0);
  if(ISERR(r0)){
    PERR("error(%ld) create epoll fd\n",r0);
    exit(-1);
  }
  i epfd=(i)r0;

  struct epoll_event evts[EVDEVS_MAX];
  memset(evts,0,sizeof(struct epoll_event)*evdevs_n);

  for(u8 j=0;j<evdevs_n;++j){
    evts[j].events=EPOLLET|EPOLLIN;
    evts[j].data._32=j;
    r0=epoll_ctl(epfd,EPOLL_CTL_ADD,evdevs[j].fd,&evts[j]);
    if(ISERR(r0)){
      PERR("error(%ld) adding fd from ev device %u\n",r0,j);
      exit(-1);
    }
  }

  //main loop
  while(1){
    do{
      memset(evts,0,sizeof(evts));
      r0=epoll_wait(epfd,evts,evdevs_n,-1);
    }while(r0==-EINTR);
    if(ISERR(r0)){
      PERR("error(%ld) while waiting epoll event\n",r0);
      exit(-1);
    }

    //parse events
    for(u8 j=0;j<(u8)r0;++j)
      if(evts[j].events&EPOLLIN){//only read events
        while(1){
          struct evdev *d=&evdevs[evts[j].data._32];
          //reset event content if it was read and dealed with
          if(d->ev_read_bytes==sizeof(d->ev)){
            memset(&d->ev,0,sizeof(d->ev));
            d->ev_read_bytes=0;
          }

          //XXX:buffer should be bigger, since one "input event" generates many
          //*evdev* (input) events. Reads get a whole number of events.
          l r1;
          do
            r1=read(d->fd,(void*)(&d->ev)+d->ev_read_bytes,sizeof(d->ev)
                                                             -d->ev_read_bytes);
          while(r1==-EINTR);
          if(r1==-EAGAIN) break;//read all we can, then go to next event
          if(ISERR(r1)){
            PERR("error(%ld) read from ev device %u\n",r1,evts[j].data._32);
            exit(-1);
          }

          d->ev_read_bytes+=(u8)r1;
          if(d->ev_read_bytes==sizeof(d->ev))
            device_event_process(evts[j].data._32);
        }
      }//not handled (hot unplugging will be ignored with consequences)
  }
}

void _start(void)
{
  u8 _dprintf_buf[DPRINTF_BUF_SZ];
  dprintf_buf=&_dprintf_buf[0];

  l r;
  do r=open("/dev/input",RDONLY|NONBLOCK,0);while(r==-EINTR);
  if(ISERR(r)){
    PERR("ERROR(%ld):unable to open /dev/input\n",r);
    exit(-1);
  }

  dev_input_parse((i)r); 
  monitor();
  exit(0);
}
