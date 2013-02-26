/* this code is protected by the GNU affero GPLv3
   author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
                           <digital.ragnarok AT gmail dot com>
   need an AT&T syntax assembler for x86-64 */
	.text
	.globl _start
	.type _start,@function
_start:
	xorl %ebp, %ebp/*suggested by abi*/
	popq %rdi/*argc is 1st argument of start*/
	movq %rsp, %rsi/*argv/envp pointer is 2nd argument of start*/
	andq  $~31, %rsp/*32 bytes aligned stack*/
	jmp start
