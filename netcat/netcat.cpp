#include <thread>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int write_n (int fd, const void* buf, int length)
{
    int written=0;
    while (written<length)
    {
        int nw = ::write(fd, reinterpret_cast<const char*>(buf) + written, length - written);
        if(nw>0){
            written += nw;
        }else if(nw==0){
            break;  // eof?
        }
        // nw == -1 
        else if(errno != EINTR){
            // if not be interupted,
            perror("write");
            break;
        }
    }
    
}

void run()
{
    
    char buf[8092];
    int nr = 0;

}