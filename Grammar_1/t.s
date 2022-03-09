.text

.globl main

main:	
	sw $t2, 50
	sub $sp,$sp,4
	sw $t2,($sp)
	lw $t2,($sp)
	addiu $sp,$sp,4
	li $v0 , 1
	li $a0 , $t2
	syscall
	li $v0 , 4
	li $a0 , nl
	syscall

	li $v0 , 4
    la $a0 , endl
    syscall

    # Terminating
    li $v0 , 10
    syscall

.data

endl: .asciiz "\n"
spc: .asciiz " "