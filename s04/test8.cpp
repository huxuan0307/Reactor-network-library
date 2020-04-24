#include <unistd.h>

#include <iostream>
#include <memory>
#include <sys/time.h>
#include "../network/net/Acceptor.h"
#include "../network/net/EventLoop.h"
#include "../network/net/InetAddress.h"
#include "../network/base/Timestamp.h"
using namespace std;

void datetime(int sockfd, const InetAddress& peeraddr) {
    cout << "datetime(): accept a new connection from: "
         << peeraddr.toIpPort() << endl;
    
    auto t = Timestamp::now().toFormatString();
    ::write(sockfd, t.data(), t.size());
}

void usage(const char* title){
    puts("Usage: \n");
    printf("%s port\n", title);
}

int main(int argc, char** argv)
{
    InetAddress inetaddr50000(50000);
    shared_ptr<EventLoop> loop{new EventLoop{}};
    loop->init();
    Acceptor accpetor50000(loop, inetaddr50000);
    accpetor50000.setNewConnecttionCallback(datetime);
    accpetor50000.listen();
    loop->loop();
}