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

fact:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp ,-32
TBD
lw $t0, -1($fp)
move $t1, $t0
li $t2, 1
sw $t1,-36($fp)
sw $t2,-40($fp)
beq $t1, $t2, l0
j l1
l0:
lw $t0, -1($fp)
move $t1, $t0
move $sp, $fp
lw $fp, 0($sp)
addi $sp, $sp, 4
lw $ra, 0($sp)
addi $sp, $sp ,4
move $v0, $t1
jr $ra
sw $t1,-44($fp)
j l2
l1:
lw $t0, -1($fp)
move $t1, $t0
move $t2, $t0
li $t3, 1
sub $t4, $t2, $t3
TBD
jar fact
move $t5, $v0
mul $t6, $t1, $t5
move $sp, $fp
lw $fp, 0($sp)
addi $sp, $sp, 4
lw $ra, 0($sp)
addi $sp, $sp ,4
move $v0, $t6
jr $ra
sw $t1,-48($fp)
sw $t2,-52($fp)
sw $t3,-56($fp)
sw $t4,-60($fp)
sw $t5,-64($fp)
sw $t6,-68($fp)
l2:
main:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp ,-32
jal read
move $t0, $v0
move $t1, $t0
move $t2, $t1
li $t3, 1
sw $t0,-36($fp)
sw $t1,-40($fp)
sw $t2,-44($fp)
sw $t3,-48($fp)
bgt $t2, $t3, l3
j l4
l3:
lw $t0, -40($fp)
move $t1, $t0
TBD
jar fact
move $t2, $v0
move $t3, $t2
sw $t1,-52($fp)
sw $t2,-56($fp)
sw $t3,-60($fp)
j l5
l4:
li $t0, 1
move $t1, $t0
sw $t0,-64($fp)
sw $t1,-60($fp)
l5:
lw $t0, -60($fp)
move $t1, $t0
move $a0, $t1
jal write
li $t2, 0
move $sp, $fp
lw $fp, 0($sp)
addi $sp, $sp, 4
lw $ra, 0($sp)
addi $sp, $sp ,4
move $v0, $t2
jr $ra
