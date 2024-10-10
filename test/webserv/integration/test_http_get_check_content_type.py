import json
from http import HTTPStatus
from http.client import HTTPConnection, HTTPException
from typing import Mapping, Tuple

from common_functions import SERVER_PORT, delete_file
from http_module.assert_http_response import assert_header, assert_status_line

ROOT_DIR = "root/"
UPLOAD_DIR = "upload/"

CONTENT_TYPE = "Content-Type"
CONTENT_LENGTH = "Content-Length"

APPLICATION_JSON = "application/json"
APPLICATION_OCTET_STREAM = "application/octet-stream"
TEXT_PLAIN = "text/plain"


# content-lengthは自動で入る
def create_headers(content_type) -> Mapping[str, str]:
    headers = {}
    headers[CONTENT_TYPE] = content_type
    return headers


# ------------------------------------------------------------------------------
JSON_FILENAME, JSON_DATA = (
    "webserv.json",
    {
        "name": "webserv",
        "team": {
            "members": 3,
        },
    },
)
JSON_FULL_PATH = ROOT_DIR + UPLOAD_DIR + JSON_FILENAME


def create_json_request(json_data) -> Tuple[str, Mapping[str, str]]:
    def create_json_body() -> str:
        # jsonを文字列に変換し、byte列にencode
        encoded_json_data = json.dumps(json_data)
        return encoded_json_data

    return create_json_body(), create_headers(APPLICATION_JSON)


def test_01_content_type_post_json_and_get_json() -> None:
    delete_file(JSON_FULL_PATH)

    try:
        con = HTTPConnection("localhost", SERVER_PORT)

        # JSONをPOSTしてuploadしておく
        con.request(
            "POST", "/" + UPLOAD_DIR + JSON_FILENAME, *create_json_request(JSON_DATA)
        )

        response = con.getresponse()
        # 201 Created のbodyを読む
        response.read()

        # GET JSONしてContent-Typeがjsonかどうかをassert
        con.request("GET", "/" + UPLOAD_DIR + JSON_FILENAME)
        response = con.getresponse()

        assert_status_line(response, HTTPStatus.OK)
        assert_header(response, CONTENT_TYPE, APPLICATION_JSON)
        assert response.read() == json.dumps(JSON_DATA).encode("utf-8")

    except HTTPException as e:
        print(f"Request failed: {e}")
        raise AssertionError
    finally:
        if con:
            con.close()
        delete_file(JSON_FULL_PATH)


UNKNOWN_FILENAME, UNKNOWN_DATA = (
    "webserv.unknown",
    b"unknownnnnnn",
)
UNKNOWN_FULL_PATH = ROOT_DIR + UPLOAD_DIR + UNKNOWN_FILENAME


def test_02_content_type_post_unknown_and_get() -> None:
    delete_file(UNKNOWN_FULL_PATH)

    try:
        con = HTTPConnection("localhost", SERVER_PORT)

        # .unknownデータをPOSTしてuploadしておく
        con.request(
            "POST",
            "/" + UPLOAD_DIR + UNKNOWN_FILENAME,
            UNKNOWN_DATA,
            create_headers(APPLICATION_OCTET_STREAM),
        )

        response = con.getresponse()
        response.read()

        # GET .unknownデータしてContent-Typeをassert
        con.request("GET", "/" + UPLOAD_DIR + UNKNOWN_FILENAME)
        response = con.getresponse()

        assert_status_line(response, HTTPStatus.OK)
        # 不明なものは"application/octet-stream"
        assert_header(response, CONTENT_TYPE, APPLICATION_OCTET_STREAM)
        assert response.read() == UNKNOWN_DATA

    except HTTPException as e:
        print(f"Request failed: {e}")
        raise AssertionError
    finally:
        if con:
            con.close()
        delete_file(UNKNOWN_FULL_PATH)
