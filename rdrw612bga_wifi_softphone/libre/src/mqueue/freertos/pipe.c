/**
 * @file pipe.c Pipe-emulation for Windows
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include "lwip/sockets.h"
#include <re_types.h>
#include <re_fmt.h>
#include <re_net.h>
#include "../mqueue.h"


/*
 * Emulate pipe on FreeRTOS/LWIP -- pipe() with select() is not working
 */
int pipe(re_sock_t fds[2])
{
	int s, rd, wr;
	struct sockaddr_in serv_addr;
	socklen_t len = sizeof(serv_addr);

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return ENOSYS;

	memset((void *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(0);
	serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (bind(s, (struct sockaddr*)&serv_addr, len) < 0)
		goto error;

	if (listen(s, 1) < 0)
		goto error;

	if (getsockname(s, (struct sockaddr*)&serv_addr, &len) < 0)
		goto error;

	if ((wr = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		goto error;

	if (connect(wr, (struct sockaddr*)&serv_addr, len) < 0) {
		closesocket(wr);
		goto error;
	}

	rd = accept(s, (struct sockaddr*)&serv_addr, &len);
	if (rd < 0) {
		closesocket(wr);
		goto error;
	}

	fds[0] = rd;
	fds[1] = wr;

	closesocket(s);
	return 0;

error:
	closesocket(s);
	return ENOSYS;
}


ssize_t pipe_read(re_sock_t s, void *buf, size_t len)
{
	int ret = recv(s, buf, (int)len, 0);

	if (ret < 0) {
		int err = errno;
		if (err == ECONNRESET || err == ENOTCONN ||
		    err == EPIPE || err == ECONNABORTED) {
			ret = 0; /* EOF */
		}
	}

	return ret;
}


ssize_t pipe_write(re_sock_t s, const void *buf, size_t len)
{
	return send(s, buf, (int)len, 0);
}

