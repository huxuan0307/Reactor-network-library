#include "net/TcpConnection.h"
#include "net/TcpServer.h"

#include <iostream>

using namespace std;

void onConnection(const shared_ptr<TcpConnection>& conn) 
{
    if (conn->connected()) {
        cout << "onConnection(): tid=" << this_thread::get_id()
             << " new connection [" << conn->name() << "] from "
             << conn->peerAddr().toIpPort() << endl;
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
    cout << "onMessage(): \n" << buf->retrieveAllAsString() << endl;
}

void Usage(char* name)
{
    puts("Usage: ");
    cout<<name;
    puts("threadNum(0~8)");
}

int main(int argc, char**argv)
{
    MessageLogger::setLogFile("./s10_test8");
    MessageLogger::setLogPostfix("-main");
    if(argc!=2){
        Usage(argv[0]);
        return -1;
    }
    cout << "main(): pid = " << getpid() << endl;
    InetAddress listenAddr(50000);
    shared_ptr<EventLoop> loop{new EventLoop{}};
    TcpServer server(loop, listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    
    server.setThreadNum(atoi(argv[1]));
    server.start();
    loop->loop();
}
