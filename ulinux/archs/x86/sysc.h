#ifndef ULINUX_ARCH_SYSC_H
#define ULINUX_ARCH_SYSC_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************

#define sysc(name, nr, args...) \
({\
  k_ul resultvar;\
  SYSC_LOAD_ARGS_##nr(args)\
  SYSC_LOAD_REGS_##nr\
  asm volatile (\
    "pushl %%ebx\n\t"\
    "int $0x80\n\t"\
    "popl %%ebx\n\t"\
    : "=a" (resultvar)\
    : "0" (__K_NR_##name) SYSC_ASM_ARGS_##nr\
    : "memory","cc");\
    (k_l) resultvar;})

#define SYSC_LOAD_ARGS_0()
#define SYSC_LOAD_REGS_0
#define SYSC_ASM_ARGS_0

#define SYSC_LOAD_ARGS_1(sc_a1) \
k_l _sc_a1=(k_l)(sc_a1); SYSC_LOAD_ARGS_0()
#define SYSC_LOAD_REGS_1 register k_l _sc_a1_reg asm ("ebx")=_sc_a1;\
                                                                SYSC_LOAD_REGS_0
#define SYSC_ASM_ARGS_1 SYSC_ASM_ARGS_0,"r" (_sc_a1_reg)

//----

#define SYSC_LOAD_ARGS_2(sc_a1,sc_a2) \
k_l _sc_a2=(k_l)(sc_a2);SYSC_LOAD_ARGS_1(sc_a1)
#define SYSC_LOAD_REGS_2 register k_l _sc_a2_reg asm ("ecx")=_sc_a2;\
                                                                SYSC_LOAD_REGS_1
#define SYSC_ASM_ARGS_2 SYSC_ASM_ARGS_1,"r" (_sc_a2_reg)

//----

#define SYSC_LOAD_ARGS_3(sc_a1,sc_a2,sc_a3) \
k_l _sc_a3=(k_l)(sc_a3);SYSC_LOAD_ARGS_2(sc_a1,sc_a2)
#define SYSC_LOAD_REGS_3 register k_l _sc_a3_reg asm ("edx")=_sc_a3;\
                                                                SYSC_LOAD_REGS_2
#define SYSC_ASM_ARGS_3 SYSC_ASM_ARGS_2,"r" (_sc_a3_reg)

//----

#define SYSC_LOAD_ARGS_4(sc_a1,sc_a2,sc_a3,sc_a4) \
k_l _sc_a4=(k_l)(sc_a4);SYSC_LOAD_ARGS_3(sc_a1,sc_a2,sc_a3)
#define SYSC_LOAD_REGS_4 register k_l _sc_a4_reg asm ("esi")=_sc_a4;\
                                                                SYSC_LOAD_REGS_3
#define SYSC_ASM_ARGS_4 SYSC_ASM_ARGS_3,"r" (_sc_a4_reg)

//----

#define SYSC_LOAD_ARGS_5(sc_a1,sc_a2,sc_a3,sc_a4,sc_a5) \
k_l _sc_a5=(k_l)(sc_a5);SYSC_LOAD_ARGS_4(sc_a1,sc_a2,sc_a3,sc_a4)
#define SYSC_LOAD_REGS_5 register k_l _sc_a5_reg asm ("edi")=_sc_a5;\
                                                                SYSC_LOAD_REGS_4
#define SYSC_ASM_ARGS_5 SYSC_ASM_ARGS_4,"r" (_sc_a5_reg)

//----

#define SYSC_LOAD_ARGS_6(sc_a1,sc_a2,sc_a3,sc_a4,sc_a5,sc_a6) \
k_l _sc_a6=(k_l)(sc_a6);SYSC_LOAD_ARGS_5(sc_a1,sc_a2,sc_a3,sc_a4,sc_a5)
#define SYSC_LOAD_REGS_6 register k_l _sc_a6_reg asm ("ebp")=_sc_a6;\
                                                                SYSC_LOAD_REGS_5
#define SYSC_ASM_ARGS_6 SYSC_ASM_ARGS_5,"r" (_sc_a6_reg)

//----

#define __K_NR_restart_syscall 0
#define __K_NR_exit 1
#define __K_NR_fork 2
#define __K_NR_read 3
#define __K_NR_write 4
#define __K_NR_open 5
#define __K_NR_close 6
#define __K_NR_waitpid 7
#define __K_NR_creat 8
#define __K_NR_link 9
#define __K_NR_unlink 10
#define __K_NR_execve 11
#define __K_NR_chdir 12
#define __K_NR_time 13
#define __K_NR_mknod 14
#define __K_NR_chmod 15
#define __K_NR_lchown 16
#define __K_NR_break 17
#define __K_NR_oldstat 18
#define __K_NR_lseek 19
#define __K_NR_getpid 20
#define __K_NR_mount 21
#define __K_NR_umount 22
#define __K_NR_setuid 23
#define __K_NR_getuid 24
#define __K_NR_stime 25
#define __K_NR_ptrace 26
#define __K_NR_alarm 27
#define __K_NR_oldfstat 28
#define __K_NR_pause 29
#define __K_NR_utime 30
#define __K_NR_stty 31
#define __K_NR_gtty 32
#define __K_NR_access 33
#define __K_NR_nice 34
#define __K_NR_ftime 35
#define __K_NR_sync 36
#define __K_NR_kill 37
#define __K_NR_rename 38
#define __K_NR_mkdir 39
#define __K_NR_rmdir 40
#define __K_NR_dup 41
#define __K_NR_pipe 42
#define __K_NR_times 43
#define __K_NR_prof 44
#define __K_NR_brk 45
#define __K_NR_setgid 46
#define __K_NR_getgid 47
#define __K_NR_signal 48
#define __K_NR_geteuid 49
#define __K_NR_getegid 50
#define __K_NR_acct 51
#define __K_NR_umount2 52
#define __K_NR_lock 53
#define __K_NR_ioctl 54
#define __K_NR_fcntl 55
#define __K_NR_mpx 56
#define __K_NR_setpgid 57
#define __K_NR_ulimit 58
#define __K_NR_oldolduname 59
#define __K_NR_umask 60
#define __K_NR_chroot 61
#define __K_NR_ustat 62
#define __K_NR_dup2 63
#define __K_NR_getppid 64
#define __K_NR_getpgrp 65
#define __K_NR_setsid 66
#define __K_NR_sigaction 67
#define __K_NR_sgetmask 68
#define __K_NR_ssetmask 69
#define __K_NR_setreuid 70
#define __K_NR_setregid 71
#define __K_NR_sigsuspend 72
#define __K_NR_sigpending 73
#define __K_NR_sethostname 74
#define __K_NR_setrlimit 75
#define __K_NR_getrlimit 76
#define __K_NR_getrusage 77
#define __K_NR_gettimeofday 78
#define __K_NR_settimeofday 79
#define __K_NR_getgroups 80
#define __K_NR_setgroups 81
#define __K_NR_select 82
#define __K_NR_symlink 83
#define __K_NR_oldlstat 84
#define __K_NR_readlink 85
#define __K_NR_uselib 86
#define __K_NR_swapon 87
#define __K_NR_reboot 88
#define __K_NR_readdir 89
#define __K_NR_mmap 90
#define __K_NR_munmap 91
#define __K_NR_truncate 92
#define __K_NR_ftruncate 93
#define __K_NR_fchmod 94
#define __K_NR_fchown 95
#define __K_NR_getpriority 96
#define __K_NR_setpriority 97
#define __K_NR_profil 98
#define __K_NR_statfs 99
#define __K_NR_fstatfs 100
#define __K_NR_ioperm 101
#define __K_NR_socketcall 102
#define __K_NR_syslog 103
#define __K_NR_setitimer 104
#define __K_NR_getitimer 105
#define __K_NR_stat 106
#define __K_NR_lstat 107
#define __K_NR_fstat 108
#define __K_NR_olduname 109
#define __K_NR_iopl 110
#define __K_NR_vhangup 111
#define __K_NR_idle 112
#define __K_NR_vm86old 113
#define __K_NR_wait4 114
#define __K_NR_swapoff 115
#define __K_NR_sysinfo 116
#define __K_NR_ipc 117
#define __K_NR_fsync 118
#define __K_NR_sigreturn 119
#define __K_NR_clone 120
#define __K_NR_setdomainname 121
#define __K_NR_uname 122
#define __K_NR_modify_ldt 123
#define __K_NR_adjtimex 124
#define __K_NR_mprotect 125
#define __K_NR_sigprocmask 126
#define __K_NR_create_module 127
#define __K_NR_init_module 128
#define __K_NR_delete_module 129
#define __K_NR_get_kernel_syms 130
#define __K_NR_quotactl 131
#define __K_NR_getpgid 132
#define __K_NR_fchdir 133
#define __K_NR_bdflush 134
#define __K_NR_sysfs 135
#define __K_NR_personality 136
#define __K_NR_afs_syscall 137
#define __K_NR_setfsuid 138
#define __K_NR_setfsgid 139
#define __K_NR__llseek 140
#define __K_NR_getdents 141
#define __K_NR__newselect 142
#define __K_NR_flock 143
#define __K_NR_msync 144
#define __K_NR_readv 145
#define __K_NR_writev 146
#define __K_NR_getsid 147
#define __K_NR_fdatasync 148
#define __K_NR__sysctl 149
#define __K_NR_mlock 150
#define __K_NR_munlock 151
#define __K_NR_mlockall 152
#define __K_NR_munlockall 153
#define __K_NR_sched_setparam 154
#define __K_NR_sched_getparam 155
#define __K_NR_sched_setscheduler 156
#define __K_NR_sched_getscheduler 157
#define __K_NR_sched_yield 158
#define __K_NR_sched_get_priority_max 159
#define __K_NR_sched_get_priority_min 160
#define __K_NR_sched_rr_get_interval 161
#define __K_NR_nanosleep 162
#define __K_NR_mremap 163
#define __K_NR_setresuid 164
#define __K_NR_getresuid 165
#define __K_NR_vm86 166
#define __K_NR_query_module 167
#define __K_NR_poll 168
#define __K_NR_nfsservctl 169
#define __K_NR_setresgid 170
#define __K_NR_getresgid 171
#define __K_NR_prctl 172
#define __K_NR_rt_sigreturn 173
#define __K_NR_rt_sigaction 174
#define __K_NR_rt_sigprocmask 175
#define __K_NR_rt_sigpending 176
#define __K_NR_rt_sigtimedwait 177
#define __K_NR_rt_sigqueueinfo 178
#define __K_NR_rt_sigsuspend 179
#define __K_NR_pread64 180
#define __K_NR_pwrite64 181
#define __K_NR_chown 182
#define __K_NR_getcwd 183
#define __K_NR_capget 184
#define __K_NR_capset 185
#define __K_NR_sigaltstack 186
#define __K_NR_sendfile 187
#define __K_NR_getpmsg 188
#define __K_NR_putpmsg 189
#define __K_NR_vfork 190
#define __K_NR_ugetrlimit 191
#define __K_NR_mmap2 192
#define __K_NR_truncate64 193
#define __K_NR_ftruncate64 194
#define __K_NR_stat64 195
#define __K_NR_lstat64 196
#define __K_NR_fstat64 197
#define __K_NR_lchown32 198
#define __K_NR_getuid32 199
#define __K_NR_getgid32 200
#define __K_NR_geteuid32 201
#define __K_NR_getegid32 202
#define __K_NR_setreuid32 203
#define __K_NR_setregid32 204
#define __K_NR_getgroups32 205
#define __K_NR_setgroups32 206
#define __K_NR_fchown32 207
#define __K_NR_setresuid32 208
#define __K_NR_getresuid32 209
#define __K_NR_setresgid32 210
#define __K_NR_getresgid32 211
#define __K_NR_chown32 212
#define __K_NR_setuid32 213
#define __K_NR_setgid32 214
#define __K_NR_setfsuid32 215
#define __K_NR_setfsgid32 216
#define __K_NR_pivot_root 217
#define __K_NR_mincore 218
#define __K_NR_madvise 219
#define __K_NR_getdents64 220
#define __K_NR_fcntl64 221
#define __K_NR_gettid 224
#define __K_NR_readahead 225
#define __K_NR_setxattr 226
#define __K_NR_lsetxattr 227
#define __K_NR_fsetxattr 228
#define __K_NR_getxattr 229
#define __K_NR_lgetxattr 230
#define __K_NR_fgetxattr 231
#define __K_NR_listxattr 232
#define __K_NR_llistxattr 233
#define __K_NR_flistxattr 234
#define __K_NR_removexattr 235
#define __K_NR_lremovexattr 236
#define __K_NR_fremovexattr 237
#define __K_NR_tkill 238
#define __K_NR_sendfile64 239
#define __K_NR_futex 240
#define __K_NR_sched_setaffinity 241
#define __K_NR_sched_getaffinity 242
#define __K_NR_set_thread_area 243
#define __K_NR_get_thread_area 244
#define __K_NR_io_setup 245
#define __K_NR_io_destroy 246
#define __K_NR_io_getevents 247
#define __K_NR_io_submit 248
#define __K_NR_io_cancel 249
#define __K_NR_fadvise64 250
#define __K_NR_exit_group 252
#define __K_NR_lookup_dcookie 253
#define __K_NR_epoll_create 254
#define __K_NR_epoll_ctl 255
#define __K_NR_epoll_wait 256
#define __K_NR_remap_file_pages 257
#define __K_NR_set_tid_address 258
#define __K_NR_timer_create 259
#define __K_NR_timer_settime 260
#define __K_NR_timer_gettime 261
#define __K_NR_timer_getoverrun 262
#define __K_NR_timer_delete 263
#define __K_NR_clock_settime 264
#define __K_NR_clock_gettime 265
#define __K_NR_clock_getres 266
#define __K_NR_clock_nanosleep 267
#define __K_NR_statfs64 268
#define __K_NR_fstatfs64 269
#define __K_NR_tgkill 270
#define __K_NR_utimes 271
#define __K_NR_fadvise64_64 272
#define __K_NR_vserver 273
#define __K_NR_mbind 274
#define __K_NR_get_mempolicy 275
#define __K_NR_set_mempolicy 276
#define __K_NR_mq_open 277
#define __K_NR_mq_unlink 278
#define __K_NR_mq_timedsend 279
#define __K_NR_mq_timedreceive 280
#define __K_NR_mq_notify 281
#define __K_NR_mq_getsetattr 282
#define __K_NR_kexec_load 283
#define __K_NR_waitid 284
#define __K_NR_add_key 286
#define __K_NR_request_key 287
#define __K_NR_keyctl 288
#define __K_NR_ioprio_set 289
#define __K_NR_ioprio_get 290
#define __K_NR_inotify_init 291
#define __K_NR_inotify_add_watch 292
#define __K_NR_inotify_rm_watch 293
#define __K_NR_migrate_pages 294
#define __K_NR_openat 295
#define __K_NR_mkdirat 296
#define __K_NR_mknodat 297
#define __K_NR_fchownat 298
#define __K_NR_futimesat 299
#define __K_NR_fstatat64 300
#define __K_NR_unlinkat 301
#define __K_NR_renameat 302
#define __K_NR_linkat 303
#define __K_NR_symlinkat 304
#define __K_NR_readlinkat 305
#define __K_NR_fchmodat 306
#define __K_NR_faccessat 307
#define __K_NR_pselect6 308
#define __K_NR_ppoll 309
#define __K_NR_unshare 310
#define __K_NR_set_robust_list 311
#define __K_NR_get_robust_list 312
#define __K_NR_splice 313
#define __K_NR_sync_file_range 314
#define __K_NR_tee 315
#define __K_NR_vmsplice 316
#define __K_NR_move_pages 317
#define __K_NR_getcpu 318
#define __K_NR_epoll_pwait 319
#define __K_NR_utimensat 320
#define __K_NR_signalfd 321
#define __K_NR_timerfd_create 322
#define __K_NR_eventfd 323
#define __K_NR_fallocate 324
#define __K_NR_timerfd_settime 325
#define __K_NR_timerfd_gettime 326
#define __K_NR_signalfd4 327
#define __K_NR_eventfd2 328
#define __K_NR_epoll_create1 329
#define __K_NR_dup3 330
#define __K_NR_pipe2 331
#define __K_NR_inotify_init1 332
#define __K_NR_preadv 333
#define __K_NR_pwritev 334
#define __K_NR_rt_tgsigqueueinfo 335
#define __K_NR_perf_event_open 336
#define __K_NR_recvmmsg 337
#define __K_NR_fanotify_init 338
#define __K_NR_fanotify_mark 339
#define __K_NR_prlimit64 340
#define __K_NR_name_to_handle_at 341
#define __K_NR_open_by_handle_at 342
#define __K_NR_clock_adjtime 343
#define __K_NR_syncfs 344
#define __K_NR_sendmmsg 345
#define __K_NR_setns 346
#define __K_NR_process_vm_readv 347
#define __K_NR_process_vm_writev 348
#define __K_NR_kcmp 349
#endif
