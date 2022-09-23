#include "client.h"
#include "faker.h"
#include "http.h"
#include <algorithm>
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

HttpResponse ProxyClient::forward(HttpRequest req) {
  // Get addrinfo for host from HttpRequest
  struct addrinfo hints;
  struct addrinfo *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int status{};

  // TODO: I don't know why we have to cut this by one more character...
  // The \n character is discarded by getline and we should be removing the \r
  // with substring(0, length - 1) in the HttpRequest ctor...
  if ((status = getaddrinfo(req.headers["Host"]
                                .substr(0, req.headers["Host"].length() - 1)
                                .c_str(),
                            "80", &hints, &res))) {
    cerr << "[ProxyClient] Failed to getaddrinfo for " << req.headers["Host"]
         << endl;
    cerr << gai_strerror(status) << endl;
    return HttpResponse{string{}};
  }

  // Open socket
  int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (sockfd == -1) {
    cerr << "[ProxyClient] Failed to create client socket!" << endl;
    return HttpResponse{string{}};
  }

  // Connect to host
  if (connect(sockfd, res->ai_addr, res->ai_addrlen)) {
    cerr << "[ProxyClient] Failed to connect to " << req.headers["Host"]
         << endl;
    return HttpResponse{string{}};
  }

  // Forward request
  int sent_cum{0};
  int sent_last{0};
  for (; sent_cum < req.raw.size(); sent_cum += sent_last) {
    string part{req.raw.substr(
        sent_cum, min(sent_cum + 8192, static_cast<int>(req.raw.size())))};
    sent_last = send(sockfd, part.c_str(), part.size(), 0);
  }
  cout << "[ProxyClient] Forwarded " << sent_cum << " bytes" << endl;

  // Receive response
  string raw{};
  char buffer[8192];
  // How much of the body is left to receive
  int remaining{INT_MAX};
  while (remaining > 0) {
    int received{static_cast<int>(recv(sockfd, buffer, sizeof(buffer), 0))};
    raw.append(buffer, received);

    // On first received
    if (remaining == INT_MAX) {
      HttpResponse temp{raw};
      // There won't be any content to handle
      if (temp.code == "304") {
        break;
      } else if (temp.code == "200") {
        remaining = stoi(temp.headers["Content-Length"]);
        remaining -= temp.body.size();
        continue;
      } else {
        cout << "[ProxyClient] Received unexpected HttpResponse code "
             << temp.code << " with phrase " << temp.phrase << endl;
        break;
      }
    }

    remaining -= received;
  }

  // Close socket
  if (close(sockfd)) {
    cerr << "[ProxyClient] Failed to close socket" << endl;
  } else {
    cout << "[ProxyClient] Closed socket" << endl;
  }
  cout << "[ProxyClient] Received " << raw.size() << " bytes in response"
       << endl;

  HttpResponse response{raw};

  // Alter stuff
  Faker f{};
  f.alter_images(response);
  f.alter_keywords(response);

  return response;
}
