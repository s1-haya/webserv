import unittest
from http import HTTPStatus
from http.client import HTTPConnection, HTTPException, HTTPResponse

from http_module.assert_http_response import (assert_body, assert_header,
                                              assert_status_line)

MOVED_PERMANENTLY_BODY_FILE = (
    "test/webserv/expected_response/default_body_message/301_moved_permanently.txt"
)
FOUND_BODY_FILE = "test/webserv/expected_response/default_body_message/302_found.txt"


def assert_body_binary(response: HTTPResponse, path: str) -> None:
    response_body = response.read()
    with open(path, "rb") as f:
        expected_body = f.read()
    assert (
        response_body == expected_body
    ), f"Expected response body (binary) {expected_body}, but got {response_body}"
    assert_header(response, "Content-Length", str(len(expected_body)))


def assert_status_line_without_reason_phrase(
    response: HTTPResponse, expected_code: HTTPStatus
) -> None:
    assert (
        response.status == expected_code
    ), f"Expected status code {expected_code}, but got {response.status}"
    assert response.version == 11, f"Expected version 1.1, but got {response.version}"


# Host2はコンテナ外からアクセスできないのでHost3を使う
class TestServerRouting(unittest.TestCase):
    def setUp(self):
        # 各テストの前に実行される(unittestの機能)
        self.con = HTTPConnection("localhost", 8080)  # todo: 定数に

    def tearDown(self):
        # 各テストの後に実行される(unittestの機能)
        self.con.close()

    def test_get_root_host1(self):
        try:
            headers = {"Host": "no_exist_host"}  # ない場合はデフォルトのサーバーになる
            self.con.request("GET", "/", headers=headers)
            response = self.con.getresponse()
            assert_status_line(response, HTTPStatus.OK)
            assert_header(response, "Connection", "keep-alive")
            assert_body(response, "root/html/index.html")
        except HTTPException as e:
            self.fail(f"Request failed: {e}")

    def test_get_root_redirect_to_sub_host3(self):
        try:
            headers = {"Host": "host3"}
            self.con.request("GET", "/", headers=headers)
            response = self.con.getresponse()
            assert_status_line(response, HTTPStatus.FOUND)
            assert_header(response, "Connection", "keep-alive")
            assert_header(response, "Location", "http://host3/sub/")
            assert_body_binary(response, FOUND_BODY_FILE)
        except HTTPException as e:
            self.fail(f"Request failed: {e}")

    def test_get_sub_host3(self):
        try:
            headers = {"Host": "host3"}
            self.con.request("GET", "/sub/", headers=headers)
            response = self.con.getresponse()
            assert_status_line(response, HTTPStatus.OK)
            assert_header(response, "Connection", "keep-alive")
            assert_body(response, "root/html/sub/index.html")
        except HTTPException as e:
            self.fail(f"Request failed: {e}")

    def test_get_sub_redirect_host3(self):
        try:
            headers = {"Host": "host3"}
            self.con.request("GET", "/sub/redirect", headers=headers)
            response = self.con.getresponse()
            assert_status_line(response, HTTPStatus.MOVED_PERMANENTLY)
            assert_header(response, "Connection", "keep-alive")
            assert_header(response, "Location", "http://localhost:8080/")
            assert_body_binary(response, MOVED_PERMANENTLY_BODY_FILE)
        except HTTPException as e:
            self.fail(f"Request failed: {e}")

    def test_get_redirect_not_implemented_status_code_host3(self):
        try:
            headers = {"Host": "host3"}
            self.con.request("GET", "/redirect", headers=headers)
            response = self.con.getresponse()
            assert_status_line_without_reason_phrase(response, 303)
            assert_header(response, "Connection", "keep-alive")
            assert_header(response, "Location", "http://localhost:9000/")
            # サーバーで設定されていない303なのでbodyは空
        except HTTPException as e:
            self.fail(f"Request failed: {e}")