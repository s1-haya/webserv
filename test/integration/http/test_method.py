from http import HTTPStatus
from http.client import HTTPConnection, HTTPException

from assert_http_response import assert_body, assert_header, assert_status_line


class TestStatusCode:
    def test_ok(self):
        try:
            con = HTTPConnection("localhost", 8080)
            con.request("GET", "/")
            response = con.getresponse()
            assert_status_line(response, HTTPStatus.OK)
            assert_header(response, "Connection", "close")
            assert_body(response, "./html/index.html")
            con.close()
        except HTTPException as e:
            print(f"Request faild: {e}")

    def test_ok_in_sub_directory(self):
        try:
            con = HTTPConnection("localhost", 8080)
            con.request("GET", "/sub")
            response = con.getresponse()
            assert_status_line(response, HTTPStatus.OK)
            assert_header(response, "Connection", "close")
            assert_body(response, "./html/sub/index.html")
            con.close()
        except HTTPException as e:
            print(f"Request faild: {e}")

    def test_not_found(self):
        try:
            con = HTTPConnection("localhost", 8080)
            con.request("GET", "/no-exist-path")
            response = con.getresponse()
            # assert_status_line(response, HTTPStatus.NOT_FOUND)
            assert_header(response, "Connection", "close")
            assert_body(response, "./html/404.html")
            con.close()
        except HTTPException as e:
            print(f"Request faild: {e}")
