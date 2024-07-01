import subprocess


def test_webserv() -> None:
    try:
        server_url = "http://localhost:8080"
        cp = subprocess.run(
            ["/usr/bin/curl", server_url], capture_output=True, text=True
        )
        # レスポンスのコンテントが出力されたらOK
        print(cp.stdout)
    except subprocess.CalledProcessError as e:
        print(f"Request failed: {e}")


if __name__ == "__main__":
    test_webserv()
