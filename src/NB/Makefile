CONFIG_NAME					= NisseBolt
THORSANVIL_ROOT				?= $(realpath ../../)
COVERAGE_REQUIRED			= 60
TARGET						= NB.prog
FILE_WARNING_FLAGS			+= -Wno-missing-braces -Wno-deprecated-copy-with-user-provided-dtor -Wno-deprecated-declarations

LDLIBS_EXTERN_BUILD			+= ThorSerialize MagicEnum Nisse event
LDLIBS						+= -lboost_filesystem -lboost_context -lcrypto -lssl

XUNITTEST_LDLIBS				+= $(EVENT_THREADS_LIB)
UNITTEST_LDLIBS_HEADERONLY	+= -lcrypto -lssl
UNITTEST_LDLIBS				+= -lcrypto -lssl


include $(THORSANVIL_ROOT)/build/tools/Makefile
