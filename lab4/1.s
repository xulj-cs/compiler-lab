.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
li $v0, 4
la $a0, _prompt
syscall
li $v0, 5
syscall
jr $ra

write:
li $v0, 1
syscall
li $v0, 4
la $a0, _ret
syscall
move $v0, $0
jr $ra

main:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp ,-32
li $t0, 0
li $t1, 1
li $t2, 0
jal read
move $t3, $v0
move $t4, $t3
sw $t0,-36($fp)
sw $t1,-40($fp)
sw $t2,-44($fp)
sw $t3,-48($fp)
sw $t4,-52($fp)
l0:
lw $t0, -44($fp)
move $t1, $t0
lw $t2, -52($fp)
move $t3, $t2
sw $t1,-56($fp)
sw $t3,-60($fp)
blt $t1, $t3, l1
j l2
l1:
lw $t0, -36($fp)
move $t1, $t0
lw $t2, -40($fp)
move $t3, $t2
add $t4, $t1, $t3
move $t5, $t2
move $a0, $t5
jal write
move $t6, $t2
move $t0, $t6
move $t7, $t4
move $t2, $t7
lw $t8, -44($fp)
move $t9, $t8
li $s0, 1
add $s1, $t9, $s0
move $t8, $s1
sw $t0,-36($fp)
sw $t1,-64($fp)
sw $t2,-40($fp)
sw $t3,-68($fp)
sw $t4,-72($fp)
sw $t5,-76($fp)
sw $t6,-80($fp)
sw $t7,-84($fp)
sw $t8,-44($fp)
sw $t9,-88($fp)
sw $s0,-92($fp)
sw $s1,-96($fp)
j l0
l2:
li $t0, 0
move $v0, $t0
jr $ra
