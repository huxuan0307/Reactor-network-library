#include "../network/net/TcpConnector.h"
#include "../network/net/EventLoop.h"

#include <iostream>
#include <memory>

using namespace std;

shared_ptr<EventLoop> g_loop;

void onConnect(int sockfd){
    cout<<"connected"<<endl;
    g_loop->quit();
}

int main()
{
    g_loop.reset(new EventLoop{});
    InetAddress addr("127.0.0.1", 50000);
    shared_ptr<TcpConnector> conn{new TcpConnector(g_loop, addr)};
    conn->setNewConnectionCallback(onConnect);
    conn->start();
    g_loop->loop();
}
