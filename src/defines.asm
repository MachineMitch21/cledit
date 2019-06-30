%ifndef DEFINES_ASM
%define DEFINES_ASM

; file descriptors for standard io paths
STDIN 					equ 0
STDOUT 					equ 1
STDERR 					equ 2

; system call numbers to be set in the eax register before the 0x80 interrupt is called
SYS_EXIT				equ 1
SYS_FORK				equ 2
SYS_READ				equ 3
SYS_WRITE				equ 4
SYS_OPEN				equ 5
SYS_CLOSE 			equ 6
SYS_WAITPID			equ 7
SYS_CREAT				equ 8
SYS_LINK				equ 9
SYS_UNLINK			equ 0x0a
SYS_EXECVE			equ 0x0b
SYS_CHDIR				equ 0x0c
SYS_TIME				equ 0x0d
SYS_MKNOD				equ 0x0e
SYS_LCHOWN16		equ 0x10
SYS_STAT				equ 0x12
SYS_LSEEK				equ 0x13
SYS_GETPID			equ 0x14
SYS_MOUNT				equ 0x15
SYS_OLDUMOUNT		equ 0x16
SYS_SETUID16		equ 0x17
SYS_GETUID16		equ 0x18
SYS_STIME				equ 0x19
SYS_PTRACE			equ 0x1a
SYS_ALARM				equ 0x1b
SYS_FSTAT				equ 0x1c
SYS_PAUSE				equ 0x1d

%endif