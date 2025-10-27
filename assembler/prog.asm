// R0 - счетчик
// R1 - инкремент
// R2 - порог
// R3 - результат проверки перехода
// R4 - адрес цикла
// R5 - адрес выхода из цикла



_loop:
	SUB R3, R2, R0
	JEZ R5, R3
	ADD R0, R0, R1
	JPR R4

_HALTING:
	HLT

START:
	LWI R0, 0 // 2
	LWI R1, 1 // 2
	LWI R2, 16 // 2
	LWI R4, _loop	 // 2
	LWI R5, _HALTING // 2 
	JPR R4			 // 1


