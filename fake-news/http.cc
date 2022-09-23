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
  while (getline(stream, line, '\r')) {
    // If we have reached the header body seperation line
    if (line.length() == 0) {
      stream.ignore(1000, '\n');
      break;
    }

    // We take lines to '\r,' which means that '\n' is still in the stream and
    // would be picked up in the next call to getline(...).
    stream.ignore(1000, '\n');

    int index{static_cast<int>(line.find_first_of(' '))};
    string name{line.substr(0, index - 1)};
    string value{line.substr(index + 1, line.length())};
    headers[name] = value;
  }

  // Read in body
  while (getline(stream, line)) {
    if (line.length() != 0) {
      body.append(line);
    }
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
  while (getline(stream, line, '\r')) {
    // If we have reached the header body seperation line
    if (line.length() == 0) {
      stream.ignore(1000, '\n');
      break;
    }

    int index{static_cast<int>(line.find_first_of(' '))};
    string name{line.substr(0, index - 1)};
    string value{line.substr(index + 1, line.length())};
    headers[name] = value;
  }

  // Read in body
  while (getline(stream, line)) {
    body.append(line);
  }
}
