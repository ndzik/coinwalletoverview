#ifndef INCLUDE_MODEL_HPP
#define INCLUDE_MODEL_HPP
#include <string>
#include <string_view>
#include <chrono>
#include <thread>
#include <regex>
#include <sqlite3.h>
#include <filesystem>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <list>
#include <atomic>
#include <future>
#include <curl/curl.h>

#include "wallet.hpp"
#include "json.hpp"
#include "utility.hpp"
#include "coingecko.hpp"
#include "walletfactory.hpp"
#include "observable.hpp"

namespace cwo {
  class Model : public Observable {
    public:
      Model();
      Model(std::string);
      ~Model();

      /*
       * Function updating state of model in regular intervals
       * Updates amount of coins for each registered wallet
       * Fetches most recent value for coins from coinmarketcap
       */
      void run();

      /*
       * Register a wallet via given address url and typeinfo
       */
      void registerwallet(CRYPTOTYPE t, std::string url);

      /*
       * Unregister a wallet via given address and typeinfo
       */
      void unregisterwallet(CRYPTOTYPE t, std::string address);

      /*
       * Returns a vector of all registered wallets for given cryptotype
       */
      std::vector<Wallet*> walletsof(CRYPTOTYPE t);

      /*
       * Returns a vector of all registered wallets
       */
      std::vector<Wallet*> wallets();

      /*
       * Returns a multimap of all registered wallets
       */
      std::multimap<CRYPTOTYPE, Wallet*> walletmap();

      /*
       * Get balance of wallet with given address
       */
      double walletbalance(CRYPTOTYPE t, std::string address);

      /*
       * Stop updateloop
       */
      void stop();

      /*
       * Set updateinterval in mins
       */
      void updateinterval(int mins);

      /*
       * Set investment for cryptotype
       */
      void investment(CRYPTOTYPE t, double amount);

      /*
       * Returns current crypto for cryptotype
       */
      double value(CRYPTOTYPE t);

      /*
       * Api key for coinmarketcap
       */
      void apikey(const std::string &s);

      /*
       * Token for blockcypher
       */
      void token(const std::string &s);

      /*
       * Callback function for curl to write received data
       */
      static int writer(char *data,
          size_t size,
          size_t nmb,
          std::string *buffer);

      /*
       * Callback function for sqlite3 empty call, no effect
       */
      static int callbackempty(void *notused, int argc, char **argv,
          char **azcolname);

      /*
       * Callback function for sqlite3
       */
      static int callbackdata(void *notused, int argc, char **argv,
          char **azcolname);

      /*
       * Callback function for sqlite3 when single value is being requested
       */
      static int callbackselectsingle(void *notused, int argc, char **argv,
          char **azcolname);

      /*
       * Selects range of values for cryptotype each 5 minutes if available
       * and inserts them into given vector
       */
      void selectdatabymin(CRYPTOTYPE t, std::vector<Statistic> *v, int limit=0);

      /*
       * Select range of values for cryptotype for each available hour and
       * insert them into a given vector
       */
      void selectdatabyhour(CRYPTOTYPE t, std::vector<Statistic> *v, int limit=0);

      /*
       * Select range of values for cryptotype for each available day and
       * insert them into a given vector
       */
      void selectdatabyday(CRYPTOTYPE t, std::vector<Statistic> *v, int limit=0);

      /*
       * Get list of CRYPTOTYPES for registered wallets
       */
      std::vector<CRYPTOTYPE> regcryptos();

      /*
       * Get Min-Max for CRYPTOTYPE for min
       */
      std::array<double, 2> minmaxmin(CRYPTOTYPE t, int limit=0);

      /*
       * Get Min-Max for CRYPTOTYPE for hour
       */
      std::array<double, 2> minmaxhour(CRYPTOTYPE t, int limit=0);

      /*
       * Get Min-Max for CRYPTOTYPE for day
       */
      std::array<double, 2> minmaxday(CRYPTOTYPE t, int limit=0);

      /*
       * Notify observers
       */

    private:
      std::atomic<bool> _running;
      int _dbinterval, _dbupdateinterval;
      std::string ETHURL;
      std::string VETURL;
      std::string BTCURL;
      std::string _apikey;
      std::string _token;
      std::shared_mutex _mtx;
      std::multimap<CRYPTOTYPE, Wallet*> _wallets;  /* ETH - 0xdeadbeef */
      std::map<CRYPTOTYPE, double> _investment; /* ETH - 5 */
      CURRENCY _currency; /* USD */
      sqlite3 *_db;

      std::string getwalleturl(CRYPTOTYPE t);
      std::string createwalleturl(std::string urltemplate, std::string addr);

      /*
       * Load and create DB if necessary
       */
      void loaddatabasefile();

      /*
       * Inserts a price for crypto at given datetime
       */
      void insertpricedb(CRYPTOTYPE t, double price);

      /*
       * Creates databasetable if necessary
       */
      void createdatabase();

      /*
       * Update db values for registered cryptotypes
       */
      void updatepricedata(API *api);

      /*
       * Update registered wallets
       */
      void updatewallets();

  }; // class Model
}; // namespace cwo
#endif // INCLUDE_MODEL_HPP
