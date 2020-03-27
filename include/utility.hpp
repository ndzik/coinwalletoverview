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
  const std::map<CRYPTOTYPE, std::string> CRYPTO = {
    {ETH, "1027"},
    {VET, "3077"},
    {BTC, "1"}
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
