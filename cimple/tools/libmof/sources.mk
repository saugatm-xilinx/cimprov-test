INCLUDES += -I.

DEFINES += -DMOF_INTERNAL

libmof_parser_SOURCES =  MOF_Yacc.cpp \
    MOF_Lex.cpp \
    REF_Yacc.cpp \
    REF_Lex.cpp

libmof_SOURCES = \
	$(libmof_parser_SOURCES) \
    MOF_Buffer.cpp \
    MOF_Class_Decl.cpp \
    MOF_Data_Type.cpp \
    MOF_Element.cpp \
    MOF_Error.cpp \
    MOF_Feature.cpp \
    MOF_Feature_Info.cpp \
    MOF_Flavor.cpp \
    MOF_Indent.cpp \
    MOF_Instance_Decl.cpp \
    MOF_Key_Value_Pair.cpp \
    MOF_Lex_Utils.cpp \
    MOF_Literal.cpp \
    MOF_Method_Decl.cpp \
    MOF_Named_Element.cpp \
    MOF_Object_Reference.cpp \
    MOF_Options.cpp \
    MOF_Parameter.cpp \
    MOF_Parser.cpp \
    MOF_Pragma.cpp \
    MOF_Property.cpp \
    MOF_Property_Decl.cpp \
    MOF_Qualified_Element.cpp \
    MOF_Qualifier.cpp \
    MOF_Qualifier_Decl.cpp \
    MOF_Qualifier_Info.cpp \
    MOF_Reference_Decl.cpp \
    MOF_String.cpp \
    REF_Parser.cpp

libmof_DIR = cimple/tools/libmof
libmof_TARGET = libmof.a

$(libmof_DIR)/%.o $(libmof_DIR)/%.d : CPPFLAGS += -I$(libmof_DIR) -DMOF_INTERNAL

$(eval $(call component,libmof,STATIC_LIBRARIES))

_libmof_parser_SOURCES = $(addprefix $(libmof_DIR)/,$(libmof_parser_SOURCES))
$(filter-out $(_libmof_parser_SOURCES),$(_libmof_SOURCES)) : $(_libmof_parser_SOURCES)

.PHONY : clean-libmof-parser
clean-libmof : clean-libmof-parser

clean-libmof-parser:
	-rm -f $(addprefix $(libmof_DIR)/,$(libmof_parser_SOURCES))
	-rm -f $(addprefix $(libmof_DIR)/,$(patsubst %.cpp,%.h,$(filter %_Yacc.cpp,$(libmof_parser_SOURCES))))
