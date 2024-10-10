import os
import shutil

import pytest
from common_functions import (delete_file, read_file,
                              send_request_and_assert_response)
from common_response import (bad_request_response, created_response_close,
                             created_response_keep, forbidden_response,
                             no_content_response_close, not_allowed_response,
                             not_found_response_close,
                             payload_too_large_response,
                             response_header_get_root_200_close,
                             root_index_file, timeout_response)

REQUEST_DIR = "test/common/request/"
REQUEST_POST_2XX_DIR = REQUEST_DIR + "post/2xx/"
REQUEST_POST_4XX_DIR = REQUEST_DIR + "post/4xx/"

UPLOAD_DIR = "root/upload/"
UPLOAD_FILE_PATH = UPLOAD_DIR + "test_upload_file"
CHUNKED_FILE_PATH = UPLOAD_DIR + "chunked_request_file"
UPLOAD_SUB_DIR = UPLOAD_DIR + "upload_sub/"

PERMISSION_DENIED_DIR = UPLOAD_DIR + "permission-denied-dir/"


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
            created_response_keep + no_content_response_close,
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
        (
            REQUEST_POST_2XX_DIR + "201_06_empty_body_size.txt",
            created_response_close,
            UPLOAD_FILE_PATH,
            "",
        ),
        (
            REQUEST_POST_2XX_DIR + "201_07_just_content_max_body_size.txt",
            created_response_close,
            UPLOAD_FILE_PATH,
            "2024_body_size_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        ),
        (
            REQUEST_POST_2XX_DIR
            + "201_08_unchunked_body_size_just_client_max_body_size.txt",
            created_response_close,
            CHUNKED_FILE_PATH,
            "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",
        ),
    ],
    ids=[
        "201_01_upload_file",
        "201_02_chunked",
        "201_03_upload_file_204_same_upload_file",
        "201_04_chunked_200_get_close",
        "201_05_chunked_hex_size",
        "201_06_empty_body_size",
        "201_07_just_content_max_body_size",
        "201_08_unchunked_body_size_just_client_max_body_size",
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
    with cleanup_file_context(upload_file_path):
        send_request_and_assert_response(request_file, expected_response)
        assert_uploaded_file_content(upload_file_path, expected_upload_file_content)


@pytest.fixture
def create_and_cleanup_dir():
    from contextlib import contextmanager

    @contextmanager
    def _set_and_cleanup():
        sub_dir_path = UPLOAD_SUB_DIR
        # ディレクトリを作成
        os.makedirs(sub_dir_path, exist_ok=True)

        yield sub_dir_path

        # 作成したディレクトリとその中身を削除
        shutil.rmtree(sub_dir_path)

    return _set_and_cleanup


@pytest.mark.parametrize(
    "request_file, expected_response, upload_file_path, expected_upload_file_content",
    [
        (
            REQUEST_POST_2XX_DIR + "201_09_upload_file_exist_sub_dir.txt",
            created_response_close,
            UPLOAD_SUB_DIR + "test_upload_file",
            "abcde",
        ),
    ],
    ids=[
        "201_09_upload_file_exist_sub_dir",
    ],
)
def test_post_201_responses(
    request_file,
    expected_response,
    upload_file_path,
    expected_upload_file_content,
    create_and_cleanup_dir,
):
    with create_and_cleanup_dir():
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
            REQUEST_POST_4XX_DIR + "400_03_no_content_type.txt",
            bad_request_response,
            UPLOAD_FILE_PATH,
        ),
        (
            REQUEST_POST_4XX_DIR
            + "400_04_no_content_length_and_no_transfer_encoding.txt",
            bad_request_response,
            UPLOAD_FILE_PATH,
        ),
        (
            REQUEST_POST_4XX_DIR
            + "400_05_no_content_length_and_not_chunked_transfer_encoding.txt",
            bad_request_response,
            CHUNKED_FILE_PATH,
        ),
        (
            REQUEST_POST_4XX_DIR + "400_06_incomplete_chunked_body.txt",
            bad_request_response,
            CHUNKED_FILE_PATH,
        ),
        (
            REQUEST_POST_4XX_DIR + "400_07_incomplete_chunked_body_0_end.txt",
            bad_request_response,
            CHUNKED_FILE_PATH,
        ),
        (
            REQUEST_POST_4XX_DIR + "400_08_incomplete_chunked_body_0crlf_end.txt",
            bad_request_response,
            CHUNKED_FILE_PATH,
        ),
        (
            REQUEST_POST_4XX_DIR + "400_09_chunked_empty_chunk_size.txt",
            bad_request_response,
            CHUNKED_FILE_PATH,
        ),
        (
            REQUEST_POST_4XX_DIR + "400_10_chunked_empty_chunk_data.txt",
            bad_request_response,
            CHUNKED_FILE_PATH,
        ),
        (
            REQUEST_POST_4XX_DIR + "400_11_overflow_chunk_size_and_crlf.txt",
            bad_request_response,
            CHUNKED_FILE_PATH,
        ),
        (
            REQUEST_POST_4XX_DIR + "400_12_only_too_large_chunk_size_early_check.txt",
            bad_request_response,
            CHUNKED_FILE_PATH,
        ),
        # 403 is below -> test_post_403_responses()
        (
            REQUEST_POST_4XX_DIR + "404_01_non_exist_directory.txt",
            not_found_response_close,
            None,
        ),
        (
            REQUEST_POST_4XX_DIR + "405_01_method_not_allowed_for_uri.txt",
            not_allowed_response,
            None,
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
        (
            REQUEST_POST_4XX_DIR + "408_03_incomplete_chunked_body.txt",
            timeout_response,
            CHUNKED_FILE_PATH,
        ),
        (
            REQUEST_POST_4XX_DIR + "408_04_incomplete_chunked_body_0_end.txt",
            timeout_response,
            CHUNKED_FILE_PATH,
        ),
        (
            REQUEST_POST_4XX_DIR + "408_05_incomplete_chunked_body_0crlf_end.txt",
            timeout_response,
            CHUNKED_FILE_PATH,
        ),
        (
            REQUEST_POST_4XX_DIR + "408_06_max_chunk_size_and_crlf.txt",
            timeout_response,
            CHUNKED_FILE_PATH,
        ),
        (
            REQUEST_POST_4XX_DIR + "413_01_too_large_content_max_body_size.txt",
            payload_too_large_response,
            UPLOAD_FILE_PATH,
        ),
        (
            REQUEST_POST_4XX_DIR + "413_02_too_large_unchunked_body_size.txt",
            payload_too_large_response,
            CHUNKED_FILE_PATH,
        ),
    ],
    ids=[
        "400_01_duplicate_content_length",
        "400_02_transfer_encoding_and_content_length",
        "400_03_no_content_type",
        "400_04_no_content_length_and_no_transfer_encoding",
        "400_05_no_content_length_and_not_chunked_transfer_encoding",
        "400_06_incomplete_chunked_body",
        "400_07_incomplete_chunked_body_0_end",
        "400_08_incomplete_chunked_body_0crlf_end",
        "400_09_chunked_empty_chunk_size",
        "400_10_chunked_empty_chunk_data",
        "400_11_overflow_chunk_size_and_crlf",
        "400_12_only_too_large_chunk_size_early_check",
        "404_01_non_exist_directory",
        "405_01_method_not_allowed_for_uri",
        "408_01_shortened_body_message",
        "408_02_no_body_message",
        "408_03_incomplete_chunked_body",
        "408_04_incomplete_chunked_body_0_end",
        "408_05_incomplete_chunked_body_0crlf_end",
        "408_06_max_chunk_size_and_crlf",
        "413_01_too_large_content_max_body_size",
        "413_02_too_large_unchunked_body_size.txt",
    ],
)
def test_post_4xx_responses(
    request_file,
    expected_response,
    upload_file_path,
    cleanup_file_context,
):
    with cleanup_file_context(upload_file_path):
        send_request_and_assert_response(request_file, expected_response)


@pytest.fixture
def set_permission_and_cleanup_restricted_dir():
    from contextlib import contextmanager

    @contextmanager
    def _set_and_cleanup(upload_file_path):
        # permissionを操作したいディレクトリ
        restricted_path = PERMISSION_DENIED_DIR
        # ディレクトリを作成
        os.makedirs(restricted_path, exist_ok=True)
        # permissionを変更して書き込み不可に設定
        os.chmod(restricted_path, 0o400)

        yield restricted_path

        # テスト後に元に戻す
        os.chmod(restricted_path, 0o775)
        # テスト失敗時にファイルがuploadされてしまった場合用
        delete_file(upload_file_path)
        # 作成したディレクトリとその中身を削除
        shutil.rmtree(restricted_path)

    return _set_and_cleanup


@pytest.mark.skip(reason="only github actions returns 201")
@pytest.mark.parametrize(
    "request_file, expected_response, upload_file_path",
    [
        (
            REQUEST_POST_4XX_DIR + "403_01_permission_denied_directory.txt",
            forbidden_response,
            PERMISSION_DENIED_DIR + "test_upload_file",
        ),
    ],
    ids=[
        "403_01_permission_denied_directory",
    ],
)
def test_post_403_responses(
    request_file,
    expected_response,
    upload_file_path,
    set_permission_and_cleanup_restricted_dir,
):
    with set_permission_and_cleanup_restricted_dir(upload_file_path):
        send_request_and_assert_response(request_file, expected_response)
