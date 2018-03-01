/**
 * cs_tcp.h
 * Copyright (c) 2007-2018 ls
 **/
#ifndef _CS_TCP_H_INCLUDED_
#define _CS_TCP_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#define CS_TCP_CONNECT_NONE     0x0
#define CS_TCP_CONNECT_NONBLOCK 0x01

#ifdef _WIN32
CS_API int       cs_tcp_init();
CS_API void      cs_tcp_cleanup();
#endif

CS_API cs_sock_t cs_tcp_v4_server(int port, char *bindaddr);
CS_API cs_sock_t cs_tcp_v6_server(int port, char *bindaddr);
CS_API cs_sock_t cs_tcp_accept(cs_sock_t s, u_char *ip, size_t ip_len, int *port);

CS_API int       cs_tcp_enable_nodelay(cs_sock_t fd);
CS_API int       cs_tcp_disable_nodelay(cs_sock_t fd);
CS_API int       cs_tcp_keepalive(cs_sock_t fd);
CS_API int       cs_tcp_send_timeout(cs_sock_t fd, long long ms);
CS_API int       cs_tcp_recv_timeout(cs_sock_t fd, long long ms);
CS_API int       cs_tcp_nonblocking(cs_sock_t fd);
CS_API int       cs_tcp_blocking(cs_sock_t fd);

CS_API int       cs_tcp_close(cs_sock_t fd);

#ifdef __cplusplus
}
#endif /* C++ */
#endif