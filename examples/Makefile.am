@LIBCXX_AM@
@LIBCXXSQL_AM@

bin_PROGRAMS=helloworld

$(call SCHEMA_GEN,testschema.inc)

helloworld_SOURCES=helloworld.C
helloworld_LDADD=-lcxxsql -lcxx
