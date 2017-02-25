#ifndef NETWORK_OBSERVABLEH
#define NETWORK_OBSERVABLEH

#include <vector>

namespace film { namespace network {

template<typename O, typename M>
class Observable {
public:
  Observable() {}
  ~Observable() {}
  void register_observer(O observer) { observers.push_back(observer); }
  virtual void notify_observers(M message) = 0;
protected:
  std::vector<O> observers;
};

} }

#endif
