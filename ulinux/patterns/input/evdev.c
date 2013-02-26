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

#define DPRINTF_BUF_SZ 1024
static k_u8 *dprintf_buf;

#define PERRC(s) {k_l rl;do{rl=sysc(write,3,2,s,sizeof(s));}\
while(rl==-K_EINTR||rl==-K_EAGAIN);}

#define PERR(f,...) u_a_dprintf(2,dprintf_buf,DPRINTF_BUF_SZ,(k_u8*)f,\
##__VA_ARGS__)

#define EVDEVS_MAX 32//see linux/Documentation/input/input.txt:#3.2.4
//bits field sizes are long size modulos
#define LONG_BITS (sizeof(long)<<3)

#define _(str) (k_u8*)(str)

struct evdev{
  k_i fd;
  struct k_input_event ev;
  k_u8 ev_read_bytes;
  k_l ev_bits[((K_EV_MAX/LONG_BITS)+1)];
  k_l key_bits[((K_KEY_MAX/LONG_BITS)+1)];
  k_l rel_bits[((K_REL_MAX/LONG_BITS)+1)];
  k_l abs_bits[((K_ABS_MAX/LONG_BITS)+1)];
  k_l msc_bits[((K_MSC_MAX/LONG_BITS)+1)];
  k_l sw_bits[((K_SW_MAX/LONG_BITS)+1)];
  k_l led_bits[((K_LED_MAX/LONG_BITS)+1)];
  k_l snd_bits[((K_SND_MAX/LONG_BITS)+1)];
  k_l ff_bits[((K_FF_MAX/LONG_BITS)+1)];
};
static k_u32 evdevs_n=0;
static struct evdev evdevs[EVDEVS_MAX];

static void bits_dump(k_u8 *hdr,k_l *p,k_u8 bytes)
{
  PERR("%s=0x",hdr);
  k_u8 *a=(k_u8*)p;
  for(k_u8 i=0;i<bytes;++i) PERR("%1x%1x",a[i]&0x0f,a[i]>>4);
  PERRC("\n");
}

static k_u8 test_bit(k_l *p,k_u16 bit_off)
{
  k_u16 byte_off=bit_off>>3;
  k_u8 byte_bit_off=bit_off%8;
  k_u8 *a=(k_u8*)p;
  return (a[byte_off]>>byte_bit_off)&1;
}

static k_s8 ev_bits(void)
{
  k_l r=sysc(ioctl,3,evdevs[evdevs_n].fd,K_EVIOCGBIT(0,
                                 sizeof(evdevs[evdevs_n].ev_bits)),(k_l)&evdevs[
                                                          evdevs_n].ev_bits[0]);
  if(K_ISERR(r)){
    PERR("error(%ld):unable to get the evdev ev bits\n",r);
    return -1;
  }
  bits_dump(_("ev_bits"),&evdevs[evdevs_n].ev_bits[0],
                                              sizeof(evdevs[evdevs_n].ev_bits));
  return 0;
}

static k_s8 xxx_bits(k_u16 type,k_l *xxx_bits_array,k_l xxx_bits_array_sz,
                                                                     k_u8 *desc)
{
  if(test_bit(evdevs[evdevs_n].ev_bits,type)){
    k_l r=sysc(ioctl,3,evdevs[evdevs_n].fd,K_EVIOCGBIT(type,xxx_bits_array_sz),
                                                                xxx_bits_array);
    if(K_ISERR(r)){
      PERR("error(%ld):unable to get the evdev %s bits\n",r,desc);
      return -1;
    }
    bits_dump(desc,xxx_bits_array,xxx_bits_array_sz);
  }else PERR("no %s bits\n",desc);
  return 0;
}

static void dentry_process(k_i dev_input_fd,struct k_dirent64 *d)
{
  if(evdevs_n==EVDEVS_MAX||u_a_strncmp(d->name,_("event"),5)) goto exit;

  PERR("\nstart processing %u:/dev/input/%s...\n",evdevs_n,d->name);

  k_l r;
  do r=sysc(openat,4,dev_input_fd,d->name,K_O_RDWR|K_O_NONBLOCK,0);
  while(r==-K_EINTR);

  if(K_ISERR(r)){
    PERR("error(%ld):unable to open the evdev node\n",r);
    goto exit;
  }

  evdevs[evdevs_n].fd=(k_i)r;

  k_i driver_version;
  r=sysc(ioctl,3,evdevs[evdevs_n].fd,K_EVIOCGVERSION,(k_l)&driver_version);
  if(K_ISERR(r)){
    PERR("error(%ld):unable to get the driver version\n",r);
    goto close_evdev_fd;   
  }
  PERR("driver version=0x%08x\n",driver_version);

  static k_u8 buf[256];
  r=sysc(ioctl,3,evdevs[evdevs_n].fd,K_EVIOCGNAME(sizeof(buf)),(k_l)&buf[0]);
  if(K_ISERR(r)){
    PERR("error(%ld):unable to get the evdev name\n",r);
    goto close_evdev_fd;   
  }
  PERR("evdev name=%s\n",buf);

  r=sysc(ioctl,3,evdevs[evdevs_n].fd,K_EVIOCGPHYS(sizeof(buf)),(k_l)&buf[0]);
  if(K_ISERR(r)){
    PERR("error(%ld):unable to get the evdev physical location\n",r);
    goto close_evdev_fd;   
  }
  PERR("evdev physical location=%s\n",buf);

  if(ev_bits()!=0) goto close_evdev_fd;
  if(xxx_bits(K_EV_KEY,&evdevs[evdevs_n].key_bits[0],
                                 sizeof(evdevs[evdevs_n].key_bits),_("key"))!=0)
    goto close_evdev_fd;
  if(xxx_bits(K_EV_REL,&evdevs[evdevs_n].rel_bits[0],
                                 sizeof(evdevs[evdevs_n].rel_bits),_("rel"))!=0)
    goto close_evdev_fd;
  if(xxx_bits(K_EV_ABS,&evdevs[evdevs_n].abs_bits[0],
                                 sizeof(evdevs[evdevs_n].abs_bits),_("abs"))!=0)
    goto close_evdev_fd;
  if(xxx_bits(K_EV_MSC,&evdevs[evdevs_n].msc_bits[0],
                                 sizeof(evdevs[evdevs_n].msc_bits),_("msc"))!=0)
    goto close_evdev_fd;
  if(xxx_bits(K_EV_SW,&evdevs[evdevs_n].sw_bits[0],
                                   sizeof(evdevs[evdevs_n].sw_bits),_("sw"))!=0)
    goto close_evdev_fd;
  if(xxx_bits(K_EV_LED,&evdevs[evdevs_n].led_bits[0],
                                 sizeof(evdevs[evdevs_n].led_bits),_("led"))!=0)
    goto close_evdev_fd;
  if(xxx_bits(K_EV_SND,&evdevs[evdevs_n].snd_bits[0],
                                 sizeof(evdevs[evdevs_n].snd_bits),_("snd"))!=0)
    goto close_evdev_fd;
  if(xxx_bits(K_EV_FF,&evdevs[evdevs_n].ff_bits[0],
                                   sizeof(evdevs[evdevs_n].ff_bits),_("ff"))!=0)
    goto close_evdev_fd;
  ++evdevs_n;
  goto exit;

close_evdev_fd:
  sysc(close,1,evdevs[evdevs_n].fd);

exit:
  return;
}

#define DIRENTS_BUF_SZ 8192
static void dev_input_parse(k_i dev_input_fd)
{
  k_u8 dirents[DIRENTS_BUF_SZ];
  while(1){
    k_l r=sysc(getdents64,3,dev_input_fd,dirents,DIRENTS_BUF_SZ);
    if(K_ISERR(r)){
      PERR("ERROR(%ld):getdents error\n",r);
      sysc(exit_group,1,-1);
    }
    if(!r) break;
    k_l i=0;
    while(i<r){
      struct k_dirent64 *d=(struct k_dirent64*)(dirents+i);

      dentry_process(dev_input_fd,d);
      i+=d->rec_len;
    }
  }
}

static k_u8 *syn_code_str(k_u16 code)
{
  switch(code){
  case K_SYN_REPORT:
    return _("report");
  case K_SYN_CONFIG:
    return _("config");
  case K_SYN_MT_REPORT:
    return _("mt_report");
  case K_SYN_DROPPED:
    return _("dropped");
  default:
    return _("unknown syn code");
  }
}

static k_u8 *rel_code_str(k_u16 code)
{
  switch(code){
  case K_REL_X:
    return _("x");
  case K_REL_Y:
    return _("y");
  case K_REL_Z:
    return _("z");
  case K_REL_RX:
    return _("rx");
  case K_REL_RY:
    return _("ry");
  case K_REL_HWHEEL:
    return _("hwheel");
  case K_REL_DIAL:
    return _("dial");
  case K_REL_WHEEL:
    return _("wheel");
  case K_REL_MISC:
    return _("misc");
  default:
    return _("unknown rel code");
  }
}

static k_u8 *msc_code_str(k_u16 code)
{
  switch(code){
  case K_MSC_SERIAL:
    return _("serial");
  case K_MSC_PULSELED:
    return _("pulseled");
  case K_MSC_GESTURE:
    return _("gesture");
  case K_MSC_RAW:
    return _("raw");
  case K_MSC_SCAN:
    return _("scan");
  default:
    return _("unknown msc code");
  }
}

static k_u8 *sw_code_str(k_u16 code)
{
  switch(code){
  case K_SW_LID:
    return _("lid");
  case K_SW_TABLET_MODE:
    return _("tablet mode");
  case K_SW_HEADPHONE_INSERT:
    return _("headphone insert");
  case K_SW_RFKILL_ALL:
    return _("rf kill all");
  case K_SW_MICROPHONE_INSERT:
    return _("microphone insert");
  case K_SW_DOCK:
    return _("dock");
  case K_SW_LINEOUT_INSERT:
    return _("line out insert");
  case K_SW_JACK_PHYSICAL_INSERT:
    return _("jack physical insert");
  case K_SW_VIDEOOUT_INSERT:
    return _("video out insert");
  case K_SW_CAMERA_LENS_COVER:
    return _("camera lens cover");
  case K_SW_KEYPAD_SLIDE:
    return _("keypad slide");
  case K_SW_FRONT_PROXIMITY:
    return _("front proximity");
  case K_SW_ROTATE_LOCK:
    return _("rotate lock");
  case K_SW_LINEIN_INSERT:
    return _("line in insert");
  default:
    return _("unknown sw code");
  }
}

static k_u8 *snd_code_str(k_u16 code)
{
  switch(code){
  case K_SND_CLICK:
    return _("click");
  case K_SND_BELL:
    return _("bell");
  case K_SND_TONE:
    return _("tone");
  default:
    return _("unknown snd code");
  }
}

static void device_event_process(k_u8 i)
{
  struct k_input_event *ev=&evdevs[i].ev;
  k_u8 *type_str;
  k_u8 *code_str;
  switch(ev->type){
  case K_EV_SYN:
    type_str=_("syn");
    code_str=syn_code_str(ev->code);
    break;
  case K_EV_KEY:
    type_str=_("key");
    code_str=_("see key table in ulinux/input/evdev.h");
    break;
  case K_EV_REL:
    type_str=_("rel");
    code_str=rel_code_str(ev->code);
    break;
  case K_EV_ABS:
    type_str=_("abs");
    code_str=_("see abs table in ulinux/input/evdev.h");
    break;
  case K_EV_MSC:
    type_str=_("msc");
    code_str=msc_code_str(ev->code);
    break;
  case K_EV_SW:
    type_str=_("sw");
    code_str=sw_code_str(ev->code);
    break;
  case K_EV_LED:
    type_str=_("led");
    code_str=_("LED IN ONLY OUPUT, NOT INPUT!!!");
    break;
  case K_EV_SND:
    type_str=_("snd");
    code_str=snd_code_str(ev->code);
    break;
  case K_EV_REP:
    type_str=_("rep");
    code_str=_("support is not implemented!");
    break;
  case K_EV_FF:
    type_str=_("ff");
    code_str=_("see ff table in ulinux/input/evdev.h");
    break;
  case K_EV_PWR:
    type_str=_("pwr");
    code_str=_("support is not implemented!");
    break;
  case K_EV_FF_STATUS:
    type_str=_("ff_status");
    code_str=_("see ff status table in ulinux_input/evdev.h");
    break;
  default:
    type_str=_("unknown");
    code_str=_("unknown type");
  }

  PERR("\ngot event from device %u:\n"
       "   time=%ld.%ld\n"
       "   type=0x%04x(%s)\n"
       "   code=0x%04x(%s)\n"
       "   value=0x%08x\n",
       i,ev->time.sec,ev->time.usec,ev->type,type_str,ev->code,code_str,
       ev->value);
}

static void monitor(void)
{
  k_l r0=sysc(epoll_create1,1,0);
  if(K_ISERR(r0)){
    PERR("error(%ld) create epoll fd\n",r0);
    sysc(exit_group,1,-1);
  }
  k_i epfd=(k_i)r0;

  struct k_epoll_event evts[EVDEVS_MAX];
  u_memset(evts,0,sizeof(struct k_epoll_event)*evdevs_n);

  for(k_u8 i=0;i<evdevs_n;++i){
    evts[i].events=K_EPOLLET|K_EPOLLIN;
    evts[i].data.u32=i;
    r0=sysc(epoll_ctl,4,epfd,K_EPOLL_CTL_ADD,evdevs[i].fd,&evts[i]);
    if(K_ISERR(r0)){
      PERR("error(%ld) adding fd from ev device %u\n",r0,i);
      sysc(exit_group,1,-1);
    }
  }

  //main loop
  while(1){
    do{
      u_memset(evts,0,sizeof(evts));
      r0=sysc(epoll_wait,4,epfd,evts,evdevs_n,-1);
    }while(r0==-K_EINTR);
    if(K_ISERR(r0)){
      PERR("error(%ld) while waiting epoll event\n",r0);
      sysc(exit_group,1,-1);
    }

    //parse events
    for(k_u8 i=0;i<(k_u8)r0;++i)
      if(evts[i].events&K_EPOLLIN){//only read events
        while(1){
          struct evdev *d=&evdevs[evts[i].data.u32];
          //reset event content if it was read and dealed with
          if(d->ev_read_bytes==sizeof(d->ev)){
            u_memset(&d->ev,0,sizeof(d->ev));
            d->ev_read_bytes=0;
          }

          //XXX:buffer should be bigger, since one "input event" generates many
          //*evdev* (input) events. Reads get a whole number of events.
          k_l r1;
          do
            r1=sysc(read,3,d->fd,(void*)(&d->ev)+d->ev_read_bytes,sizeof(d->ev)
                                                             -d->ev_read_bytes);
          while(r1==-K_EINTR);
          if(r1==-K_EAGAIN) break;//read all we can, then go to next event
          if(K_ISERR(r1)){
            PERR("error(%ld) read from ev device %u\n",r1,evts[i].data.u32);
            sysc(exit_group,1,-1);
          }

          d->ev_read_bytes+=(k_u8)r1;
          if(d->ev_read_bytes==sizeof(d->ev))
            device_event_process(evts[i].data.u32);
        }
      }//not handled (hot unplugging will be ignored with consequences)
  }
}

void _start(void)
{
  k_u8 _dprintf_buf[DPRINTF_BUF_SZ];
  dprintf_buf=&_dprintf_buf[0];

  k_l r;
  do r=sysc(open,3,"/dev/input",K_O_RDONLY|K_O_NONBLOCK,0);while(r==-K_EINTR);
  if(K_ISERR(r)){
    PERR("ERROR(%ld):unable to open /dev/input\n",r);
    sysc(exit_group,1,-1);
  }

  dev_input_parse((k_i)r); 
  monitor();
  sysc(exit_group,1,0);
}
