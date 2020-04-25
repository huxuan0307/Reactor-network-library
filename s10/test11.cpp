#include "net/TcpConnection.h"
#include "net/TcpServer.h"

#include <iostream>

using namespace std;

string message;

void onConnection(const shared_ptr<TcpConnection>& conn) 
{
    if (conn->connected()) {
        cout << "onConnection(): tid=" << this_thread::get_id()
             << " new connection [" << conn->name() << "] from "
             << conn->peerAddr().toIpPort() << endl;
        auto m = message;
        conn->send(move(m));
    } else {
        cout << "onConnection(): tid=" << this_thread::get_id()
             << " connection [" << conn->name() << "] is down "<<endl;
    }
}

void onMessage(const shared_ptr<TcpConnection>& conn,
               RingBuffer* buf,
               Timestamp receiveTime)
{
    cout << "onMessage(): tid=" << this_thread::get_id() << " received "
         << buf->readableLength() << " bytes from [" << conn->name() << "] at "
         << receiveTime.toFormatString() << endl;
    buf->retrieveAll();
}

void onWriteComplete(const shared_ptr<TcpConnection>& conn){
    auto m = message;
    conn->send(move(m));
}

void Usage(char* name)
{
    puts("Usage: ");
    cout<<name;
    puts("threadNum(0~8)");
}

int main(int argc, char**argv)
{
    cout << "main(): pid = " << getpid() << endl;
    MessageLogger::setLogFile(argv[0]);
    MessageLogger::setLogPostfix("-main");
    if(argc!=2){
        Usage(argv[0]);
        return -1;
    }

    message.resize(20);
    std::string line;
    for (int i = 33; i < 127; ++i) {
        line.push_back(char(i));
    }
    line += line;

    for (size_t i = 0; i < 127 - 33; ++i) {
        message += line.substr(i, 72) + '\n';
    }
    InetAddress listenAddr(50000);
    shared_ptr<EventLoop> loop{new EventLoop{}};
    TcpServer server(loop, listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.setWriteCompleteCallback(onWriteComplete);
    server.startWithThreadNum(atoi(argv[1]));
    loop->loop();
}
