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
  if ((status = getaddrinfo(req.headers["Host"].c_str(), "80", &hints, &res))) {
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
  int received_cum{};
  string raw{};
  char buffer[8192];
  int received{};
  while ((received = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
    raw.append(buffer, received);

    // On first response, check for response code
    if (received_cum == 0) {
      HttpResponse temp{raw};
      cout << temp.code << endl;
      // There won't be any content to handle
      // recv(...) will block until timeout or the connection is closed
      if (temp.code == "304") {
        received_cum = received;
        break;
      } else if (temp.code != "200") {
        received_cum = received;
        cout << "Received unexpected HttpResponse code " << temp.code
             << " with phrase " << temp.phrase << endl;
        break;
      }
    }

    received_cum += received;
  }

  // Close socket
  if (close(sockfd)) {
    cerr << "[ProxyClient] Failed to close socket" << endl;
  } else {
    cout << "[ProxyClient] Closed socket" << endl;
  }
  cout << "[ProxyClient] Received " << received_cum << " bytes in response"
       << endl;

  HttpResponse response{raw};

  // Alter stuff
  Faker f{};
  f.alter_images(response);
  f.alter_keywords(response);

  return response;
}
