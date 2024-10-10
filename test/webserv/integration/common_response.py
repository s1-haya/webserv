from common_functions import read_file, read_file_binary

KEEP_ALIVE = "keep-alive"
CLOSE = "close"
TEXT_PLAIN = "text/plain"
TEXT_HTML = "text/html"

STATUS = {
    200: "OK",
    201: "Created",
    204: "No Content",
    400: "Bad Request",
    403: "Forbidden",
    404: "Not Found",
    405: "Method Not Allowed",
    408: "Request Timeout",
    413: "Payload Too Large",
    501: "Not Implemented",
}

SUCCESS_FILES = (
    ("201_created.txt", "created_file_201", "created_file_201_length"),
    ("204_no_content.txt", "no_content_file_204", "no_content_file_204_length"),
)

ERROR_FILES = (
    ("400_bad_request.txt", "bad_request_file_400", "bad_request_file_400_length"),
    ("403_forbidden.txt", "forbidden_file_403", "forbidden_file_403_length"),
    ("404_not_found.txt", "not_found_file_404", "not_found_file_404_length"),
    (
        "405_method_not_allowed.txt",
        "not_allowed_file_405",
        "not_allowed_file_405_length",
    ),
    ("408_timeout.txt", "timeout_file_408", "timeout_file_408_length"),
    (
        "413_payload_too_large.txt",
        "payload_too_large_file_413",
        "payload_too_large_file_413_length",
    ),
    (
        "501_not_implemented.txt",
        "not_implemented_file_501",
        "not_implemented_file_501_length",
    ),
)


def read_files_data(file_list):
    files_data = {}
    for filename, data_var, length_var in file_list:
        data, length = read_file_binary(
            f"test/webserv/expected_response/default_body_message/{filename}"
        )
        files_data[data_var] = data
        files_data[length_var] = length
    return files_data


success_files_data = read_files_data(SUCCESS_FILES)
error_files_data = read_files_data(ERROR_FILES)


def create_response_header(
    status_code, connection, content_length, content_type=TEXT_PLAIN
):
    return f"HTTP/1.1 {status_code} {STATUS[status_code]}\r\nconnection: {connection}\r\ncontent-length: {content_length}\r\ncontent-type: {content_type}\r\nserver: webserv/1.1\r\n\r\n"


root_index_file, root_index_file_length = read_file("root/html/index.html")
sub_index_file, sub_index_file_length = read_file("root/html/sub/index.html")


response_header_get_root_200_close = create_response_header(
    200, CLOSE, root_index_file_length, TEXT_HTML
)
response_header_get_root_200_keep = create_response_header(
    200, KEEP_ALIVE, root_index_file_length, TEXT_HTML
)
response_header_get_sub_200_close = create_response_header(
    200, CLOSE, sub_index_file_length, TEXT_HTML
)
response_header_201_close = create_response_header(
    201, CLOSE, success_files_data["created_file_201_length"], TEXT_HTML
)
response_header_201_keep = create_response_header(
    201, KEEP_ALIVE, success_files_data["created_file_201_length"], TEXT_HTML
)
response_header_204_close = create_response_header(
    204, CLOSE, success_files_data["no_content_file_204_length"], TEXT_HTML
)
response_header_204_keep = create_response_header(
    204, KEEP_ALIVE, success_files_data["no_content_file_204_length"], TEXT_HTML
)
response_header_400 = create_response_header(
    400, CLOSE, error_files_data["bad_request_file_400_length"], TEXT_HTML
)
response_header_403 = create_response_header(
    403, CLOSE, error_files_data["forbidden_file_403_length"], TEXT_HTML
)
response_header_404_close = create_response_header(
    404, CLOSE, error_files_data["not_found_file_404_length"], TEXT_HTML
)
response_header_404_keep = create_response_header(
    404, KEEP_ALIVE, error_files_data["not_found_file_404_length"], TEXT_HTML
)
response_header_405 = create_response_header(
    405, CLOSE, error_files_data["not_allowed_file_405_length"], TEXT_HTML
)
response_header_408 = create_response_header(
    408, CLOSE, error_files_data["timeout_file_408_length"], TEXT_HTML
)
response_header_413 = create_response_header(
    413, CLOSE, error_files_data["payload_too_large_file_413_length"], TEXT_HTML
)
response_header_501 = create_response_header(
    501, CLOSE, error_files_data["not_implemented_file_501_length"], TEXT_HTML
)

created_response_close = response_header_201_close + success_files_data[
    "created_file_201"
].decode("utf-8")
created_response_keep = response_header_201_keep + success_files_data[
    "created_file_201"
].decode("utf-8")
no_content_response_close = response_header_204_close + success_files_data[
    "no_content_file_204"
].decode("utf-8")
no_content_response_keep = response_header_204_keep + success_files_data[
    "no_content_file_204"
].decode("utf-8")
bad_request_response = response_header_400 + error_files_data[
    "bad_request_file_400"
].decode("utf-8")
forbidden_response = response_header_403 + error_files_data[
    "forbidden_file_403"
].decode("utf-8")
not_found_response_close = response_header_404_close + error_files_data[
    "not_found_file_404"
].decode("utf-8")
not_found_response_keep = response_header_404_keep + error_files_data[
    "not_found_file_404"
].decode("utf-8")
not_allowed_response = response_header_405 + error_files_data[
    "not_allowed_file_405"
].decode("utf-8")
timeout_response = response_header_408 + error_files_data["timeout_file_408"].decode(
    "utf-8"
)
payload_too_large_response = response_header_413 + error_files_data[
    "payload_too_large_file_413"
].decode("utf-8")
not_implemented_response = response_header_501 + error_files_data[
    "not_implemented_file_501"
].decode("utf-8")
