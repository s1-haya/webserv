import os
import subprocess
from subprocess import CompletedProcess
from typing import Tuple

RETURN_CODE_FAIL = 1


def run_command(command: str) -> Tuple[int, str]:
    try:
        # コマンド実行
        ret: CompletedProcess[str] = subprocess.run(
            command, shell=True, capture_output=True, text=True
        )
        return ret.returncode, ret.stderr

    except subprocess.CalledProcessError as e:
        print(f"subprocess error: {e}")
        return RETURN_CODE_FAIL, str(e)


def put_log(log_filepath: str, content: str) -> None:
    # ディレクトリ部分を抽出
    log_dir = os.path.dirname(log_filepath)

    # ディレクトリが存在しない場合は新規作成
    if log_dir and not os.path.exists(log_dir):
        os.makedirs(log_dir)

    # logディレクトリの中にlogファイルを作成して書き込み
    with open(log_filepath, "w") as f:
        f.write(content)
