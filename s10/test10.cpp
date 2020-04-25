#include "net/TcpConnection.h"
#include "net/TcpServer.h"

#include <iostream>

using namespace std;

string message1;
string message2;

void onConnection(const shared_ptr<TcpConnection>& conn) 
{
    if (conn->connected()) {
        cout << "onConnection(): tid=" << this_thread::get_id()
             << " new connection [" << conn->name() << "] from "
             << conn->peerAddr().toIpPort() << endl;
        if(!message1.empty()){
            auto m = message1;
            conn->send(move(message1));
        }
        if(!message2.empty()){
            auto m = message2;
            conn->send(move(message2));
        }
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

void Usage(char* name)
{
    puts("Usage: ");
    cout<<name;
    puts("threadNum(0~8)");
}

int main(int argc, char**argv)
{
    MessageLogger::setLogFile(argv[0]);
    MessageLogger::setLogPostfix("-main");
    if(argc!=2){
        Usage(argv[0]);
        return -1;
    }

    message1.resize(20);
    message2.resize(10);
    std::fill(message1.begin(), message1.end(), 'A');
    std::fill(message2.begin(), message2.end(), 'B');

    cout << "main(): pid = " << getpid() << endl;
    InetAddress listenAddr(50000);
    shared_ptr<EventLoop> loop{new EventLoop{}};
    TcpServer server(loop, listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.startWithThreadNum(atoi(argv[1]));
    loop->loop();
}
