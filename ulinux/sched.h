#ifndef ULINUX_SCHED_H
#define ULINUX_SCHED_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
#define ULINUX_CSIGNAL              0x000000ff/*signal mask to be sent at exit*/
#define ULINUX_CLONE_VM             0x00000100/*set if vm shared between
                                                processes*/
#define ULINUX_CLONE_FS             0x00000200/*set if fs info shared between
                                                processes*/
#define ULINUX_CLONE_FILES          0x00000400/*set if open files shared between
                                                processes*/
#define ULINUX_CLONE_SIGHAND        0x00000800/*set if signal handlers and
                                                blocked signals shared*/
#define ULINUX_CLONE_PTRACE         0x00002000/*set if we want to let tracing
                                                continue on the child too*/
#define ULINUX_CLONE_VFORK          0x00004000/*set if the parent wants the
                                                child to wake it up on
                                                mm_release*/
#define ULINUX_CLONE_PARENT         0x00008000/*set if we want to have the same
                                                parent as the cloner*/
#define ULINUX_CLONE_THREAD         0x00010000/*same thread group?*/
#define ULINUX_CLONE_NEWNS          0x00020000/*new namespace group?*/
#define ULINUX_CLONE_SYSVSEM        0x00040000/*share system v sem_undo
                                                semantics*/
#define ULINUX_CLONE_SETTLS         0x00080000/*create a new tls for the child*/
#define ULINUX_CLONE_PARENT_SETTID  0x00100000/*set the tid in the parent*/
#define ULINUX_CLONE_CHILD_CLEARTID 0x00200000/*clear the tid in the child*/
#define ULINUX_CLONE_DETACHED       0x00400000/*unused, ignored*/
#define ULINUX_CLONE_UNTRACED       0x00800000/*set if the tracing process can't
                                                force clone_ptrace on this clone*/
#define ULINUX_CLONE_CHILD_SETTID   0x01000000/*set the tid in the child*/
/*
0x0200000 was previously the unused clone_stopped (start in stopped state)
and isnow available for re-use.
*/
#define ULINUX_CLONE_NEWUTS         0x04000000/*new utsname group?*/
#define ULINUX_CLONE_NEWIPC         0x08000000/*new ipcs*/
#define ULINUX_CLONE_NEWUSER        0x10000000/*new user namespace*/
#define ULINUX_CLONE_NEWPID         0x20000000/*new pid namespace*/
#define ULINUX_CLONE_NEWNET         0x40000000/*new network namespace*/
#define ULINUX_CLONE_IO             0x80000000/*clone io context*/
#endif
