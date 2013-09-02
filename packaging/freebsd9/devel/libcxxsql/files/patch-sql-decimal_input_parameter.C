diff --git a/sql/decimal_input_parameter.C b/sql/decimal_input_parameter.C
index 9a5d084..6e8be3f 100644
--- sql/decimal_input_parameter.C
+++ sql/decimal_input_parameter.C
@@ -2,6 +2,8 @@
 ** Copyright 2013 Double Precision, Inc.
 ** See COPYING for distribution information.
 */
+class max_align_t {
+};
 
 #include "libcxx_config.h"
 #include "sql_internal.H"
