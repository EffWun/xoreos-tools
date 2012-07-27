include $(top_srcdir)/Makefile.common

SUBDIRS = common aurora xml

bin_PROGRAMS = gff2xml unerf

gff2xml_SOURCES = gff2xml.cpp
gff2xml_LDADD = xml/libxml.la aurora/libaurora.la common/libcommon.la

unerf_SOURCES = unerf.cpp
unerf_LDADD = aurora/libaurora.la common/libcommon.la
