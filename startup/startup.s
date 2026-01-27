.syntax unified
.cpu cortex-m7
.thumb

.global _estack
.global Reset_Handler

.section .isr_vector,"a",%progbits
.word _estack
.word Reset_Handler

.section .text.Reset_Handler
Reset_Handler:
    bl main
1:  b 1b

