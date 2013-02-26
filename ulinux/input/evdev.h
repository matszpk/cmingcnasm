#ifndef ULINUX_INPUT_EVDEV_H
#define ULINUX_INPUT_EVDEV_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************

//this is a derived version of linux headers for ulinux

struct k_input_event{
  struct k_timeval time;
  k_u16 type;
  k_u16 code;
  k_s32 value;
};

struct k_input_id{
  k_u16 bustype;
  k_u16 vendor;
  k_u16 product;
  k_u16 version;
};

//struct input_keymap_entry - used by EVIOCGKEYCODE/EVIOCSKEYCODE ioctls
//@scancode: scancode represented in machine-endian form.
//@len: length of the scancode that resides in @scancode buffer.
//@index: index in the keymap, may be used instead of scancode
//@flags: allows to specify how kernel should handle the request. For
// example, setting INPUT_KEYMAP_BY_INDEX flag indicates that kernel
// should perform lookup in keymap by @index instead of @scancode
//@keycode: key code assigned to this scancode
//
//The structure is used to retrieve and modify keymap data. Users have
//option of performing lookup either by @scancode itself or by @index
//in keymap entry. EVIOCGKEYCODE will also return scancode or index
//(depending on which element was used to perform lookup).

struct k_input_keymap_entry{
  #define INPUT_KEYMAP_BY_INDEX (1<<0)
  k_u8  flags;
  k_u8  len;
  k_u16 index;
  k_u32 keycode;
  k_u8  scancode[32];
};

#define K_EVIOCGVERSION	K_IOR('E',0x01,int)//get driver version
#define K_EVIOCGID      K_IOR('E',0x02,struct k_input_id)//get device ID 
#define K_EVIOCGREP     K_IOR('E',0x03,unsigned int[2])//get repeat settings
#define K_EVIOCSREP     K_IOW('E',0x03,unsigned int[2])//set repeat settings

#define K_EVIOCGKEYCODE    K_IOR('E',0x04,unsigned int[2])//get keycode
#define K_EVIOCGKEYCODE_V2 K_IOR('E',0x04,struct k_input_keymap_entry)
#define K_EVIOCSKEYCODE    K_IOW('E',0x04,unsigned int[2])//set keycode
#define K_EVIOCSKEYCODE_V2 K_IOW('E',0x04,struct k_input_keymap_entry)

#define K_EVIOCGNAME(len) K_IOC(K_IOC_READ,'E',0x06,len)//get device name
#define K_EVIOCGPHYS(len) K_IOC(K_IOC_READ,'E',0x07,len)//get physical location
#define K_EVIOCGUNIQ(len) K_IOC(K_IOC_READ,'E',0x08,len)//get unique identifier
#define K_EVIOCGPROP(len) K_IOC(K_IOC_READ,'E',0x09,len)//get device properties

#define K_EVIOCGBIT(ev,len) K_IOC(K_IOC_READ,'E',0x20+(ev),len)//get event bits

//event types bit offsets
#define K_EV_SYN       0x00
#define K_EV_KEY       0x01
#define K_EV_REL       0x02
#define K_EV_ABS       0x03
#define K_EV_MSC       0x04
#define K_EV_SW        0x05
#define K_EV_LED       0x11
#define K_EV_SND       0x12
#define K_EV_REP       0x14
#define K_EV_FF        0x15
#define K_EV_PWR       0x16
#define K_EV_FF_STATUS 0x17
#define K_EV_MAX       0x1f
#define K_EV_CNT       (K_EV_MAX+1)

//Synchronization events.
#define K_SYN_REPORT    0
#define K_SYN_CONFIG    1
#define K_SYN_MT_REPORT 2
#define K_SYN_DROPPED   3

//Keys and buttons
//
//Most of the keys/buttons are modeled after USB HUT 1.12
//(see http://www.usb.org/developers/hidpage).
//Abbreviations in the comments:
//AC - Application Control
//AL - Application Launch Button
//SC - System Control
#define K_KEY_RESERVED         0
#define K_KEY_ESC              1
#define K_KEY_1                2
#define K_KEY_2                3
#define K_KEY_3                4
#define K_KEY_4                5
#define K_KEY_5                6
#define K_KEY_6                7
#define K_KEY_7                8
#define K_KEY_8                9
#define K_KEY_9                10
#define K_KEY_0                11
#define K_KEY_MINUS            12
#define K_KEY_EQUAL            13
#define K_KEY_BACKSPACE        14
#define K_KEY_TAB              15
#define K_KEY_Q                16
#define K_KEY_W                17
#define K_KEY_E                18
#define K_KEY_R                19
#define K_KEY_T                20
#define K_KEY_Y                21
#define K_KEY_U                22
#define K_KEY_I                23
#define K_KEY_O                24
#define K_KEY_P                25
#define K_KEY_LEFTBRACE        26
#define K_KEY_RIGHTBRACE       27
#define K_KEY_ENTER            28
#define K_KEY_LEFTCTRL         29
#define K_KEY_A                30
#define K_KEY_S                31
#define K_KEY_D                32
#define K_KEY_F                33
#define K_KEY_G                34
#define K_KEY_H                35
#define K_KEY_J                36
#define K_KEY_K                37
#define K_KEY_L                38
#define K_KEY_SEMICOLON        39
#define K_KEY_APOSTROPHE       40
#define K_KEY_GRAVE            41
#define K_KEY_LEFTSHIFT        42
#define K_KEY_BACKSLASH        43
#define K_KEY_Z                44
#define K_KEY_X                45
#define K_KEY_C                46
#define K_KEY_V                47
#define K_KEY_B                48
#define K_KEY_N                49
#define K_KEY_M                50
#define K_KEY_COMMA            51
#define K_KEY_DOT              52
#define K_KEY_SLASH            53
#define K_KEY_RIGHTSHIFT       54
#define K_KEY_KPASTERISK       55
#define K_KEY_LEFTALT          56
#define K_KEY_SPACE            57
#define K_KEY_CAPSLOCK         58
#define K_KEY_F1               59
#define K_KEY_F2               60
#define K_KEY_F3               61
#define K_KEY_F4               62
#define K_KEY_F5               63
#define K_KEY_F6               64
#define K_KEY_F7               65
#define K_KEY_F8               66
#define K_KEY_F9               67
#define K_KEY_F10              68
#define K_KEY_NUMLOCK          69
#define K_KEY_SCROLLLOCK       70
#define K_KEY_KP7              71
#define K_KEY_KP8              72
#define K_KEY_KP9              73
#define K_KEY_KPMINUS          74
#define K_KEY_KP4              75
#define K_KEY_KP5              76
#define K_KEY_KP6              77
#define K_KEY_KPPLUS           78
#define K_KEY_KP1              79
#define K_KEY_KP2              80
#define K_KEY_KP3              81
#define K_KEY_KP0              82
#define K_KEY_KPDOT            83

#define K_KEY_ZENKAKUHANKAKU   85
#define K_KEY_102ND            86
#define K_KEY_F11              87
#define K_KEY_F12              88
#define K_KEY_RO               89
#define K_KEY_KATAKANA         90
#define K_KEY_HIRAGANA         91
#define K_KEY_HENKAN           92
#define K_KEY_KATAKANAHIRAGANA 93
#define K_KEY_MUHENKAN         94
#define K_KEY_KPJPCOMMA        95
#define K_KEY_KPENTER          96
#define K_KEY_RIGHTCTRL        97
#define K_KEY_KPSLASH          98
#define K_KEY_SYSRQ            99
#define K_KEY_RIGHTALT         100
#define K_KEY_LINEFEED         101
#define K_KEY_HOME             102
#define K_KEY_UP               103
#define K_KEY_PAGEUP           104
#define K_KEY_LEFT             105
#define K_KEY_RIGHT            106
#define K_KEY_END              107
#define K_KEY_DOWN             108
#define K_KEY_PAGEDOWN         109
#define K_KEY_INSERT           110
#define K_KEY_DELETE           111
#define K_KEY_MACRO            112
#define K_KEY_MUTE             113
#define K_KEY_VOLUMEDOWN       114
#define K_KEY_VOLUMEUP         115
#define K_KEY_POWER            116//SC System Power Down
#define K_KEY_KPEQUAL          117
#define K_KEY_KPPLUSMINUS      118
#define K_KEY_PAUSE            119
#define K_KEY_SCALE            120//AL Compiz Scale (Expose)

#define K_KEY_KPCOMMA          121
#define K_KEY_HANGEUL          122
#define K_KEY_HANGUEL          K_KEY_HANGEUL
#define K_KEY_HANJA            123
#define K_KEY_YEN              124
#define K_KEY_LEFTMETA         125
#define K_KEY_RIGHTMETA        126
#define K_KEY_COMPOSE          127

#define K_KEY_STOP             128//AC Stop
#define K_KEY_AGAIN            129
#define K_KEY_PROPS            130//AC Properties
#define K_KEY_UNDO             131//AC Undo
#define K_KEY_FRONT            132
#define K_KEY_COPY             133//AC Copy 
#define K_KEY_OPEN             134//AC Open
#define K_KEY_PASTE            135//AC Paste
#define K_KEY_FIND             136//AC Search
#define K_KEY_CUT              137//AC Cut
#define K_KEY_HELP             138//AL Integrated Help Center
#define K_KEY_MENU             139//Menu (show menu)
#define K_KEY_CALC             140//AL Calculator
#define K_KEY_SETUP            141
#define K_KEY_SLEEP            142//SC System Sleep
#define K_KEY_WAKEUP           143//System Wake Up
#define K_KEY_FILE             144//AL Local Machine Browser
#define K_KEY_SENDFILE         145
#define K_KEY_DELETEFILE       146
#define K_KEY_XFER             147
#define K_KEY_PROG1            148
#define K_KEY_PROG2            149
#define K_KEY_WWW              150//AL Internet Browser
#define K_KEY_MSDOS            151
#define K_KEY_COFFEE           152//AL Terminal Lock/Screensaver
#define K_KEY_SCREENLOCK       K_KEY_COFFEE
#define K_KEY_DIRECTION        153
#define K_KEY_CYCLEWINDOWS     154
#define K_KEY_MAIL             155
#define K_KEY_BOOKMARKS        156//AC Bookmarks
#define K_KEY_COMPUTER         157
#define K_KEY_BACK             158//AC Back
#define K_KEY_FORWARD          159//AC Forward
#define K_KEY_CLOSECD          160
#define K_KEY_EJECTCD          161
#define K_KEY_EJECTCLOSECD     162
#define K_KEY_NEXTSONG         163
#define K_KEY_PLAYPAUSE        164
#define K_KEY_PREVIOUSSONG     165
#define K_KEY_STOPCD           166
#define K_KEY_RECORD           167
#define K_KEY_REWIND           168
#define K_KEY_PHONE            169//Media Select Telephone
#define K_KEY_ISO              170
#define K_KEY_CONFIG           171//AL Consumer Control Configuration
#define K_KEY_HOMEPAGE         172//AC Home
#define K_KEY_REFRESH          173//AC Refresh
#define K_KEY_EXIT             174//AC Exit
#define K_KEY_MOVE             175
#define K_KEY_EDIT             176
#define K_KEY_SCROLLUP         177
#define K_KEY_SCROLLDOWN       178
#define K_KEY_KPLEFTPAREN      179
#define K_KEY_KPRIGHTPAREN     180
#define K_KEY_NEW              181//AC New
#define K_KEY_REDO             182//AC Redo/Repeat

#define K_KEY_F13              183
#define K_KEY_F14              184
#define K_KEY_F15              185
#define K_KEY_F16              186
#define K_KEY_F17              187
#define K_KEY_F18              188
#define K_KEY_F19              189
#define K_KEY_F20              190
#define K_KEY_F21              191
#define K_KEY_F22              192
#define K_KEY_F23              193
#define K_KEY_F24              194

#define K_KEY_PLAYCD           200
#define K_KEY_PAUSECD          201
#define K_KEY_PROG3            202
#define K_KEY_PROG4            203
#define K_KEY_DASHBOARD        204//AL Dashboard
#define K_KEY_SUSPEND          205
#define K_KEY_CLOSE            206//AC Close
#define K_KEY_PLAY             207
#define K_KEY_FASTFORWARD      208
#define K_KEY_BASSBOOST        209
#define K_KEY_PRINT            210//AC Print
#define K_KEY_HP               211
#define K_KEY_CAMERA           212
#define K_KEY_SOUND            213
#define K_KEY_QUESTION         214
#define K_KEY_EMAIL            215
#define K_KEY_CHAT             216
#define K_KEY_SEARCH           217
#define K_KEY_CONNECT          218
#define K_KEY_FINANCE          219//AL Checkbook/Finance
#define K_KEY_SPORT            220
#define K_KEY_SHOP             221
#define K_KEY_ALTERASE         222
#define K_KEY_CANCEL           223//AC Cancel
#define K_KEY_BRIGHTNESSDOWN   224
#define K_KEY_BRIGHTNESSUP     225
#define K_KEY_MEDIA            226

#define K_KEY_SWITCHVIDEOMODE  227//Cycle between available video outputs
                                  //(Monitor/LCD/TV-out/etc)
#define K_KEY_KBDILLUMTOGGLE   228
#define K_KEY_KBDILLUMDOWN     229
#define K_KEY_KBDILLUMUP       230

#define K_KEY_SEND             231//AC Send
#define K_KEY_REPLY            232//AC Reply
#define K_KEY_FORWARDMAIL      233//AC Forward Msg
#define K_KEY_SAVE             234//AC Save
#define K_KEY_DOCUMENTS        235

#define K_KEY_BATTERY          236

#define K_KEY_BLUETOOTH        237
#define K_KEY_WLAN             238
#define K_KEY_UWB              239

#define K_KEY_UNKNOWN          240

#define K_KEY_VIDEO_NEXT       241//drive next video source
#define K_KEY_VIDEO_PREV       242//drive previous video source
#define K_KEY_BRIGHTNESS_CYCLE 243//brightness up, after max is min
#define K_KEY_BRIGHTNESS_ZERO  244//brightness off, use ambient
#define K_KEY_DISPLAY_OFF      245//display device to off state

#define K_KEY_WIMAX            246
#define K_KEY_RFKILL           247//Key that controls all radios

#define K_KEY_MICMUTE          248//Mute / unmute the microphone

//Code 255 is reserved for special needs of AT keyboard driver

#define K_BTN_MISC             0x100
#define K_BTN_0                0x100
#define K_BTN_1                0x101
#define K_BTN_2                0x102
#define K_BTN_3                0x103
#define K_BTN_4                0x104
#define K_BTN_5                0x105
#define K_BTN_6                0x106
#define K_BTN_7                0x107
#define K_BTN_8                0x108
#define K_BTN_9                0x109

#define K_BTN_MOUSE            0x110
#define K_BTN_LEFT             0x110
#define K_BTN_RIGHT            0x111
#define K_BTN_MIDDLE           0x112
#define K_BTN_SIDE             0x113
#define K_BTN_EXTRA            0x114
#define K_BTN_FORWARD          0x115
#define K_BTN_BACK             0x116
#define K_BTN_TASK             0x117

#define K_BTN_JOYSTICK         0x120
#define K_BTN_TRIGGER          0x120
#define K_BTN_THUMB            0x121
#define K_BTN_THUMB2           0x122
#define K_BTN_TOP              0x123
#define K_BTN_TOP2             0x124
#define K_BTN_PINKIE           0x125
#define K_BTN_BASE             0x126
#define K_BTN_BASE2            0x127
#define K_BTN_BASE3            0x128
#define K_BTN_BASE4            0x129
#define K_BTN_BASE5            0x12a
#define K_BTN_BASE6            0x12b
#define K_BTN_DEAD             0x12f

#define K_BTN_GAMEPAD          0x130
#define K_BTN_A                0x130
#define K_BTN_B                0x131
#define K_BTN_C                0x132
#define K_BTN_X                0x133
#define K_BTN_Y                0x134
#define K_BTN_Z                0x135
#define K_BTN_TL               0x136
#define K_BTN_TR               0x137
#define K_BTN_TL2              0x138
#define K_BTN_TR2              0x139
#define K_BTN_SELECT           0x13a
#define K_BTN_START            0x13b
#define K_BTN_MODE             0x13c
#define K_BTN_THUMBL           0x13d
#define K_BTN_THUMBR           0x13e

#define K_BTN_DIGI             0x140
#define K_BTN_TOOL_PEN         0x140
#define K_BTN_TOOL_RUBBER      0x141
#define K_BTN_TOOL_BRUSH       0x142
#define K_BTN_TOOL_PENCIL      0x143
#define K_BTN_TOOL_AIRBRUSH    0x144
#define K_BTN_TOOL_FINGER      0x145
#define K_BTN_TOOL_MOUSE       0x146
#define K_BTN_TOOL_LENS        0x147
#define K_BTN_TOOL_QUINTTAP    0x148//Five fingers on trackpad 
#define K_BTN_TOUCH	           0x14a
#define K_BTN_STYLUS           0x14b
#define K_BTN_STYLUS2          0x14c
#define K_BTN_TOOL_DOUBLETAP   0x14d
#define K_BTN_TOOL_TRIPLETAP   0x14e
#define K_BTN_TOOL_QUADTAP     0x14f//Four fingers on trackpad

#define K_BTN_WHEEL            0x150
#define K_BTN_GEAR_DOWN        0x150
#define K_BTN_GEAR_UP          0x151

#define K_KEY_OK               0x160
#define K_KEY_SELECT           0x161
#define K_KEY_GOTO             0x162
#define K_KEY_CLEAR            0x163
#define K_KEY_POWER2           0x164
#define K_KEY_OPTION           0x165
#define K_KEY_INFO             0x166//AL OEM Features/Tips/Tutorial
#define K_KEY_TIME             0x167
#define K_KEY_VENDOR           0x168
#define K_KEY_ARCHIVE          0x169
#define K_KEY_PROGRAM          0x16a//Media Select Program Guide
#define K_KEY_CHANNEL          0x16b
#define K_KEY_FAVORITES        0x16c
#define K_KEY_EPG              0x16d
#define K_KEY_PVR              0x16e//Media Select Home
#define K_KEY_MHP              0x16f
#define K_KEY_LANGUAGE         0x170
#define K_KEY_TITLE            0x171
#define K_KEY_SUBTITLE         0x172
#define K_KEY_ANGLE            0x173
#define K_KEY_ZOOM             0x174
#define K_KEY_MODE             0x175
#define K_KEY_KEYBOARD         0x176
#define K_KEY_SCREEN           0x177
#define K_KEY_PC               0x178//Media Select Computer
#define K_KEY_TV               0x179//Media Select TV
#define K_KEY_TV2              0x17a//Media Select Cable
#define K_KEY_VCR              0x17b//Media Select VCR
#define K_KEY_VCR2             0x17c//VCR Plus
#define K_KEY_SAT              0x17d//Media Select Satellite
#define K_KEY_SAT2             0x17e
#define K_KEY_CD               0x17f//Media Select CD
#define K_KEY_TAPE             0x180//Media Select Tape
#define K_KEY_RADIO            0x181
#define K_KEY_TUNER            0x182//Media Select Tuner
#define K_KEY_PLAYER           0x183
#define K_KEY_TEXT             0x184
#define K_KEY_DVD              0x185//Media Select DVD
#define K_KEY_AUX              0x186
#define K_KEY_MP3              0x187
#define K_KEY_AUDIO            0x188//AL Audio Browser
#define K_KEY_VIDEO            0x189//AL Movie Browser
#define K_KEY_DIRECTORY        0x18a
#define K_KEY_LIST             0x18b
#define K_KEY_MEMO             0x18c//Media Select Messages
#define K_KEY_CALENDAR         0x18d
#define K_KEY_RED              0x18e
#define K_KEY_GREEN            0x18f
#define K_KEY_YELLOW           0x190
#define K_KEY_BLUE             0x191
#define K_KEY_CHANNELUP        0x192//Channel Increment
#define K_KEY_CHANNELDOWN      0x193//Channel Decrement
#define K_KEY_FIRST            0x194
#define K_KEY_LAST             0x195//Recall Last
#define K_KEY_AB               0x196
#define K_KEY_NEXT             0x197
#define K_KEY_RESTART          0x198
#define K_KEY_SLOW             0x199
#define K_KEY_SHUFFLE          0x19a
#define K_KEY_BREAK            0x19b
#define K_KEY_PREVIOUS         0x19c
#define K_KEY_DIGITS           0x19d
#define K_KEY_TEEN             0x19e
#define K_KEY_TWEN             0x19f
#define K_KEY_VIDEOPHONE       0x1a0//Media Select Video Phone
#define K_KEY_GAMES            0x1a1//Media Select Games
#define K_KEY_ZOOMIN           0x1a2//AC Zoom In
#define K_KEY_ZOOMOUT          0x1a3//AC Zoom Out
#define K_KEY_ZOOMRESET        0x1a4//AC Zoom
#define K_KEY_WORDPROCESSOR    0x1a5//AL Word Processor
#define K_KEY_EDITOR           0x1a6//AL Text Editor
#define K_KEY_SPREADSHEET      0x1a7//AL Spreadsheet
#define K_KEY_GRAPHICSEDITOR   0x1a8//AL Graphics Editor
#define K_KEY_PRESENTATION     0x1a9//AL Presentation App
#define K_KEY_DATABASE         0x1aa//AL Database App
#define K_KEY_NEWS             0x1ab//AL Newsreader
#define K_KEY_VOICEMAIL        0x1ac//AL Voicemail
#define K_KEY_ADDRESSBOOK      0x1ad//AL Contacts/Address Book
#define K_KEY_MESSENGER        0x1ae//AL Instant Messaging
#define K_KEY_DISPLAYTOGGLE    0x1af//Turn display (LCD) on and off
#define K_KEY_SPELLCHECK       0x1b0//AL Spell Check
#define K_KEY_LOGOFF           0x1b1//AL Logoff

#define K_KEY_DOLLAR           0x1b2
#define K_KEY_EURO             0x1b3

#define K_KEY_FRAMEBACK        0x1b4//Consumer - transport controls
#define K_KEY_FRAMEFORWARD     0x1b5
#define K_KEY_CONTEXT_MENU     0x1b6//GenDesc - system context menu
#define K_KEY_MEDIA_REPEAT     0x1b7//Consumer - transport control
#define K_KEY_10CHANNELSUP     0x1b8//10 channels up (10+)
#define K_KEY_10CHANNELSDOWN   0x1b9//10 channels down (10-)
#define K_KEY_IMAGES           0x1ba//AL Image Browser

#define K_KEY_DEL_EOL          0x1c0
#define K_KEY_DEL_EOS          0x1c1
#define K_KEY_INS_LINE         0x1c2
#define K_KEY_DEL_LINE         0x1c3

#define K_KEY_FN               0x1d0
#define K_KEY_FN_ESC           0x1d1
#define K_KEY_FN_F1            0x1d2
#define K_KEY_FN_F2            0x1d3
#define K_KEY_FN_F3            0x1d4
#define K_KEY_FN_F4            0x1d5
#define K_KEY_FN_F5            0x1d6
#define K_KEY_FN_F6            0x1d7
#define K_KEY_FN_F7            0x1d8
#define K_KEY_FN_F8            0x1d9
#define K_KEY_FN_F9            0x1da
#define K_KEY_FN_F10           0x1db
#define K_KEY_FN_F11           0x1dc
#define K_KEY_FN_F12           0x1dd
#define K_KEY_FN_1             0x1de
#define K_KEY_FN_2             0x1df
#define K_KEY_FN_D             0x1e0
#define K_KEY_FN_E             0x1e1
#define K_KEY_FN_F             0x1e2
#define K_KEY_FN_S             0x1e3
#define K_KEY_FN_B             0x1e4

#define K_KEY_BRL_DOT1         0x1f1
#define K_KEY_BRL_DOT2         0x1f2
#define K_KEY_BRL_DOT3         0x1f3
#define K_KEY_BRL_DOT4         0x1f4
#define K_KEY_BRL_DOT5         0x1f5
#define K_KEY_BRL_DOT6         0x1f6
#define K_KEY_BRL_DOT7         0x1f7
#define K_KEY_BRL_DOT8         0x1f8
#define K_KEY_BRL_DOT9         0x1f9
#define K_KEY_BRL_DOT10        0x1fa

#define K_KEY_NUMERIC_0        0x200//used by phones, remote controls, and other
#define K_KEY_NUMERIC_1        0x201//keypads
#define K_KEY_NUMERIC_2        0x202
#define K_KEY_NUMERIC_3        0x203
#define K_KEY_NUMERIC_4        0x204
#define K_KEY_NUMERIC_5        0x205
#define K_KEY_NUMERIC_6        0x206
#define K_KEY_NUMERIC_7        0x207
#define K_KEY_NUMERIC_8        0x208
#define K_KEY_NUMERIC_9        0x209
#define K_KEY_NUMERIC_STAR     0x20a
#define K_KEY_NUMERIC_POUND    0x20b

#define K_KEY_CAMERA_FOCUS     0x210
#define K_KEY_WPS_BUTTON       0x211//WiFi Protected Setup key

#define K_KEY_TOUCHPAD_TOGGLE  0x212//Request switch touchpad on or off
#define K_KEY_TOUCHPAD_ON      0x213
#define K_KEY_TOUCHPAD_OFF     0x214

#define K_KEY_CAMERA_ZOOMIN    0x215
#define K_KEY_CAMERA_ZOOMOUT   0x216
#define K_KEY_CAMERA_UP        0x217
#define K_KEY_CAMERA_DOWN      0x218
#define K_KEY_CAMERA_LEFT      0x219
#define K_KEY_CAMERA_RIGHT     0x21a

#define K_BTN_TRIGGER_HAPPY    0x2c0
#define K_BTN_TRIGGER_HAPPY1   0x2c0
#define K_BTN_TRIGGER_HAPPY2   0x2c1
#define K_BTN_TRIGGER_HAPPY3   0x2c2
#define K_BTN_TRIGGER_HAPPY4   0x2c3
#define K_BTN_TRIGGER_HAPPY5   0x2c4
#define K_BTN_TRIGGER_HAPPY6   0x2c5
#define K_BTN_TRIGGER_HAPPY7   0x2c6
#define K_BTN_TRIGGER_HAPPY8   0x2c7
#define K_BTN_TRIGGER_HAPPY9   0x2c8
#define K_BTN_TRIGGER_HAPPY10  0x2c9
#define K_BTN_TRIGGER_HAPPY11  0x2ca
#define K_BTN_TRIGGER_HAPPY12  0x2cb
#define K_BTN_TRIGGER_HAPPY13  0x2cc
#define K_BTN_TRIGGER_HAPPY14  0x2cd
#define K_BTN_TRIGGER_HAPPY15  0x2ce
#define K_BTN_TRIGGER_HAPPY16  0x2cf
#define K_BTN_TRIGGER_HAPPY17  0x2d0
#define K_BTN_TRIGGER_HAPPY18  0x2d1
#define K_BTN_TRIGGER_HAPPY19  0x2d2
#define K_BTN_TRIGGER_HAPPY20  0x2d3
#define K_BTN_TRIGGER_HAPPY21  0x2d4
#define K_BTN_TRIGGER_HAPPY22  0x2d5
#define K_BTN_TRIGGER_HAPPY23  0x2d6
#define K_BTN_TRIGGER_HAPPY24  0x2d7
#define K_BTN_TRIGGER_HAPPY25  0x2d8
#define K_BTN_TRIGGER_HAPPY26  0x2d9
#define K_BTN_TRIGGER_HAPPY27  0x2da
#define K_BTN_TRIGGER_HAPPY28  0x2db
#define K_BTN_TRIGGER_HAPPY29  0x2dc
#define K_BTN_TRIGGER_HAPPY30  0x2dd
#define K_BTN_TRIGGER_HAPPY31  0x2de
#define K_BTN_TRIGGER_HAPPY32  0x2df
#define K_BTN_TRIGGER_HAPPY33  0x2e0
#define K_BTN_TRIGGER_HAPPY34  0x2e1
#define K_BTN_TRIGGER_HAPPY35  0x2e2
#define K_BTN_TRIGGER_HAPPY36  0x2e3
#define K_BTN_TRIGGER_HAPPY37  0x2e4
#define K_BTN_TRIGGER_HAPPY38  0x2e5
#define K_BTN_TRIGGER_HAPPY39  0x2e6
#define K_BTN_TRIGGER_HAPPY40  0x2e7

//We avoid low common keys in module aliases so they don't get huge.
#define K_KEY_MIN_INTERESTING  K_KEY_MUTE
#define K_KEY_MAX              0x2ff
#define K_KEY_CNT              (K_KEY_MAX+1)

//relative axes bit offsets
#define K_REL_X      0x00
#define K_REL_Y      0x01
#define K_REL_Z      0x02
#define K_REL_RX     0x03
#define K_REL_RY     0x04
#define K_REL_RZ     0x05
#define K_REL_HWHEEL 0x06
#define K_REL_DIAL   0x07
#define K_REL_WHEEL  0x08
#define K_REL_MISC   0x09
#define K_REL_MAX    0x0f
#define K_REL_CNT    (K_REL_MAX+1)

//Absolute axes bit offsets
#define K_ABS_X              0x00
#define K_ABS_Y              0x01
#define K_ABS_Z              0x02
#define K_ABS_RX             0x03
#define K_ABS_RY             0x04
#define K_ABS_RZ             0x05
#define K_ABS_THROTTLE       0x06
#define K_ABS_RUDDER         0x07
#define K_ABS_WHEEL          0x08
#define K_ABS_GAS            0x09
#define K_ABS_BRAKE          0x0a
#define K_ABS_HAT0X          0x10
#define K_ABS_HAT0Y          0x11
#define K_ABS_HAT1X          0x12
#define K_ABS_HAT1Y          0x13
#define K_ABS_HAT2X          0x14
#define K_ABS_HAT2Y          0x15
#define K_ABS_HAT3X          0x16
#define K_ABS_HAT3Y          0x17
#define K_ABS_PRESSURE       0x18
#define K_ABS_DISTANCE       0x19
#define K_ABS_TILT_X         0x1a
#define K_ABS_TILT_Y         0x1b
#define K_ABS_TOOL_WIDTH     0x1c

#define K_ABS_VOLUME         0x20

#define K_ABS_MISC           0x28

#define K_ABS_MT_SLOT        0x2f//MT slot being modified
#define K_ABS_MT_TOUCH_MAJOR 0x30//Major axis of touching ellipse
#define K_ABS_MT_TOUCH_MINOR 0x31//Minor axis (omit if circular)
#define K_ABS_MT_WIDTH_MAJOR 0x32//Major axis of approaching ellipse
#define K_ABS_MT_WIDTH_MINOR 0x33//Minor axis (omit if circular)
#define K_ABS_MT_ORIENTATION 0x34//Ellipse orientation
#define K_ABS_MT_POSITION_X  0x35//Center X touch position
#define K_ABS_MT_POSITION_Y  0x36//Center Y touch position
#define K_ABS_MT_TOOL_TYPE   0x37//Type of touching device
#define K_ABS_MT_BLOB_ID     0x38//Group a set of packets as a blob
#define K_ABS_MT_TRACKING_ID 0x39//Unique ID of initiated contact
#define K_ABS_MT_PRESSURE    0x3a//Pressure on contact area
#define K_ABS_MT_DISTANCE    0x3b//Contact hover distance
#define K_ABS_MT_TOOL_X      0x3c//Center X tool position
#define K_ABS_MT_TOOL_Y      0x3d//Center Y tool position

#define K_ABS_MAX            0x3f
#define K_ABS_CNT           (K_ABS_MAX+1)

//Misc events bit offsets
#define K_MSC_SERIAL         0x00
#define K_MSC_PULSELED       0x01
#define K_MSC_GESTURE        0x02
#define K_MSC_RAW            0x03
#define K_MSC_SCAN           0x04
#define K_MSC_MAX            0x07
#define K_MSC_CNT            (K_MSC_MAX+1)

//Switch events bit offsets
#define K_SW_LID                  0x00//set = lid shut
#define K_SW_TABLET_MODE          0x01//set = tablet mode
#define K_SW_HEADPHONE_INSERT     0x02//set = inserted
#define K_SW_RFKILL_ALL           0x03//rfkill master switch, type "any"
                                      //set = radio enabled
#define K_SW_RADIO                K_SW_RFKILL_ALL//deprecated
#define K_SW_MICROPHONE_INSERT    0x04//set = inserted
#define K_SW_DOCK                 0x05//set = plugged into dock
#define K_SW_LINEOUT_INSERT       0x06//set = inserted
#define K_SW_JACK_PHYSICAL_INSERT 0x07//set = mechanical switch set
#define K_SW_VIDEOOUT_INSERT      0x08//set = inserted
#define K_SW_CAMERA_LENS_COVER    0x09//set = lens covered
#define K_SW_KEYPAD_SLIDE         0x0a//set = keypad slide out
#define K_SW_FRONT_PROXIMITY      0x0b//set = front proximity sensor active
#define K_SW_ROTATE_LOCK          0x0c//set = rotate locked/disabled
#define K_SW_LINEIN_INSERT        0x0d//set = inserted
#define K_SW_MAX                  0x0f
#define K_SW_CNT                  (K_SW_MAX+1)

//LEDs bit offsets
#define K_LED_NUML     0x00
#define K_LED_CAPSL    0x01
#define K_LED_SCROLLL  0x02
#define K_LED_COMPOSE  0x03
#define K_LED_KANA     0x04
#define K_LED_SLEEP    0x05
#define K_LED_SUSPEND  0x06
#define K_LED_MUTE     0x07
#define K_LED_MISC     0x08
#define K_LED_MAIL     0x09
#define K_LED_CHARGING 0x0a
#define K_LED_MAX      0x0f
#define K_LED_CNT      (K_LED_MAX+1)

//Sounds bit offsets
#define K_SND_CLICK 0x00
#define K_SND_BELL  0x01
#define K_SND_TONE  0x02
#define K_SND_MAX   0x07
#define K_SND_CNT   (K_SND_MAX+1)

//ff status of a force-feedback effect offset bits
#define K_FF_STATUS_STOPPED 0x00
#define K_FF_STATUS_PLAYING 0x01
#define K_FF_STATUS_MAX     0x01

//Force feedback effect types
#define K_FF_RUMBLE       0x50
#define K_FF_PERIODIC     0x51
#define K_FF_CONSTANT     0x52
#define K_FF_SPRING       0x53
#define K_FF_FRICTION     0x54
#define K_FF_DAMPER       0x55
#define K_FF_INERTIA      0x56
#define K_FF_RAMP         0x57

#define K_FF_EFFECT_MIN   FF_RUMBLE
#define K_FF_EFFECT_MAX   FF_RAMP

//Force feedback periodic effect types
#define K_FF_SQUARE       0x58
#define K_FF_TRIANGLE     0x59
#define K_FF_SINE         0x5a
#define K_FF_SAW_UP       0x5b
#define K_FF_SAW_DOWN     0x5c
#define K_FF_CUSTOM       0x5d

#define K_FF_WAVEFORM_MIN FF_SQUARE
#define K_FF_WAVEFORM_MAX FF_CUSTOM

//Set ff device properties
#define K_FF_GAIN         0x60
#define K_FF_AUTOCENTER   0x61

#define K_FF_MAX          0x7f
#define K_FF_CNT          (K_FF_MAX+1)
#endif
