#include <iostream>

#include "../network/net/TcpConnection.h"
#include "../network/net/TcpServer.h"
using namespace std;

void onConnection(const shared_ptr<TcpConnection>& conn) {
    if (conn->connected()) {
        cout << "onConnection(): receive a new connection from "
             << conn->peerAddr().toIpPort() << endl;
    } else if (conn->disconnected()) {
        cout << "onConnection(): connection [" << conn->name() << "] is down"
             << endl;
    } else {
        cout << "something unexpected happenned" << endl;
    }
}

void onMessage(const shared_ptr<TcpConnection>& conn, const char* data,
               ssize_t len) {
    cout << "onMessage(): received" << len << " bytes from "
         << conn->peerAddr().toIpPort() << endl;
    cout << "received data: " << string(data, len) << endl;
}

int main() {
    shared_ptr<EventLoop> loop{new EventLoop{}};
    InetAddress addr(50000);
    TcpServer server(loop, addr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();

    loop->loop();
}