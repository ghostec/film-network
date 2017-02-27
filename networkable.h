#ifndef NETWORK_NETWORKABLEH
#define NETWORK_NETWORKABLEH

#include <uv.h>
#include "observable.h"
#include "message.h"

namespace film { namespace network {

typedef std::function<void(Message)> Observer;

template<typename Klass> 
class Networkable : public Observable<Observer, Message> {
public:
  Networkable();
  ~Networkable() {};
  virtual void start(const char* ip, int port) = 0;
  uv_loop_t* get_loop();
  void set_loop(uv_loop_t* loop);
protected: 
  uv_loop_t* loop;

  void notify_observers(Message message);
  static void delete_buf_cb(uv_handle_t* handle, void* ptr);
  static void alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
  static void msg_read_cb(uv_stream_t *handle, void *msg, int size);
};

template<typename Klass>
Networkable<Klass>::Networkable() {
  loop = new uv_loop_t();
  uv_loop_init(loop);
}

template<typename Klass>
uv_loop_t* Networkable<Klass>::get_loop() {
  return loop;
}

template<typename Klass>
void Networkable<Klass>::set_loop(uv_loop_t* loop) {
  this->loop = loop;
}

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
  delete (char*) ptr;
}

template<typename Klass>
void Networkable<Klass>::msg_read_cb(uv_stream_t *handle, void *msg, int size) {
  if (size <= 0) return;
  auto buf = new char[size];
  memcpy(buf, (char*) msg, size * sizeof(char));
  ((Klass*)handle->data)->notify_observers({
    .handle = handle, .data = buf, .length = (size_t) size
  });
}

template<typename Klass>
void Networkable<Klass>::alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  *buf = uv_buf_init(new char[suggested_size], suggested_size);
}

} }

#endif
