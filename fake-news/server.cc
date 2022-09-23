#include "server.h"
#include "client.h"
#include <algorithm>
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int ProxyServer::setup(int p) {
  port = p;
  // Get IP/Port combination
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints)); // Zero struct out
  hints.ai_family = AF_INET;        // IPv4
  hints.ai_socktype = SOCK_STREAM;  // Streaming socket
  hints.ai_flags = AI_PASSIVE;      // Use local IP

  // Create addrinfo
  int status;
  struct addrinfo *res;
  if ((status = getaddrinfo(nullptr, to_string(port).c_str(), &hints, &res)) !=
      0) {
    cerr << "[ProxyServer] getaddrinfo error: " << strerror(status) << endl;
    return -1;
  }

  // Print IP/Port combination
  struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
  char ipstr[INET_ADDRSTRLEN];
  inet_ntop(res->ai_family, &(ipv4->sin_addr.s_addr), ipstr, sizeof(ipstr));
  cout << "[ProxyServer] Creating socket " << ipstr << ":"
       << htons(ipv4->sin_port) << endl;

  // Create socket
  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (sockfd == -1) {
    cout << strerror(errno) << endl;
    return -1;
  }

  // Bind socket
  status = ::bind(sockfd, res->ai_addr, res->ai_addrlen);
  if (status == -1) {
    cout << strerror(status) << endl;
    return -1;
  }
  return 0;
}

void ProxyServer::start() {
  // Begin listening
  int status = listen(sockfd, 20);
  if (status == -1) {
    cout << strerror(status) << endl;
    return;
  }
  cout << "[ProxyServer] Began listening on port " << port << endl;

  while (true) {
    cout << "---" << endl;
    // Accept connection
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);
    int conn_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
    // Todo print connection endpoint
    cout << "[ProxyServer] Accepted local connection!" << endl;

    // Receive request from browser
    string request{};
    char buffer[8192];
    int received{};
    while ((received = recv(conn_fd, &buffer, sizeof(buffer), 0)) > 0) {
      request.append(buffer);
      break; // TODO handle larger http requests
    }

    HttpRequest req{request};
    cout << "[ProxyServer] Received HttpRequest from local connection to "
         << req.url << endl;

    // if (req.headers["Host"] != "zebroid.ida.liu.se") {
    //   cout << "[ProxyServer] Not handeling requests to hosts other than "
    //           "zebroid.ida.liu.se"
    //        << endl;
    //   close(conn_fd);
    //   continue;
    // }

    ProxyClient client{};
    HttpResponse response = client.forward(req);
    cout << "[ProxyServer] Received HttpResponse from proxy " << endl;

    // Send the response back to the browser
    string raw{response.raw};
    int sent_cum{};
    do {
      int bytes_left{static_cast<int>(raw.size()) - sent_cum};
      int index = sent_cum + min(bytes_left, 8192);
      int sent{
          static_cast<int>(send(conn_fd, raw.substr(sent_cum, index).c_str(),
                                min(bytes_left, 8192), 0))};
      sent_cum += sent;
    } while (sent_cum < static_cast<int>(raw.size()));
    cout << "[ProxyServer] Sent " << sent_cum << " bytes to browser" << endl;

    if (close(conn_fd)) {
      cerr << "[ProxyServer] Failed to close local connection" << endl;
    } else {
      cout << "[ProxyServer] Closed local connection" << endl;
    }
  }
}

int main() {
  ProxyServer server{};
  if (!server.setup(2222)) {
    server.start();
  };
}
