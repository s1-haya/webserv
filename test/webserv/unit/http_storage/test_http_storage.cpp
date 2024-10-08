#include "http_storage.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <iostream>

int GetTestCaseNum() {
	static unsigned int test_case_num = 0;
	++test_case_num;
	return test_case_num;
}

int ResultOk() {
	std::cout << utils::color::GREEN << GetTestCaseNum() << ".[OK]" << utils::color::RESET
			  << std::endl;
	return EXIT_SUCCESS;
}

int ResultNg() {
	std::cerr << utils::color::RED << GetTestCaseNum() << ".[NG] " << utils::color::RESET
			  << std::endl;
	return EXIT_FAILURE;
}

template <typename T>
int HandleResult(const T &result, const T &expected) {
	if (result == expected) {
		return ResultOk();
	} else {
		return ResultNg();
	}
}

int main(void) {
	int               ret_code = 0;
	http::HttpStorage storage;

	// クライアントのHttpRequestParsedDataを新規作成 -> OK
	http::HttpRequestParsedData new_data = storage.GetClientSaveData(1);
	ret_code |= HandleResult(new_data.request_result.status_code.GetEStatusCode(), http::OK);

	// 登録したクライアントのHttpRequestParsedData取得 -> OK
	http::HttpRequestParsedData save_data = storage.GetClientSaveData(1);
	ret_code |= HandleResult(save_data.request_result.status_code.GetEStatusCode(), http::OK);

	// ClientSaveDataを更新 -> OK
	try {
		save_data.request_result.status_code = http::StatusCode(http::BAD_REQUEST);
		storage.UpdateClientSaveData(1, save_data);
		http::HttpRequestParsedData update_data = storage.GetClientSaveData(1);
		ret_code |= HandleResult(
			update_data.request_result.status_code.GetEStatusCode(), http::BAD_REQUEST
		);
	} catch (const std::logic_error &e) {
		ret_code |= ResultNg();
		std::cerr << e.what() << std::endl;
	}

	// 更新するClientSaveDataが存在しない場合 -> Logic Error
	try {
		storage.UpdateClientSaveData(100, save_data);
		ret_code |= ResultNg();
	} catch (const std::logic_error &e) {
		ret_code |= ResultOk();
		std::cerr << e.what() << std::endl;
	}

	// ClientSaveDataを削除 -> OK
	try {
		storage.DeleteClientSaveData(1);
		new_data = storage.GetClientSaveData(1);
		// 更新したSaveData情報を削除されているかどうかを確認するため、BAD_REQUESTではなくOK
		ret_code |= HandleResult(new_data.request_result.status_code.GetEStatusCode(), http::OK);
	} catch (const std::logic_error &e) {
		ret_code |= ResultNg();
		std::cerr << e.what() << std::endl;
	}

	// 削除するClientSaveDataが存在しない場合 -> Logic Error
	try {
		storage.DeleteClientSaveData(100);
		ret_code |= ResultNg();
	} catch (const std::logic_error &e) {
		ret_code |= ResultOk();
		std::cerr << e.what() << std::endl;
	}
	return ret_code;
}
