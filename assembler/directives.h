#pragma once

constexpr int MAX_INCLUDE_DEPTH = 99;

enum PreprocessorDirective 
{
	PREP_INCLUDE = 0,
	PREP_DEFINE,
	PREP_IF,
	PREP_ELSE,
	PREP_ELIF,
	PREP_ELIFDEF,
	PREP_ELIFNDEF,
	PREP_IFDEF,
	PREP_IFNDEF,
	PREP_ENDIF,
	PREP_MACRO,
	PREP_ENDMACRO,

	PREPROCESSOR_DIRECTIVES_COUNT
};

const std::map<std::string, PreprocessorDirective> PREPROCESSOR_DIRECTIVES = {
	{"include" , PREP_INCLUDE },
	{"define"  , PREP_DEFINE  },
	{"if"      , PREP_IF      },
	{"elif"    , PREP_ELIF    },
	{"else"    , PREP_ELSE    },
	{"elifdef" , PREP_ELIFDEF },
	{"elifndef", PREP_ELIFNDEF},
	{"ifdef"   , PREP_IFDEF   },
	{"ifndef"  , PREP_IFNDEF  },
	{"endif"   , PREP_ENDIF   },
	{"macro"   , PREP_MACRO   },
	{"endmacro", PREP_ENDMACRO},
};

enum AssemblerDirective
{
	ASM_BYTE = 0,
	ASM_DATA16,
	ASM_DATA32,

	ASM_STRING,
	ASM_INCBIN,
	ASM_DIRECTIVES_COUNT
};



const std::map<std::string, AssemblerDirective> ASSEMBLER_DIRECTIVES =
{
	{"byte"  , ASM_BYTE  },
	{"data16", ASM_DATA16},
	{"data32", ASM_DATA32},
	{"string", ASM_STRING},
	{"incbin", ASM_INCBIN},
};