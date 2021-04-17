#include "coingecko.hpp"
#include <algorithm>
#include <iostream>
#include <ostream>

namespace cwo {
CoinGecko::CoinGecko() : API("coingecko") {}

CoinGecko::~CoinGecko() {}

double CoinGecko::price(CRYPTOTYPE t)
{
  double price;
  if (!_data.empty())
    price = _data[_idcache.at(t)]["current_price"];
  else
    price = 0;
  return price;
}

double CoinGecko::onehourchange(CRYPTOTYPE t)
{
  double change;
  if (!_data.empty())
    change = _data[_idcache.at(t)]["price_change_percentage_1h_in_currency"];
  else
    change = 0;
  return change;
}

double CoinGecko::onedaychange(CRYPTOTYPE t)
{
  double change;
  if (!_data.empty())
    change = _data[_idcache.at(t)]["price_change_percentage_24h_in_currency"];
  else
    change = 0;
  return change;
}

double CoinGecko::oneweekchange(CRYPTOTYPE t)
{
  double change;
  if (!_data.empty())
    change = _data[_idcache.at(t)]["price_change_percentage_7d_in_currency"];
  else
    change = 0;
  return change;
}

bool CoinGecko::update()
{
  CURL *c;
  CURLcode r;
  std::string b;
  std::string baseURL = "https://api.coingecko.com/api/v3";

  /* construct query url */
  std::stringstream URL;
  /* having 250 per page might not be enough */
  URL << baseURL
      << "/coins/markets?order=market_cap_desc"
         "&per_page=250&page=1&sparkline=false&price_change_percentage=1h,24h,"
         "7d"
         "&ids=";
  for (auto &a : _cryptos)
    URL << CRYPTOG.at(a) << ((a != _cryptos[_cryptos.size() - 1]) ? "," : "");
  URL << "&vs_currency=" << CTOS.at(_currency);

  curl_global_init(CURL_GLOBAL_DEFAULT);
  struct curl_slist *hlist = NULL;
  hlist = curl_slist_append(hlist, "Accept: application/json");
  c = curl_easy_init();
  if (c) {
    curl_easy_setopt(c, CURLOPT_URL, URL.str().c_str());
    curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 0);
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, &CoinGecko::writer);
    curl_easy_setopt(c, CURLOPT_WRITEDATA, &b);
    curl_easy_setopt(c, CURLOPT_FAILONERROR, 1);
    curl_easy_setopt(c, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  }
  else {
    return false;
  }
  r = curl_easy_perform(c);
  curl_easy_cleanup(c);
  curl_global_cleanup();
  if (r != CURLE_OK)
    return false;
  _data = nlohmann::json::parse(b);
  updateidcache();
  return true;
}

void CoinGecko::updateidcache()
{
  if (_data.empty())
    return;
  unsigned int i = 0;
  while (true) {
    if (_data[i].size() > i) {
      /* compare to mapped ids and cryptotypes */
      for (auto ct : CRYPTOG) {
        if (ct.second == _data[i]["id"]) {
          _idcache[ct.first] = i;
        }
      }
    }
    else {
      return;
    }
    ++i;
  }
}
}; // namespace cwo
