#ifndef NETWORK_SERVERH
#define NETWORK_SERVERH

#include <uv.h>
#include "networkable.h"

namespace film { namespace network {

class Server : public Networkable<Server> {
public:
  static constexpr const char* const CONNECTED_MESSAGE = "CONNECTED";
  static constexpr const char* const REGISTER_WORKER_MESSAGE = "WORKER";

  Server();
  ~Server();
  virtual void start(const char* ip, int port);
  void _start(const char* ip, int port);
private:
  static void connection_cb(uv_stream_t *server, int status);
  
};

} }

#endif
