import os
import time

import pytest
from common_functions import send_request_and_assert_response
from common_response import (forbidden_response, no_content_response_close,
                             no_content_response_keep, not_allowed_response,
                             not_found_response_close)

REQUEST_DIR = "test/common/request/"
REQUEST_DELETE_2XX_DIR = REQUEST_DIR + "delete/2xx/"
REQUEST_DELETE_4XX_DIR = REQUEST_DIR + "delete/4xx/"

UPLOAD_DIR = "root/upload/"
DELETE_FILE_PATH = UPLOAD_DIR + "delete_file"
NOT_EXISTING_FILE_PATH = UPLOAD_DIR + "not_found"


def create_file(file_path, content=""):
    if file_path is None:
        return

    try:
        os.makedirs(os.path.dirname(file_path), exist_ok=True)
        with open(file_path, "w") as f:
            f.write(content)
        print(f"Created file: {file_path}")
    except Exception as e:
        print(f"Error creating file: {file_path}, {e}")
        raise AssertionError


# ファイルがない場合に作成する関数
@pytest.fixture
def setup_file_context():
    # コンテキストマネージャとして使用可能なフィクスチャ
    from contextlib import contextmanager

    @contextmanager
    def _setup(file_path, content=""):
        create_file(file_path, content)
        yield
        if os.path.exists(file_path):
            print(f"File exists: {file_path}")
            raise AssertionError

    return _setup


@pytest.mark.parametrize(
    "request_file, expected_response, file_path, file_content",
    [
        (
            REQUEST_DELETE_2XX_DIR + "204_01_delete_existing_file.txt",
            no_content_response_close,
            DELETE_FILE_PATH,
            "Sample content for deletion",
        ),
        (
            REQUEST_DELETE_2XX_DIR
            + "204_02_delete_existing_file_with_body_message.txt",
            no_content_response_close,
            DELETE_FILE_PATH,
            "Sample content for deletion",
        ),
        (
            REQUEST_DELETE_2XX_DIR
            + "204_03_delete_existing_file_then_404_on_second_attempt.txt",
            no_content_response_keep + not_found_response_close,
            DELETE_FILE_PATH,
            "Sample content for deletion",
        ),
        (
            REQUEST_DELETE_2XX_DIR + "204_04_delete_empty_file.txt",
            no_content_response_close,
            DELETE_FILE_PATH,
            "",
        ),
        (
            REQUEST_DELETE_2XX_DIR + "204_05_delete_existing_file_keep.txt",
            no_content_response_keep,
            DELETE_FILE_PATH,
            "Sample content for deletion",
        ),
    ],
    ids=[
        "204_01_delete_existing_file",
        "204_02_delete_existing_file_with_body_message",
        "204_03_delete_existing_file_then_404_on_second_attempt",
        "204_04_delete_empty_file",
        "204_05_delete_existing_file_keep",
    ],
)
def test_delete_responses(
    request_file,
    expected_response,
    file_path,
    file_content,
    setup_file_context,
):
    # setup_file_contextフィクスチャを使用してファイル作成を実行
    if file_path:
        with setup_file_context(file_path, file_content):
            send_request_and_assert_response(request_file, expected_response)


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


# ファイルがある場合に削除する関数
@pytest.fixture
def cleanup_file_context():
    # コンテキストマネージャとして使用可能なフィクスチャ
    from contextlib import contextmanager

    @contextmanager
    def _cleanup(file_path):
        delete_file(file_path)
        yield

    return _cleanup


@pytest.mark.parametrize(
    "request_file, expected_response, upload_file_path",
    [
        (
            REQUEST_DELETE_4XX_DIR + "403_01_delete_directory.txt",
            forbidden_response,
            None,
        ),
        (
            REQUEST_DELETE_4XX_DIR + "404_01_delete_nonexistent_file.txt",
            not_found_response_close,
            NOT_EXISTING_FILE_PATH,
        ),
        (
            REQUEST_DELETE_4XX_DIR + "405_01_method_not_allowed_for_uri.txt",
            not_allowed_response,
            None,
        ),
    ],
    ids=[
        "403_01_delete_directory",
        "404_01_delete_nonexistent_file",
        "405_01_method_not_allowed_for_uri",
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
