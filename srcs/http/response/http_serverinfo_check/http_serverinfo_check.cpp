#include "http_serverinfo_check.hpp"
#include "http_exception.hpp"
#include "http_message.hpp"
#include "status_code.hpp"
#include "utils.hpp"
#include "virtual_server.hpp"
#include <cstdlib> // atoi
#include <iostream>

namespace http {

/*=====================================================================*/ // 消す

CheckServerInfoResult HttpServerInfoCheck::Check(
	const MockDtoServerInfos &server_info, const HttpRequestFormat &request
) {
	CheckServerInfoResult result;

	CheckDTOServerInfo(result, server_info, request.header_fields);
	CheckLocationList(result, server_info.locations, request.request_line.request_target);
	return result;
}

// Check Server_info
void HttpServerInfoCheck::CheckDTOServerInfo(
	CheckServerInfoResult    &result,
	const MockDtoServerInfos &server_info,
	const HeaderFields       &header_fields
) {
	if (header_fields.find(CONTENT_LENGTH) != header_fields.end()) {
		utils::Result<std::size_t> content_length =
			utils::ConvertStrToSize(header_fields.at(CONTENT_LENGTH));
		if (content_length.IsOk() && content_length.GetValue() > server_info.client_max_body_size) {
			throw HttpException("Error: payload too large.", StatusCode(PAYLOAD_TOO_LARGE));
		}
	}
	if (!server_info.error_page.second.empty()) {
		result.error_page.Set(true, server_info.error_page);
	}
}

// Check LocationList
void HttpServerInfoCheck::CheckLocationList(
	CheckServerInfoResult &result, const LocationList &locations, const std::string &request_target
) {
	const MockLocationCon &match_location = CheckLocation(result, locations, request_target);
	// std::cout << match_location.request_uri << std::endl; // for debug
	CheckIndex(result, match_location);
	CheckAutoIndex(result, match_location);
	CheckAlias(result, match_location);
	CheckRedirect(result, match_location);
	CheckAllowedMethods(result, match_location);
	CheckCgiExtension(result, match_location);
	CheckUploadDirectory(result, match_location);
	return;
}

const MockLocationCon HttpServerInfoCheck::CheckLocation(
	CheckServerInfoResult &result, const LocationList &locations, const std::string &request_target
) {
	MockLocationCon match_loc;

	// ex. request.target /www/target.html:
	// uri = /www/, target = target.html
	// location1 /www/data/
	// location1 /www/
	// location2 /
	// -> /www/
	result.path = request_target;
	for (LocationList::const_iterator it = locations.begin(); it != locations.end(); ++it) {
		if (request_target.find((*it).request_uri) == 0 &&
			(*it).request_uri.length() > match_loc.request_uri.length()) { // Longest Match
			match_loc = *it;
		}
	}
	if (match_loc.request_uri.empty()) {
		throw HttpException("Error: location not found", StatusCode(NOT_FOUND));
	}
	return match_loc;
}

void HttpServerInfoCheck::CheckIndex(
	CheckServerInfoResult &result, const MockLocationCon &location
) {
	result.index = location.index;
}

void HttpServerInfoCheck::CheckAutoIndex(
	CheckServerInfoResult &result, const MockLocationCon &location
) {
	if (location.autoindex == true) {
		result.autoindex = true;
	}
}

void HttpServerInfoCheck::CheckAlias(
	CheckServerInfoResult &result, const MockLocationCon &location
) {
	if (location.alias.empty()) {
		return;
	}
	// ex. request_uri /www/ alias /var/ -> /www/を削除して/var/に
	// /www/target.html -> /var/target.html
	std::string::size_type alias_start_pos = result.path.find(location.request_uri);
	if (alias_start_pos != std::string::npos) {
		result.path.replace(alias_start_pos, location.request_uri.length(), location.alias);
	}
}

void HttpServerInfoCheck::CheckRedirect(
	CheckServerInfoResult &result, const MockLocationCon &location
) {
	if (location.redirect.second.empty()) {
		return;
	}
	// ex. return 301 /var/data/index.html
	// /www/target.html -> /var/data/index.html
	// status code: 301
	result.redirect.Set(true, location.redirect);
}

void HttpServerInfoCheck::CheckAllowedMethods(
	CheckServerInfoResult &result, const MockLocationCon &location
) {
	result.allowed_methods = location.allowed_methods;
}

void HttpServerInfoCheck::CheckCgiExtension(
	CheckServerInfoResult &result, const MockLocationCon &location
) {
	result.cgi_extension = location.cgi_extension;
}

void HttpServerInfoCheck::CheckUploadDirectory(
	CheckServerInfoResult &result, const MockLocationCon &location
) {
	result.upload_directory = location.upload_directory;
}

/*=====================================================================*/ // 消す

const server::VirtualServer *CheckVSList(
	const server::VirtualServerAddrList &virtual_servers, const HeaderFields &header_fields
) {
	typedef server::VirtualServerAddrList::const_iterator VSAddrListItr;
	for (VSAddrListItr it = virtual_servers.begin(); it != virtual_servers.end(); ++it) {
		if (std::find(
				(*it)->GetServerNameList().begin(),
				(*it)->GetServerNameList().end(),
				header_fields.at(HOST)
			) != (*it)->GetServerNameList().end()) {
			return *it;
		}
	}
	return virtual_servers.front();
	// 見つからなかった場合は一番最初のサーバーを返す（デフォルトサーバー）
}

CheckServerInfoResult HttpServerInfoCheck::Check(
	const server::VirtualServerAddrList &server_infos, const HttpRequestFormat &request
) {
	CheckServerInfoResult        result;
	const server::VirtualServer *vs = CheckVSList(server_infos, request.header_fields);

	CheckDtoServerInfo(result, *vs, request.header_fields);
	CheckLocationList(result, vs->GetLocationList(), request.request_line.request_target);
	return result;
}

// Check Server_info
void HttpServerInfoCheck::CheckDtoServerInfo(
	CheckServerInfoResult       &result,
	const server::VirtualServer &virtual_server,
	const HeaderFields          &header_fields
) {
	// if (static_cast<size_t>(std::atoi(header_fields["Content-Length"].c_str())) >
	// 	server_info.client_max_body_size) { // Check content_length
	// 	throw HttpException("Error: payload too large.", PAYLOAD_TOO_LARGE);
	// } else if (!server_info.error_page.second.empty()) { // Check error_page
	// 	result.error_page = server_info.error_page;
	// }
	// tmp
	return;
}

// Check LocationList
void HttpServerInfoCheck::CheckLocationList(
	CheckServerInfoResult &result,
	const VSLocationList  &locations,
	const std::string     &request_target
) {
	const server::Location &match_location = CheckLocation(result, locations, request_target);
	// std::cout << match_location.request_uri << std::endl; // for debug
	CheckIndex(result, match_location);
	CheckAutoIndex(result, match_location);
	CheckAlias(result, match_location);
	CheckRedirect(result, match_location);
	CheckAllowedMethods(result, match_location);
	CheckCgiExtension(result, match_location);
	CheckUploadDirectory(result, match_location);
	return;
}

const server::Location HttpServerInfoCheck::CheckLocation(
	CheckServerInfoResult &result,
	const VSLocationList  &locations,
	const std::string     &request_target
) {
	// MockLocationCon match_loc;

	// // ex. request.target /www/target.html:
	// // uri = /www/, target = target.html
	// // location1 /www/data/
	// // location1 /www/
	// // location2 /
	// // -> /www/
	// result.path = request_target;
	// for (LocationList::const_iterator it = locations.begin(); it != locations.end(); ++it) {
	// 	if (request_target.find((*it).request_uri) == 0 &&
	// 		(*it).request_uri.length() > match_loc.request_uri.length()) { // Longest Match
	// 		match_loc = *it;
	// 	}
	// }
	// if (match_loc.request_uri.empty()) {
	// 	throw HttpException("Error: location not found", NOT_FOUND);
	// }
	// return match_loc;
}

void HttpServerInfoCheck::CheckIndex(
	CheckServerInfoResult &result, const server::Location &location
) {
	result.index = location.index;
}

void HttpServerInfoCheck::CheckAutoIndex(
	CheckServerInfoResult &result, const server::Location &location
) {
	// if (location.autoindex == true) {
	// 	result.autoindex = true;
	// }
}

void HttpServerInfoCheck::CheckAlias(
	CheckServerInfoResult &result, const server::Location &location
) {
	// if (location.alias.empty()) {
	// 	return;
	// }
	// // ex. request_uri /www/ alias /var/ -> /www/を削除して/var/に
	// // /www/target.html -> /var/target.html
	// std::string::size_type alias_start_pos = result.path.find(location.request_uri);
	// if (alias_start_pos != std::string::npos) {
	// 	result.path.replace(alias_start_pos, location.request_uri.length(), location.alias);
	// }
}

void HttpServerInfoCheck::CheckRedirect(
	CheckServerInfoResult &result, const server::Location &location
) {
	// if (location.redirect.second.empty()) {
	// 	return;
	// }
	// // ex. return 301 /var/data/index.html
	// // /www/target.html -> /var/data/index.html
	// // status code: 301
	// result.redirect.Set(true, location.redirect);
}

void HttpServerInfoCheck::CheckAllowedMethods(
	CheckServerInfoResult &result, const server::Location &location
) {
	// result.allowed_methods = location.allowed_methods;
}

void HttpServerInfoCheck::CheckCgiExtension(
	CheckServerInfoResult &result, const server::Location &location
) {
	// result.cgi_extension = location.cgi_extension;
}

void HttpServerInfoCheck::CheckUploadDirectory(
	CheckServerInfoResult &result, const server::Location &location
) {
	// result.upload_directory = location.upload_directory;
}

} // namespace http
