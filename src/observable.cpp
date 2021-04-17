#include "observable.hpp"

namespace cwo {
Observable::Observable() {}

Observable::~Observable() {}

void Observable::notifyall()
{
  for (auto &a : _obslist)
    a->update();
}

void Observable::addobs(Observer *o) { _obslist.push_back(o); }
}; // namespace cwo
