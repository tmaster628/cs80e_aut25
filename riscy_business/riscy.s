# This is a starter file for your RISC-y Business Lab
# Please read the assingment handout for an implementation specification.

# Here's some sample code from the Venus specification.
# Place a breakpoint on the first line of the code and 
# Run the program by hitting the green play button in the top
# right corner.
# Hit the down arrow key to step through the execution.

loop:
li a0, 0x122
ecall #This call detects which buttons are pressed.
mv a1, a0
#LED's use A1 as the arg.
li a0, 0x121
ecall #This call lights up the selected buttons.
j loop