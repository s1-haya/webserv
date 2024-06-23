from http import HTTPStatus
from http.client import HTTPConnection
from typing import Optional

def assert_status_line(code: HTTPStatus, output: HTTPConnection) -> None:
	assert code == output.status
	assert code.phrase == output.reason
	assert 11 == output.version

def assert_body(path: Optional[str], output: HTTPConnection) -> None:
	expected_body = b""
	with open(path, "r") as f:
		expected_body = f.read().encode("utf-8")
	assert expected_body == output.read()

