/**
 * cs_tcp.h
 * Copyright (c) 2007-2018 ls
 **/
#include "../inc/cs.h"
#include "../inc/cs_str.h"
#include "../inc/cs_tcp.h"

#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")

//#define EINPROGRESS WSAEWOULDBLOCK

CS_API int cs_tcp_init() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	return (CS_OK);
}

CS_API void cs_tcp_cleanup() {
	WSACleanup();
}
#endif

static int cs_tcp_v6_only(cs_sock_t s) {
  int yes = 1;
  if (setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &yes, sizeof(yes)) == -1) {
    cs_tcp_close(s);

    return (CS_ERR);
  }

  return (CS_OK);
}

static int cs_tcp_reuseaddr(cs_sock_t fd) {
  int yes = 1;
  /* Make sure connection-intensive things like the redis benckmark
   * will be able to close/open sockets a zillion of times */
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    // cs_strerror(err, "setsockopt SO_REUSEADDR: %s", strerror(errno));
    return (CS_ERR);
  }

  return (CS_OK);
}

static int _cs_tcp_listen(
	cs_sock_t s, struct sockaddr *sa, socklen_t len, int backlog) {
  if (bind(s,sa,len) == -1) {
    // cs_strerror(err, "bind: %s", strerror(errno));
    cs_tcp_close(s);

    return (CS_ERR);
  }

  if (listen(s, backlog) == -1) {
    // cs_strerror(err, "listen: %s", strerror(errno));
    cs_tcp_close(s);

    return (CS_ERR);
  }

  return (CS_OK);
}

static cs_sock_t _cs_tcp_server(
	int port, char *bindaddr, int af, int backlog) {
  cs_sock_t s = -1;
	int rv;
  char _port[6];  /* strlen("65535") */
  struct addrinfo hints, *servinfo, *p;

#ifdef _WIN32
  sprintf_s(_port, 6, "%d", port);
#else
  snprintf(_port, 6, "%d", port);
#endif

  cs_bzero(&hints, sizeof(hints));
  hints.ai_family = af;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;    /* No effect if bindaddr != NULL */

  if ((rv = getaddrinfo(bindaddr, _port, &hints, &servinfo)) != 0) {
    // cs_strerror(err, "%s", gai_strerror(rv));
    return (CS_ERR);
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
    #ifdef _WIN32
	  if ((s = WSASocket(
      p->ai_family, 
      p->ai_socktype, 
      p->ai_protocol, 
      NULL, 
      0, 
      WSA_FLAG_OVERLAPPED)) == -1) {
		  continue;
	  }
    #else
    if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			continue;
		}
    #endif 

    if (af == AF_INET6 && cs_tcp_v6_only(s) == CS_ERR) {
			goto error;
		}

    if (cs_tcp_reuseaddr(s) == CS_ERR) {
			goto error;
		}

    if (_cs_tcp_listen(s, p->ai_addr, p->ai_addrlen, backlog) == CS_ERR) {
			goto error;
		}
    
    goto end;
  }

  if (p == NULL) {
    // cs_strerror(err, "unable to bind socket, errno: %d", errno);
    goto error;
  }

error:
    if (s != -1) {
			cs_tcp_close(s);
		}
    s = CS_ERR;
end:
    freeaddrinfo(servinfo);

    return (s);
}

/*
static cs_sock_t cs_tcp_v4_create() {
	cs_sock_t s;
	int flag = 1;
	
#ifdef _WIN32
	if ((s = WSASocket(
    AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == -1) {
		return (CS_ERR);
	}
#else
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		return (CS_ERR);
	}
#endif
	if (setsockopt(
    s, SOL_SOCKET, SO_REUSEADDR, (void *)&flag, sizeof(flag)) == 1) {
		return (CS_ERR);
	}
	
	return (s);
}

static int cs_tcp_v4_listen(
	cs_sock_t s, struct sockaddr *sa, unsigned int len) {
	if (bind(s, sa, len) == -1) {
		cs_tcp_close(s);

		return (CS_ERR);
	}
	if (listen(s, 511) == -1) { // 511 is from nginx
		cs_tcp_close(s);

		return (CS_ERR);
	}

	return (CS_OK);
}
// */

CS_API cs_sock_t cs_tcp_v4_server(int port, char *bindaddr) {
	return _cs_tcp_server(port, bindaddr, AF_INET, 511);
}

CS_API cs_sock_t cs_tcp_v6_server(int port, char *bindaddr) {
	return _cs_tcp_server(port, bindaddr, AF_INET6, 511);
}

static cs_inline cs_sock_t _cs_tcp_generic_accept(
  cs_sock_t s, struct sockaddr *sa, unsigned int *len) {
	cs_sock_t fd;

	while (1) {
		fd = accept(s, sa, len);
		if (fd == -1) {
			if (cs_sock_errno == EINTR) {
				continue;
			} else {
				// cs_strerror(err, "accept: %s", strerror(errno));
				return (CS_ERR);
			}
		}
		break;
	}

	return (fd);
}

CS_API cs_sock_t cs_tcp_accept(
	cs_sock_t s, u_char *ip, size_t ip_len, int *port) {
	cs_sock_t fd;
  struct sockaddr_storage sa;
  socklen_t salen = sizeof(sa);

  if ((fd = _cs_tcp_generic_accept(s, (struct sockaddr*)&sa, &salen)) == -1) {
    return CS_ERR;
	}

  if (sa.ss_family == AF_INET) {
    struct sockaddr_in *s = (struct sockaddr_in *)&sa;
    if (ip) {
			inet_ntop(AF_INET, (void*)&(s->sin_addr), (char *)ip, ip_len);
		}
    if (port) {
			*port = ntohs(s->sin_port);
		}
  } else {
    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&sa;
    if (ip) {
			inet_ntop(AF_INET6, (void*)&(s->sin6_addr), (char *)ip, ip_len);
		}
    if (port) {
			*port = ntohs(s->sin6_port);
		}
  }

  return (fd);
}

static int _cs_tcp_nodelay(cs_sock_t fd, int yes) {
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&yes, sizeof(yes)) 
    == -1) {
		// cs_strerror(err, "setsockopt TCP_NODELAY: %s", strerror(errno));
		return (CS_ERR);
	}

	return (CS_OK);
}

CS_API int cs_tcp_enable_nodelay(cs_sock_t fd) {
	return _cs_tcp_nodelay(fd, 1);
}

CS_API int cs_tcp_disable_nodelay(cs_sock_t fd) {
	return _cs_tcp_nodelay(fd, 0);
}

CS_API int cs_tcp_keepalive(cs_sock_t fd) {
	int yes = 1;
	
	if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&yes, sizeof(yes)) 
    == -1) {
		return (CS_ERR);
	}
	
	return (CS_OK);
}

/* Set the socket send timeout (SO_SNDTIMEO socket option) to the specified
 * number of milliseconds, or disable it if the 'ms' argument is zero. */
CS_API int cs_tcp_send_timeout(cs_sock_t fd, long long ms) {
  struct timeval tv;

  tv.tv_sec = ms / 1000;
  tv.tv_usec = (ms % 1000) * 1000;
  if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == -1) {
    // cs_strerror(err, "setsockopt SO_SNDTIMEO: %s", strerror(errno));
    return (CS_ERR);
  }

  return (CS_OK);
}

CS_API int cs_tcp_recv_timeout(cs_sock_t fd, long long ms) {
  struct timeval tv;

  tv.tv_sec = ms / 1000;
  tv.tv_usec = (ms % 1000) * 1000;
  if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
    // cs_strerror(err, "setsockopt SO_RCVTIMEO: %s", strerror(errno));
    return (CS_ERR);
  }

  return (CS_OK);
}

static int _cs_tcp_blocking(cs_sock_t fd, int yes) {
  int flags;

  /* Set the socket blocking (if yes is zero) or non-blocking.
   * Note that fcntl(2) for F_GETFL and F_SETFL can't be
   * interrupted by a signal. */
#ifdef _WIN32
	u_long arg = yes;
	if (ioctlsocket(fd, FIONBIO, &arg) == SOCKET_ERROR){		
		return (CS_ERR);
	}
#else
  if ((flags = fcntl(fd, F_GETFL)) == -1) {
    // cs_strerror(err, "fcntl(F_GETFL): %s", strerror(errno));
    return (CS_ERR);
  }

  if (yes) {
    flags |= O_NONBLOCK;
	} else {
		flags &= ~O_NONBLOCK;
	}
  if (fcntl(fd, F_SETFL, flags) == -1) {
    // cs_strerror(err, "fcntl(F_SETFL,O_NONBLOCK): %s", strerror(errno));
    return (CS_ERR);
  }
#endif

  return (CS_OK);
}

CS_API int cs_tcp_nonblocking(cs_sock_t fd) {
	return _cs_tcp_blocking(fd, 1);
}

CS_API int cs_tcp_blocking(cs_sock_t fd) {
	return _cs_tcp_blocking(fd, 0);
}

CS_API int cs_tcp_close(cs_sock_t fd) {
#ifdef _WIN32
	shutdown(fd, SD_BOTH);
	closesocket(fd);
#else
	shutdown(fd, SHUT_RDWR);
	close(fd);	
#endif

	return (CS_OK);
}