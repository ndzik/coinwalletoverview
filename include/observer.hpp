#ifndef INCLUDE_OBSERVER_HPP
#define INCLUDE_OBSERVER_HPP
namespace cwo {
class Observer {
public:
  Observer();
  ~Observer();

  virtual void update() = 0;
};     // class Observer
};     // namespace cwo
#endif // INCLUDE_OBSERVER_HPP
