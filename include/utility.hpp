#ifndef INCLUDE_UTILITY_HPP
#define INCLUDE_UTILITY_HPP
#include <map>
#include <string>

#define DBNAME(x) x "DATA"

namespace cwo {
enum CRYPTOTYPE { ETH, VET, BTC, CKB, LASTCRYPTO };
enum CURRENCY { EUR, USD };
enum DATARES { M, H, D };
enum ALIGNMENT { VERTICAL, HORIZONTAL };
/* This can be done smarter with a single map mapping things to api related
 * structs. At least for most of the stuff here...
 */
const std::map<CRYPTOTYPE, std::string> CRYPTOG = {
    {ETH, "ethereum"},
    {VET, "vechain"},
    {BTC, "bitcoin"},
    {CKB, "nervos-network"},
};
const std::map<CRYPTOTYPE, std::string> CRTOS = {
    {ETH, "ETH"},
    {BTC, "BTC"},
    {VET, "VET"},
    {CKB, "CKB"},
};
const std::map<CRYPTOTYPE, std::string> DBNAME = {
    {ETH, "ETHDATA"},
    {VET, "VETDATA"},
    {BTC, "BTCDATA"},
    {CKB, "CKBDATA"},
};
const std::map<CRYPTOTYPE, std::string> URL = {
    {ETH, "https://api.blockcypher.com/v1/eth/main/addrs/{}/balance?token="},
    {VET, "https://explore.vechain.org/accounts/{}"},
    {BTC, "https://blockchain.info/q/addressbalance/{}"},
    {CKB, "https://explorer.nervos.org/address/{}"},
};
const std::map<CURRENCY, std::string> CTOS = {
    {EUR, "EUR"},
    {USD, "USD"},
};
const std::map<std::string, CRYPTOTYPE> STOC = {
    {"ETH", ETH},
    {"VET", VET},
    {"BTC", BTC},
    {"CKB", CKB},
};
struct Statistic {
  std::string datetime;
  double price;
};
};     // namespace cwo
#endif // INCLUDE_UTILITY_HPP
