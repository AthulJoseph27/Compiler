section .bss
input resb 8
num resb 8
temp resb 8
lho resb 8
rho resb 8
a resb 8
b resb 8
c resb 8
d resb 8
e resb 8
f resb 8
g resb 8
h resb 8
i resb 8
j resb 8
k resb 8
l resb 8
m resb 8
n resb 8
o resb 8
p resb 8
q resb 8
r resb 8
s resb 8
t resb 8
u resb 8
v resb 8
w resb 8
x resb 8
y resb 8
z resb 8
digitSpace resb 100
digitSpacePos resb 8
section .text
global _start
_scanInt:
	mov rax, 0
	mov rdi, 0
	mov rsi, input
	mov rdx, 1
	syscall
	mov rax, [input]
	cmp rax, 10
	je _exit
	mov rcx , [input]
	sub rcx , 30h
	mov rbx , 10
	mov rax , [num]
	mul rbx
	add rax , rcx
	mov [num] , rax
	jmp _scanInt
_exit:
	ret
_printRAX:
	mov rcx, digitSpace
	mov rbx, 10
	mov [rcx], rbx
	inc rcx
	mov [digitSpacePos], rcx
_printRAXLoop:	mov rdx, 0
	mov rbx, 10
	div rbx
	push rax
	add rdx, 48
	mov rcx, [digitSpacePos]
	mov [rcx], dl
	inc rcx
	mov [digitSpacePos], rcx
	
	pop rax
	cmp rax, 0
	jne _printRAXLoop
_printRAXLoop2:
	mov rcx, [digitSpacePos]
	mov rax, 1
	mov rdi, 1
	mov rsi, rcx
	mov rdx, 1
	syscall
	mov rcx, [digitSpacePos]
	dec rcx
	mov [digitSpacePos], rcx
	cmp rcx, digitSpace
	jge _printRAXLoop2
	ret
_compareLT:
	mov rax, [lho]
	cmp rax, [rho]
	jl _push_one
	jmp _push_zero
_compareGT:
	mov rax, [lho]
	cmp rax, [rho]
	jg _push_one
	jmp _push_zero
_compareLE:
	mov rax, [lho]
	cmp rax, [rho]
	jle _push_one
	jmp _push_zero
_compareGE:
	mov rax, [lho]
	cmp rax, [rho]
	jge _push_one
	jmp _push_zero
_compareEE:
	mov rax, [lho]
	cmp rax, [rho]
	je _push_one
	jmp _push_zero
_compareNE:
	mov rax, [lho]
	cmp rax, [rho]
	jne _push_one
	jmp _push_zero
_push_zero:
	mov rax, 0
	mov [temp], rax
	ret
_push_one:
	mov rax, 1
	mov [temp], rax
	ret
_start:
	push 0
	pop rax
	mov [a], rax
	jmp L000
L000:
	mov rax, 0
	mov [num], rax
	call _scanInt
	mov rax, [num]
	mov [b], rax
	mov rax, [a]
	push rax
	mov rax, [b]
	push rax
	pop rax
	mov [temp], rax
	pop rax
	add rax, [temp]
	push rax
	pop rax
	mov [a], rax
	mov rax, [b]
	push rax
	push 0
	pop rax
	mov [rho], rax
	pop rax
	mov [lho], rax
	call _compareNE
	mov rax,[temp]
	push rax
	pop rax
	cmp rax,1
	je L000
	jmp	L001
L001:
	mov rax, [a]
	push rax
	pop rax
	call _printRAX
	mov rax, [a]
	push rax
	push 32
	pop rax
	mov [temp], rax
	pop rax
	add rax, [temp]
	push rax
	pop rax
	call _printRAX
	mov rax, 60
	mov rdi, 0
	syscall
