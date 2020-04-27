#ifndef INCLUDE_API_HPP
#define INCLUDE_API_HPP
#include <string>
#include <vector>
#include <curl/curl.h>
#include "json.hpp"
#include "utility.hpp"
#include "json.hpp"
/* abstract class for api wrappers with curl */
namespace cwo {

  class API {
    public:
      /* set cryptos to query api for */
      API(const std::string &apikey)
      {
        _apikey = apikey;
      };

      /* retrieve price of given cryptotype */
      virtual double price(CRYPTOTYPE t) = 0;

      /* get 1 hour change of price for cryptotype */
      virtual double onehourchange(CRYPTOTYPE t) = 0;

      /* get 24 hour change of price for cryptotype */
      virtual double onedaychange(CRYPTOTYPE t) = 0;

      /* get 1 week change of price for cryptotype */
      virtual double oneweekchange(CRYPTOTYPE t) = 0;

      /* update method to fetch api data */
      virtual bool update() = 0;

      /* set which cryptotypes to query for */
      API* crypto(const std::vector<CRYPTOTYPE> &t)
      {
        _cryptos = t;
        return this;
      }

      /* return price of given cryptotype in predefined currency */
      API* currency(CURRENCY c)
      {
        _currency = c;
        return this;
      }

      /* auxiliary writer for curl */
      static int writer(char *data, size_t size, size_t nmb, std::string *buffer)
      {
        int result = 0;
        if (buffer != NULL) {
          buffer->append(data, size * nmb);
          result = size * nmb;
        }
        return result;
      }



    protected:
      std::string _apikey;
      std::vector<CRYPTOTYPE> _cryptos;
      CURRENCY _currency;
      nlohmann::json _data;
  }; // class API
}; // namespace cwo
#endif // INCLUDE_COINMARKETCAP_HPP
