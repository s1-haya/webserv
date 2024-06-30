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
    with open(log_filepath, "w") as f:
        f.write(content)
