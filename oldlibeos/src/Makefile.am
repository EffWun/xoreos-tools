AUTOMAKE_OPTIONS = gnu
AM_CXXFLAGS = -I../lib -ggdb -Wall -Wpointer-arith -Werror -Wno-unused-variable -fno-strict-aliasing
bin_PROGRAMS = eos
eos_SOURCES = assemble.cpp common.cpp eos.cpp explode.cpp extract.cpp info.cpp xmlstuff.cpp
noinst_HEADERS = assemble.h common.h explode.h extract.h info.h xmlstuff.h
eos_LDADD = ../lib/libeos.la
