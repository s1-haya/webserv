from common import run_command, put_log

RETURN_CODE_SUCCESS = 0


class TestUnit:
    UNIT_TEST_DIR: str = "test/unit"
    UNIT_TEST_LOG_DIR: str = "test/unit/log"

    # utils/SplitStr()
    def test_split_str(self) -> None:
        test_dir: str = self.UNIT_TEST_DIR + "/split"
        command: str = "make -C " + test_dir + " re && make -C " + test_dir + " run"

        # テストコマンド実行
        ret_code, ret_stderr = run_command(command)

        # テスト結果をログとしてファイル出力
        log_filepath: str = self.UNIT_TEST_LOG_DIR + "/log_split_str.txt"
        put_log(log_filepath, ret_stderr)

        assert ret_code == RETURN_CODE_SUCCESS

    # unit testはここに追加 ↓


if __name__ == "__main__":
    test = TestUnit()
    test.test_split_str()
