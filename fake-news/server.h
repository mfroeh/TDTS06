#pragma once
// The proxy server that intercepts HttpRequests made over the browser.
class ProxyServer {
private:
  int port;
  int sockfd;

public:
  // Creates a socket and binds it to 0.0.0.0:port
  int setup(int port);
  // Places the socket in a listening state and accepts incoming connections
  // forever. For every connection it receives and forwards HttpRequests to the
  // proxy client and finally returns the HttpResponse over the connection.
  void start();
};
