#ifndef NETWORK_MESSAGEH
#define NETWORK_MESSAGEH

#include <uv.h>

namespace film { namespace network {

struct Message {
  uv_stream_t* handle;
  const char* data;
  size_t length;
};

} }

#endif
