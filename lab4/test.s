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

hanoi:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp ,-32
move $t0,$a0
move $t1,$a1
move $t2,$a2
move $t3,$a3
move $t4, $t0
li $t5, 1
sw $t0,-36($fp)
sw $t1,-40($fp)
sw $t2,-44($fp)
sw $t3,-48($fp)
sw $t4,-52($fp)
sw $t5,-56($fp)
beq $t4, $t5, l0
j l1
l0:
lw $t0, -40($fp)
move $t1, $t0
li $t2, 1000000
mul $t3, $t1, $t2
lw $t4, -48($fp)
move $t5, $t4
add $t6, $t3, $t5
move $a0, $t6
jal write
sw $t1,-60($fp)
sw $t2,-64($fp)
sw $t3,-68($fp)
sw $t5,-72($fp)
sw $t6,-76($fp)
j l2
l1:
lw $t0, -36($fp)
move $t1, $t0
li $t2, 1
sub $t3, $t1, $t2
lw $t4, -40($fp)
move $t5, $t4
lw $t6, -48($fp)
move $t7, $t6
lw $t8, -44($fp)
move $t9, $t8
sw $t1,-80($fp)
sw $t2,-84($fp)
sw $t3,-88($fp)
sw $t5,-92($fp)
sw $t7,-96($fp)
sw $t9,-100($fp)
addi $sp, $sp, -68
move $a3, $t9
move $a2, $t7
move $a1, $t5
move $a0, $t3
jal hanoi
move $t0, $v0
lw $t1, -40($fp)
move $t2, $t1
li $t3, 1000000
mul $t4, $t2, $t3
lw $t5, -48($fp)
move $t6, $t5
add $t7, $t4, $t6
move $a0, $t7
jal write
lw $t8, -36($fp)
move $t9, $t8
li $s0, 1
sub $s1, $t9, $s0
lw $s2, -44($fp)
move $s3, $s2
move $s4, $t1
move $s5, $t5
sw $t0,-104($fp)
sw $t2,-108($fp)
sw $t3,-112($fp)
sw $t4,-116($fp)
sw $t6,-120($fp)
sw $t7,-124($fp)
sw $t9,-128($fp)
sw $s0,-132($fp)
sw $s1,-136($fp)
sw $s3,-140($fp)
sw $s4,-144($fp)
sw $s5,-148($fp)
addi $sp, $sp, -116
move $a3, $s5
move $a2, $s4
move $a1, $s3
move $a0, $s1
jal hanoi
move $t0, $v0
sw $t0,-152($fp)
l2:
li $t0, 0
move $sp, $fp
lw $fp, 0($sp)
addi $sp, $sp, 4
lw $ra, 0($sp)
addi $sp, $sp ,4
move $v0, $t0
jr $ra

main:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp ,-32
li $t0, 3
move $t1, $t0
li $t2, 1
li $t3, 2
li $t4, 3
sw $t0,-36($fp)
sw $t1,-40($fp)
sw $t2,-44($fp)
sw $t3,-48($fp)
sw $t4,-52($fp)
addi $sp, $sp, -20
move $a3, $t4
move $a2, $t3
move $a1, $t2
move $a0, $t1
jal hanoi
move $t0, $v0
li $t1, 0
move $sp, $fp
lw $fp, 0($sp)
addi $sp, $sp, 4
lw $ra, 0($sp)
addi $sp, $sp ,4
move $v0, $t1
jr $ra
