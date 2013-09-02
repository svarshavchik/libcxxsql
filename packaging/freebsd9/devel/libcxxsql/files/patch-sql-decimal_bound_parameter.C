diff --git a/sql/decimal_bound_parameter.C b/sql/decimal_bound_parameter.C
index 07dbe49..39a750a 100644
--- sql/decimal_bound_parameter.C
+++ sql/decimal_bound_parameter.C
@@ -2,6 +2,8 @@
 ** Copyright 2013 Double Precision, Inc.
 ** See COPYING for distribution information.
 */
+class max_align_t {
+};
 
 #include "libcxx_config.h"
 #include "sql_internal.H"
