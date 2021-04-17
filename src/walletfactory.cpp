#include "walletfactory.hpp"

namespace cwo {
Walletfactory Walletfactory::url(std::string url)
{
  _url = url;
  return *this;
}

Walletfactory Walletfactory::address(std::string address)
{
  _address = address;
  return *this;
}

Walletfactory Walletfactory::cryptotype(CRYPTOTYPE t)
{
  _cryptotype = t;
  return *this;
}

Wallet *Walletfactory::build()
{
  switch (_cryptotype) {
  case ETH:
    return new EtherWallet(_address, _url);
    reset();
    break;
  case VET:
    return new VechainWallet(_address, _url);
    reset();
    break;
  case BTC:
    return new BitcoinWallet(_address, _url);
    reset();
    break;
  case CKB:
    return new NervosWallet(_address, _url);
    reset();
    break;
  default:
    reset();
    return NULL;
  }
}

void Walletfactory::reset()
{
  _url = "127.0.0.1";
  _address = "0xdeadbeef";
  _cryptotype = BTC;
}
}; // namespace cwo
