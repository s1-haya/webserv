#include "http_storage.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <iostream>

int GetTestCaseNum() {
	static unsigned int test_case_num = 0;
	++test_case_num;
	return test_case_num;
}

template <typename T>
int HandleResult(const T &result, const T &expected) {
	if (result == expected) {
		std::cout << utils::color::GREEN << GetTestCaseNum() << ".[OK]" << utils::color::RESET
				  << std::endl;
		return EXIT_SUCCESS;
	} else {
		std::cerr << utils::color::RED << GetTestCaseNum() << ".[NG] " << utils::color::RESET
				  << std::endl;
		return EXIT_FAILURE;
	}
}

int main(void) {
	int               ret_code = 0;
	http::HttpStorage storage;

	// ClientSaveDataを新規作成 -> OK
	http::ClientSaveData new_data = storage.GetClientSaveData(1);
	ret_code |= HandleResult(new_data.save_request_result.status_code, http::OK);

	// client_fdを登録したClientSaveData取得 -> OK
	http::ClientSaveData save_data = storage.GetClientSaveData(1);
	ret_code |= HandleResult(save_data.save_request_result.status_code, http::OK);

	// ClientSaveDataを更新 -> OK
	save_data.save_request_result.status_code = http::BAD_REQUEST;
	storage.UpdateClientSaveData(1, save_data);
	http::ClientSaveData update_data = storage.GetClientSaveData(1);
	ret_code |= HandleResult(update_data.save_request_result.status_code, http::BAD_REQUEST);

	// 更新するClientSaveDataが存在しない場合 -> Logic Error
	// storage.UpdateClientSaveData(2, save_data);
	return ret_code;
}
