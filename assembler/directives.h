#pragma once

enum PreprocessorDirective {
	PREP_INCLUDE,
	PREP_DEFINE,
	PREP_IF,
	PREP_IFDEF,
	PREP_IFNDEF,
	PREP_ENDIF,
	PREP_MACRO
};

const std::map<std::string, PreprocessorDirective> PREPROCESSOR_DIRECTIVES = {
	{"#include", PREP_INCLUDE},
	{"#define",  PREP_DEFINE },
	{"#if"   ,   PREP_IF     },
	{"#ifdef",   PREP_IFDEF  },
	{"#ifndef",  PREP_IFNDEF },
	{"#endif",   PREP_ENDIF  },
	{"#macro",   PREP_MACRO  }
};

enum AssemblerDirective {
	ASM_DATA,
	ASM_STRING,
	ASM_ORG,
	ASM_ALIGN,
	ASM_RESERVE,
	ASM_EQU
};

const std::map<std::string, AssemblerDirective> ASSEMBLER_DIRECTIVES = {
	{".data",    ASM_DATA},
	{".string",  ASM_STRING},
	{".org",     ASM_ORG},
	{".align",   ASM_ALIGN},
	{".reserve", ASM_RESERVE},
	{".equ",     ASM_EQU}
};