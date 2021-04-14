#include "wallet.hpp"
#include <curl/curl.h>

namespace cwo {
  /********************** WALLET *********************************************/
  Wallet::Wallet(CRYPTOTYPE t, std::string_view address,
      std::string_view walleturl)
    : _cryptotype(t), _balance(0), _value(0), _onehourchange(0),
    _onedaychange(0), _oneweekchange(0), _currency(USD), _URL(walleturl),
    _address(address)
  { }

  Wallet::~Wallet()
  { }

  double Wallet::balance() { return _balance; }

  std::string Wallet::address() { return _address; }

  int Wallet::writer(char *data, size_t size, size_t nmb, std::string *buffer)
  {
    int result = 0;
    if (buffer != NULL) {
      buffer->append(data, size * nmb);
      result = size * nmb;
    }
    return result;
  }

  bool Wallet::curlget(std::string &b)
  {
    CURL *c;
    CURLcode r;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    c = curl_easy_init();
    if (c) {
      curl_easy_setopt(c, CURLOPT_URL, _URL.c_str());
      curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 0);
      curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, &Wallet::writer);
      curl_easy_setopt(c, CURLOPT_WRITEDATA, &b);
      curl_easy_setopt(c, CURLOPT_FAILONERROR, 1);
    } else {
      return false;
    }
    r = curl_easy_perform(c);
    curl_easy_cleanup(c);
    curl_global_cleanup();
    return (r == CURLE_OK) ? true : false;
  }

  CRYPTOTYPE Wallet::crypto()
  {
    return _cryptotype;
  }

  void Wallet::value(const double &v)
  {
    _value = v;
  }

  double Wallet::value()
  {
    return _value;
  }

  void Wallet::currency(CURRENCY c)
  {
    _currency = c;
  }

  CURRENCY Wallet::currency()
  {
    return _currency;
  }

  void Wallet::onehourchange(const double &v)
  {
    _onehourchange = v;
  }
  double Wallet::onehourchange()
  {
    return _onehourchange;
  }

  void Wallet::onedaychange(const double &v)
  {
    _onedaychange = v;
  }
  double Wallet::onedaychange()
  {
    return _onedaychange;
  }

  void Wallet::oneweekchange(const double &v)
  {
    _oneweekchange = v;
  }
  double Wallet::oneweekchange()
  {
    return _oneweekchange;
  }

  /********************** ETHERWALLET ****************************************/
  EtherWallet::EtherWallet(std::string_view address,
      std::string_view walleturl)
    : Wallet(ETH, address, walleturl)
  { }
  EtherWallet::~EtherWallet()
  { }

  void EtherWallet::update()
  {
    std::string buffer;
    if (!curlget(buffer))
      return;
    auto j = nlohmann::json::parse(buffer);
    float balance = j["balance"];

    _balance = balance/std::pow(10, 18);
  }

  /********************** VECHAINWALLET **************************************/
  VechainWallet::VechainWallet(std::string_view address,
      std::string_view walleturl)
    : Wallet(VET, address, walleturl)
  { }

  VechainWallet::~VechainWallet()
  { }

  void VechainWallet::update()
  {
    std::regex e ("(<span data-v-bc223534><span class=\"text-monospace\">)"
        "(.*?..*?)(</span>)");
    std::regex f ("(\\d+?.\\d+?)");
    std::string buffer;
    if (!curlget(buffer))
      return;
    curlget(buffer);

    std::smatch m;
    std::regex_search(buffer, m, e);
    std::string output = m[2].str();
    auto it = output.find(",");
    output.erase(it, 1);
    _balance = std::atof(output.c_str());
  }

  /********************** BITCOINWALLET **************************************/
  BitcoinWallet::BitcoinWallet(std::string_view address,
      std::string_view walleturl)
    : Wallet(BTC, address, walleturl)
  { }

  BitcoinWallet::~BitcoinWallet()
  { }

  void BitcoinWallet::update()
  {
    std::string buffer;
    if (!curlget(buffer))
      return;
    _balance = atof(buffer.c_str())*std::pow(10, -8);
  }

  /********************** NERVOSWALLET **************************************/
  NervosWallet::NervosWallet(std::string_view address,
          std::string_view walleturl)
      : Wallet(CKB, address, walleturl)
  { }

  NervosWallet::~NervosWallet()
  { }


  void NervosWallet::update()
  {
    _balance = 700000;
  }

}; // namespace cwo
