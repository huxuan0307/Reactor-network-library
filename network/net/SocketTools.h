#ifndef _SOCKETTOOLS_H_
#define _SOCKETTOOLS_H_
#include <arpa/inet.h>
#include <boost/implicit_cast.hpp>
#include <string> 

// #define SOMAXCONN 16384
namespace sockets
{
// all

// @close_on_exec : safe for subprocess exec a new program before close this socket
int createNonblockingSocket(sa_family_t family, bool close_on_exec=true);

void shutdownWrite(int sockfd);
void close(int sockfd);

// server
// bind
int bind(int sockfd, const sockaddr* addr);
void bindOrDie(int sockfd, const sockaddr* addr);
int listen(int sockfd);
void listenOrDie(int sockfd);
// accept new connection and set nonblocking 
// @addr use sockaddr_in6 to meet sockaddr_in&sockaddr_in
int accept(int sockfd, sockaddr_in6* addr);

// client
int connect(int sockfd, const sockaddr* addr);

// tools

// struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr);
// struct sockaddr* sockaddr_cast(struct sockaddr_in* addr);
// const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);

template<typename From, typename To>
struct sockaddr_cast_help
{
	constexpr static To* cast (From* x)
	{
		return static_cast<To*>(static_cast<void*>(x));
	}
};

template<typename To, typename From>
To sockaddr_cast (From* x)
{
	using std::remove_cv;
	using std::remove_const;
	using std::remove_pointer;
	using std::is_same;
	using std::add_pointer;

	using _from = typename remove_cv<From>::type;
	using _to = typename remove_const <typename remove_pointer<To>::type>::type;
	static_assert(is_same<_from, sockaddr>::value || is_same<_from, sockaddr_in>::value || is_same<_from, sockaddr_in6>::value,"sockaddr_cast invalid argument");
	static_assert(is_same<_to, sockaddr>::value || is_same<_to, sockaddr_in>::value || is_same<_to, sockaddr_in6>::value, "sockaddr_cast invalid template argument");
	return sockaddr_cast_help<typename remove_cv<From>::type, typename remove_pointer<To>::type>::cast (
		const_cast<typename add_pointer<_from>::type>(x));
}

// to network BIG_ENDIAN
inline uint16_t hostToNet16(uint16_t x){return htobe16(x);}
inline uint32_t hostToNet32(uint32_t x){return htobe32(x);}
inline uint64_t hostToNet32(uint64_t x){return htobe64(x);}

// from network BID_ENDIAN
inline uint16_t netToHost16(uint16_t x){return be16toh(x);}
inline uint32_t netToHost16(uint32_t x){return be32toh(x);}
inline uint64_t netToHost16(uint64_t x){return be64toh(x);}

// ip port convert
std::string toIp(const sockaddr* addr);
std::string toIpPort(const sockaddr* addr);
std::uint16_t toPort(const sockaddr* addr);

}

#endif