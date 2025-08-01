#include "tas_sock.h"
#include <string.h>

int tas_sock_init(void)
{
#ifdef _WIN32

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return TAS_SOCK_ERROR;
	}
#endif
	return TAS_SOCK_SUCCESS;
}

int tas_sock_cleanup(void)
{
#ifdef _WIN32
	if (WSACleanup() != 0) {
		return TAS_SOCK_ERROR;
	}
#endif
	return TAS_SOCK_SUCCESS;
}

tas_sock_t tas_sock_create(void)
{
	return socket(AF_INET, SOCK_STREAM, 0);
}

int tas_sock_connect(tas_sock_t sock, const char *ip, int port)
{
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

#ifdef _WIN32
	if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
		return TAS_SOCK_ERROR;
	}
#else
	if (inet_aton(ip, &serv_addr.sin_addr) == 0) {
        return TAS_SOCK_ERROR;
    }
#endif


	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		return TAS_SOCK_ERROR;
	}

	return TAS_SOCK_SUCCESS;
}

int tas_sock_close(tas_sock_t sock)
{
	int status;
#ifdef _WIN32
	status = closesocket(sock);
#else
	status = close(sock);
#endif
	return status == 0 ? TAS_SOCK_SUCCESS : TAS_SOCK_ERROR;
}

int tas_sock_send(tas_sock_t sock, const void *buf, size_t len, int flags)
{
	return send(sock, buf, len, flags);
}

int tas_sock_recv(tas_sock_t sock, void *buf, size_t len, int flags)
{
	return recv(sock, buf, len, flags);
}

int tas_sock_get_last_error(void)
{
#ifdef _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

int tas_get_pid(void)
{
#ifdef _WIN32
	return _getpid();
#else
	return getpid();
#endif
}

int tas_get_login(char *name, size_t len)
{
#ifdef _WIN32
	DWORD buf_len = len;
	if (GetUserName(name, &buf_len)) {
		return TAS_SOCK_SUCCESS;
	}
#else
	if (getlogin_r(name, len) == 0) {
		return TAS_SOCK_SUCCESS;
	}
#endif
	/* Fallback if platform call fails or is not supported */
	strncpy(name, TAS_DEFAULT_LOGIN, len - 1);
	name[len - 1] = '\0';
	return TAS_SOCK_SUCCESS;
}