#include <uv.h>
#include <uv_msg_framing.h>
#include <thread>
#include <cstdio>
#include <cstdlib>
#include "server.h"
#include "write.h"

namespace film { namespace network {

Server::Server() : Networkable<Server>() {}
Server::~Server() {}

void Server::start(const char* ip, int port) {
  std::thread t(&Server::_start, this, ip, port);
  t.detach();
}

void Server::_start(const char* ip, int port) {
  auto loop = get_loop();

  uv_tcp_t server;
  uv_tcp_init(loop, &server);

  sockaddr_in addr;
  uv_ip4_addr(ip, port, &addr);

  uv_tcp_bind(&server, (const struct sockaddr*) &addr, 0);
  server.data = this;
  int r = uv_listen((uv_stream_t*) &server, SOMAXCONN, connection_cb);
  if (r) {
    fprintf(stderr, "Listen error %s\n", uv_strerror(r));
    abort();
  }

  uv_run(loop, UV_RUN_DEFAULT);
}

void Server::connection_cb(uv_stream_t *server, int status) {
  if (status < 0) {
    fprintf(stderr, "New connection error %s\n", uv_strerror(status));
    abort();
  }

  auto loop = ((Server*) server->data)->get_loop();
  uv_msg_t* client = new uv_msg_t();
  uv_msg_init(loop, client, UV_TCP);
  if (uv_accept(server, (uv_stream_t*) client) == 0) {
    client->data = server->data;
    uv_msg_read_start((uv_msg_t*) client, alloc_cb, msg_read_cb, delete_buf_cb);
    write({
      .handle = (uv_stream_t*) client,
      .data = CONNECTED_MESSAGE,
      .length = strlen(CONNECTED_MESSAGE)
    });
  }
  else {
    uv_close((uv_handle_t*) client, NULL);
  }
}

} }
