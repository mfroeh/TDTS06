#pragma once
#include <map>
#include <string>

// Convenience struct for HttpRequests
struct HttpRequest {
  HttpRequest(std::string raw_request);
  std::string raw;
  std::string method;
  std::string url;
  std::string version;
  std::map<std::string, std::string> headers;
  std::string body;
};

// Convenience struct for HttpResponses
struct HttpResponse {
  HttpResponse(std::string raw_response);
  std::string raw;
  std::string code;
  std::string phrase;
  std::string version;
  std::map<std::string, std::string> headers;
  std::string body;
};
