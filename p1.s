	.file	"p1.c"
	.section	.rodata
.LC0:
	.string	"%02X"
	.text
	.globl	print_bytes
	.type	print_bytes, @function
print_bytes:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movl	$40, %edi
	call	putchar
	movq	$0, -8(%rbp)
	jmp	.L2
.L3:
	movq	-8(%rbp), %rax
	movq	-24(%rbp), %rdx
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movzbl	%al, %eax
	movl	%eax, %esi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	addq	$1, -8(%rbp)
.L2:
	movq	-8(%rbp), %rax
	cmpq	-32(%rbp), %rax
	jb	.L3
	movl	$41, %edi
	call	putchar
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	print_bytes, .-print_bytes
	.globl	p
	.type	p, @function
p:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movl	$40, %edi
	call	putchar
	movq	$0, -8(%rbp)
	jmp	.L5
.L6:
	movq	-8(%rbp), %rax
	movq	-24(%rbp), %rdx
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movzbl	%al, %eax
	movl	%eax, %esi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	addq	$1, -8(%rbp)
.L5:
	movq	-8(%rbp), %rax
	cmpq	-32(%rbp), %rax
	jb	.L6
	movl	$41, %edi
	call	putchar
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	p, .-p
	.globl	ex_endian
	.type	ex_endian, @function
ex_endian:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	-32(%rbp), %rax
	andl	$1, %eax
	testq	%rax, %rax
	je	.L8
	jmp	.L7
.L8:
	movl	$0, -4(%rbp)
	jmp	.L10
.L11:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movb	%al, -5(%rbp)
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-24(%rbp), %rax
	addq	%rax, %rdx
	movl	-4(%rbp), %eax
	cltq
	movq	-32(%rbp), %rcx
	subq	%rax, %rcx
	movq	%rcx, %rax
	leaq	-1(%rax), %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	movzbl	(%rax), %eax
	movb	%al, (%rdx)
	movl	-4(%rbp), %eax
	cltq
	movq	-32(%rbp), %rdx
	subq	%rax, %rdx
	movq	%rdx, %rax
	leaq	-1(%rax), %rdx
	movq	-24(%rbp), %rax
	addq	%rax, %rdx
	movzbl	-5(%rbp), %eax
	movb	%al, (%rdx)
	addl	$1, -4(%rbp)
.L10:
	movl	-4(%rbp), %eax
	cltq
	movq	-32(%rbp), %rdx
	shrq	%rdx
	cmpq	%rdx, %rax
	jb	.L11
.L7:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	ex_endian, .-ex_endian
	.globl	ibp_type
	.type	ibp_type, @function
ibp_type:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	movzbl	-4(%rbp), %eax
	andb	$3, %al
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	ibp_type, .-ibp_type
	.globl	is_fill
	.type	is_fill, @function
is_fill:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	movzbl	-4(%rbp), %eax
	andl	$-128, %eax
	cmpl	$128, %eax
	sete	%al
	movzbl	%al, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	is_fill, .-is_fill
	.globl	data_len
	.type	data_len, @function
data_len:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	data_len, .-data_len
	.comm	mutex,40,32
	.comm	prod,8,8
	.comm	cons,80,32
	.comm	t_test,8,8
	.comm	s_full,32,32
	.comm	s_empty,32,32
	.comm	put,4,4
	.comm	get,4,4
	.comm	count,4,4
	.comm	list,80,32
	.comm	fbin,8,8
	.comm	fh264,8,8
	.comm	frams,320,32
	.globl	m
	.bss
	.align 32
	.type	m, @object
	.size	m, 40
m:
	.zero	40
	.globl	m1
	.align 32
	.type	m1, @object
	.size	m1, 40
m1:
	.zero	40
	.text
	.globl	list_is_full
	.type	list_is_full, @function
list_is_full:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$m, %edi
	call	pthread_mutex_lock
	movl	put(%rip), %eax
	leal	1(%rax), %ecx
	movl	$1717986919, %edx
	movl	%ecx, %eax
	imull	%edx
	sarl	$3, %edx
	movl	%ecx, %eax
	sarl	$31, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	subl	%eax, %ecx
	movl	%ecx, %edx
	movl	get(%rip), %eax
	cmpl	%eax, %edx
	sete	%al
	movzbl	%al, %eax
	movl	%eax, -4(%rbp)
	movl	$m, %edi
	call	pthread_mutex_unlock
	movl	-4(%rbp), %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	list_is_full, .-list_is_full
	.globl	list_is_empty
	.type	list_is_empty, @function
list_is_empty:
.LFB9:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$m, %edi
	call	pthread_mutex_lock
	movl	get(%rip), %edx
	movl	put(%rip), %eax
	cmpl	%eax, %edx
	sete	%al
	movzbl	%al, %eax
	movl	%eax, -4(%rbp)
	movl	$m, %edi
	call	pthread_mutex_unlock
	movl	-4(%rbp), %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	list_is_empty, .-list_is_empty
	.comm	fin,4,4
	.section	.rodata
	.align 8
.LC1:
	.string	" [RANK%d  %d]: get: %d  put: %d count: %d\n"
	.text
	.globl	producer
	.type	producer, @function
producer:
.LFB10:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movq	%rdi, -72(%rbp)
	movl	$0, fin(%rip)
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	jmp	.L23
.L27:
	movzwl	-50(%rbp), %eax
	testw	%ax, %ax
	jne	.L23
	movq	fbin(%rip), %rdx
	leaq	-52(%rbp), %rax
	movq	%rdx, %rcx
	movl	$1, %edx
	movl	$2, %esi
	movq	%rax, %rdi
	call	fread
	cmpq	$1, %rax
	je	.L24
	jmp	.L29
.L24:
	addl	$1, -4(%rbp)
	leaq	-52(%rbp), %rax
	movl	$2, %esi
	movq	%rax, %rdi
	call	ex_endian
	movzwl	-52(%rbp), %eax
	cmpw	$250, %ax
	jne	.L23
	movq	fbin(%rip), %rax
	movl	$1, %edx
	movq	$-4, %rsi
	movq	%rax, %rdi
	call	fseek
	movq	fbin(%rip), %rdx
	leaq	-48(%rbp), %rax
	movq	%rdx, %rcx
	movl	$1, %edx
	movl	$16, %esi
	movq	%rax, %rdi
	call	fread
	movq	fbin(%rip), %rax
	movl	$1, %edx
	movq	$-2, %rsi
	movq	%rax, %rdi
	call	fseek
	leaq	-48(%rbp), %rax
	movl	$4, %esi
	movq	%rax, %rdi
	call	ex_endian
	movl	-44(%rbp), %eax
	movl	%eax, %edi
	call	data_len
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	movq	%rax, %rdi
	call	malloc
	movq	%rax, -24(%rbp)
	movq	fbin(%rip), %rdx
	movl	-12(%rbp), %esi
	movq	-24(%rbp), %rax
	movq	%rdx, %rcx
	movl	$1, %edx
	movq	%rax, %rdi
	call	fread
	nop
.L26:
	movl	$0, %eax
	call	list_is_full
	testl	%eax, %eax
	jne	.L26
	movl	put(%rip), %edx
	movl	-12(%rbp), %eax
	movslq	%edx, %rdx
	salq	$4, %rdx
	addq	$frams, %rdx
	movl	%eax, 8(%rdx)
	movl	put(%rip), %eax
	cltq
	salq	$4, %rax
	leaq	frams(%rax), %rdx
	movq	-24(%rbp), %rax
	movq	%rax, (%rdx)
	movl	put(%rip), %eax
	leal	1(%rax), %ecx
	movl	$1717986919, %edx
	movl	%ecx, %eax
	imull	%edx
	sarl	$3, %edx
	movl	%ecx, %eax
	sarl	$31, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	subl	%eax, %ecx
	movl	%ecx, %edx
	movl	%edx, put(%rip)
	movl	count(%rip), %eax
	addl	$1, %eax
	movl	%eax, count(%rip)
	movl	count(%rip), %edx
	movl	put(%rip), %esi
	movl	get(%rip), %ecx
	movq	stdout(%rip), %rax
	movl	%edx, (%rsp)
	movl	%esi, %r9d
	movl	%ecx, %r8d
	movl	$161, %ecx
	movl	$0, %edx
	movl	$.LC1, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf
.L23:
	movq	fbin(%rip), %rdx
	leaq	-50(%rbp), %rax
	movq	%rdx, %rcx
	movl	$1, %edx
	movl	$2, %esi
	movq	%rax, %rdi
	call	fread
	cmpq	$1, %rax
	je	.L27
	movl	$1, fin(%rip)
	nop
.L29:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	producer, .-producer
	.section	.rodata
	.align 8
.LC2:
	.string	" [RANK%d  %d]: get:%d put:%d count: %d\n"
	.align 8
.LC3:
	.string	" [RANK%d  %d]: \n\nconsumer finish!\n\n"
	.text
	.globl	consumer
	.type	consumer, @function
consumer:
.LFB11:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	call	pthread_self
	movq	%rax, -8(%rbp)
	jmp	.L31
.L33:
	nop
.L32:
	movl	$0, %eax
	call	list_is_empty
	testl	%eax, %eax
	jne	.L32
	movl	$m, %edi
	call	pthread_mutex_lock
	movl	count(%rip), %ecx
	movl	put(%rip), %edi
	movl	get(%rip), %esi
	movq	stdout(%rip), %rax
	movq	-8(%rbp), %rdx
	movl	%ecx, (%rsp)
	movl	%edi, %r9d
	movl	%esi, %r8d
	movl	$207, %ecx
	movl	$.LC2, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf
	movl	get(%rip), %eax
	leal	1(%rax), %ecx
	movl	$1717986919, %edx
	movl	%ecx, %eax
	imull	%edx
	sarl	$3, %edx
	movl	%ecx, %eax
	sarl	$31, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	subl	%eax, %ecx
	movl	%ecx, %edx
	movl	%edx, get(%rip)
	movl	count(%rip), %eax
	subl	$1, %eax
	movl	%eax, count(%rip)
	movl	$m, %edi
	call	pthread_mutex_unlock
	movq	fh264(%rip), %rdx
	movl	get(%rip), %eax
	cltq
	salq	$4, %rax
	addq	$frams, %rax
	movl	8(%rax), %eax
	movslq	%eax, %rsi
	movl	get(%rip), %eax
	cltq
	salq	$4, %rax
	addq	$frams, %rax
	movq	(%rax), %rax
	movq	%rdx, %rcx
	movl	$1, %edx
	movq	%rax, %rdi
	call	fwrite
.L31:
	movl	fin(%rip), %eax
	testl	%eax, %eax
	je	.L33
	movq	stdout(%rip), %rax
	movl	$215, %ecx
	movl	$333, %edx
	movl	$.LC3, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	consumer, .-consumer
	.section	.rodata
.LC4:
	.string	"rb"
.LC5:
	.string	"./BB.bin"
.LC6:
	.string	"wb"
.LC7:
	.string	"./BB.h264.mp4"
.LC8:
	.string	" [RANK%d  %d]: \n\nIN JOIN!\n\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB12:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$0, %edx
	movl	$0, %esi
	movl	$s_full, %edi
	call	sem_init
	movl	$0, %edx
	movl	$0, %esi
	movl	$s_empty, %edi
	call	sem_init
	movl	$0, count(%rip)
	movl	$0, put(%rip)
	movl	put(%rip), %eax
	movl	%eax, get(%rip)
	movl	$.LC4, %esi
	movl	$.LC5, %edi
	call	fopen
	movq	%rax, fbin(%rip)
	movl	$.LC6, %esi
	movl	$.LC7, %edi
	call	fopen
	movq	%rax, fh264(%rip)
	movl	$0, %ecx
	movl	$producer, %edx
	movl	$0, %esi
	movl	$prod, %edi
	call	pthread_create
	movl	$0, -4(%rbp)
	jmp	.L36
.L37:
	movl	-4(%rbp), %eax
	cltq
	salq	$3, %rax
	addq	$cons, %rax
	movl	$0, %ecx
	movl	$consumer, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_create
	addl	$1, -4(%rbp)
.L36:
	cmpl	$1, -4(%rbp)
	jle	.L37
	movq	prod(%rip), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_join
	movl	$0, -8(%rbp)
	jmp	.L38
.L39:
	movl	-8(%rbp), %eax
	cltq
	movq	cons(,%rax,8), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_join
	movq	stdout(%rip), %rax
	movl	$242, %ecx
	movl	$444, %edx
	movl	$.LC8, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf
	addl	$1, -8(%rbp)
.L38:
	cmpl	$1, -8(%rbp)
	jle	.L39
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-16)"
	.section	.note.GNU-stack,"",@progbits
