CONFIG_NAME					= NisseBolt
THORSANVIL_ROOT				?= $(realpath ../../)
TARGET						= NisseBolt.slib

LDLIBS_EXTERN_BUILD			+= ThorSerialize MagicEnum Nisse event

UNITTEST_LDLIBS				+= $(EVENT_THREADS_LIB)


include $(THORSANVIL_ROOT)/build/tools/Makefile
