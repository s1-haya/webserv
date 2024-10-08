#include "test_case.hpp"
#include <cstdlib>
#include <iostream>

/**
 * @brief Test Http class: HttpResultの返り値を確認するテスト。
 *
 * テスト一覧（今後追加）
 * - メソッドが機能しているかどうか
 * 	- GET
 * 	- POST
 * 	- DELETE
 * - リクエストターゲットが機能しているかどうか(CGIは今後)
 * 	- 存在してるファイル
 * 	- 存在してないファイル
 * 	- 存在してるディレクトリ（末尾に/ありとなし）
 * 	- 存在してないディレクトリ（末尾に/ありとなし）
 * - ヘッダーフィールドごとに機能しているかどうか(今後、追加)
 * 	- Host
 * 	- Content-Length
 * - VirtualServer, Locationの各ディレクティブが機能しているかどうか
 * 	- VirtualServer
 * 	 - server_names
 * 	- Location
 * 	 - allow_methods
 *
 * responseの期待値はstatus_line, header_fields, CRLF, body_messageを組み合わせる。
 * status_lineとデフォルトのステータスコードを返すbody_messageはファイルで用意する。header_fieldsは可変のため文字列で作成する。
 * response以外のHttpResultのパラメータの期待値はテストごとに作成。
 *
 * HttpResult
 * - is_response_complete レスポンスの書式が完全かどうか
 * - is_connection_keep   次の接続が可能かどうか
 * - request_buf          バッファ
 * - response             文字列
 *
 * Locationのディレクティブを網羅するテストができるようなvirtual_serversを設定する。
 * client_infosはfdは1で固定する。request_bufはファイルから読み込むで文字列にする。
 *
 * @param client_infos
 * @param virtual_servers
 *
 * @return HttpResult indicating whether the response is complete
 *         and containing the response data.
 */

server::VirtualServerAddrList BuildVirtualServerAddrList();
void DeleteVirtualServerAddrList(const server::VirtualServerAddrList &virtual_servers);
int  main(void) {
    int ret_code = EXIT_SUCCESS;

    std::cout << "\033[44;37m[ Test Run ]\033[m" << std::endl;
    server::VirtualServerAddrList server_infos = BuildVirtualServerAddrList();
    ret_code |= test::TestGetOk1ConnectionClose(server_infos);
    ret_code |= test::TestGetOk2ConnectionKeep(server_infos);
    ret_code |= test::TestGetOk3SubConnectionClose(server_infos);
    ret_code |= test::TestGetOk4ConnectionKeepAndOkConnectionKeep(server_infos);
    ret_code |= test::TestGetOk5ConnectionCloseAndOkConnectionClose(server_infos);
    ret_code |= test::TestGetOk6ConnectionKeepAndOkConnectionClose(server_infos);
    ret_code |= test::TestGetOk11UpperAndLowerHeaderFields(server_infos);
    ret_code |= test::TestGetOk12HeaderFieldValueSpace(server_infos);
    ret_code |= test::TestGetOk13SpaceHeaderFieldValue(server_infos);
    ret_code |= test::TestGetOk14ExtraRequest(server_infos);
    ret_code |= test::TestGetOk15BodyMessageDefault(server_infos);
    ret_code |= test::TestGetOk17NotExistHeaderField(server_infos);
    ret_code |= test::TestGetOk21NoConnection(server_infos);

    ret_code |= test::TestGetBadRequest1OnlyCrlf(server_infos);
    ret_code |= test::TestGetBadRequest2LowerMethod(server_infos);
    ret_code |= test::TestGetBadRequest3NoAsciiMethod(server_infos);
    ret_code |= test::TestGetBadRequest4NoRoot(server_infos);
    ret_code |= test::TestGetBadRequest5RelativePath(server_infos);
    ret_code |= test::TestGetBadRequest6LowerHttpVersion(server_infos);
    ret_code |= test::TestGetBadRequest7WrongHttpName(server_infos);
    ret_code |= test::TestGetBadRequest8WrongHttpVersion(server_infos);
    ret_code |= test::TestGetBadRequest9NoHost(server_infos);
    ret_code |= test::TestGetBadRequest10DuplicateHost(server_infos);
    ret_code |= test::TestGetBadRequest11NoHeaderFieldColon(server_infos);
    ret_code |= test::TestGetBadRequest12NoConnectionName(server_infos);
    ret_code |= test::TestGetBadRequest15SpaceInHeaderFieldName(server_infos);
    ret_code |= test::TestGetBadRequest16HeaderFieldNameSpaceColon(server_infos);
    ret_code |= test::TestGetBadRequest17SpaceHeaderFieldName(server_infos);
    ret_code |= test::TestGetBadRequest18NonVchrHeaderFieldName(server_infos);
    ret_code |= test::TestGetBadRequest19NonVchrHeaderFieldValue(server_infos);
    ret_code |= test::TestGetBadRequest20TooFewStatusLineElements(server_infos);
    ret_code |= test::TestGetBadRequest21TooManyStatusLineElements(server_infos);

    ret_code |= test::TestGetNotFound1NotExistFile(server_infos);
    ret_code |= test::TestGetMethodNotAllowed(server_infos);
    ret_code |= test::TestGetNotImplemented1NotExistMethod(server_infos);

    std::cout << "\n\033[44;37m[ Test DELETE ]\033[m" << std::endl;
    ret_code |= test::TestDeleteNoContent1ExistingFile(server_infos);
    ret_code |= test::TestDeleteNoContent2ExistingFileWithBodyMessage(server_infos);
    ret_code |= test::TestDeleteNoContent3ExistingFileThenNotFoundOnSecondAttempt(server_infos);

    ret_code |= test::TestDelete1ForbiddenDirectory(server_infos);
    ret_code |= test::TestDelete1NotFoundNonexistentFile(server_infos);
    ret_code |= test::TestDeleteMethodNotAllowed(server_infos);

    // test GetErrorResponse
    std::cout << "\n\033[44;37m[ Test GetErrorResponse ]\033[m" << std::endl;
    ret_code |= test::TestRequestTimeoutResponse();
    ret_code |= test::TestInternalServerErrorResponse();

    // test GetResponseFromCgi
    std::cout << "\n\033[44;37m[ Test GetResponseFromCgi ]\033[m" << std::endl;
    ret_code |= test::TestGetResponseFromCgi1();
    ret_code |= test::TestGetResponseFromCgi2();
    ret_code |= test::TestGetResponseFromCgi3();
    ret_code |= test::TestGetResponseFromCgi4();
    return ret_code;
}
