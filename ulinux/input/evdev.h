#ifndef ULINUX_INPUT_EVDEV_H
#define ULINUX_INPUT_EVDEV_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/

/*this is a derived version of linux headers for ulinux*/

struct ulinux_input_event{
  struct ulinux_timeval time;
  ulinux_u16 type;
  ulinux_u16 code;
  ulinux_s32 value;
};

struct unlinux_input_id{
  ulinux_u16 bustype;
  ulinux_u16 vendor;
  ulinux_u16 product;
  ulinux_u16 version;
};

/*
struct input_keymap_entry - used by EVIOCGKEYCODE/EVIOCSKEYCODE ioctls
@scancode: scancode represented in machine-endian form.
@len: length of the scancode that resides in @scancode buffer.
@index: index in the keymap, may be used instead of scancode
@flags: allows to specify how kernel should handle the request. For
 example, setting INPUT_KEYMAP_BY_INDEX flag indicates that kernel
 should perform lookup in keymap by @index instead of @scancode
@keycode: key code assigned to this scancode

The structure is used to retrieve and modify keymap data. Users have
option of performing lookup either by @scancode itself or by @index
in keymap entry. EVIOCGKEYCODE will also return scancode or index
(depending on which element was used to perform lookup).
*/

struct ulinux_input_keymap_entry{
  #define INPUT_KEYMAP_BY_INDEX (1<<0)
  ulinux_u8  flags;
  ulinux_u8  len;
  ulinux_u16 index;
  ulinux_u32 keycode;
  ulinux_u8  scancode[32];
};

/*get driver version*/
#define ULINUX_EVIOCGVERSION ULINUX_IOR('E',0x01,ulinux_si)
/*get device ID*/
#define ULINUX_EVIOCGID      ULINUX_IOR('E',0x02,struct ulinux_input_id)
/*get repeat settings*/
#define ULINUX_EVIOCGREP     ULINUX_IOR('E',0x03,ulinux_ui[2])
/*set repeat settings*/
#define ULINUX_EVIOCSREP     ULINUX_IOW('E',0x03,ulinux_ui[2])

/*get keycode*/
#define ULINUX_EVIOCGKEYCODE    ULINUX_IOR('E',0x04,ulinux_ui[2])
#define ULINUX_EVIOCGKEYCODE_V2 ULINUX_IOR('E',0x04,\
struct ulinux_input_keymap_entry)
/*set keycode*/
#define ULINUX_EVIOCSKEYCODE    ULINUX_IOW('E',0x04,unlinux_i[2])
#define ULINUX_EVIOCSKEYCODE_V2 ULINUX_IOW('E',0x04,\
struct ulinux_input_keymap_entry)

/*get device name*/
#define ULINUX_EVIOCGNAME(len) ULINUX_IOC(ULINUX_IOC_READ,'E',0x06,len)
/*get physical location*/
#define ULINUX_EVIOCGPHYS(len) ULINUX_IOC(ULINUX_IOC_READ,'E',0x07,len)
/*get unique identifier*/
#define ULINUX_EVIOCGUNIQ(len) ULINUX_IOC(ULINUX_IOC_READ,'E',0x08,len)
/*get device properties*/
#define ULINUX_EVIOCGPROP(len) ULINUX_IOC(ULINUX_IOC_READ,'E',0x09,len)
/*get event bits*/
#define ULINUX_EVIOCGBIT(ev,len) ULINUX_IOC(ULINUX_IOC_READ,'E',0x20+(ev),len)

/*event types bit offsets*/
#define ULINUX_EV_SYN       0x00
#define ULINUX_EV_KEY       0x01
#define ULINUX_EV_REL       0x02
#define ULINUX_EV_ABS       0x03
#define ULINUX_EV_MSC       0x04
#define ULINUX_EV_SW        0x05
#define ULINUX_EV_LED       0x11
#define ULINUX_EV_SND       0x12
#define ULINUX_EV_REP       0x14
#define ULINUX_EV_FF        0x15
#define ULINUX_EV_PWR       0x16
#define ULINUX_EV_FF_STATUS 0x17
#define ULINUX_EV_MAX       0x1f
#define ULINUX_EV_CNT       (ULINUX_EV_MAX+1)

/*synchronization events*/
#define ULINUX_SYN_REPORT    0
#define ULINUX_SYN_CONFIG    1
#define ULINUX_SYN_MT_REPORT 2
#define ULINUX_SYN_DROPPED   3

/*
Keys and buttons

Most of the keys/buttons are modeled after USB HUT 1.12
(see http://www.usb.org/developers/hidpage).
Abbreviations in the comments:
AC - Application Control
AL - Application Launch Button
SC - System Control
*/
#define ULINUX_KEY_RESERVED         0
#define ULINUX_KEY_ESC              1
#define ULINUX_KEY_1                2
#define ULINUX_KEY_2                3
#define ULINUX_KEY_3                4
#define ULINUX_KEY_4                5
#define ULINUX_KEY_5                6
#define ULINUX_KEY_6                7
#define ULINUX_KEY_7                8
#define ULINUX_KEY_8                9
#define ULINUX_KEY_9                10
#define ULINUX_KEY_0                11
#define ULINUX_KEY_MINUS            12
#define ULINUX_KEY_EQUAL            13
#define ULINUX_KEY_BACKSPACE        14
#define ULINUX_KEY_TAB              15
#define ULINUX_KEY_Q                16
#define ULINUX_KEY_W                17
#define ULINUX_KEY_E                18
#define ULINUX_KEY_R                19
#define ULINUX_KEY_T                20
#define ULINUX_KEY_Y                21
#define ULINUX_KEY_U                22
#define ULINUX_KEY_I                23
#define ULINUX_KEY_O                24
#define ULINUX_KEY_P                25
#define ULINUX_KEY_LEFTBRACE        26
#define ULINUX_KEY_RIGHTBRACE       27
#define ULINUX_KEY_ENTER            28
#define ULINUX_KEY_LEFTCTRL         29
#define ULINUX_KEY_A                30
#define ULINUX_KEY_S                31
#define ULINUX_KEY_D                32
#define ULINUX_KEY_F                33
#define ULINUX_KEY_G                34
#define ULINUX_KEY_H                35
#define ULINUX_KEY_J                36
#define ULINUX_KEY_K                37
#define ULINUX_KEY_L                38
#define ULINUX_KEY_SEMICOLON        39
#define ULINUX_KEY_APOSTROPHE       40
#define ULINUX_KEY_GRAVE            41
#define ULINUX_KEY_LEFTSHIFT        42
#define ULINUX_KEY_BACKSLASH        43
#define ULINUX_KEY_Z                44
#define ULINUX_KEY_X                45
#define ULINUX_KEY_C                46
#define ULINUX_KEY_V                47
#define ULINUX_KEY_B                48
#define ULINUX_KEY_N                49
#define ULINUX_KEY_M                50
#define ULINUX_KEY_COMMA            51
#define ULINUX_KEY_DOT              52
#define ULINUX_KEY_SLASH            53
#define ULINUX_KEY_RIGHTSHIFT       54
#define ULINUX_KEY_KPASTERISK       55
#define ULINUX_KEY_LEFTALT          56
#define ULINUX_KEY_SPACE            57
#define ULINUX_KEY_CAPSLOCK         58
#define ULINUX_KEY_F1               59
#define ULINUX_KEY_F2               60
#define ULINUX_KEY_F3               61
#define ULINUX_KEY_F4               62
#define ULINUX_KEY_F5               63
#define ULINUX_KEY_F6               64
#define ULINUX_KEY_F7               65
#define ULINUX_KEY_F8               66
#define ULINUX_KEY_F9               67
#define ULINUX_KEY_F10              68
#define ULINUX_KEY_NUMLOCK          69
#define ULINUX_KEY_SCROLLLOCK       70
#define ULINUX_KEY_KP7              71
#define ULINUX_KEY_KP8              72
#define ULINUX_KEY_KP9              73
#define ULINUX_KEY_KPMINUS          74
#define ULINUX_KEY_KP4              75
#define ULINUX_KEY_KP5              76
#define ULINUX_KEY_KP6              77
#define ULINUX_KEY_KPPLUS           78
#define ULINUX_KEY_KP1              79
#define ULINUX_KEY_KP2              80
#define ULINUX_KEY_KP3              81
#define ULINUX_KEY_KP0              82
#define ULINUX_KEY_KPDOT            83

#define ULINUX_KEY_ZENKAKUHANKAKU   85
#define ULINUX_KEY_102ND            86
#define ULINUX_KEY_F11              87
#define ULINUX_KEY_F12              88
#define ULINUX_KEY_RO               89
#define ULINUX_KEY_KATAKANA         90
#define ULINUX_KEY_HIRAGANA         91
#define ULINUX_KEY_HENKAN           92
#define ULINUX_KEY_KATAKANAHIRAGANA 93
#define ULINUX_KEY_MUHENKAN         94
#define ULINUX_KEY_KPJPCOMMA        95
#define ULINUX_KEY_KPENTER          96
#define ULINUX_KEY_RIGHTCTRL        97
#define ULINUX_KEY_KPSLASH          98
#define ULINUX_KEY_SYSRQ            99
#define ULINUX_KEY_RIGHTALT         100
#define ULINUX_KEY_LINEFEED         101
#define ULINUX_KEY_HOME             102
#define ULINUX_KEY_UP               103
#define ULINUX_KEY_PAGEUP           104
#define ULINUX_KEY_LEFT             105
#define ULINUX_KEY_RIGHT            106
#define ULINUX_KEY_END              107
#define ULINUX_KEY_DOWN             108
#define ULINUX_KEY_PAGEDOWN         109
#define ULINUX_KEY_INSERT           110
#define ULINUX_KEY_DELETE           111
#define ULINUX_KEY_MACRO            112
#define ULINUX_KEY_MUTE             113
#define ULINUX_KEY_VOLUMEDOWN       114
#define ULINUX_KEY_VOLUMEUP         115
#define ULINUX_KEY_POWER            116/*SC System Power Down*/
#define ULINUX_KEY_KPEQUAL          117
#define ULINUX_KEY_KPPLUSMINUS      118
#define ULINUX_KEY_PAUSE            119
#define ULINUX_KEY_SCALE            120/*AL Compiz Scale (Expose)*/

#define ULINUX_KEY_KPCOMMA          121
#define ULINUX_KEY_HANGEUL          122
#define ULINUX_KEY_HANGUEL          ULINUX_KEY_HANGEUL
#define ULINUX_KEY_HANJA            123
#define ULINUX_KEY_YEN              124
#define ULINUX_KEY_LEFTMETA         125
#define ULINUX_KEY_RIGHTMETA        126
#define ULINUX_KEY_COMPOSE          127

#define ULINUX_KEY_STOP             128/*AC Stop*/
#define ULINUX_KEY_AGAIN            129
#define ULINUX_KEY_PROPS            130/*AC Properties*/
#define ULINUX_KEY_UNDO             131/*AC Undo*/
#define ULINUX_KEY_FRONT            132
#define ULINUX_KEY_COPY             133/*AC Copy */
#define ULINUX_KEY_OPEN             134/*AC Open*/
#define ULINUX_KEY_PASTE            135/*AC Paste*/
#define ULINUX_KEY_FIND             136/*AC Search*/
#define ULINUX_KEY_CUT              137/*AC Cut*/
#define ULINUX_KEY_HELP             138/*AL Integrated Help Center*/
#define ULINUX_KEY_MENU             139/*Menu (show menu)*/
#define ULINUX_KEY_CALC             140/*AL Calculator*/
#define ULINUX_KEY_SETUP            141
#define ULINUX_KEY_SLEEP            142/*SC System Sleep*/
#define ULINUX_KEY_WAKEUP           143/*System Wake Up*/
#define ULINUX_KEY_FILE             144/*AL Local Machine Browser*/
#define ULINUX_KEY_SENDFILE         145
#define ULINUX_KEY_DELETEFILE       146
#define ULINUX_KEY_XFER             147
#define ULINUX_KEY_PROG1            148
#define ULINUX_KEY_PROG2            149
#define ULINUX_KEY_WWW              150/*AL Internet Browser*/
#define ULINUX_KEY_MSDOS            151
#define ULINUX_KEY_COFFEE           152/*AL Terminal Lock/Screensaver*/
#define ULINUX_KEY_SCREENLOCK       ULINUX_KEY_COFFEE
#define ULINUX_KEY_DIRECTION        153
#define ULINUX_KEY_CYCLEWINDOWS     154
#define ULINUX_KEY_MAIL             155
#define ULINUX_KEY_BOOKMARKS        156/*AC Bookmarks*/
#define ULINUX_KEY_COMPUTER         157
#define ULINUX_KEY_BACK             158/*AC Back*/
#define ULINUX_KEY_FORWARD          159/*AC Forward*/
#define ULINUX_KEY_CLOSECD          160
#define ULINUX_KEY_EJECTCD          161
#define ULINUX_KEY_EJECTCLOSECD     162
#define ULINUX_KEY_NEXTSONG         163
#define ULINUX_KEY_PLAYPAUSE        164
#define ULINUX_KEY_PREVIOUSSONG     165
#define ULINUX_KEY_STOPCD           166
#define ULINUX_KEY_RECORD           167
#define ULINUX_KEY_REWIND           168
#define ULINUX_KEY_PHONE            169/*Media Select Telephone*/
#define ULINUX_KEY_ISO              170
#define ULINUX_KEY_CONFIG           171/*AL Consumer Control Configuration*/
#define ULINUX_KEY_HOMEPAGE         172/*AC Home*/
#define ULINUX_KEY_REFRESH          173/*AC Refresh*/
#define ULINUX_KEY_EXIT             174/*AC Exit*/
#define ULINUX_KEY_MOVE             175
#define ULINUX_KEY_EDIT             176
#define ULINUX_KEY_SCROLLUP         177
#define ULINUX_KEY_SCROLLDOWN       178
#define ULINUX_KEY_KPLEFTPAREN      179
#define ULINUX_KEY_KPRIGHTPAREN     180
#define ULINUX_KEY_NEW              181/*AC New*/
#define ULINUX_KEY_REDO             182/*AC Redo/Repeat*/

#define ULINUX_KEY_F13              183
#define ULINUX_KEY_F14              184
#define ULINUX_KEY_F15              185
#define ULINUX_KEY_F16              186
#define ULINUX_KEY_F17              187
#define ULINUX_KEY_F18              188
#define ULINUX_KEY_F19              189
#define ULINUX_KEY_F20              190
#define ULINUX_KEY_F21              191
#define ULINUX_KEY_F22              192
#define ULINUX_KEY_F23              193
#define ULINUX_KEY_F24              194

#define ULINUX_KEY_PLAYCD           200
#define ULINUX_KEY_PAUSECD          201
#define ULINUX_KEY_PROG3            202
#define ULINUX_KEY_PROG4            203
#define ULINUX_KEY_DASHBOARD        204/*AL Dashboard*/
#define ULINUX_KEY_SUSPEND          205
#define ULINUX_KEY_CLOSE            206/*AC Close*/
#define ULINUX_KEY_PLAY             207
#define ULINUX_KEY_FASTFORWARD      208
#define ULINUX_KEY_BASSBOOST        209
#define ULINUX_KEY_PRINT            210/*AC Print*/
#define ULINUX_KEY_HP               211
#define ULINUX_KEY_CAMERA           212
#define ULINUX_KEY_SOUND            213
#define ULINUX_KEY_QUESTION         214
#define ULINUX_KEY_EMAIL            215
#define ULINUX_KEY_CHAT             216
#define ULINUX_KEY_SEARCH           217
#define ULINUX_KEY_CONNECT          218
#define ULINUX_KEY_FINANCE          219/*AL Checkbook/Finance*/
#define ULINUX_KEY_SPORT            220
#define ULINUX_KEY_SHOP             221
#define ULINUX_KEY_ALTERASE         222
#define ULINUX_KEY_CANCEL           223/*AC Cancel*/
#define ULINUX_KEY_BRIGHTNESSDOWN   224
#define ULINUX_KEY_BRIGHTNESSUP     225
#define ULINUX_KEY_MEDIA            226

#define ULINUX_KEY_SWITCHVIDEOMODE  227/*Cycle between available video outputs
                                         (Monitor/LCD/TV-out/etc)*/
#define ULINUX_KEY_KBDILLUMTOGGLE   228
#define ULINUX_KEY_KBDILLUMDOWN     229
#define ULINUX_KEY_KBDILLUMUP       230

#define ULINUX_KEY_SEND             231/*AC Send*/
#define ULINUX_KEY_REPLY            232/*AC Reply*/
#define ULINUX_KEY_FORWARDMAIL      233/*AC Forward Msg*/
#define ULINUX_KEY_SAVE             234/*AC Save*/
#define ULINUX_KEY_DOCUMENTS        235

#define ULINUX_KEY_BATTERY          236

#define ULINUX_KEY_BLUETOOTH        237
#define ULINUX_KEY_WLAN             238
#define ULINUX_KEY_UWB              239

#define ULINUX_KEY_UNKNOWN          240

#define ULINUX_KEY_VIDEO_NEXT       241/*drive next video source*/
#define ULINUX_KEY_VIDEO_PREV       242/*drive previous video source*/
#define ULINUX_KEY_BRIGHTNESS_CYCLE 243/*brightness up, after max is min*/
#define ULINUX_KEY_BRIGHTNESS_ZERO  244/*brightness off, use ambient*/
#define ULINUX_KEY_DISPLAY_OFF      245/*display device to off state*/

#define ULINUX_KEY_WIMAX            246
#define ULINUX_KEY_RFKILL           247/*Key that controls all radios*/

#define ULINUX_KEY_MICMUTE          248/*Mute / unmute the microphone*/

/*Code 255 is reserved for special needs of AT keyboard driver*/

#define ULINUX_BTN_MISC             0x100
#define ULINUX_BTN_0                0x100
#define ULINUX_BTN_1                0x101
#define ULINUX_BTN_2                0x102
#define ULINUX_BTN_3                0x103
#define ULINUX_BTN_4                0x104
#define ULINUX_BTN_5                0x105
#define ULINUX_BTN_6                0x106
#define ULINUX_BTN_7                0x107
#define ULINUX_BTN_8                0x108
#define ULINUX_BTN_9                0x109

#define ULINUX_BTN_MOUSE            0x110
#define ULINUX_BTN_LEFT             0x110
#define ULINUX_BTN_RIGHT            0x111
#define ULINUX_BTN_MIDDLE           0x112
#define ULINUX_BTN_SIDE             0x113
#define ULINUX_BTN_EXTRA            0x114
#define ULINUX_BTN_FORWARD          0x115
#define ULINUX_BTN_BACK             0x116
#define ULINUX_BTN_TASK             0x117

#define ULINUX_BTN_JOYSTICK         0x120
#define ULINUX_BTN_TRIGGER          0x120
#define ULINUX_BTN_THUMB            0x121
#define ULINUX_BTN_THUMB2           0x122
#define ULINUX_BTN_TOP              0x123
#define ULINUX_BTN_TOP2             0x124
#define ULINUX_BTN_PINKIE           0x125
#define ULINUX_BTN_BASE             0x126
#define ULINUX_BTN_BASE2            0x127
#define ULINUX_BTN_BASE3            0x128
#define ULINUX_BTN_BASE4            0x129
#define ULINUX_BTN_BASE5            0x12a
#define ULINUX_BTN_BASE6            0x12b
#define ULINUX_BTN_DEAD             0x12f

#define ULINUX_BTN_GAMEPAD          0x130
#define ULINUX_BTN_A                0x130
#define ULINUX_BTN_B                0x131
#define ULINUX_BTN_C                0x132
#define ULINUX_BTN_X                0x133
#define ULINUX_BTN_Y                0x134
#define ULINUX_BTN_Z                0x135
#define ULINUX_BTN_TL               0x136
#define ULINUX_BTN_TR               0x137
#define ULINUX_BTN_TL2              0x138
#define ULINUX_BTN_TR2              0x139
#define ULINUX_BTN_SELECT           0x13a
#define ULINUX_BTN_START            0x13b
#define ULINUX_BTN_MODE             0x13c
#define ULINUX_BTN_THUMBL           0x13d
#define ULINUX_BTN_THUMBR           0x13e

#define ULINUX_BTN_DIGI             0x140
#define ULINUX_BTN_TOOL_PEN         0x140
#define ULINUX_BTN_TOOL_RUBBER      0x141
#define ULINUX_BTN_TOOL_BRUSH       0x142
#define ULINUX_BTN_TOOL_PENCIL      0x143
#define ULINUX_BTN_TOOL_AIRBRUSH    0x144
#define ULINUX_BTN_TOOL_FINGER      0x145
#define ULINUX_BTN_TOOL_MOUSE       0x146
#define ULINUX_BTN_TOOL_LENS        0x147
#define ULINUX_BTN_TOOL_QUINTTAP    0x148/*Five fingers on trackpad*/
#define ULINUX_BTN_TOUCH            0x14a
#define ULINUX_BTN_STYLUS           0x14b
#define ULINUX_BTN_STYLUS2          0x14c
#define ULINUX_BTN_TOOL_DOUBLETAP   0x14d
#define ULINUX_BTN_TOOL_TRIPLETAP   0x14e
#define ULINUX_BTN_TOOL_QUADTAP     0x14f/*Four fingers on trackpad*/

#define ULINUX_BTN_WHEEL            0x150
#define ULINUX_BTN_GEAR_DOWN        0x150
#define ULINUX_BTN_GEAR_UP          0x151

#define ULINUX_KEY_OK               0x160
#define ULINUX_KEY_SELECT           0x161
#define ULINUX_KEY_GOTO             0x162
#define ULINUX_KEY_CLEAR            0x163
#define ULINUX_KEY_POWER2           0x164
#define ULINUX_KEY_OPTION           0x165
#define ULINUX_KEY_INFO             0x166/*AL OEM Features/Tips/Tutorial*/
#define ULINUX_KEY_TIME             0x167
#define ULINUX_KEY_VENDOR           0x168
#define ULINUX_KEY_ARCHIVE          0x169
#define ULINUX_KEY_PROGRAM          0x16a/*Media Select Program Guide*/
#define ULINUX_KEY_CHANNEL          0x16b
#define ULINUX_KEY_FAVORITES        0x16c
#define ULINUX_KEY_EPG              0x16d
#define ULINUX_KEY_PVR              0x16e/*Media Select Home*/
#define ULINUX_KEY_MHP              0x16f
#define ULINUX_KEY_LANGUAGE         0x170
#define ULINUX_KEY_TITLE            0x171
#define ULINUX_KEY_SUBTITLE         0x172
#define ULINUX_KEY_ANGLE            0x173
#define ULINUX_KEY_ZOOM             0x174
#define ULINUX_KEY_MODE             0x175
#define ULINUX_KEY_KEYBOARD         0x176
#define ULINUX_KEY_SCREEN           0x177
#define ULINUX_KEY_PC               0x178/*Media Select Computer*/
#define ULINUX_KEY_TV               0x179/*Media Select TV*/
#define ULINUX_KEY_TV2              0x17a/*Media Select Cable*/
#define ULINUX_KEY_VCR              0x17b/*Media Select VCR*/
#define ULINUX_KEY_VCR2             0x17c/*VCR Plus*/
#define ULINUX_KEY_SAT              0x17d/*Media Select Satellite*/
#define ULINUX_KEY_SAT2             0x17e
#define ULINUX_KEY_CD               0x17f/*Media Select CD*/
#define ULINUX_KEY_TAPE             0x180/*Media Select Tape*/
#define ULINUX_KEY_RADIO            0x181
#define ULINUX_KEY_TUNER            0x182/*Media Select Tuner*/
#define ULINUX_KEY_PLAYER           0x183
#define ULINUX_KEY_TEXT             0x184
#define ULINUX_KEY_DVD              0x185/*Media Select DVD*/
#define ULINUX_KEY_AUX              0x186
#define ULINUX_KEY_MP3              0x187
#define ULINUX_KEY_AUDIO            0x188/*AL Audio Browser*/
#define ULINUX_KEY_VIDEO            0x189/*AL Movie Browser*/
#define ULINUX_KEY_DIRECTORY        0x18a
#define ULINUX_KEY_LIST             0x18b
#define ULINUX_KEY_MEMO             0x18c/*Media Select Messages*/
#define ULINUX_KEY_CALENDAR         0x18d
#define ULINUX_KEY_RED              0x18e
#define ULINUX_KEY_GREEN            0x18f
#define ULINUX_KEY_YELLOW           0x190
#define ULINUX_KEY_BLUE             0x191
#define ULINUX_KEY_CHANNELUP        0x192/*Channel Increment*/
#define ULINUX_KEY_CHANNELDOWN      0x193/*Channel Decrement*/
#define ULINUX_KEY_FIRST            0x194
#define ULINUX_KEY_LAST             0x195/*Recall Last*/
#define ULINUX_KEY_AB               0x196
#define ULINUX_KEY_NEXT             0x197
#define ULINUX_KEY_RESTART          0x198
#define ULINUX_KEY_SLOW             0x199
#define ULINUX_KEY_SHUFFLE          0x19a
#define ULINUX_KEY_BREAK            0x19b
#define ULINUX_KEY_PREVIOUS         0x19c
#define ULINUX_KEY_DIGITS           0x19d
#define ULINUX_KEY_TEEN             0x19e
#define ULINUX_KEY_TWEN             0x19f
#define ULINUX_KEY_VIDEOPHONE       0x1a0/*Media Select Video Phone*/
#define ULINUX_KEY_GAMES            0x1a1/*Media Select Games*/
#define ULINUX_KEY_ZOOMIN           0x1a2/*AC Zoom In*/
#define ULINUX_KEY_ZOOMOUT          0x1a3/*AC Zoom Out*/
#define ULINUX_KEY_ZOOMRESET        0x1a4/*AC Zoom*/
#define ULINUX_KEY_WORDPROCESSOR    0x1a5/*AL Word Processor*/
#define ULINUX_KEY_EDITOR           0x1a6/*AL Text Editor*/
#define ULINUX_KEY_SPREADSHEET      0x1a7/*AL Spreadsheet*/
#define ULINUX_KEY_GRAPHICSEDITOR   0x1a8/*AL Graphics Editor*/
#define ULINUX_KEY_PRESENTATION     0x1a9/*AL Presentation App*/
#define ULINUX_KEY_DATABASE         0x1aa/*AL Database App*/
#define ULINUX_KEY_NEWS             0x1ab/*AL Newsreader*/
#define ULINUX_KEY_VOICEMAIL        0x1ac/*AL Voicemail*/
#define ULINUX_KEY_ADDRESSBOOK      0x1ad/*AL Contacts/Address Book*/
#define ULINUX_KEY_MESSENGER        0x1ae/*AL Instant Messaging*/
#define ULINUX_KEY_DISPLAYTOGGLE    0x1af/*Turn display (LCD) on and off*/
#define ULINUX_KEY_SPELLCHECK       0x1b0/*AL Spell Check*/
#define ULINUX_KEY_LOGOFF           0x1b1/*AL Logoff*/

#define ULINUX_KEY_DOLLAR           0x1b2
#define ULINUX_KEY_EURO             0x1b3

#define ULINUX_KEY_FRAMEBACK        0x1b4/*Consumer - transport controls*/
#define ULINUX_KEY_FRAMEFORWARD     0x1b5
#define ULINUX_KEY_CONTEXT_MENU     0x1b6/*GenDesc - system context menu*/
#define ULINUX_KEY_MEDIA_REPEAT     0x1b7/*Consumer - transport control*/
#define ULINUX_KEY_10CHANNELSUP     0x1b8/*10 channels up (10+)*/
#define ULINUX_KEY_10CHANNELSDOWN   0x1b9/*10 channels down (10-)*/
#define ULINUX_KEY_IMAGES           0x1ba/*AL Image Browser*/

#define ULINUX_KEY_DEL_EOL          0x1c0
#define ULINUX_KEY_DEL_EOS          0x1c1
#define ULINUX_KEY_INS_LINE         0x1c2
#define ULINUX_KEY_DEL_LINE         0x1c3

#define ULINUX_KEY_FN               0x1d0
#define ULINUX_KEY_FN_ESC           0x1d1
#define ULINUX_KEY_FN_F1            0x1d2
#define ULINUX_KEY_FN_F2            0x1d3
#define ULINUX_KEY_FN_F3            0x1d4
#define ULINUX_KEY_FN_F4            0x1d5
#define ULINUX_KEY_FN_F5            0x1d6
#define ULINUX_KEY_FN_F6            0x1d7
#define ULINUX_KEY_FN_F7            0x1d8
#define ULINUX_KEY_FN_F8            0x1d9
#define ULINUX_KEY_FN_F9            0x1da
#define ULINUX_KEY_FN_F10           0x1db
#define ULINUX_KEY_FN_F11           0x1dc
#define ULINUX_KEY_FN_F12           0x1dd
#define ULINUX_KEY_FN_1             0x1de
#define ULINUX_KEY_FN_2             0x1df
#define ULINUX_KEY_FN_D             0x1e0
#define ULINUX_KEY_FN_E             0x1e1
#define ULINUX_KEY_FN_F             0x1e2
#define ULINUX_KEY_FN_S             0x1e3
#define ULINUX_KEY_FN_B             0x1e4

#define ULINUX_KEY_BRL_DOT1         0x1f1
#define ULINUX_KEY_BRL_DOT2         0x1f2
#define ULINUX_KEY_BRL_DOT3         0x1f3
#define ULINUX_KEY_BRL_DOT4         0x1f4
#define ULINUX_KEY_BRL_DOT5         0x1f5
#define ULINUX_KEY_BRL_DOT6         0x1f6
#define ULINUX_KEY_BRL_DOT7         0x1f7
#define ULINUX_KEY_BRL_DOT8         0x1f8
#define ULINUX_KEY_BRL_DOT9         0x1f9
#define ULINUX_KEY_BRL_DOT10        0x1fa

#define ULINUX_KEY_NUMERIC_0        0x200/*used by phones, remote controls,*/
#define ULINUX_KEY_NUMERIC_1        0x201/*and other keypads*/
#define ULINUX_KEY_NUMERIC_2        0x202
#define ULINUX_KEY_NUMERIC_3        0x203
#define ULINUX_KEY_NUMERIC_4        0x204
#define ULINUX_KEY_NUMERIC_5        0x205
#define ULINUX_KEY_NUMERIC_6        0x206
#define ULINUX_KEY_NUMERIC_7        0x207
#define ULINUX_KEY_NUMERIC_8        0x208
#define ULINUX_KEY_NUMERIC_9        0x209
#define ULINUX_KEY_NUMERIC_STAR     0x20a
#define ULINUX_KEY_NUMERIC_POUND    0x20b

#define ULINUX_KEY_CAMERA_FOCUS     0x210
#define ULINUX_KEY_WPS_BUTTON       0x211/*WiFi Protected Setup key*/

#define ULINUX_KEY_TOUCHPAD_TOGGLE  0x212/*Request switch touchpad on or off*/
#define ULINUX_KEY_TOUCHPAD_ON      0x213
#define ULINUX_KEY_TOUCHPAD_OFF     0x214

#define ULINUX_KEY_CAMERA_ZOOMIN    0x215
#define ULINUX_KEY_CAMERA_ZOOMOUT   0x216
#define ULINUX_KEY_CAMERA_UP        0x217
#define ULINUX_KEY_CAMERA_DOWN      0x218
#define ULINUX_KEY_CAMERA_LEFT      0x219
#define ULINUX_KEY_CAMERA_RIGHT     0x21a

#define ULINUX_BTN_TRIGGER_HAPPY    0x2c0
#define ULINUX_BTN_TRIGGER_HAPPY1   0x2c0
#define ULINUX_BTN_TRIGGER_HAPPY2   0x2c1
#define ULINUX_BTN_TRIGGER_HAPPY3   0x2c2
#define ULINUX_BTN_TRIGGER_HAPPY4   0x2c3
#define ULINUX_BTN_TRIGGER_HAPPY5   0x2c4
#define ULINUX_BTN_TRIGGER_HAPPY6   0x2c5
#define ULINUX_BTN_TRIGGER_HAPPY7   0x2c6
#define ULINUX_BTN_TRIGGER_HAPPY8   0x2c7
#define ULINUX_BTN_TRIGGER_HAPPY9   0x2c8
#define ULINUX_BTN_TRIGGER_HAPPY10  0x2c9
#define ULINUX_BTN_TRIGGER_HAPPY11  0x2ca
#define ULINUX_BTN_TRIGGER_HAPPY12  0x2cb
#define ULINUX_BTN_TRIGGER_HAPPY13  0x2cc
#define ULINUX_BTN_TRIGGER_HAPPY14  0x2cd
#define ULINUX_BTN_TRIGGER_HAPPY15  0x2ce
#define ULINUX_BTN_TRIGGER_HAPPY16  0x2cf
#define ULINUX_BTN_TRIGGER_HAPPY17  0x2d0
#define ULINUX_BTN_TRIGGER_HAPPY18  0x2d1
#define ULINUX_BTN_TRIGGER_HAPPY19  0x2d2
#define ULINUX_BTN_TRIGGER_HAPPY20  0x2d3
#define ULINUX_BTN_TRIGGER_HAPPY21  0x2d4
#define ULINUX_BTN_TRIGGER_HAPPY22  0x2d5
#define ULINUX_BTN_TRIGGER_HAPPY23  0x2d6
#define ULINUX_BTN_TRIGGER_HAPPY24  0x2d7
#define ULINUX_BTN_TRIGGER_HAPPY25  0x2d8
#define ULINUX_BTN_TRIGGER_HAPPY26  0x2d9
#define ULINUX_BTN_TRIGGER_HAPPY27  0x2da
#define ULINUX_BTN_TRIGGER_HAPPY28  0x2db
#define ULINUX_BTN_TRIGGER_HAPPY29  0x2dc
#define ULINUX_BTN_TRIGGER_HAPPY30  0x2dd
#define ULINUX_BTN_TRIGGER_HAPPY31  0x2de
#define ULINUX_BTN_TRIGGER_HAPPY32  0x2df
#define ULINUX_BTN_TRIGGER_HAPPY33  0x2e0
#define ULINUX_BTN_TRIGGER_HAPPY34  0x2e1
#define ULINUX_BTN_TRIGGER_HAPPY35  0x2e2
#define ULINUX_BTN_TRIGGER_HAPPY36  0x2e3
#define ULINUX_BTN_TRIGGER_HAPPY37  0x2e4
#define ULINUX_BTN_TRIGGER_HAPPY38  0x2e5
#define ULINUX_BTN_TRIGGER_HAPPY39  0x2e6
#define ULINUX_BTN_TRIGGER_HAPPY40  0x2e7

/*We avoid low common keys in module aliases so they don't get huge.*/
#define ULINUX_KEY_MIN_INTERESTING  ULINUX_KEY_MUTE
#define ULINUX_KEY_MAX              0x2ff
#define ULINUX_KEY_CNT              (ULINUX_KEY_MAX+1)

/*relative axes bit offsets*/
#define ULINUX_REL_X      0x00
#define ULINUX_REL_Y      0x01
#define ULINUX_REL_Z      0x02
#define ULINUX_REL_RX     0x03
#define ULINUX_REL_RY     0x04
#define ULINUX_REL_RZ     0x05
#define ULINUX_REL_HWHEEL 0x06
#define ULINUX_REL_DIAL   0x07
#define ULINUX_REL_WHEEL  0x08
#define ULINUX_REL_MISC   0x09
#define ULINUX_REL_MAX    0x0f
#define ULINUX_REL_CNT    (ULINUX_REL_MAX+1)

/*Absolute axes bit offsets*/
#define ULINUX_ABS_X              0x00
#define ULINUX_ABS_Y              0x01
#define ULINUX_ABS_Z              0x02
#define ULINUX_ABS_RX             0x03
#define ULINUX_ABS_RY             0x04
#define ULINUX_ABS_RZ             0x05
#define ULINUX_ABS_THROTTLE       0x06
#define ULINUX_ABS_RUDDER         0x07
#define ULINUX_ABS_WHEEL          0x08
#define ULINUX_ABS_GAS            0x09
#define ULINUX_ABS_BRAKE          0x0a
#define ULINUX_ABS_HAT0X          0x10
#define ULINUX_ABS_HAT0Y          0x11
#define ULINUX_ABS_HAT1X          0x12
#define ULINUX_ABS_HAT1Y          0x13
#define ULINUX_ABS_HAT2X          0x14
#define ULINUX_ABS_HAT2Y          0x15
#define ULINUX_ABS_HAT3X          0x16
#define ULINUX_ABS_HAT3Y          0x17
#define ULINUX_ABS_PRESSURE       0x18
#define ULINUX_ABS_DISTANCE       0x19
#define ULINUX_ABS_TILT_X         0x1a
#define ULINUX_ABS_TILT_Y         0x1b
#define ULINUX_ABS_TOOL_WIDTH     0x1c

#define ULINUX_ABS_VOLUME         0x20

#define ULINUX_ABS_MISC           0x28

#define ULINUX_ABS_MT_SLOT        0x2f/*MT slot being modified*/
#define ULINUX_ABS_MT_TOUCH_MAJOR 0x30/*Major axis of touching ellipse*/
#define ULINUX_ABS_MT_TOUCH_MINOR 0x31/*Minor axis (omit if circular)*/
#define ULINUX_ABS_MT_WIDTH_MAJOR 0x32/*Major axis of approaching ellipse*/
#define ULINUX_ABS_MT_WIDTH_MINOR 0x33/*Minor axis (omit if circular)*/
#define ULINUX_ABS_MT_ORIENTATION 0x34/*Ellipse orientation*/
#define ULINUX_ABS_MT_POSITION_X  0x35/*Center X touch position*/
#define ULINUX_ABS_MT_POSITION_Y  0x36/*Center Y touch position*/
#define ULINUX_ABS_MT_TOOL_TYPE   0x37/*Type of touching device*/
#define ULINUX_ABS_MT_BLOB_ID     0x38/*Group a set of packets as a blob*/
#define ULINUX_ABS_MT_TRACKING_ID 0x39/*Unique ID of initiated contact*/
#define ULINUX_ABS_MT_PRESSURE    0x3a/*Pressure on contact area*/
#define ULINUX_ABS_MT_DISTANCE    0x3b/*Contact hover distance*/
#define ULINUX_ABS_MT_TOOL_X      0x3c/*Center X tool position*/
#define ULINUX_ABS_MT_TOOL_Y      0x3d/*Center Y tool position*/

#define ULINUX_ABS_MAX            0x3f
#define ULINUX_ABS_CNT           (ULINUX_ABS_MAX+1)

/*Misc events bit offsets*/
#define ULINUX_MSC_SERIAL         0x00
#define ULINUX_MSC_PULSELED       0x01
#define ULINUX_MSC_GESTURE        0x02
#define ULINUX_MSC_RAW            0x03
#define ULINUX_MSC_SCAN           0x04
#define ULINUX_MSC_MAX            0x07
#define ULINUX_MSC_CNT            (ULINUX_MSC_MAX+1)

/*Switch events bit offsets*/
#define ULINUX_SW_LID                  0x00/*set = lid shut*/
#define ULINUX_SW_TABLET_MODE          0x01/*set = tablet mode*/
#define ULINUX_SW_HEADPHONE_INSERT     0x02/*set = inserted*/
#define ULINUX_SW_RFKILL_ALL           0x03/*rfkill master switch, type "any"*/
                                       /*set = radio enabled*/
#define ULINUX_SW_RADIO                ULINUX_SW_RFKILL_ALL/*deprecated*/
#define ULINUX_SW_MICROPHONE_INSERT    0x04/*set = inserted*/
#define ULINUX_SW_DOCK                 0x05/*set = plugged into dock*/
#define ULINUX_SW_LINEOUT_INSERT       0x06/*set = inserted*/
#define ULINUX_SW_JACK_PHYSICAL_INSERT 0x07/*set = mechanical switch set*/
#define ULINUX_SW_VIDEOOUT_INSERT      0x08/*set = inserted*/
#define ULINUX_SW_CAMERA_LENS_COVER    0x09/*set = lens covered*/
#define ULINUX_SW_KEYPAD_SLIDE         0x0a/*set = keypad slide out*/
#define ULINUX_SW_FRONT_PROXIMITY      0x0b/*set = front proximity sensor
                                                   active*/
#define ULINUX_SW_ROTATE_LOCK          0x0c/*set = rotate locked/disabled*/
#define ULINUX_SW_LINEIN_INSERT        0x0d/*set = inserted*/
#define ULINUX_SW_MAX                  0x0f
#define ULINUX_SW_CNT                  (ULINUX_SW_MAX+1)

/*LEDs bit offsets*/
#define ULINUX_LED_NUML     0x00
#define ULINUX_LED_CAPSL    0x01
#define ULINUX_LED_SCROLLL  0x02
#define ULINUX_LED_COMPOSE  0x03
#define ULINUX_LED_KANA     0x04
#define ULINUX_LED_SLEEP    0x05
#define ULINUX_LED_SUSPEND  0x06
#define ULINUX_LED_MUTE     0x07
#define ULINUX_LED_MISC     0x08
#define ULINUX_LED_MAIL     0x09
#define ULINUX_LED_CHARGING 0x0a
#define ULINUX_LED_MAX      0x0f
#define ULINUX_LED_CNT      (ULINUX_LED_MAX+1)

/*Sounds bit offsets*/
#define ULINUX_SND_CLICK 0x00
#define ULINUX_SND_BELL  0x01
#define ULINUX_SND_TONE  0x02
#define ULINUX_SND_MAX   0x07
#define ULINUX_SND_CNT   (ULINUX_SND_MAX+1)

/*ff status of a force-feedback effect offset bits*/
#define ULINUX_FF_STATUS_STOPPED 0x00
#define ULINUX_FF_STATUS_PLAYING 0x01
#define ULINUX_FF_STATUS_MAX     0x01

/*Force feedback effect types*/
#define ULINUX_FF_RUMBLE       0x50
#define ULINUX_FF_PERIODIC     0x51
#define ULINUX_FF_CONSTANT     0x52
#define ULINUX_FF_SPRING       0x53
#define ULINUX_FF_FRICTION     0x54
#define ULINUX_FF_DAMPER       0x55
#define ULINUX_FF_INERTIA      0x56
#define ULINUX_FF_RAMP         0x57

#define ULINUX_FF_EFFECT_MIN   FF_RUMBLE
#define ULINUX_FF_EFFECT_MAX   FF_RAMP

/*Force feedback periodic effect types*/
#define ULINUX_FF_SQUARE       0x58
#define ULINUX_FF_TRIANGLE     0x59
#define ULINUX_FF_SINE         0x5a
#define ULINUX_FF_SAW_UP       0x5b
#define ULINUX_FF_SAW_DOWN     0x5c
#define ULINUX_FF_CUSTOM       0x5d

#define ULINUX_FF_WAVEFORM_MIN FF_SQUARE
#define ULINUX_FF_WAVEFORM_MAX FF_CUSTOM

/*Set ff device properties*/
#define ULINUX_FF_GAIN         0x60
#define ULINUX_FF_AUTOCENTER   0x61

#define ULINUX_FF_MAX          0x7f
#define ULINUX_FF_CNT          (ULINUX_FF_MAX+1)
#endif
