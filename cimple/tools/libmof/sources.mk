libmof_PURPOSE = host
libmof_GENERATED =  MOF_Yacc.cpp \
    MOF_Lex.cpp \
    REF_Yacc.cpp \
    REF_Lex.cpp

libmof_SOURCES = \
	$(libmof_GENERATED) \
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
libmof_DEPENDS = libtools
libmof_INCLUDES = $(libmof_DIR)
libmof_CPPFLAGS = $(libmof_PROVIDE_CPPFLAGS) -DMOF_INTERNAL
libmof_MOF_Lex_CXXFLAGS = -Wno-sign-compare
libmof_SDK = 1

$(eval $(call component,libmof,STATIC_LIBRARIES))

libmof_EXTRA_CLEAN = rm $(patsubst %.cpp,%.h,$(filter %_Yacc.cpp,$(_libmof_GENERATED)))
