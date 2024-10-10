import os
import subprocess
import time

SERVER_PORT = 8080


def read_file(file):
    with open(file, "r") as f:
        data = f.read()
        return data, len(data)


# \r\nをそのまま読み込む用(ヘッダー部分)
def read_file_binary(file):
    with open(file, "rb") as f:
        data = f.read()
    return data, len(data)


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


def assert_response(response, expected_response):
    assert (
        response == expected_response
    ), f"Expected response\n\n {repr(expected_response)}, but got\n\n {repr(response)}"


def assert_file_content(result_file_path, expected_content):
    result_content, _ = read_file(result_file_path)
    assert_response(result_content, expected_content)


def build_client_module():
    # client_module ディレクトリに移動
    client_module_dir = os.path.join(os.path.dirname(__file__), "client_module")
    # コマンドを実行
    subprocess.run(
        ["python3", "setup.py", "build_ext", "--inplace"],
        cwd=client_module_dir,
        check=True,
    )


try:
    # client_module をビルド
    build_client_module()
    # client_module.client をインポート
    import client_module.client as client
except subprocess.CalledProcessError as e:
    print(f"Build failed: {e}")
    raise
except ImportError as e:
    print(f"Import failed: {e}")
    raise


def send_request_and_assert_response(request_file, expected_response):
    client_instance = client.Client(SERVER_PORT)
    request, _ = read_file_binary(request_file)
    response = client_instance.SendRequestAndReceiveResponse(request)
    assert_response(response, expected_response)
