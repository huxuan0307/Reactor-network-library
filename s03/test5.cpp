#include <stdio.h>
#include <memory>
#include "EventLoop.h"
#include "TimerId.h"
#include <unistd.h>

using namespace std;

shared_ptr<EventLoop> g_loop;
int g_flag = 0;

void run4() {
    printf("run4(): pid = %d, flag = %d\n", getpid(), g_flag);
    g_loop->quit();
}

void run3() {
    printf("run3(): pid = %d, flag = %d\n", getpid(), g_flag);
    g_loop->run_after(3, run4);
    g_flag = 3;
}

void run2() {
    printf("run2(): pid = %d, flag = %d\n", getpid(), g_flag);
    g_loop->queueInLoop(run3);
}

void run1() {
    g_flag = 1;
    printf("run1(): pid = %d, flag = %d\n", getpid(), g_flag);
    g_loop->runInLoop(run2);
    g_flag = 2;
}

int main() {
    printf("main(): pid = %d, flag = %d\n", getpid(), g_flag);
    g_loop.reset(new EventLoop{});
    g_loop->init();
    g_loop->run_after(2, run1);
    g_loop->loop();
    printf("main(): pid = %d, flag = %d\n", getpid(), g_flag);
}
