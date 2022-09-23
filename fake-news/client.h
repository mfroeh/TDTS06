#pragma once
#include "http.h"
#include <string>

// Contains a method for forwarding a HttpRequest intercepted by the proxy
// server to the orignal endpoint. After receiving a response, it alters it
// using Faker.
class ProxyClient {
public:
  // Forwards a given HttpRequest to its endpoint, receives the response, alters
  // it using Faker and returns it.
  HttpResponse forward(HttpRequest req);
};
