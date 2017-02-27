#ifndef NETWORK_CLIENTH
#define NETWORK_CLIENTH

#include <uv.h>
#include "networkable.h"

namespace film { namespace network {

class Client : public Networkable<Client> {
public:
  Client();
  ~Client();
  virtual void start(const char* ip, int port);
private:
  void _start(const char* ip, int port);
  static void connection_cb(uv_connect_t* connection, int status);
};

} }

#endif
