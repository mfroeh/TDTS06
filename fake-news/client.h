#pragma once
#include "http.h"
#include <string>

class ProxyClient {
public:
    HttpResponse forward(HttpRequest req);
};