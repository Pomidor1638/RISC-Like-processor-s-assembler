

.org 0x8000

_loop_sys_call:
	MOV R0, R2, R3
	ADD R3, R5, R6
	SUB R5, R3, R0
	LWI R3, _loop_sys_request
	JPR R3

.org 0x0000

_loop_sys_request:
	HLT

START:
	LWI R0, 0x0000
	LWI R1, _loop_sys_call
	JPR R1

