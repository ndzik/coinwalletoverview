#ifndef INCLUDE_COINGECKO_HPP
#define INCLUDE_COINGECKO_HPP
#include "api.hpp"
#include <sstream>
#include <string>
namespace cwo {
/* class to wrap up coingeckos api */
class CoinGecko : public API {
public:
  CoinGecko();

  ~CoinGecko();

  /* return price of given cryptotype in predefined currency */
  double price(CRYPTOTYPE t);

  /* update method to fetch coingecko api */
  bool update();

  /* get 1 hour change of price for cryptotype */
  double onehourchange(CRYPTOTYPE t);

  /* get 24 hour change of price for cryptotype */
  double onedaychange(CRYPTOTYPE t);

  /* get 1 week change of price for cryptotype */
  double oneweekchange(CRYPTOTYPE t);

  /* updated id cache to store indexes of api result for easier lookup */
  void updateidcache();

private:
  std::map<CRYPTOTYPE, int> _idcache;

};     // class CoinGecko
};     // namespace cwo
#endif // INCLUDE_COINGECKO_HPP
