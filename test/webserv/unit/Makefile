# Add target test directories.
# Each directory should have a Makefile with a 'run' target.
TEST_DIRS	:=	convert_str \
				http \
				http_parse \
				http_response \
				http_method \
				http_storage \
				http_status_code \
				http_serverinfo_check \
				cgi_parse \
				cgi \
				cgi_response_parse \
				cgi_manager \
				sock_context \
				split_str \
				virtual_server \
				virtual_server_storage \
				message_manager \
				config_parse/lexer \
				config_parse/parser \
				config_parse \
				custom_const_iterator

.PHONY	: run
run:
	@status=0; \
	for dir in $(TEST_DIRS); do \
		echo "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"; \
		echo "┃ $$dir"; \
		echo "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"; \
		(MAKEFLAGS="-s" make -C $$dir run); \
		dir_status=$$?; \
		status=$$((status | dir_status)); \
	done; \
	exit $$status;

.PHONY	: fclean
fclean:
	@status=0; \
	for dir in $(TEST_DIRS); do \
		(MAKEFLAGS="-s" make -C $$dir fclean); \
		dir_status=$$?; \
		status=$$((status | dir_status)); \
	done; \
	exit $$status;
