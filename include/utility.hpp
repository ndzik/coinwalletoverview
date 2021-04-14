#ifndef INCLUDE_UTILITY_HPP
#define INCLUDE_UTILITY_HPP
#include <map>

#define DBNAME(x) x "DATA"

namespace cwo {
  enum CRYPTOTYPE {
    ETH,
    VET,
    BTC,
    LASTCRYPTO
  };
  enum CURRENCY {
    EUR,
    USD
  };
  enum DATARES {
    M,
    H,
    D
  };
  enum ALIGNMENT {
    VERTICAL,
    HORIZONTAL
  };
  /* this can be done smarter with a single map mapping things to api related
   * structs. at least for most of the stuff here...
   */
  const std::map<CRYPTOTYPE, std::string> CRYPTOG = {
    {ETH, "ethereum"},
    {VET, "vechain"},
    {BTC, "bitcoin"},
  };
  const std::map<CRYPTOTYPE, std::string> URL = {
    {ETH, "https://api.blockcypher.com/v1/eth/main/addrs/{}/balance?token="},
    {VET, "https://explore.vechain.org/accounts/{}"},
    {BTC, "https://blockchain.info/q/addressbalance/{}"}
  };
  const std::map<CURRENCY, std::string> CTOS = {
    {EUR, "EUR"},
    {USD, "USD"}
  };
  const std::map<std::string, CRYPTOTYPE> STOC = {
    {"ETH", ETH},
    {"VET", VET},
    {"BTC", BTC}
  };
  const std::map<CRYPTOTYPE, std::string> CRTOS = {
    {ETH, "ETH"},
    {BTC, "BTC"},
    {VET, "VET"}
  };
  const std::map<CRYPTOTYPE, std::string> DBNAME = {
    {ETH, "ETHDATA"},
    {VET, "VETDATA"},
    {BTC, "BTCDATA"}
  };
  struct Statistic {
    std::string datetime;
    double price;
  };
};
#endif // INCLUDE_UTILITY_HPP
