#include "http.h"
#include <iostream>
#include <sstream>

using namespace std;

HttpRequest::HttpRequest(string str) {
  istringstream stream{str};
  raw = str;

  stream >> method;
  stream >> url;
  stream >> version;
  stream.ignore(1000, '\n');

  headers = map<string, string>{};
  string line{};
  while (getline(stream, line)) {
    // If the only character is \r we have reached header body seperation line
    if (line.length() == 1) {
      break;
    }

    int index{static_cast<int>(line.find_first_of(' '))};
    string name{line.substr(0, index - 1)};
    // We need line.length() - 1 to cut out \r
    string value{line.substr(index + 1, line.length() - 1)};
    headers[name] = value;
  }

  // Read in body
  while (getline(stream, line)) {
    body.append(line);
    body.append("\n");
  }
}

HttpResponse::HttpResponse(string str) {
  istringstream stream{str};
  raw = str;

  stream >> version;
  stream >> code;
  stream >> phrase;
  stream.ignore(1000, '\n');

  headers = map<string, string>{};
  string line{};
  while (getline(stream, line)) {
    // If the only character is \r we have reached header body seperation line
    if (line.length() == 1) {
      break;
    }

    int index{static_cast<int>(line.find_first_of(' '))};
    string name{line.substr(0, index - 1)};
    // We need line.length() - 1 to cut out \r
    string value{line.substr(index + 1, line.length() - 1)};
    headers[name] = value;
  }

  // Read in body
  while (getline(stream, line)) {
    body.append(line);
    body.append("\n");
  }
}
