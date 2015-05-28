#ifndef ULINUX_ARCH_SYSC_H
#define ULINUX_ARCH_SYSC_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/

#define ulinux_sysc(name,nr,args...)                                           \
  ({                                                                           \
    unsigned long resultvar;                                                   \
    SYSC_LOAD_ARGS_##nr(args)                                                  \
    SYSC_LOAD_REGS_##nr                                                        \
    asm volatile (                                                             \
    "syscall\n\t"                                                              \
    :"=a"(resultvar)                                                           \
    :"0"(__ULINUX_NR_##name)SYSC_ASM_ARGS_##nr                                 \
    :"memory","cc","r11","cx");                                                \
    (long)resultvar;})

/*----------------------------------------------------------------------------*/
#define SYSC_LOAD_ARGS_0()
#define SYSC_LOAD_REGS_0
#define SYSC_ASM_ARGS_0
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
#define SYSC_LOAD_ARGS_1(a1)                                                   \
  long __arg1=(long)(a1);                                                      \
  SYSC_LOAD_ARGS_0()

#define SYSC_LOAD_REGS_1                                                       \
  register long _a1 asm("rdi")=__arg1;                                         \
  SYSC_LOAD_REGS_0

#define SYSC_ASM_ARGS_1	SYSC_ASM_ARGS_0,"r"(_a1)
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
#define SYSC_LOAD_ARGS_2(a1,a2)                                                \
  long __arg2=(long)(a2);                                                      \
  SYSC_LOAD_ARGS_1(a1)

#define SYSC_LOAD_REGS_2                                                       \
  register long _a2 asm("rsi")=__arg2;                                         \
  SYSC_LOAD_REGS_1

#define SYSC_ASM_ARGS_2	SYSC_ASM_ARGS_1,"r"(_a2)
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
#define SYSC_LOAD_ARGS_3(a1,a2,a3)                                             \
  long __arg3=(long)(a3);                                                      \
  SYSC_LOAD_ARGS_2(a1,a2)

#define SYSC_LOAD_REGS_3                                                       \
  register long _a3 asm("rdx")=__arg3;                                         \
  SYSC_LOAD_REGS_2

#define SYSC_ASM_ARGS_3	SYSC_ASM_ARGS_2,"r"(_a3)
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
#define SYSC_LOAD_ARGS_4(a1,a2,a3,a4)                                          \
  long __arg4=(long)(a4);                                                      \
  SYSC_LOAD_ARGS_3(a1,a2,a3)

#define SYSC_LOAD_REGS_4                                                       \
  register long _a4 asm("r10")=__arg4;                                         \
  SYSC_LOAD_REGS_3

#define SYSC_ASM_ARGS_4	SYSC_ASM_ARGS_3,"r"(_a4)
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
#define SYSC_LOAD_ARGS_5(a1,a2,a3,a4,a5)                                       \
  long __arg5=(long)(a5);                                                      \
  SYSC_LOAD_ARGS_4(a1,a2,a3,a4)

#define SYSC_LOAD_REGS_5                                                       \
  register long _a5 asm("r8")=__arg5;                                          \
  SYSC_LOAD_REGS_4

#define SYSC_ASM_ARGS_5	SYSC_ASM_ARGS_4,"r"(_a5)
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
#define SYSC_LOAD_ARGS_6(a1,a2,a3,a4,a5,a6)                                    \
  long __arg6=(long)(a6);                                                      \
  SYSC_LOAD_ARGS_5(a1,a2,a3,a4,a5)

#define SYSC_LOAD_REGS_6                                                       \
  register long _a6 asm("r9")=__arg6;                                          \
  SYSC_LOAD_REGS_5

#define SYSC_ASM_ARGS_6	SYSC_ASM_ARGS_5,"r"(_a6)
/*----------------------------------------------------------------------------*/


/*============================================================================*/
/*
This file contains the system call numbers.
Note: holes are not allowed.
at least 8 syscall per cacheline
*/
#define __ULINUX_NR_read                     0
#define __ULINUX_NR_write                    1
#define __ULINUX_NR_open                     2
#define __ULINUX_NR_close                    3
#define __ULINUX_NR_stat                     4
#define __ULINUX_NR_fstat                    5
#define __ULINUX_NR_lstat                    6
#define __ULINUX_NR_poll                     7

#define __ULINUX_NR_lseek                    8
#define __ULINUX_NR_mmap                     9
#define __ULINUX_NR_mprotect                10
#define __ULINUX_NR_munmap                  11
#define __ULINUX_NR_brk                     12
#define __ULINUX_NR_rt_sigaction            13
#define __ULINUX_NR_rt_sigprocmask          14
#define __ULINUX_NR_rt_sigreturn            15

#define __ULINUX_NR_ioctl                   16
#define __ULINUX_NR_pread64                 17
#define __ULINUX_NR_pwrite64                18
#define __ULINUX_NR_readv                   19
#define __ULINUX_NR_writev                  20
#define __ULINUX_NR_access                  21
#define __ULINUX_NR_pipe                    22
#define __ULINUX_NR_select                  23

#define __ULINUX_NR_sched_yield             24
#define __ULINUX_NR_mremap                  25
#define __ULINUX_NR_msync                   26
#define __ULINUX_NR_mincore                 27
#define __ULINUX_NR_madvise                 28
#define __ULINUX_NR_shmget                  29
#define __ULINUX_NR_shmat                   30
#define __ULINUX_NR_shmctl                  31

#define __ULINUX_NR_dup                     32
#define __ULINUX_NR_dup2                    33
#define __ULINUX_NR_pause                   34
#define __ULINUX_NR_nanosleep               35
#define __ULINUX_NR_getitimer               36
#define __ULINUX_NR_alarm                   37
#define __ULINUX_NR_setitimer               38
#define __ULINUX_NR_getpid                  39

#define __ULINUX_NR_sendfilei               40
#define __ULINUX_NR_socket                  41
#define __ULINUX_NR_connect                 42
#define __ULINUX_NR_accept                  43
#define __ULINUX_NR_sendto                  44
#define __ULINUX_NR_recvfrom                45
#define __ULINUX_NR_sendmsg                 46
#define __ULINUX_NR_recvmsg                 47

#define __ULINUX_NR_shutdown                48
#define __ULINUX_NR_bind                    49
#define __ULINUX_NR_listen                  50
#define __ULINUX_NR_getsockname             51
#define __ULINUX_NR_getpeername             52
#define __ULINUX_NR_socketpair              53
#define __ULINUX_NR_setsockopt              54
#define __ULINUX_NR_getsockopt              55

#define __ULINUX_NR_clone                   56
#define __ULINUX_NR_fork                    57
#define __ULINUX_NR_vfork                   58
#define __ULINUX_NR_execve                  59
#define __ULINUX_NR_exit                    60
#define __ULINUX_NR_wait4                   61
#define __ULINUX_NR_kill                    62
#define __ULINUX_NR_uname                   63

#define __ULINUX_NR_semget                  64
#define __ULINUX_NR_semop                   65
#define __ULINUX_NR_semctl                  66
#define __ULINUX_NR_shmdt                   67
#define __ULINUX_NR_msgget                  68
#define __ULINUX_NR_msgsnd                  69
#define __ULINUX_NR_msgrcv                  70
#define __ULINUX_NR_msgctl                  71

#define __ULINUX_NR_fcntl                   72
#define __ULINUX_NR_flock                   73
#define __ULINUX_NR_fsync                   74
#define __ULINUX_NR_fdatasync               75
#define __ULINUX_NR_truncate                76
#define __ULINUX_NR_ftruncate               77
#define __ULINUX_NR_getdents                78
#define __ULINUX_NR_getcwd                  79

#define __ULINUX_NR_chdir                   80
#define __ULINUX_NR_fchdir                  81
#define __ULINUX_NR_rename                  82
#define __ULINUX_NR_mkdir                   83
#define __ULINUX_NR_rmdir                   84
#define __ULINUX_NR_creat                   85
#define __ULINUX_NR_link                    86
#define __ULINUX_NR_unlink                  87

#define __ULINUX_NR_symlink                 88
#define __ULINUX_NR_readlink                89
#define __ULINUX_NR_chmod                   90
#define __ULINUX_NR_fchmod                  91
#define __ULINUX_NR_chown                   92
#define __ULINUX_NR_fchown                  93
#define __ULINUX_NR_lchown                  94
#define __ULINUX_NR_umask                   95

#define __ULINUX_NR_gettimeofday            96
#define __ULINUX_NR_getrlimit               97
#define __ULINUX_NR_getrusage               98
#define __ULINUX_NR_sysinfo                 99
#define __ULINUX_NR_times                  100
#define __ULINUX_NR_ptrace                 101
#define __ULINUX_NR_getuid                 102
#define __ULINUX_NR_syslog                 103

/*at the very end the stuff that never runs during the benchmarks*/
#define __ULINUX_NR_getgid                 104
#define __ULINUX_NR_setuid                 105
#define __ULINUX_NR_setgid                 106
#define __ULINUX_NR_geteuid                107
#define __ULINUX_NR_getegid                108
#define __ULINUX_NR_setpgid                109
#define __ULINUX_NR_getppid                110
#define __ULINUX_NR_getpgrp                111

#define __ULINUX_NR_setsid                 112
#define __ULINUX_NR_setreuid               113
#define __ULINUX_NR_setregid               114
#define __ULINUX_NR_getgroups              115
#define __ULINUX_NR_setgroups              116
#define __ULINUX_NR_setresuid              117
#define __ULINUX_NR_getresuid              118
#define __ULINUX_NR_setresgid              119

#define __ULINUX_NR_getresgid              120
#define __ULINUX_NR_getpgid                121
#define __ULINUX_NR_setfsuid               122
#define __ULINUX_NR_setfsgid               123
#define __ULINUX_NR_getsid                 124
#define __ULINUX_NR_capget                 125
#define __ULINUX_NR_capset                 126

#define __ULINUX_NR_rt_sigpending          127
#define __ULINUX_NR_rt_sigtimedwait        128
#define __ULINUX_NR_rt_sigqueueinfo        129
#define __ULINUX_NR_rt_sigsuspend          130
#define __ULINUX_NR_sigaltstack            131
#define __ULINUX_NR_utime                  132
#define __ULINUX_NR_mknod                  133

/*only needed for a.out*/
#define __ULINUX_NR_uselib                 134
#define __ULINUX_NR_personality            135

#define __ULINUX_NR_ustat                  136
#define __ULINUX_NR_statfs                 137
#define __ULINUX_NR_fstatfs                138
#define __ULINUX_NR_sysfs                  139

#define __ULINUX_NR_getpriority            140
#define __ULINUX_NR_setpriority            141
#define __ULINUX_NR_sched_setparam         142
#define __ULINUX_NR_sched_getparam         143
#define __ULINUX_NR_sched_setscheduler     144
#define __ULINUX_NR_sched_getscheduler     145
#define __ULINUX_NR_sched_get_priority_max 146
#define __ULINUX_NR_sched_get_priority_min 147
#define __ULINUX_NR_sched_rr_get_interval  148

#define __ULINUX_NR_mlock                  149
#define __ULINUX_NR_munlock                150
#define __ULINUX_NR_mlockall               151
#define __ULINUX_NR_munlockall             152

#define __ULINUX_NR_vhangup                153

#define __ULINUX_NR_modify_ldt             154

#define __ULINUX_NR_pivot_root             155

#define __ULINUX_NR__sysctl                156

#define __ULINUX_NR_prctl                  157
#define __ULINUX_NR_arch_prctl             158

#define __ULINUX_NR_adjtimex               159

#define __ULINUX_NR_setrlimit              160

#define __ULINUX_NR_chroot                 161

#define __ULINUX_NR_sync                   162

#define __ULINUX_NR_acct                   163

#define __ULINUX_NR_settimeofday           164

#define __ULINUX_NR_mount                  165
#define __ULINUX_NR_umount2                166

#define __ULINUX_NR_swapon                 167
#define __ULINUX_NR_swapoff                168

#define __ULINUX_NR_reboot                 169

#define __ULINUX_NR_sethostname            170
#define __ULINUX_NR_setdomainname          171

#define __ULINUX_NR_iopl                   172
#define __ULINUX_NR_ioperm                 173

#define __ULINUX_NR_create_module          174
#define __ULINUX_NR_init_module            175
#define __ULINUX_NR_delete_module          176
#define __ULINUX_NR_get_kernel_syms        177
#define __ULINUX_NR_query_module           178

#define __ULINUX_NR_quotactl               179

#define __ULINUX_NR_nfsservctl             180

#define __ULINUX_NR_getpmsg                181
#define __ULINUX_NR_putpmsg                182

/*reserved for AFS*/
#define __ULINUX_NR_afs_syscall            183

/*reserved or tux*/
#define __ULINUX_NR_tuxcall                184

#define __ULINUX_NR_security               185

#define __ULINUX_NR_gettid                 186

#define __ULINUX_NR_readahead              187
#define __ULINUX_NR_setxattr               188
#define __ULINUX_NR_lsetxattr              189
#define __ULINUX_NR_fsetxattr              190
#define __ULINUX_NR_getxattr               191
#define __ULINUX_NR_lgetxattr              192
#define __ULINUX_NR_fgetxattr              193
#define __ULINUX_NR_listxattr              194
#define __ULINUX_NR_llistxattr             195
#define __ULINUX_NR_flistxattr             196
#define __ULINUX_NR_removexattr            197
#define __ULINUX_NR_lremovexattr           198
#define __ULINUX_NR_fremovexattr           199
#define __ULINUX_NR_tkill                  200
#define __ULINUX_NR_time                   201
#define __ULINUX_NR_futex                  202
#define __ULINUX_NR_sched_setaffinity      203
#define __ULINUX_NR_sched_getaffinity      204
#define __ULINUX_NR_set_thread_area        205
#define __ULINUX_NR_io_setup               206
#define __ULINUX_NR_io_destroy             207
#define __ULINUX_NR_io_getevents           208
#define __ULINUX_NR_io_submit              209
#define __ULINUX_NR_io_cancel              210
#define __ULINUX_NR_get_thread_area        211
#define __ULINUX_NR_lookup_dcookie         212
#define __ULINUX_NR_epoll_create           213
#define __ULINUX_NR_epoll_ctl_old          214
#define __ULINUX_NR_epoll_wait_oldi        215
#define __ULINUX_NR_remap_file_pages       216
#define __ULINUX_NR_getdents64             217
#define __ULINUX_NR_set_tid_address        218
#define __ULINUX_NR_restart_syscall        219
#define __ULINUX_NR_semtimedop             220
#define __ULINUX_NR_fadvise64              221
#define __ULINUX_NR_timer_create           222
#define __ULINUX_NR_timer_settime          223
#define __ULINUX_NR_timer_gettime          224
#define __ULINUX_NR_timer_getoverrun       225
#define __ULINUX_NR_timer_delete           226
#define __ULINUX_NR_clock_settime          227
#define __ULINUX_NR_clock_gettime          228
#define __ULINUX_NR_clock_getres           229
#define __ULINUX_NR_clock_nanosleep        230
#define __ULINUX_NR_exit_group             231
#define __ULINUX_NR_epoll_wait             232
#define __ULINUX_NR_epoll_ctl              233
#define __ULINUX_NR_tgkill                 234
#define __ULINUX_NR_utimes                 235
#define __ULINUX_NR_vserver                236
#define __ULINUX_NR_mbind                  237
#define __ULINUX_NR_set_mempolicy          238
#define __ULINUX_NR_get_mempolicy          239
#define __ULINUX_NR_mq_open                240
#define __ULINUX_NR_mq_unlink              241
#define __ULINUX_NR_mq_timedsend           242
#define __ULINUX_NR_mq_timedreceive        243
#define __ULINUX_NR_mq_notify              244
#define __ULINUX_NR_mq_getsetattr          245
#define __ULINUX_NR_kexec_load             246
#define __ULINUX_NR_waitid                 247
#define __ULINUX_NR_add_key                248
#define __ULINUX_NR_request_key            249
#define __ULINUX_NR_keyctl                 250
#define __ULINUX_NR_ioprio_set             251
#define __ULINUX_NR_ioprio_get             252
#define __ULINUX_NR_inotify_init           253
#define __ULINUX_NR_inotify_add_watch      254
#define __ULINUX_NR_inotify_rm_watchi      255
#define __ULINUX_NR_migrate_pages          256
#define __ULINUX_NR_openat                 257
#define __ULINUX_NR_mkdirat                258
#define __ULINUX_NR_mknodat                259
#define __ULINUX_NR_fchownat               260
#define __ULINUX_NR_futimesat              261
#define __ULINUX_NR_newfstatat             262
#define __ULINUX_NR_unlinkat               263
#define __ULINUX_NR_renameat               264
#define __ULINUX_NR_linkat                 265
#define __ULINUX_NR_symlinkat              266
#define __ULINUX_NR_readlinkat             267
#define __ULINUX_NR_fchmodat               268
#define __ULINUX_NR_faccessat              269
#define __ULINUX_NR_pselect6               270
#define __ULINUX_NR_ppoll                  271
#define __ULINUX_NR_unshare                272
#define __ULINUX_NR_set_robust_list        273
#define __ULINUX_NR_get_robust_list        274
#define __ULINUX_NR_splice                 275
#define __ULINUX_NR_tee                    276
#define __ULINUX_NR_sync_file_range        277
#define __ULINUX_NR_vmsplice               278
#define __ULINUX_NR_move_pages             279
#define __ULINUX_NR_utimensat              280
#define __ULINUX_IGNORE_getcpu/*implemented as a vsyscall*/
#define __ULINUX_NR_epoll_pwait            281
#define __ULINUX_NR_signalfd               282
#define __ULINUX_NR_timerfd_create         283
#define __ULINUX_NR_eventfd                284
#define __ULINUX_NR_fallocate              285
#define __ULINUX_NR_timerfd_settime        286
#define __ULINUX_NR_timerfd_gettime        287
#define __ULINUX_NR_accept4                288
#define __ULINUX_NR_signalfd4              289
#define __ULINUX_NR_eventfd2               290
#define __ULINUX_NR_epoll_create1          291
#define __ULINUX_NR_dup3                   292
#define __ULINUX_NR_pipe2                  293
#define __ULINUX_NR_inotify_init1          294
#define __ULINUX_NR_preadv                 295
#define __ULINUX_NR_pwritev                296
#define __ULINUX_NR_rt_tgsigqueueinfo      297
#define __ULINUX_NR_perf_event_open        298
#define __ULINUX_NR_recvmmsg               299
    
#endif
