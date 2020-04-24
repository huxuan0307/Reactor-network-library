

#include <iostream>
#include <memory>
#include <chrono>
#include "../network/net/TcpConnection.h"
#include "../network/net/TcpServer.h"
#include "../network/base/Timestamp.h"

using namespace std;



void onConnection(const shared_ptr< TcpConnection>& conn){
    if(conn->connected()){
        cout << Timestamp::now().toFormatString()
             << " onConnection(): new connection[" << conn->name()
             << "] from: " << conn->peerAddr().toIpPort() << endl;
        this_thread::sleep_for(chrono::seconds(5));
        conn->send("welcome!!!\n");
        conn->send(string(10000, '6'));
        conn->shutdown();

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