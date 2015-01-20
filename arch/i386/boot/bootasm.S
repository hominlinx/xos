
#start the CPU
# %cs = 0 ; %ip = 7c00

.globl start
start:
.code16
    cli
    cld
    call kernel_init
