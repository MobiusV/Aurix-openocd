#ifndef TAS_SOCK_H
#define TAS_SOCK_H

#include <stddef.h>

#ifdef _WIN32
#include <winsock2.h>
#include <process.h> /* for _getpid */
#include <windows.h> /* for GetUserName */
#include <ws2tcpip.h> /* for inet_pton */
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#endif

/* General success/error codes */
#define TAS_SOCK_SUCCESS 0
#define TAS_SOCK_ERROR  -1

/* Default fallback username */
#define TAS_DEFAULT_LOGIN "openocd_user"

#ifdef __cplusplus
extern "C" {
#endif

int tas_sock_init(void);
int tas_sock_cleanup(void);

#ifdef _WIN32
typedef SOCKET tas_sock_t;
#else
typedef int tas_sock_t;
#define INVALID_SOCKET (-1)
#endif

tas_sock_t tas_sock_create(void);
int tas_sock_connect(tas_sock_t sock, const char *ip, int port);
int tas_sock_close(tas_sock_t sock);
int tas_sock_send(tas_sock_t sock, const void *buf, size_t len, int flags);
int tas_sock_recv(tas_sock_t sock, void *buf, size_t len, int flags);
int tas_sock_get_last_error(void);

int tas_get_pid(void);
int tas_get_login(char *name, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* TAS_SOCK_H */