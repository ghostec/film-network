#include <uv.h>
#include <uv_msg_framing.h>
#include <mutex>
#include <iostream>
#include "write.h"

namespace film { namespace network {

void write(Message message) {
  static std::mutex mutex;

  uv_buf_t* buf = new uv_buf_t();
  buf->len = message.length;
  buf->base = new char[buf->len];
  memcpy(buf->base, message.data, buf->len);

  uv_msg_write_t* msg_req = new uv_msg_write_t();
  msg_req->data = (void*) buf;

  std::lock_guard<std::mutex> lock(mutex);
  uv_msg_send(msg_req, message.handle, buf->base, buf->len,
    [](uv_write_t* req, int status) -> void {
      auto helper = (uv_buf_t*) req->data;
      if(req->data) delete (uv_buf_t*) req->data;
      if(req) delete req;
  });
}

} }
