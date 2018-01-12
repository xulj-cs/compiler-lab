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
li $t1, 5
move $t2, $t1
move $t3, $t2
move $t4, $t2
mul $t5, $t3, $t4
move $t6, $t2
li $t7, 2
add $t8, $t6, $t7
mul $t9, $t5, $t8
move $s0, $t9
move $s1, $s0
move $s2, $t2
div $s1, $s2
mflo $s3
li $s4, 1
add $s5, $s3, $s4
move $s6, $s5
move $s7, $t2

sw $t0,-36($fp)
move $t0, $s0
sw $t0,-40($fp)
add $t0, $s7, $t0
sw $t0,-44($fp)
move $t0, $s6
sw $t0,-48($fp)
li $t0, 3
sw $t0,-52($fp)
lw $t0, -48($fp)
sw $t0,-48($fp)
mul $t0, $t0, $t0
sw $t0,-56($fp)
lw $t0, -44($fp)
sw $t0,-44($fp)
sub $t0, $t0, $t0
sw $t0,-60($fp)
move $t0, $s0
sw $t0,-64($fp)
move $t0, $t2
sw $t0,-68($fp)
lw $t0, -64($fp)
div $t0, $t0
sw $t0,-64($fp)
mflo $t0
sw $t0,-72($fp)
li $t0, 4
sw $t0,-76($fp)
lw $t0, -72($fp)
sw $t0,-72($fp)
sub $t0, $t0, $t0
sw $t0,-80($fp)
lw $t0, -60($fp)
sw $t0,-60($fp)
add $t0, $t0, $t0
sw $t0,-84($fp)
move $t0, $t0
sw $t0,-36($fp)
move $t0, $t0
move $a0, $t0
jal write
sw $t0,-88($fp)
li $t0, 0
move $sp, $fp
lw $fp, 0($sp)
addi $sp, $sp, 4
lw $ra, 0($sp)
addi $sp, $sp ,4
move $v0, $t0
jr $ra
