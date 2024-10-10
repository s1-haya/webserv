import unittest
from http.client import HTTPConnection, HTTPException

from common_functions import SERVER_PORT


# ------------------------------------------------------------------------------
class TestConnection(unittest.TestCase):
    def setUp(self):
        # 各テストの前に実行される(unittestの機能)
        self.con = HTTPConnection("localhost", SERVER_PORT)

    def tearDown(self):
        # 各テストの後に実行される(unittestの機能)
        if self.con:
            self.con.close()

    # serverがその後も正常に動いていればOK
    def test_01_timeout_no_request_disconnect(self) -> None:
        try:
            # requestを何も送らない

            # 何も送らずに接続を切断
            self.con.close()

        except HTTPException as e:
            print(f"Request failed: {e}")
            raise AssertionError

    # serverがその後も正常に動いていればOK
    def test_02_shortened_body_message_disconnect(self) -> None:
        try:
            # content-length分以下のbody
            self.con.request(
                "GET",
                "/",
                "abc",
                {
                    "Connection": "keep-alive",
                    "Content-Length": "5",
                },
            )

            # content-length分を送る前に接続を切る
            self.con.close()

        except HTTPException as e:
            print(f"Request failed: {e}")
            raise AssertionError
