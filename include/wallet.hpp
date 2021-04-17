#ifndef INCLUDE_WALLET_HPP
#define INCLUDE_WALLET_HPP
#include "json.hpp"
#include "utility.hpp"
#include <curl/curl.h>
#include <iostream>
#include <regex>
#include <string>
#include <string_view>
namespace cwo {
class Wallet {
public:
  /*
   * Constructor for wallet
   * @param CRYPTOTYPE t specifies how update() will handle state update
   * @param walleturl provides necessary apiendpoint to fetch data
   */
  Wallet(CRYPTOTYPE t, std::string_view address, std::string_view walleturl);

  virtual ~Wallet();

  /* Returns current balance of wallet */
  double balance();

  /* Returns cryptotype */
  CRYPTOTYPE crypto();

  /* Returns address of wallet */
  std::string address();

  /*
   * Fetches information for wallet from given walleturl resulting in an
   * update of its state
   */
  void virtual update() = 0;

  /* Callback function for curl */
  static int writer(char *data, size_t size, size_t nmemb, std::string *buffer);

  /*
   * Set/Get current value of cryptoasset as _currency
   */
  void value(const double &v);
  double value();

  /*
   * Set/Get currency type for cryptoasset and wallet
   */
  void currency(CURRENCY c);
  CURRENCY currency();

  /*
   * Set/Get change of asset for timeinterval
   */
  void onehourchange(const double &v);
  double onehourchange();

  void onedaychange(const double &v);
  double onedaychange();

  void oneweekchange(const double &v);
  double oneweekchange();

protected:
  const CRYPTOTYPE _cryptotype;
  double _balance;
  double _value;
  double _onehourchange, _onedaychange, _oneweekchange;
  CURRENCY _currency;
  const std::string _URL;
  const std::string _address;

  /*
   * Auxiliaryfunction to setup curl
   * @param buffer storing curl get request
   */
  bool curlget(std::string &b);
}; // class Wallet

class EtherWallet : public Wallet {
public:
  EtherWallet(std::string_view address, std::string_view walleturl);
  ~EtherWallet();
  void update();
}; // class EtherWallet

class VechainWallet : public Wallet {
public:
  VechainWallet(std::string_view address, std::string_view walleturl);
  ~VechainWallet();
  void update();
}; // class VechainWallet

class BitcoinWallet : public Wallet {
public:
  BitcoinWallet(std::string_view address, std::string_view walleturl);
  ~BitcoinWallet();
  void update();
}; // class BitcoinWallet

class NervosWallet : public Wallet {
public:
  NervosWallet(std::string_view address, std::string_view walleturl);
  ~NervosWallet();
  void update();
};     // class NervosWallet
};     // namespace cwo
#endif // INCLUDE_WALLET_HPP
