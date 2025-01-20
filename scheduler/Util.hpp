#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <utility>
#include <String.hpp>

static constexpr unsigned int MAX_MESSAGE_SIZE = 256;

inline std::pair<String, void*> getInAddr(struct addrinfo* sa) {
    if (sa->ai_family == AF_INET) {
        struct sockaddr_in* res = (struct sockaddr_in*) sa->ai_addr;
        return {"IPv4", &res->sin_addr};
    }
    struct sockaddr_in6* res = (struct sockaddr_in6*) sa->ai_addr;
    return {"IPv6", &res->sin6_addr};
}
