# @TEST-DOC: ZAM maintenance script for tracking changes in BiFs.
#
# @TEST-REQUIRES: have-spicy
# @TEST-REQUIRES: have-zeromq
#
# @TEST-EXEC: zeek -b %INPUT >output
# @TEST-EXEC: btest-diff output

# This set tracks the BiFs that have been characterized for ZAM analysis.
# As new ones are added or old ones removed, attend to updating FuncInfo.cc
# for ZAM, and then update the list here.
global known_BiFs = set(
	"Analyzer::__disable_all_analyzers",
	"Analyzer::__disable_analyzer",
	"Analyzer::__enable_analyzer",
	"Analyzer::__has_tag",
	"Analyzer::__name",
	"Analyzer::__register_for_port",
	"Analyzer::__schedule_analyzer",
	"Analyzer::__tag",
	"Broker::__append",
	"Broker::__auto_publish",
	"Broker::__auto_unpublish",
	"Broker::__clear",
	"Broker::__close",
	"Broker::__create_clone",
	"Broker::__create_master",
	"Broker::__data",
	"Broker::__data_type",
	"Broker::__decrement",
	"Broker::__erase",
	"Broker::__exists",
	"Broker::__flush_logs",
	"Broker::__forward",
	"Broker::__get",
	"Broker::__get_index_from_value",
	"Broker::__increment",
	"Broker::__insert_into_set",
	"Broker::__insert_into_table",
	"Broker::__is_closed",
	"Broker::__keys",
	"Broker::__listen",
	"Broker::__node_id",
	"Broker::__opaque_clone_through_serialization",
	"Broker::__peer",
	"Broker::__peer_no_retry",
	"Broker::__peers",
	"Broker::__pop",
	"Broker::__publish_id",
	"Broker::__push",
	"Broker::__put",
	"Broker::__put_unique",
	"Broker::__record_assign",
	"Broker::__record_create",
	"Broker::__record_iterator",
	"Broker::__record_iterator_last",
	"Broker::__record_iterator_next",
	"Broker::__record_iterator_value",
	"Broker::__record_lookup",
	"Broker::__record_size",
	"Broker::__remove_from",
	"Broker::__set_clear",
	"Broker::__set_contains",
	"Broker::__set_create",
	"Broker::__set_insert",
	"Broker::__set_iterator",
	"Broker::__set_iterator_last",
	"Broker::__set_iterator_next",
	"Broker::__set_iterator_value",
	"Broker::__set_remove",
	"Broker::__set_size",
	"Broker::__store_name",
	"Broker::__subscribe",
	"Broker::__table_clear",
	"Broker::__table_contains",
	"Broker::__table_create",
	"Broker::__table_insert",
	"Broker::__table_iterator",
	"Broker::__table_iterator_last",
	"Broker::__table_iterator_next",
	"Broker::__table_iterator_value",
	"Broker::__table_lookup",
	"Broker::__table_remove",
	"Broker::__table_size",
	"Broker::__unpeer",
	"Broker::__unsubscribe",
	"Broker::__vector_clear",
	"Broker::__vector_create",
	"Broker::__vector_insert",
	"Broker::__vector_iterator",
	"Broker::__vector_iterator_last",
	"Broker::__vector_iterator_next",
	"Broker::__vector_iterator_value",
	"Broker::__vector_lookup",
	"Broker::__vector_remove",
	"Broker::__vector_replace",
	"Broker::__vector_size",
	"Broker::make_event",
	"Broker::publish",
	"Cluster::Backend::__init",
	"Cluster::__subscribe",
	"Cluster::__unsubscribe",
	"Cluster::make_event",
	"Cluster::publish",
	"Cluster::publish_hrw",
	"Cluster::publish_rr",
	"FileExtract::__set_limit",
	"Files::__add_analyzer",
	"Files::__analyzer_enabled",
	"Files::__analyzer_name",
	"Files::__disable_analyzer",
	"Files::__disable_reassembly",
	"Files::__enable_analyzer",
	"Files::__enable_reassembly",
	"Files::__file_exists",
	"Files::__lookup_file",
	"Files::__remove_analyzer",
	"Files::__set_reassembly_buffer",
	"Files::__set_timeout_interval",
	"Files::__stop",
	"Input::__create_analysis_stream",
	"Input::__create_event_stream",
	"Input::__create_table_stream",
	"Input::__force_update",
	"Input::__remove_stream",
	"Log::__add_filter",
	"Log::__create_stream",
	"Log::__delay",
	"Log::__delay_finish",
	"Log::__disable_stream",
	"Log::__enable_stream",
	"Log::__flush",
	"Log::__get_delay_queue_size",
	"Log::__remove_filter",
	"Log::__remove_stream",
	"Log::__set_buf",
	"Log::__set_max_delay_interval",
	"Log::__set_max_delay_queue_size",
	"Log::__write",
	"Option::any_set_to_any_vec",
	"Option::set",
	"Option::set_change_handler",
	"PacketAnalyzer::GTPV1::remove_gtpv1_connection",
	"PacketAnalyzer::Geneve::get_options",
	"PacketAnalyzer::TEREDO::remove_teredo_connection",
	"PacketAnalyzer::__disable_analyzer",
	"PacketAnalyzer::__enable_analyzer",
	"PacketAnalyzer::__set_ignore_checksums_nets",
	"PacketAnalyzer::register_packet_analyzer",
	"PacketAnalyzer::register_protocol_detection",
	"PacketAnalyzer::try_register_packet_analyzer_by_name",
	"Pcap::error",
	"Pcap::findalldevs",
	"Pcap::get_filter_state",
	"Pcap::get_filter_state_string",
	"Pcap::install_pcap_filter",
	"Pcap::precompile_pcap_filter",
	"Reporter::conn_weird",
	"Reporter::error",
	"Reporter::fatal",
	"Reporter::fatal_error_with_core",
	"Reporter::file_weird",
	"Reporter::flow_weird",
	"Reporter::get_weird_sampling_duration",
	"Reporter::get_weird_sampling_global_list",
	"Reporter::get_weird_sampling_rate",
	"Reporter::get_weird_sampling_threshold",
	"Reporter::get_weird_sampling_whitelist",
	"Reporter::info",
	"Reporter::net_weird",
	"Reporter::set_weird_sampling_duration",
	"Reporter::set_weird_sampling_global_list",
	"Reporter::set_weird_sampling_rate",
	"Reporter::set_weird_sampling_threshold",
	"Reporter::set_weird_sampling_whitelist",
	"Reporter::warning",
	"Spicy::__resource_usage",
	"Spicy::__toggle_analyzer",
	"Supervisor::__create",
	"Supervisor::__destroy",
	"Supervisor::__is_supervised",
	"Supervisor::__is_supervisor",
	"Supervisor::__node",
	"Supervisor::__restart",
	"Supervisor::__status",
	"Supervisor::__stem_pid",
	"Telemetry::__collect_histogram_metrics",
	"Telemetry::__collect_metrics",
	"Telemetry::__counter_family",
	"Telemetry::__counter_inc",
	"Telemetry::__counter_metric_get_or_add",
	"Telemetry::__counter_value",
	"Telemetry::__gauge_dec",
	"Telemetry::__gauge_family",
	"Telemetry::__gauge_inc",
	"Telemetry::__gauge_metric_get_or_add",
	"Telemetry::__gauge_value",
	"Telemetry::__histogram_family",
	"Telemetry::__histogram_metric_get_or_add",
	"Telemetry::__histogram_observe",
	"Telemetry::__histogram_sum",
	"WebSocket::__configure_analyzer",
	"Cluster::Backend::ZeroMQ::spawn_zmq_proxy_thread",
	"__init_primary_bifs",
	"__init_secondary_bifs",
	"active_file",
	"addr_to_counts",
	"addr_to_ptr_name",
	"addr_to_subnet",
	"all_set",
	"anonymize_addr",
	"any_set",
	"backtrace",
	"bare_mode",
	"bloomfilter_add",
	"bloomfilter_basic_init",
	"bloomfilter_basic_init2",
	"bloomfilter_clear",
	"bloomfilter_counting_init",
	"bloomfilter_decrement",
	"bloomfilter_internal_state",
	"bloomfilter_intersect",
	"bloomfilter_lookup",
	"bloomfilter_merge",
	"bytestring_to_count",
	"bytestring_to_double",
	"bytestring_to_float",
	"bytestring_to_hexstr",
	"calc_next_rotate",
	"cat",
	"cat_sep",
	"ceil",
	"check_subnet",
	"clean",
	"clear_table",
	"close",
	"community_id_v1",
	"compress_path",
	"connection_exists",
	"continue_processing",
	"convert_for_pattern",
	"count_substr",
	"count_to_double",
	"count_to_port",
	"count_to_v4_addr",
	"counts_to_addr",
	"current_analyzer",
	"current_event_time",
	"current_time",
	"decode_base64",
	"decode_base64_conn",
	"decode_netbios_name",
	"decode_netbios_name_type",
	"disable_analyzer",
	"disable_event_group",
	"disable_module_events",
	"do_profiling",
	"double_to_count",
	"double_to_int",
	"double_to_interval",
	"double_to_time",
	"dump_current_packet",
	"dump_packet",
	"dump_rule_stats",
	"edit",
	"enable_event_group",
	"enable_module_events",
	"enable_raw_output",
	"encode_base64",
	"ends_with",
	"entropy_test_add",
	"entropy_test_finish",
	"entropy_test_init",
	"enum_names",
	"enum_to_int",
	"escape_string",
	"exit",
	"exp",
	"file_magic",
	"file_mode",
	"file_size",
	"filter_subnet_table",
	"find_all",
	"find_all_ordered",
	"find_entropy",
	"find_in_zeekpath",
	"find_last",
	"find_str",
	"floor",
	"flush_all",
	"fmt",
	"fmt_ftp_port",
	"fnv1a32",
	"fnv1a64",
	"from_json",
	"generate_all_events",
	"get_broker_stats",
	"get_conn_stats",
	"get_conn_transport_proto",
	"get_contents_file",
	"get_current_conn_bytes_threshold",
	"get_current_conn_duration_threshold",
	"get_current_conn_packets_threshold",
	"get_current_packet",
	"get_current_packet_header",
	"get_current_packet_ts",
	"get_dns_stats",
	"get_event_handler_stats",
	"get_event_stats",
	"get_file_analysis_stats",
	"get_file_name",
	"get_gap_stats",
	"get_identifier_comments",
	"get_identifier_declaring_script",
	"get_login_state",
	"get_matcher_stats",
	"get_net_stats",
	"get_orig_seq",
	"get_package_readme",
	"get_port_transport_proto",
	"get_proc_stats",
	"get_reassembler_stats",
	"get_record_field_comments",
	"get_record_field_declaring_script",
	"get_reporter_stats",
	"get_resp_seq",
	"get_script_comments",
	"get_thread_stats",
	"get_timer_stats",
	"getenv",
	"gethostname",
	"getpid",
	"global_container_footprints",
	"global_ids",
	"global_options",
	"gsub",
	"has_event_group",
	"has_module_events",
	"have_spicy",
	"have_spicy_analyzers",
	"haversine_distance",
	"hexdump",
	"hexstr_to_bytestring",
	"hll_cardinality_add",
	"hll_cardinality_copy",
	"hll_cardinality_estimate",
	"hll_cardinality_init",
	"hll_cardinality_merge_into",
	"hrw_weight",
	"identify_data",
	"install_dst_addr_filter",
	"install_dst_net_filter",
	"install_src_addr_filter",
	"install_src_net_filter",
	"int_to_count",
	"int_to_double",
	"interval_to_double",
	"is_alnum",
	"is_alpha",
	"is_ascii",
	"is_event_handled",
	"is_file_analyzer",
	"is_icmp_port",
	"is_local_interface",
	"is_num",
	"is_packet_analyzer",
	"is_processing_suspended",
	"is_protocol_analyzer",
	"is_remote_event",
	"is_tcp_port",
	"is_udp_port",
	"is_v4_addr",
	"is_v4_subnet",
	"is_v6_addr",
	"is_v6_subnet",
	"is_valid_ip",
	"join_string_set",
	"join_string_vec",
	"levenshtein_distance",
	"ljust",
	"ln",
	"load_CPP",
	"log10",
	"log2",
	"lookup_ID",
	"lookup_addr",
	"lookup_autonomous_system",
	"lookup_connection",
	"lookup_connection_analyzer_id",
	"lookup_hostname",
	"lookup_hostname_txt",
	"lookup_location",
	"lstrip",
	"mask_addr",
	"match_signatures",
	"matching_subnets",
	"md5_hash",
	"md5_hash_finish",
	"md5_hash_init",
	"md5_hash_update",
	"md5_hmac",
	"mkdir",
	"mmdb_open_asn_db",
	"mmdb_open_location_db",
	"network_time",
	"open",
	"open_for_append",
	"order",
	"packet_source",
	"paraglob_equals",
	"paraglob_init",
	"paraglob_match",
	"parse_distinguished_name",
	"parse_eftp_port",
	"parse_ftp_epsv",
	"parse_ftp_pasv",
	"parse_ftp_port",
	"piped_exec",
	"port_to_count",
	"pow",
	"preserve_prefix",
	"preserve_subnet",
	"print_raw",
	"ptr_name_to_addr",
	"rand",
	"raw_bytes_to_v4_addr",
	"raw_bytes_to_v6_addr",
	"reading_live_traffic",
	"reading_traces",
	"record_fields",
	"record_type_to_vector",
	"remask_addr",
	"remove_prefix",
	"remove_suffix",
	"rename",
	"resize",
	"reverse",
	"rfind_str",
	"rjust",
	"rmdir",
	"rotate_file",
	"rotate_file_by_name",
	"routing0_data_to_addrs",
	"rstrip",
	"safe_shell_quote",
	"same_object",
	"sct_verify",
	"set_buf",
	"set_contents_file",
	"set_current_conn_bytes_threshold",
	"set_current_conn_duration_threshold",
	"set_current_conn_packets_threshold",
	"set_file_handle",
	"set_inactivity_timeout",
	"set_keys",
	"set_login_state",
	"set_network_time",
	"set_record_packets",
	"set_secret",
	"set_ssl_established",
	"setenv",
	"sha1_hash",
	"sha1_hash_finish",
	"sha1_hash_init",
	"sha1_hash_update",
	"sha256_hash",
	"sha256_hash_finish",
	"sha256_hash_init",
	"sha256_hash_update",
	"skip_further_processing",
	"skip_http_entity_data",
	"skip_smtp_data",
	"sleep",
	"sort",
	"split_string",
	"split_string1",
	"split_string_all",
	"split_string_n",
	"sqrt",
	"srand",
	"starts_with",
	"str_smith_waterman",
	"str_split_indices",
	"strcmp",
	"strftime",
	"string_cat",
	"string_fill",
	"string_to_ascii_hex",
	"string_to_pattern",
	"strip",
	"strptime",
	"strstr",
	"sub",
	"sub_bytes",
	"subnet_to_addr",
	"subnet_width",
	"subst_string",
	"suspend_processing",
	"swap_case",
	"syslog",
	"system",
	"system_env",
	"table_keys",
	"table_pattern_matcher_stats",
	"table_values",
	"terminate",
	"time_to_double",
	"to_addr",
	"to_count",
	"to_double",
	"to_int",
	"to_json",
	"to_lower",
	"to_port",
	"to_string_literal",
	"to_subnet",
	"to_title",
	"to_upper",
	"topk_add",
	"topk_count",
	"topk_epsilon",
	"topk_get_top",
	"topk_init",
	"topk_merge",
	"topk_merge_prune",
	"topk_size",
	"topk_sum",
	"type_aliases",
	"type_name",
	"unescape_URI",
	"uninstall_dst_addr_filter",
	"uninstall_dst_net_filter",
	"uninstall_src_addr_filter",
	"uninstall_src_net_filter",
	"unique_id",
	"unique_id_from",
	"unlink",
	"uuid_to_string",
	"val_footprint",
	"write_file",
	"x509_check_cert_hostname",
	"x509_check_hostname",
	"x509_from_der",
	"x509_get_certificate_string",
	"x509_issuer_name_hash",
	"x509_ocsp_verify",
	"x509_parse",
	"x509_set_certificate_cache",
	"x509_set_certificate_cache_hit_callback",
	"x509_spki_hash",
	"x509_subject_name_hash",
	"x509_verify",
	"zeek_args",
	"zeek_is_terminating",
	"zeek_version",
	"zfill",
);

function fmt_str_set(s: set[string]): string
	{
	local set_str = "";
	for ( s_i in s )
		{
		if ( set_str == "" )
			set_str = s_i;
		else
			set_str += ", " + s_i;
		}

	return set_str;
	}

event zeek_init()
	{
	local unseen_bifs = known_BiFs;
	local seen_bifs: set[string];
	local new_bifs: set[string];

	for ( gn, gi in global_ids() )
		if ( /^function/ in gi$type_name && gi?$value &&
		     # BiFs format to their name, while script functions
		     # format to that plus their body.
		     fmt("%s", gi$value) == gn )
			{
			if ( gn in unseen_bifs )
				{
				add seen_bifs[gn];
				delete unseen_bifs[gn];
				}
			else
				add new_bifs[gn];
			}

	print fmt("%d seen BiFs, %d unseen BiFs (%s), %d new BiFs (%s)",
		|seen_bifs|,
		|unseen_bifs|, fmt_str_set(unseen_bifs),
		|new_bifs|, fmt_str_set(new_bifs));
	}
