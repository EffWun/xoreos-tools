include $(top_srcdir)/Makefile.common

SUBDIRS = common aurora xml

noinst_HEADERS = util.h

bin_PROGRAMS = gff2xml unerf unrim unkeybif unnds

gff2xml_SOURCES = gff2xml.cpp
gff2xml_LDADD = xml/libxml.la aurora/libaurora.la common/libcommon.la

unerf_SOURCES = unerf.cpp util.cpp
unerf_LDADD = aurora/libaurora.la common/libcommon.la

unrim_SOURCES = unrim.cpp util.cpp
unrim_LDADD = aurora/libaurora.la common/libcommon.la

unkeybif_SOURCES = unkeybif.cpp util.cpp
unkeybif_LDADD = aurora/libaurora.la common/libcommon.la

unnds_SOURCES = unnds.cpp util.cpp
unnds_LDADD = aurora/libaurora.la common/libcommon.la
