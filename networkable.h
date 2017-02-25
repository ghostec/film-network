#ifndef NETWORK_NETWORKABLEH
#define NETWORK_NETWORKABLEH

#include <uv.h>
#include <functional>
#include "observable.h"
#include "message.h"

namespace film { namespace network {

typedef std::function<void(Message)> Observer;

template<typename Klass> 
class Networkable : public Observable<Observer, Message> {
public:
  Networkable() {};
  ~Networkable() {};
  virtual int start() = 0;
protected: 
  void notify_observers(Message message);
  static void delete_buf_cb(uv_handle_t* handle, void* ptr);
  static void alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
  static void msg_read_cb(uv_stream_t *handle, void *msg, int size);
};

template<typename Klass>
void Networkable<Klass>::notify_observers(Message message) {
  auto loop = uv_default_loop();

  struct ObserverMessage { Observer observer; Message message; };

  for(const auto& observer : observers) {
    uv_work_t* req = new uv_work_t();

    req->data = (void*) new ObserverMessage {
      .observer = observer,
      .message = message
    };

    uv_queue_work(loop, req,
      [](uv_work_t* req) -> void {
        auto om = (ObserverMessage*) req->data;
        om->observer(om->message);
      },
      [](uv_work_t* req, int status) -> void {
        delete (ObserverMessage*) req->data;
        delete req;
    });
  }
}

template<typename Klass>
void Networkable<Klass>::delete_buf_cb(uv_handle_t* handle, void* ptr) {
   delete ptr;
}

template<typename Klass>
void Networkable<Klass>::msg_read_cb(uv_stream_t *handle, void *msg, int size) {
  if (size <= 0) return;
  ((Klass*)handle->data)->notify_observers({
    .handle = handle, .data = (char*) msg, .length = (size_t) size
  });
}

template<typename Klass>
void Networkable<Klass>::alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  *buf = uv_buf_init(new char[suggested_size], suggested_size);
}

} }

#endif
