#pragma once
#include <map>
#include <string>

enum class HttpMethod {
    GET, POST, HEAD, PUT, DELETE
};

struct HttpRequest {
    std::string raw;
    HttpRequest(std::string raw_request);
    std::string method;
    std::string url;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
};

enum class HttpResponseCode {
    OK = 200,
    NOT_MODIFIED = 304,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
};

struct HttpResponse {
    std::string raw;
    HttpResponse(std::string raw_response);
    std::string code;
    std::string phrase;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
};
