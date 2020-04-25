
#include <chrono>
#include <iostream>
#include <memory>

#include "net/TcpConnection.h"
#include "net/TcpServer.h"

using namespace std;
IgnoreSigpipe initObj;
string message;
void onConnection(const shared_ptr<TcpConnection>& conn)
{
    if(conn->connected()){
        cout<<Timestamp::now().toFormatString()<<" onConnection(): new connection ["<<conn->name()<<"] from"<<conn->peerAddr().toIpPort()<<endl;
        auto m = message;
        conn->send(move(m));
    }
    else if(conn->disconnected()){
        cout<<Timestamp::now().toFormatString()<<" onConnection(): connection is down ["<<conn->name()<<"]"<<endl;
    }else{
        cout<<conn->stateToString()<<endl;
        assert(false);
    }
}

void onWriteComplete(const shared_ptr<TcpConnection>& conn){
    this_thread::sleep_for(chrono::milliseconds(50));
    auto m = message;
    conn->send(move(m));
}

void onMessage(const shared_ptr<TcpConnection>& conn, RingBuffer* buf, Timestamp t){
    cout << Timestamp::now().toFormatString() << " onMessage(): received "
         << buf->readableLength() << " bytes from [" << conn->name() << "] at "
         << t.toFormatString() << endl;
    buf->retrieveAll();
}

int main()
{
    MessageLogger::setLogFile("./test11.log");
    cout<<"main(): pid = "<<getpid()<<endl;
    string line;
    for (int i = 33; i < 127; ++i) {
        line.push_back(char(i));
    }
    line += line;

    for (size_t i = 0; i < 127 - 33; ++i) {
        message += line.substr(i, 72) + '\n';
    }

    InetAddress addr(50000);
    shared_ptr<EventLoop> loop{new EventLoop{}};
    TcpServer server (loop, addr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.setWriteCompleteCallback(onWriteComplete);
    server.start();
    loop->loop();

}