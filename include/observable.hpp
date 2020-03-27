#ifndef INCLUDE_OBSERVABLE_HPP
#define INCLUDE_OBSERVABLE_HPP
#include <vector>
#include "observer.hpp"
namespace cwo {
  class Observable {
    public:
      Observable();
      ~Observable();

      void notifyall();
      void addobs(Observer *o);

    protected:
      std::vector<Observer*> _obslist;
  }; // class Observable
}; // namespace cwo
#endif // INCLUDE_OBSERVABLE_HPP
