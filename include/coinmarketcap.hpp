#ifndef INCLUDE_COINMARKETCAP_HPP
#define INCLUDE_COINMARKETCAP_HPP
#include <string>
#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include <curl/easy.h>
#include "utility.hpp"
#include "json.hpp"
namespace cwo {
  /*
   * Class to wrap up CoinMarketCap API in a c++ class
   */
  class CMC {
    public:
      CMC(const std::string &apikey);

      /*
       * Set cryptos to query cmc api for
       */
      CMC* crypto(std::vector<CRYPTOTYPE> &t);

      /*
       * Set currency for upcoming update call
       */
      CMC* currency(CURRENCY c);

      /*
       * Return price of cryptotype in specified currency
       */
      double price(CRYPTOTYPE t);

      /*
       * Return percentual change of cryptoasset from last call
       */
      double onehourchange(CRYPTOTYPE t);

      /*
       * Return percentual change of cryptoasset from last call
       */
      double onedaychange(CRYPTOTYPE t);

      /*
       * Return percentual change of cryptoasset from last call
       */
      double oneweekchange(CRYPTOTYPE t);

      /*
       * Callback function for curl to write received data
       */
      static int writer(char *data,
          size_t size,
          size_t nmb,
          std::string *buffer);

      /*
       * Query coinmarketcap api for information and write result into
       * _data
       */
      void update();
    private:
      std::vector<CRYPTOTYPE> _cryptos;
      CURRENCY _currency;
      nlohmann::json _data;
      std::string _apikey;
  }; // class CMC
}; // namespace cwo
#endif // INCLUDE_COINMARKETCAP_HPP
