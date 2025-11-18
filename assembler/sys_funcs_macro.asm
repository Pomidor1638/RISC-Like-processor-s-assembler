
#macro LOCAL_ALLOC size
    // Резервируем место в стеке
    LWI R6, SP_PTR
    LWD R7, R6      // R7 = текущий SP
    LWI R5, size    // R5 = размер
    SUB R7, R7, R5  // SP -= size
    SWD R6, R7      // Обновляем SP
#endmacro

#macro LOCAL_FREE size
    // Освобождаем место в стеке
    LWI R6, SP_PTR
    LWD R7, R6      // R7 = текущий SP
    LWI R5, size    // R5 = размер
    ADD R7, R7, R5  // SP += size
    SWD R6, R7      // Обновляем SP
#endmacro

#macro CALL func
    LWI R7, func
    JRL R7, R7    // Сохраняем адрес возврата в R7
#endmacro

#macro FUNCTION name, alloc_local
name:
    // Сохраняем адрес возврата
    PUSH R7
    
    // Сохраняем текущий BP в стеке
    LWI R6, BP_PTR
    LWD R7, R6      // R7 = текущий BP
    PUSH R7         // Сохраняем старый BP
    
    // Устанавливаем новый BP = текущий SP
    LWI R6, SP_PTR
    LWD R7, R6      // R7 = текущий SP
    LWI R6, BP_PTR
    SWD R6, R7      // BP = SP
    
    // Резервируем место для локальных переменных
    LOCAL_ALLOC alloc_local
#endmacro

#macro ENDFUNCTION
    // Восстанавливаем старый BP из стека
    POP R7          // R7 = старый BP
    LWI R6, BP_PTR
    SWD R6, R7      // Восстанавливаем BP
    
    // Восстанавливаем адрес возврата
    POP R7
    JPR R7          // Возврат
#endmacro
