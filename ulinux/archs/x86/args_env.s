/* this code is protected by the GNU affero GPLv3
   author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
                           <digital.ragnarok AT gmail dot com> */
	.text
	.globl _start
	.type _start,@function
_start:
	xorl %ebp, %ebp/*suggested by abi*/
	popl %esi/*argc*/
	movl %esp, %ecx/*argv then envp*/
	andl  $0xfffffff0, %esp/*16 bytes aligned stack*/
	pushl %ecx/* push the argv/envp pointer as 2nd argument of start*/
	pushl %esi/* push argc as 1st argument of start*/
	call start
