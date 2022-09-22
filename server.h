#pragma once
class ProxyServer {
private:    
    int port;
    int sockfd;
public:
    int setup(int port);
    void start();
};