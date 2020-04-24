

#include <iostream>
#include <memory>

#include "TcpConnection.h"
#include "TcpServer.h"
#include "Timestamp.h"

using namespace std;



void onConnection(const shared_ptr< TcpConnection>& conn){
    if(conn->connected()){
        cout << Timestamp::now().toFormatString()
             << " onConnection(): new connection[" << conn->name()
             << "] from: " << conn->peerAddr().toIpPort() << endl;

    }else if(conn->disconnected()){
        cout << Timestamp::now().toFormatString()
             << " onConnection(): connection[" << conn->name() << "] is down"
             << endl;
    }
}

void onMessage(const shared_ptr<TcpConnection>& conn, RingBuffer* buffer, Timestamp t){
    cout << Timestamp::now().toFormatString()
        << " onMessage(): new message[" << conn->name()
        << "] from: " << conn->peerAddr().toIpPort() << endl;
    cout << "message length: " << buffer->readableLength() << endl
         << buffer->retrieveAllAsString() << endl;
    conn->send("welcome!!!\n");
    conn->send(string(10000,'6'));
    conn->shutdown();
}

int main()
{
    InetAddress addr(50000);
    shared_ptr<EventLoop> loop{new EventLoop{}};
    TcpServer server(loop, addr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();
    loop->loop();
}