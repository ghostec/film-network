#include <uv_msg_framing.h>
#include "client.h"

namespace film { namespace network {

Client::Client() {}
Client::~Client() {}

int Client::start() {
  auto loop = uv_default_loop();

  uv_msg_t socket;
  uv_msg_init(loop, &socket, UV_TCP);
  uv_tcp_keepalive((uv_tcp_t*) &socket, 1, 60);

  sockaddr_in dest;
  uv_ip4_addr("0.0.0.0", 3000, &dest);

  uv_connect_t connect;
  connect.data = this;
  uv_tcp_connect(&connect, (uv_tcp_t*) &socket, (const struct sockaddr*) &dest, connection_cb);

  return uv_run(loop, UV_RUN_DEFAULT);
}

void Client::connection_cb(uv_connect_t* connection, int status)
{
  uv_stream_t* stream = connection->handle;
  stream->data = connection->data;
  uv_msg_read_start((uv_msg_t*) stream, alloc_cb, msg_read_cb, delete_buf_cb);
}

} }
