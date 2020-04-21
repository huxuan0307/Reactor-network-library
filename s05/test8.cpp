
#include <unistd.h>

#include <iostream>

#include "TcpConnection.h"
#include "TcpServer.h"
using namespace std;

void onConnection(const shared_ptr<TcpConnection>& conn) {
    if (conn->connected()) {
        cout << "onConnection(): new connection [" << conn->name() << "] from "
             << conn->peerAddr().toIpPort() << endl;
    } else {
        cout << "onConnection(): connection [" << conn->name() << "] is down "
             << endl;
    }
}

void onMessage(const shared_ptr<TcpConnection>& conn, const char* data,
               ssize_t len) {
    cout << "onMessage(): recieved " << len << " bytes from connection ["
         << conn->peerAddr().toIpPort() << "]" << endl;
}

int main() {
    cout << "main(): pid = " << getpid() << endl;
    InetAddress listenAddr(50000);
    shared_ptr<EventLoop> loop{new EventLoop{}};
    TcpServer server(loop, listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();

    loop->loop();
}