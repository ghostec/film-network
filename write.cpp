#include <uv.h>
#include <uv_msg_framing.h>
#include <mutex>
#include "write.h"

namespace film { namespace network {

void write(Message message) {
  static std::mutex mutex;

  uv_buf_t* buf = new uv_buf_t();
  buf->len = message.length;
  buf->base = new char[buf->len + 1];
  memcpy(buf->base, message.data, buf->len);

  struct BufMutex { uv_buf_t* buf; std::mutex* mutex; };
  uv_write_t req;
  req.handle = message.handle;
  req.data = (void*) new BufMutex({ .buf = buf, .mutex = &mutex });

  uv_msg_write_t* msg_req = new uv_msg_write_t();
  msg_req->req = req;

  mutex.lock();
  uv_msg_send(msg_req, msg_req->req.handle, buf->base, buf->len,
    [](uv_write_t* req, int status) -> void {
      if(req && req->data) {
        auto helper = (BufMutex*) req->data;
        helper->mutex->unlock();
        delete helper->buf;
        delete helper;
      }
      if(req) delete req;
  });
}

} }
