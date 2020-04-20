#include "EventLoop.h"
#include "EventLoopThread.h"
#include "TimerId.h"
#include <iostream>
#include <unistd.h>

using namespace std;

void runInThread(const char* name)
{
    std::cout << name << "pid = " << getpid()
              << ", tid = " << this_thread::get_id() << endl;
}

int main()
{
    runInThread("main(): ");
    EventLoopThread loopThread;
    cout<<"here"<<endl;
    auto loop = loopThread.startLoop();
    cout<<"here"<<endl;
    loop->runInLoop(bind(&runInThread, "runInThread 1"));
    sleep(1);
    loop->run_after(2, bind(&runInThread, "runInThread 2"));
    sleep(3);
    loop->quit();
}