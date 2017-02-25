#ifndef NETWORK_SERVERH
#define NETWORK_SERVERH

#include <uv.h>
#include "networkable.h"

namespace film { namespace network {

class Server : public Networkable<Server> {
public:
  Server();
  ~Server();
  virtual int start();
private:
  static void connection_cb(uv_stream_t *server, int status);
};

} }

#endif
