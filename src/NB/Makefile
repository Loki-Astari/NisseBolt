CONFIG_NAME					= NisseBolt
THORSANVIL_ROOT				?= $(realpath ../../)
TARGET						= NB.prog
FILE_WARNING_FLAGS			+= -Wno-missing-braces

LDLIBS_EXTERN_BUILD			+= ThorSerialize MagicEnum Nisse event
LDLIBS						+= -lboost_filesystem -lboost_context

XUNITTEST_LDLIBS				+= $(EVENT_THREADS_LIB)


include $(THORSANVIL_ROOT)/build/tools/Makefile
