#ifdef x_sql_connectionobj_H
virtual bool config_get_accessible_procedures()=0;
//!< Return the SQL_ACCESSIBLE_PROCEDURES configuration setting
virtual bool config_get_accessible_tables()=0;
//!< Return the SQL_ACCESSIBLE_TABLES configuration setting
virtual long config_get_active_environments()=0;
//!< Return the SQL_ACTIVE_ENVIRONMENTS configuration setting
virtual config_bitmask_t config_get_aggregate_functions()=0;
//!< Return the SQL_AGGREGATE_FUNCTIONS configuration setting
virtual config_bitmask_t config_get_alter_domain()=0;
//!< Return the SQL_ALTER_DOMAIN configuration setting
virtual config_bitmask_t config_get_alter_table()=0;
//!< Return the SQL_ALTER_TABLE configuration setting
virtual long config_get_async_dbc_functions()=0;
//!< Return the SQL_ASYNC_DBC_FUNCTIONS configuration setting
virtual config_bitmask_t config_get_async_mode()=0;
//!< Return the SQL_ASYNC_MODE configuration setting
virtual config_bitmask_t config_get_batch_row_count()=0;
//!< Return the SQL_BATCH_ROW_COUNT configuration setting
virtual config_bitmask_t config_get_batch_support()=0;
//!< Return the SQL_BATCH_SUPPORT configuration setting
virtual config_bitmask_t config_get_bookmark_persistence()=0;
//!< Return the SQL_BOOKMARK_PERSISTENCE configuration setting
virtual config_bitmask_t config_get_catalog_location()=0;
//!< Return the SQL_CATALOG_LOCATION configuration setting
virtual bool config_get_catalog_name()=0;
//!< Return the SQL_CATALOG_NAME configuration setting
virtual std::string config_get_catalog_name_separator()=0;
//!< Return the SQL_CATALOG_NAME_SEPARATOR configuration setting
virtual std::string config_get_catalog_term()=0;
//!< Return the SQL_CATALOG_TERM configuration setting
virtual config_bitmask_t config_get_catalog_usage()=0;
//!< Return the SQL_CATALOG_USAGE configuration setting
virtual std::string config_get_collation_seq()=0;
//!< Return the SQL_COLLATION_SEQ configuration setting
virtual bool config_get_column_alias()=0;
//!< Return the SQL_COLUMN_ALIAS configuration setting
virtual config_bitmask_t config_get_concat_null_behavior()=0;
//!< Return the SQL_CONCAT_NULL_BEHAVIOR configuration setting
virtual config_bitmask_t config_get_convert_bigint()=0;
//!< Return the SQL_CONVERT_BIGINT configuration setting
virtual config_bitmask_t config_get_convert_binary()=0;
//!< Return the SQL_CONVERT_BINARY configuration setting
virtual config_bitmask_t config_get_convert_bit()=0;
//!< Return the SQL_CONVERT_BIT configuration setting
virtual config_bitmask_t config_get_convert_char()=0;
//!< Return the SQL_CONVERT_CHAR configuration setting
virtual config_bitmask_t config_get_convert_guid()=0;
//!< Return the SQL_CONVERT_GUID configuration setting
virtual config_bitmask_t config_get_convert_date()=0;
//!< Return the SQL_CONVERT_DATE configuration setting
virtual config_bitmask_t config_get_convert_decimal()=0;
//!< Return the SQL_CONVERT_DECIMAL configuration setting
virtual config_bitmask_t config_get_convert_double()=0;
//!< Return the SQL_CONVERT_DOUBLE configuration setting
virtual config_bitmask_t config_get_convert_float()=0;
//!< Return the SQL_CONVERT_FLOAT configuration setting
virtual config_bitmask_t config_get_convert_integer()=0;
//!< Return the SQL_CONVERT_INTEGER configuration setting
virtual config_bitmask_t config_get_convert_interval_year_month()=0;
//!< Return the SQL_CONVERT_INTERVAL_YEAR_MONTH configuration setting
virtual config_bitmask_t config_get_convert_interval_day_time()=0;
//!< Return the SQL_CONVERT_INTERVAL_DAY_TIME configuration setting
virtual config_bitmask_t config_get_convert_longvarbinary()=0;
//!< Return the SQL_CONVERT_LONGVARBINARY configuration setting
virtual config_bitmask_t config_get_convert_longvarchar()=0;
//!< Return the SQL_CONVERT_LONGVARCHAR configuration setting
virtual config_bitmask_t config_get_convert_numeric()=0;
//!< Return the SQL_CONVERT_NUMERIC configuration setting
virtual config_bitmask_t config_get_convert_real()=0;
//!< Return the SQL_CONVERT_REAL configuration setting
virtual config_bitmask_t config_get_convert_smallint()=0;
//!< Return the SQL_CONVERT_SMALLINT configuration setting
virtual config_bitmask_t config_get_convert_time()=0;
//!< Return the SQL_CONVERT_TIME configuration setting
virtual config_bitmask_t config_get_convert_timestamp()=0;
//!< Return the SQL_CONVERT_TIMESTAMP configuration setting
virtual config_bitmask_t config_get_convert_tinyint()=0;
//!< Return the SQL_CONVERT_TINYINT configuration setting
virtual config_bitmask_t config_get_convert_varbinary()=0;
//!< Return the SQL_CONVERT_VARBINARY configuration setting
virtual config_bitmask_t config_get_convert_varchar()=0;
//!< Return the SQL_CONVERT_VARCHAR configuration setting
virtual config_bitmask_t config_get_convert_functions()=0;
//!< Return the SQL_CONVERT_FUNCTIONS configuration setting
virtual config_bitmask_t config_get_correlation_name()=0;
//!< Return the SQL_CORRELATION_NAME configuration setting
virtual config_bitmask_t config_get_create_assertion()=0;
//!< Return the SQL_CREATE_ASSERTION configuration setting
virtual config_bitmask_t config_get_create_character_set()=0;
//!< Return the SQL_CREATE_CHARACTER_SET configuration setting
virtual long config_get_create_collation()=0;
//!< Return the SQL_CREATE_COLLATION configuration setting
virtual config_bitmask_t config_get_create_domain()=0;
//!< Return the SQL_CREATE_DOMAIN configuration setting
virtual config_bitmask_t config_get_create_schema()=0;
//!< Return the SQL_CREATE_SCHEMA configuration setting
virtual config_bitmask_t config_get_create_table()=0;
//!< Return the SQL_CREATE_TABLE configuration setting
virtual config_bitmask_t config_get_create_translation()=0;
//!< Return the SQL_CREATE_TRANSLATION configuration setting
virtual config_bitmask_t config_get_create_view()=0;
//!< Return the SQL_CREATE_VIEW configuration setting
virtual config_bitmask_t config_get_cursor_commit_behavior()=0;
//!< Return the SQL_CURSOR_COMMIT_BEHAVIOR configuration setting
virtual config_bitmask_t config_get_cursor_rollback_behavior()=0;
//!< Return the SQL_CURSOR_ROLLBACK_BEHAVIOR configuration setting
virtual long config_get_cursor_sensitivity()=0;
//!< Return the SQL_CURSOR_SENSITIVITY configuration setting
virtual std::string config_get_data_source_name()=0;
//!< Return the SQL_DATA_SOURCE_NAME configuration setting
virtual bool config_get_data_source_read_only()=0;
//!< Return the SQL_DATA_SOURCE_READ_ONLY configuration setting
virtual std::string config_get_database_name()=0;
//!< Return the SQL_DATABASE_NAME configuration setting
virtual config_bitmask_t config_get_datetime_literals()=0;
//!< Return the SQL_DATETIME_LITERALS configuration setting
virtual std::string config_get_dbms_name()=0;
//!< Return the SQL_DBMS_NAME configuration setting
virtual std::string config_get_dbms_ver()=0;
//!< Return the SQL_DBMS_VER configuration setting
virtual config_bitmask_t config_get_ddl_index()=0;
//!< Return the SQL_DDL_INDEX configuration setting
virtual config_bitmask_t config_get_default_txn_isolation()=0;
//!< Return the SQL_DEFAULT_TXN_ISOLATION configuration setting
virtual bool config_get_describe_parameter()=0;
//!< Return the SQL_DESCRIBE_PARAMETER configuration setting
virtual std::string config_get_dm_ver()=0;
//!< Return the SQL_DM_VER configuration setting
virtual long config_get_driver_hdesc()=0;
//!< Return the SQL_DRIVER_HDESC configuration setting
virtual long config_get_driver_hlib()=0;
//!< Return the SQL_DRIVER_HLIB configuration setting
virtual long config_get_driver_hstmt()=0;
//!< Return the SQL_DRIVER_HSTMT configuration setting
virtual std::string config_get_driver_name()=0;
//!< Return the SQL_DRIVER_NAME configuration setting
virtual std::string config_get_driver_odbc_ver()=0;
//!< Return the SQL_DRIVER_ODBC_VER configuration setting
virtual std::string config_get_driver_ver()=0;
//!< Return the SQL_DRIVER_VER configuration setting
virtual config_bitmask_t config_get_drop_assertion()=0;
//!< Return the SQL_DROP_ASSERTION configuration setting
virtual config_bitmask_t config_get_drop_character_set()=0;
//!< Return the SQL_DROP_CHARACTER_SET configuration setting
virtual config_bitmask_t config_get_drop_collation()=0;
//!< Return the SQL_DROP_COLLATION configuration setting
virtual config_bitmask_t config_get_drop_domain()=0;
//!< Return the SQL_DROP_DOMAIN configuration setting
virtual config_bitmask_t config_get_drop_schema()=0;
//!< Return the SQL_DROP_SCHEMA configuration setting
virtual config_bitmask_t config_get_drop_table()=0;
//!< Return the SQL_DROP_TABLE configuration setting
virtual config_bitmask_t config_get_drop_translation()=0;
//!< Return the SQL_DROP_TRANSLATION configuration setting
virtual config_bitmask_t config_get_drop_view()=0;
//!< Return the SQL_DROP_VIEW configuration setting
virtual config_bitmask_t config_get_dynamic_cursor_attributes1()=0;
//!< Return the SQL_DYNAMIC_CURSOR_ATTRIBUTES1 configuration setting
virtual config_bitmask_t config_get_dynamic_cursor_attributes2()=0;
//!< Return the SQL_DYNAMIC_CURSOR_ATTRIBUTES2 configuration setting
virtual bool config_get_expressions_in_orderby()=0;
//!< Return the SQL_EXPRESSIONS_IN_ORDERBY configuration setting
virtual long config_get_file_usage()=0;
//!< Return the SQL_FILE_USAGE configuration setting
virtual config_bitmask_t config_get_forward_only_cursor_attributes1()=0;
//!< Return the SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1 configuration setting
virtual config_bitmask_t config_get_forward_only_cursor_attributes2()=0;
//!< Return the SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2 configuration setting
virtual config_bitmask_t config_get_getdata_extensions()=0;
//!< Return the SQL_GETDATA_EXTENSIONS configuration setting
virtual config_bitmask_t config_get_group_by()=0;
//!< Return the SQL_GROUP_BY configuration setting
virtual config_bitmask_t config_get_identifier_case()=0;
//!< Return the SQL_IDENTIFIER_CASE configuration setting
virtual std::string config_get_identifier_quote_char()=0;
//!< Return the SQL_IDENTIFIER_QUOTE_CHAR configuration setting
virtual config_bitmask_t config_get_index_keywords()=0;
//!< Return the SQL_INDEX_KEYWORDS configuration setting
virtual config_bitmask_t config_get_info_schema_views()=0;
//!< Return the SQL_INFO_SCHEMA_VIEWS configuration setting
virtual config_bitmask_t config_get_insert_statement()=0;
//!< Return the SQL_INSERT_STATEMENT configuration setting
virtual bool config_get_integrity()=0;
//!< Return the SQL_INTEGRITY configuration setting
virtual config_bitmask_t config_get_keyset_cursor_attributes1()=0;
//!< Return the SQL_KEYSET_CURSOR_ATTRIBUTES1 configuration setting
virtual config_bitmask_t config_get_keyset_cursor_attributes2()=0;
//!< Return the SQL_KEYSET_CURSOR_ATTRIBUTES2 configuration setting
virtual std::string config_get_keywords()=0;
//!< Return the SQL_KEYWORDS configuration setting
virtual bool config_get_like_escape_clause()=0;
//!< Return the SQL_LIKE_ESCAPE_CLAUSE configuration setting
virtual long config_get_max_async_concurrent_statements()=0;
//!< Return the SQL_MAX_ASYNC_CONCURRENT_STATEMENTS configuration setting
virtual long config_get_max_binary_literal_len()=0;
//!< Return the SQL_MAX_BINARY_LITERAL_LEN configuration setting
virtual config_bitmask_t config_get_max_catalog_name_len()=0;
//!< Return the SQL_MAX_CATALOG_NAME_LEN configuration setting
virtual long config_get_max_char_literal_len()=0;
//!< Return the SQL_MAX_CHAR_LITERAL_LEN configuration setting
virtual long config_get_max_column_name_len()=0;
//!< Return the SQL_MAX_COLUMN_NAME_LEN configuration setting
virtual long config_get_max_columns_in_group_by()=0;
//!< Return the SQL_MAX_COLUMNS_IN_GROUP_BY configuration setting
virtual long config_get_max_columns_in_index()=0;
//!< Return the SQL_MAX_COLUMNS_IN_INDEX configuration setting
virtual long config_get_max_columns_in_order_by()=0;
//!< Return the SQL_MAX_COLUMNS_IN_ORDER_BY configuration setting
virtual long config_get_max_columns_in_select()=0;
//!< Return the SQL_MAX_COLUMNS_IN_SELECT configuration setting
virtual long config_get_max_columns_in_table()=0;
//!< Return the SQL_MAX_COLUMNS_IN_TABLE configuration setting
virtual long config_get_max_concurrent_activities()=0;
//!< Return the SQL_MAX_CONCURRENT_ACTIVITIES configuration setting
virtual long config_get_max_cursor_name_len()=0;
//!< Return the SQL_MAX_CURSOR_NAME_LEN configuration setting
virtual long config_get_max_driver_connections()=0;
//!< Return the SQL_MAX_DRIVER_CONNECTIONS configuration setting
virtual long config_get_max_identifier_len()=0;
//!< Return the SQL_MAX_IDENTIFIER_LEN configuration setting
virtual long config_get_max_index_size()=0;
//!< Return the SQL_MAX_INDEX_SIZE configuration setting
virtual long config_get_max_procedure_name_len()=0;
//!< Return the SQL_MAX_PROCEDURE_NAME_LEN configuration setting
virtual long config_get_max_row_size()=0;
//!< Return the SQL_MAX_ROW_SIZE configuration setting
virtual bool config_get_max_row_size_includes_long()=0;
//!< Return the SQL_MAX_ROW_SIZE_INCLUDES_LONG configuration setting
virtual config_bitmask_t config_get_max_schema_name_len()=0;
//!< Return the SQL_MAX_SCHEMA_NAME_LEN configuration setting
virtual long config_get_max_statement_len()=0;
//!< Return the SQL_MAX_STATEMENT_LEN configuration setting
virtual long config_get_max_table_name_len()=0;
//!< Return the SQL_MAX_TABLE_NAME_LEN configuration setting
virtual long config_get_max_tables_in_select()=0;
//!< Return the SQL_MAX_TABLES_IN_SELECT configuration setting
virtual long config_get_max_user_name_len()=0;
//!< Return the SQL_MAX_USER_NAME_LEN configuration setting
virtual bool config_get_mult_result_sets()=0;
//!< Return the SQL_MULT_RESULT_SETS configuration setting
virtual bool config_get_multiple_active_txn()=0;
//!< Return the SQL_MULTIPLE_ACTIVE_TXN configuration setting
virtual bool config_get_need_long_data_len()=0;
//!< Return the SQL_NEED_LONG_DATA_LEN configuration setting
virtual config_bitmask_t config_get_non_nullable_columns()=0;
//!< Return the SQL_NON_NULLABLE_COLUMNS configuration setting
virtual config_bitmask_t config_get_null_collation()=0;
//!< Return the SQL_NULL_COLLATION configuration setting
virtual config_bitmask_t config_get_numeric_functions()=0;
//!< Return the SQL_NUMERIC_FUNCTIONS configuration setting
virtual config_bitmask_t config_get_odbc_interface_conformance()=0;
//!< Return the SQL_ODBC_INTERFACE_CONFORMANCE configuration setting
virtual std::string config_get_odbc_ver()=0;
//!< Return the SQL_ODBC_VER configuration setting
virtual config_bitmask_t config_get_oj_capabilities()=0;
//!< Return the SQL_OJ_CAPABILITIES configuration setting
virtual bool config_get_order_by_columns_in_select()=0;
//!< Return the SQL_ORDER_BY_COLUMNS_IN_SELECT configuration setting
virtual long config_get_param_array_row_counts()=0;
//!< Return the SQL_PARAM_ARRAY_ROW_COUNTS configuration setting
virtual config_bitmask_t config_get_param_array_selects()=0;
//!< Return the SQL_PARAM_ARRAY_SELECTS configuration setting
virtual std::string config_get_procedure_term()=0;
//!< Return the SQL_PROCEDURE_TERM configuration setting
virtual bool config_get_procedures()=0;
//!< Return the SQL_PROCEDURES configuration setting
virtual config_bitmask_t config_get_pos_operations()=0;
//!< Return the SQL_POS_OPERATIONS configuration setting
virtual config_bitmask_t config_get_quoted_identifier_case()=0;
//!< Return the SQL_QUOTED_IDENTIFIER_CASE configuration setting
virtual bool config_get_row_updates()=0;
//!< Return the SQL_ROW_UPDATES configuration setting
virtual std::string config_get_schema_term()=0;
//!< Return the SQL_SCHEMA_TERM configuration setting
virtual config_bitmask_t config_get_schema_usage()=0;
//!< Return the SQL_SCHEMA_USAGE configuration setting
virtual config_bitmask_t config_get_scroll_options()=0;
//!< Return the SQL_SCROLL_OPTIONS configuration setting
virtual std::string config_get_search_pattern_escape()=0;
//!< Return the SQL_SEARCH_PATTERN_ESCAPE configuration setting
virtual std::string config_get_server_name()=0;
//!< Return the SQL_SERVER_NAME configuration setting
virtual std::string config_get_special_characters()=0;
//!< Return the SQL_SPECIAL_CHARACTERS configuration setting
virtual config_bitmask_t config_get_sql_conformance()=0;
//!< Return the SQL_SQL_CONFORMANCE configuration setting
virtual config_bitmask_t config_get_sql92_datetime_functions()=0;
//!< Return the SQL_SQL92_DATETIME_FUNCTIONS configuration setting
virtual long config_get_sql92_foreign_key_delete_rule()=0;
//!< Return the SQL_SQL92_FOREIGN_KEY_DELETE_RULE configuration setting
virtual long config_get_sql92_foreign_key_update_rule()=0;
//!< Return the SQL_SQL92_FOREIGN_KEY_UPDATE_RULE configuration setting
virtual config_bitmask_t config_get_sql92_grant()=0;
//!< Return the SQL_SQL92_GRANT configuration setting
virtual long config_get_sql92_numeric_value_functions()=0;
//!< Return the SQL_SQL92_NUMERIC_VALUE_FUNCTIONS configuration setting
virtual config_bitmask_t config_get_sql92_predicates()=0;
//!< Return the SQL_SQL92_PREDICATES configuration setting
virtual long config_get_sql92_relational_join_operators()=0;
//!< Return the SQL_SQL92_RELATIONAL_JOIN_OPERATORS configuration setting
virtual config_bitmask_t config_get_sql92_revoke()=0;
//!< Return the SQL_SQL92_REVOKE configuration setting
virtual long config_get_sql92_row_value_constructor()=0;
//!< Return the SQL_SQL92_ROW_VALUE_CONSTRUCTOR configuration setting
virtual config_bitmask_t config_get_sql92_string_functions()=0;
//!< Return the SQL_SQL92_STRING_FUNCTIONS configuration setting
virtual config_bitmask_t config_get_sql92_value_expressions()=0;
//!< Return the SQL_SQL92_VALUE_EXPRESSIONS configuration setting
virtual config_bitmask_t config_get_standard_cli_conformance()=0;
//!< Return the SQL_STANDARD_CLI_CONFORMANCE configuration setting
virtual config_bitmask_t config_get_static_cursor_attributes1()=0;
//!< Return the SQL_STATIC_CURSOR_ATTRIBUTES1 configuration setting
virtual config_bitmask_t config_get_static_cursor_attributes2()=0;
//!< Return the SQL_STATIC_CURSOR_ATTRIBUTES2 configuration setting
virtual config_bitmask_t config_get_string_functions()=0;
//!< Return the SQL_STRING_FUNCTIONS configuration setting
virtual config_bitmask_t config_get_subqueries()=0;
//!< Return the SQL_SUBQUERIES configuration setting
virtual config_bitmask_t config_get_system_functions()=0;
//!< Return the SQL_SYSTEM_FUNCTIONS configuration setting
virtual std::string config_get_table_term()=0;
//!< Return the SQL_TABLE_TERM configuration setting
virtual config_bitmask_t config_get_timedate_add_intervals()=0;
//!< Return the SQL_TIMEDATE_ADD_INTERVALS configuration setting
virtual config_bitmask_t config_get_timedate_diff_intervals()=0;
//!< Return the SQL_TIMEDATE_DIFF_INTERVALS configuration setting
virtual config_bitmask_t config_get_timedate_functions()=0;
//!< Return the SQL_TIMEDATE_FUNCTIONS configuration setting
virtual config_bitmask_t config_get_txn_capable()=0;
//!< Return the SQL_TXN_CAPABLE configuration setting
virtual config_bitmask_t config_get_txn_isolation_option()=0;
//!< Return the SQL_TXN_ISOLATION_OPTION configuration setting
virtual config_bitmask_t config_get_union()=0;
//!< Return the SQL_UNION configuration setting
virtual std::string config_get_user_name()=0;
//!< Return the SQL_USER_NAME configuration setting
virtual std::string config_get_xopen_cli_year()=0;
//!< Return the SQL_XOPEN_CLI_YEAR configuration setting
#endif
