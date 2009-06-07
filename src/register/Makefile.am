INCLUDES = -I$(top_srcdir)/src/errors -I$(top_srcdir)/src/hash_table \
	-I$(top_srcdir)/src/network_server $(all_includes)
METASOURCES = AUTO
bin_PROGRAMS = registerP2P
register_include_SOURCES = register.h
registerP2P_SOURCES = register.c
registerP2P_LDADD = $(top_builddir)/src/network_server/libnet_server.a \
	$(top_builddir)/src/hash_table/libhash_table.a -lpthread
