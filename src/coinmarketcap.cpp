#include "coinmarketcap.hpp"

namespace cwo {
  /************************* CONSTRUCTORS ************************************/
  CMC::CMC(const std::string &apikey)
    : _cryptos({ETH}), _currency(USD)
  {
    std::stringstream s;
    s << "X-CMC_PRO_API_KEY: " << apikey;
    _apikey = s.str();
  }

  /************************* PUBLIC ******************************************/
  CMC* CMC::crypto(std::vector<CRYPTOTYPE> &t)
  {
    _cryptos = t;
    return this;
  }

  CMC* CMC::currency(CURRENCY c)
  {
    _currency = c;
    return this;
  }

  int CMC::writer(char *data, size_t size, size_t nmb, std::string *buffer)
  {
    int result = 0;
    if (buffer != NULL) {
      buffer->append(data, size * nmb);
      result = size * nmb;
    }
    return result;
  }

  bool CMC::update()
  {
    CURL *c;
    CURLcode r;
    std::string b;
    std::string baseURL = "https://pro-api.coinmarketcap.com/"
      "v1/cryptocurrency/quotes/latest";

    /* construct query url */
    std::stringstream URL;
    URL << baseURL << "?convert=" << CTOS.at(_currency) << "&id=";
    for (auto &a : _cryptos)
      URL << CRYPTO.at(a) << ((a != _cryptos[_cryptos.size()-1]) ? "," : "");

    curl_global_init(CURL_GLOBAL_DEFAULT);
    struct curl_slist *hlist = NULL;
    hlist = curl_slist_append(hlist, _apikey.c_str());
    hlist = curl_slist_append(hlist, "Accept: application/json");
    c = curl_easy_init();
    if (c) {
      curl_easy_setopt(c, CURLOPT_URL, URL.str().c_str());
      curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 0);
      curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, &CMC::writer);
      curl_easy_setopt(c, CURLOPT_WRITEDATA, &b);
      curl_easy_setopt(c, CURLOPT_HTTPHEADER, hlist);
      curl_easy_setopt(c, CURLOPT_FAILONERROR, 1);
    } else {
      return false;
    }
    r = curl_easy_perform(c);
    curl_easy_cleanup(c);
    curl_global_cleanup();
    if (r != CURLE_OK)
      return false;
    _data = nlohmann::json::parse(b);
    return true;
  }

  double CMC::price(CRYPTOTYPE t)
  {
    double price;
    if (!_data.empty())
      price = _data["data"][CRYPTO.at(t)]["quote"][CTOS.at(_currency)]["price"];
    else
      price = 420;
    return price;
  }

  double CMC::onehourchange(CRYPTOTYPE t)
  {
    double change;
    if (!_data.empty())
      change = _data["data"][CRYPTO.at(t)]["quote"][CTOS.at(_currency)]
        ["percent_change_1h"];
    else
      change = 0;
    return change;
  }

  double CMC::onedaychange(CRYPTOTYPE t)
  {
    double change;
    if (!_data.empty())
      change = _data["data"][CRYPTO.at(t)]["quote"][CTOS.at(_currency)]
        ["percent_change_24h"];
    else
      change = 0;
    return change;
  }

  double CMC::oneweekchange(CRYPTOTYPE t)
  {
    double change;
    if (!_data.empty())
      change = _data["data"][CRYPTO.at(t)]["quote"][CTOS.at(_currency)]
        ["percent_change_7d"];
    else
      change = 0;
    return change;
  }

  /************************* PRIVATE *****************************************/
};
