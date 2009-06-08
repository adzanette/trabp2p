INCLUDES = -I$(top_srcdir)/src/errors -I$(top_srcdir)/src/hash_table $(all_includes)
METASOURCES = AUTO
bin_PROGRAMS = serventP2P
serventP2P_SOURCES = serventMain.c
noinst_HEADERS = servent.h
serventP2P_LDADD = $(top_builddir)/src/hash_table/libhash_table.a -lpthread

