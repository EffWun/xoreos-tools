include $(top_srcdir)/Makefile.common

SUBDIRS = common aurora xml

bin_PROGRAMS = gff2xml

gff2xml_SOURCES = gff2xml.cpp
gff2xml_LDADD = xml/libxml.la aurora/libaurora.la common/libcommon.la
