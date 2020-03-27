#ifndef INCLUDE_WALLETFACTORY_HPP
#define INCLUDE_WALLETFACTORY_HPP
#include "wallet.hpp"
namespace cwo {
  class Walletfactory {
    public:
      Walletfactory() { }
      Walletfactory url(std::string url);
      Walletfactory address(std::string address);
      Walletfactory cryptotype(CRYPTOTYPE t);
      Wallet* build();

    private:
      std::string _url;
      std::string _address;
      CRYPTOTYPE _cryptotype;
      void reset();
  }; // class Walletfactory
}; // namespace cwo
#endif // INCLUDE_WALLETFACTORY_HPP
