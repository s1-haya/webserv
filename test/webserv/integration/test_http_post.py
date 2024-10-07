import os
import time

import pytest
from common_functions import read_file, send_request_and_assert_response
from common_response import (bad_request_response, created_response_close,
                             created_response_keep, no_content_response,
                             response_header_get_root_200_close,
                             root_index_file, timeout_response)

REQUEST_DIR = "test/common/request/"
REQUEST_POST_2XX_DIR = REQUEST_DIR + "post/2xx/"
REQUEST_POST_4XX_DIR = REQUEST_DIR + "post/4xx/"

UPLOAD_DIR = "root/upload/"
UPLOAD_FILE_PATH = UPLOAD_DIR + "test_upload_file"
CHUNKED_FILE_PATH = UPLOAD_DIR + "chunked_request_file"


def delete_file(file_path):
    if file_path is None:
        return

    try:
        if os.path.exists(file_path):
            os.remove(file_path)
            time.sleep(1)
            print(f"Deleted file: {file_path}")
    except Exception as e:
        print(f"Error deleting file: {file_path}, {e}")
        raise AssertionError


def assert_uploaded_file_content(upload_file_path, expected_upload_file_content):
    if upload_file_path is None:
        return

    result_body_message, result_content_length = read_file(upload_file_path)

    assert result_body_message == expected_upload_file_content
    assert result_content_length == len(expected_upload_file_content)


@pytest.fixture
def cleanup_file_context():
    # コンテキストマネージャとして使用可能なフィクスチャ
    from contextlib import contextmanager

    @contextmanager
    def _cleanup(file_path):
        delete_file(file_path)
        yield
        delete_file(file_path)

    return _cleanup


# upload_file_path: テスト前に削除しておきたいupload_file_pathが特になければNoneを指定
# expected_upload_file_content: ファイル作成された場合に期待するファイルの中身
@pytest.mark.parametrize(
    "request_file, expected_response, upload_file_path, expected_upload_file_content",
    [
        (
            REQUEST_POST_2XX_DIR + "201_01_upload_file.txt",
            created_response_close,
            UPLOAD_FILE_PATH,
            "abcde",
        ),
        (
            REQUEST_POST_2XX_DIR + "201_02_chunked.txt",
            created_response_close,
            CHUNKED_FILE_PATH,
            "SAWAMURA HAYATO",
        ),
        (
            REQUEST_POST_2XX_DIR + "201_03_upload_file_204_same_upload_file.txt",
            created_response_keep + no_content_response,
            UPLOAD_FILE_PATH,
            "first",
        ),
        (
            REQUEST_POST_2XX_DIR + "201_04_chunked_200_get_close.txt",
            created_response_keep
            + response_header_get_root_200_close
            + root_index_file,
            CHUNKED_FILE_PATH,
            "Wikipedia",
        ),
        (
            REQUEST_POST_2XX_DIR + "201_05_chunked_hex_size.txt",
            created_response_close,
            CHUNKED_FILE_PATH,
            "Wikipedia is a free online encyclopedia that anyone can edit.",
        ),
    ],
    ids=[
        "201_01_upload_file",
        "201_02_chunked",
        "201_03_upload_file_204_same_upload_file",
        "201_04_chunked_200_get_close",
        "201_05_chunked_hex_size",
    ],
)
def test_post_upload_responses(
    request_file,
    expected_response,
    upload_file_path,
    expected_upload_file_content,
    cleanup_file_context,
):
    # cleanup_file_contextフィクスチャを使用してファイル削除を実行
    if upload_file_path:
        with cleanup_file_context(upload_file_path):
            send_request_and_assert_response(request_file, expected_response)
            assert_uploaded_file_content(upload_file_path, expected_upload_file_content)


# upload_file_path: ファイルを作らない想定でもテスト失敗時用にupload_file_pathを指定。ない場合はNoneを指定
@pytest.mark.parametrize(
    "request_file, expected_response, upload_file_path",
    [
        (
            REQUEST_POST_4XX_DIR + "400_01_duplicate_content_length.txt",
            bad_request_response,
            UPLOAD_DIR + "duplicate_content_length",
        ),
        (
            REQUEST_POST_4XX_DIR + "400_02_transfer_encoding_and_content_length.txt",
            bad_request_response,
            UPLOAD_FILE_PATH,
        ),
        (
            REQUEST_POST_4XX_DIR + "408_01_shortened_body_message.txt",
            timeout_response,
            UPLOAD_DIR + "shortened_body_message",
        ),
        (
            REQUEST_POST_4XX_DIR + "408_02_no_body_message.txt",
            timeout_response,
            UPLOAD_DIR + "no_body_message",
        ),
    ],
    ids=[
        "400_01_duplicate_content_length",
        "400_02_transfer_encoding_and_content_length",
        "408_01_shortened_body_message",
        "408_02_no_body_message",
    ],
)
def test_post_4xx_responses(
    request_file,
    expected_response,
    upload_file_path,
    cleanup_file_context,
):
    if upload_file_path:
        with cleanup_file_context(upload_file_path):
            send_request_and_assert_response(request_file, expected_response)