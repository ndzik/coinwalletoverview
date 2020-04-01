#include "model.hpp"
#include <chrono>
#include <map>
#include <mutex>
#include <thread>

namespace cwo {
  /********************************** CTORS **********************************/
  Model::Model()
    : Observable(), _running(true), _dbinterval(5), _currency(USD)
  {
    ETHURL = "https://api.blockcypher.com/v1/eth/main/addrs/"
        "{}/"
        "balance?token=";
    VETURL = "https://explore.vechain.org/accounts/"
        "{}";
    BTCURL = "https://blockchain.info/q/addressbalance/{}";
    loaddatabasefile();
    createdatabase();
  }

  Model::~Model()
  {
    sqlite3_close(_db);
    auto it = _wallets.begin();
    while (it != _wallets.end()) {
      delete it->second;
      ++it;
    }
  }

  /********************************* PUBLIC **********************************/
  void Model::run()
  {
    CMC cm(_apikey);
    int cnt = 0;
    while(_running) {
      /* Sleep as long as necessary (debug purpose so far because of MT) */
      if (cnt%6000 != 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ++cnt;
        continue;
      }

      if (_wallets.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        continue;
      }

      /* Write new values for cryptos to DB */
      if (_dbupdateinterval%_dbinterval == 0) {
        updatepricedata(&cm);
      }

      updatewallets();

      /* Notify observers if existent */
      notifyall();

      /* Set variables to track intervals */
      cnt = 1;
      ++_dbupdateinterval;

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  void Model::updatewallets()
  {
    std::vector<std::thread> futs;
    {
      std::unique_lock lock(_mtx);
      /* Check balance of each registered wallet */
      auto it = _wallets.begin();
      while (it != _wallets.end()) {
        Wallet *w = it->second;
        futs.push_back(std::thread([w](Wallet *wallet)
                { wallet->update(); }, w));
        ++it;
      }
    }
    for (auto &th : futs)
      th.join();
  }

  void Model::updatepricedata(CMC *cm)
  {
    std::vector<CRYPTOTYPE> v = regcryptos();
    _dbupdateinterval = 0;

    { /* CRITICAL AREA START */
      std::unique_lock lock(_mtx);
      if (cm->crypto(v)->currency(_currency)->update()) {
        for (auto &a : v)
          insertpricedb(a, cm->price(a));
        for (auto &w : _wallets) {
          w.second->value(cm->price(w.first));
          w.second->onehourchange(cm->onehourchange(w.first));
          w.second->onedaychange(cm->onedaychange(w.first));
          w.second->oneweekchange(cm->oneweekchange(w.first));
        }
      }
    } /* CRITICAL AREA END */
  }

  void Model::apikey(const std::string &s)
  {
    _apikey = s;
  }

  void Model::stop()
  {
    _running = false;
  }

  void Model::updateinterval(int mins)
  {
    _dbinterval = mins;
  }

  void Model::investment(CRYPTOTYPE t, double amount)
  {
    _investment.insert_or_assign(t, amount);
  }

  void Model::registerwallet(CRYPTOTYPE t, std::string address)
  {
    std::string wurl = getwalleturl(t);
    Walletfactory wf;
    wurl = createwalleturl(wurl, address);
    { /*** CRITICAL AREA START ***/
      std::unique_lock lock(_mtx);
      _wallets.insert(std::pair<CRYPTOTYPE, Wallet*>(t,
            wf.address(address).cryptotype(t).url(wurl).build()));
    } /*** CRITICAL AREA END ***/
  }

  void Model::unregisterwallet(CRYPTOTYPE t, std::string address)
  {
    { /*** CRITICAL AREA START ***/
      std::unique_lock lock(_mtx);
      auto it = _wallets.find(t);
      while (it != _wallets.end()) {
        if (it->second->address() == address) {
          _wallets.erase(it);
          return;
        }
        ++it;
      }
    } /*** CRITICAL AREA END ***/
  }

  double Model::walletbalance(CRYPTOTYPE t, std::string address)
  {
    { /*** CRITICAL AREA START ***/
      std::shared_lock lock(_mtx);
      auto it = _wallets.find(t);
      while (it != _wallets.end()) {
        if (it->second->address() == address) return it->second->balance();
        ++it;
      }
    } /*** CRITICAL AREA END ***/
    return 0;
  }

  std::multimap<CRYPTOTYPE, Wallet*> Model::walletmap()
  {
    std::multimap<CRYPTOTYPE, Wallet*> w;
    { /*** CRITICAL AREA START ***/
      std::shared_lock lock(_mtx);
      w = _wallets;
    } /*** CRITICAL AREA END ***/
    return w;
  }

  std::vector<Wallet*> Model::wallets()
  {
    std::vector<Wallet*> w;
    { /*** CRITICAL AREA START ***/
      std::shared_lock lock(_mtx);
      for (auto it = _wallets.begin(); it != _wallets.end(); ++it)
        w.push_back(it->second);
    } /*** CRITICAL AREA END ***/
    return w;
  }

  std::vector<Wallet*> Model::walletsof(CRYPTOTYPE t)
  {
    typedef std::multimap<CRYPTOTYPE, Wallet*>::iterator mip;
    std::vector<Wallet*> w;
    { /*** CRITICAL AREA START ***/
      std::shared_lock lock(_mtx);
      std::pair<mip, mip> ret = _wallets.equal_range(t);
      for (mip it = ret.first; it != ret.second; ++it)
        w.push_back(it->second);
    } /*** CRITICAL AREA END ***/
    return w;
  }

  int Model::writer(char *data,
      size_t size,
      size_t nmb,
      std::string *buffer)
  {
    int result = 0;
    if (buffer != NULL) {
      buffer->append(data, size * nmb);
      result = size * nmb;
    }
    return result;
  }

  int Model::callbackempty(void *vec, int argc, char **argv, char **azcolname)
  {
    return 0;
  }

  int Model::callbackdata(void *vec, int argc, char **argv, char **azcolname)
  {
    std::vector<Statistic> *v = static_cast<std::vector<Statistic>*>(vec);
    for (int i = 0; i<argc; i = i+2) {
      Statistic s;
      s.datetime = std::string(argv[i]);
      s.price = atof(argv[i+1]);
      v->push_back(s);
    }
    return 0;
  }

  int Model::callbackselectsingle(void *dnum, int argc, char **argv,
      char **azcolname)
  {
    double *v = static_cast<double*>(dnum);
    /*
     * If database is empty and no min/max value could be retrieved, argc still
     * returns 1 while argv is unmapped? -> leading to a seg-fault
     */
    if (argc > 0)
      *v = atof(argv[0]);
    else
      *v = 0;
    return 0;
  }

  void Model::selectdatabymin(CRYPTOTYPE t,
      std::vector<Statistic> *v, int limit)
  {
    std::stringstream sql;
    char *err;
    sql << "SELECT * FROM ("
      << " SELECT * FROM " << DBNAME.at(t)
      << " ORDER BY DATA_TD DESC LIMIT " << limit << ")"
      << " ORDER BY DATA_TD ASC;";
    int rc = sqlite3_exec(_db, sql.str().c_str(),
        &Model::callbackdata, v, &err);
    if (rc != SQLITE_OK) {
      v->clear();
      fprintf(stderr, "SQL error: %s\n", err);
      sqlite3_free(err);
    }
  }

  void Model::selectdatabyhour(CRYPTOTYPE t,
      std::vector<Statistic> *v, int limit)
  {
    std::stringstream sql;
    char *err;
    sql << "SELECT * FROM ("
      << "SELECT STRFTIME('%Y-%m-%d %H:00:00.000', DATA_TD) AS ACCDT,"
      << " AVG(DATA_PRICE)"
      << " FROM " << DBNAME.at(t)
      << " GROUP BY ACCDT"
      << " ORDER BY ACCDT DESC LIMIT " << limit  << ")"
      << " ORDER BY ACCDT ASC;";
    int rc = sqlite3_exec(_db, sql.str().c_str(), &Model::callbackdata, v,
        &err);
    if (rc != SQLITE_OK) {
      v->clear();
      fprintf(stderr, "SQL error: %s\n", err);
      sqlite3_free(err);
    }
  }

  void Model::selectdatabyday(CRYPTOTYPE t,
      std::vector<Statistic> *v, int limit)
  {
    std::stringstream sql;
    char *err;
    sql << "SELECT * FROM ("
      << "SELECT STRFTIME('%Y-%m-%d 00:00:00.000', DATA_TD) AS ACCDT,"
      << " AVG(DATA_PRICE)"
      << " FROM " << DBNAME.at(t)
      << " GROUP BY ACCDT"
      << " ORDER BY ACCDT DESC LIMIT " << limit  << ")"
      << " ORDER BY ACCDT ASC;";
    int rc = sqlite3_exec(_db, sql.str().c_str(), &Model::callbackdata, v,
        &err);
    if (rc != SQLITE_OK) {
      v->clear();
      fprintf(stderr, "SQL error: %s\n", err);
      sqlite3_free(err);
    }
  }

  std::array<double, 2> Model::minmaxmin(CRYPTOTYPE t, int limit)
  {
    std::stringstream sql;
    char *err;
    double min = 0;
    double max = 0;
    sql << "SELECT MIN(DATA_PRICE) FROM ("
     << "SELECT * FROM " << DBNAME.at(t)
     << " ORDER BY DATA_TD DESC " << "limit " << limit << ");";
    int rc = sqlite3_exec(_db, sql.str().c_str(), &Model::callbackselectsingle,
        &min, &err);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", err);
      sqlite3_free(err);
    }
    sql.str(std::string());
    sql << "SELECT MAX(DATA_PRICE) FROM ("
     << "SELECT * FROM " << DBNAME.at(t)
     << " ORDER BY DATA_TD DESC " << "limit " << limit << ");";
    rc = sqlite3_exec(_db, sql.str().c_str(), &Model::callbackselectsingle,
        &max, &err);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", err);
      sqlite3_free(err);
    }
    return {min, max};
  }

  std::array<double, 2> Model::minmaxhour(CRYPTOTYPE t, int limit)
  {
    std::stringstream sql;
    char *err;
    double min = 0;
    double max = 0;
    sql << "SELECT MIN(DATA_PRICE) FROM ("
      << "SELECT STRFTIME('%Y-%m-%d %H:00:00.000', DATA_TD) AS ACCDT,"
      << " AVG(DATA_PRICE) AS DATA_PRICE"
      << " FROM " << DBNAME.at(t)
      << " GROUP BY ACCDT"
      << " ORDER BY ACCDT DESC LIMIT " << limit  << ");";
    int rc = sqlite3_exec(_db, sql.str().c_str(), &Model::callbackselectsingle,
        &min, &err);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", err);
      sqlite3_free(err);
    }
    sql.str(std::string());
    sql << "SELECT MAX(DATA_PRICE) FROM ("
      << "SELECT STRFTIME('%Y-%m-%d %H:00:00.000', DATA_TD) AS ACCDT,"
      << " AVG(DATA_PRICE) AS DATA_PRICE"
      << " FROM " << DBNAME.at(t)
      << " GROUP BY ACCDT"
      << " ORDER BY ACCDT DESC LIMIT " << limit  << ");";
    rc = sqlite3_exec(_db, sql.str().c_str(), &Model::callbackselectsingle,
        &max, &err);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", err);
      sqlite3_free(err);
    }
    return {min, max};
  }

  std::array<double, 2> Model::minmaxday(CRYPTOTYPE t, int limit)
  {
    std::stringstream sql;
    char *err;
    double min = 0;
    double max = 0;
    sql << "SELECT MIN(DATA_PRICE) FROM ("
      << "SELECT STRFTIME('%Y-%m-%d 00:00:00.000', DATA_TD) AS ACCDT,"
      << " AVG(DATA_PRICE) AS DATA_PRICE"
      << " FROM " << DBNAME.at(t)
      << " GROUP BY ACCDT"
      << " ORDER BY ACCDT DESC LIMIT " << limit  << ");";
    int rc = sqlite3_exec(_db, sql.str().c_str(), &Model::callbackselectsingle,
        &min, &err);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", err);
      sqlite3_free(err);
    }
    sql.str(std::string());
    sql << "SELECT MAX(DATA_PRICE) FROM ("
      << "SELECT STRFTIME('%Y-%m-%d 00:00:00.000', DATA_TD) AS ACCDT,"
      << " AVG(DATA_PRICE) AS DATA_PRICE"
      << " FROM " << DBNAME.at(t)
      << " GROUP BY ACCDT"
      << " ORDER BY ACCDT DESC LIMIT " << limit  << ");";
    rc = sqlite3_exec(_db, sql.str().c_str(), &Model::callbackselectsingle,
        &max, &err);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", err);
      sqlite3_free(err);
    }
    return {min, max};
  }

  std::vector<CRYPTOTYPE> Model::regcryptos()
  {
    std::vector<CRYPTOTYPE> v;
    for (int i=ETH; i<LASTCRYPTO; ++i) {
      CRYPTOTYPE t = static_cast<CRYPTOTYPE>(i);
      /* concurrent access on specific elements are supposed to be safe */
      if (_wallets.find(t) != _wallets.end())
        v.push_back(t);
    }
    return v;
  }

  void Model::token(const std::string &s)
  {
    _token = s;
    std::stringstream a;
    a << ETHURL << _token;
    ETHURL = a.str();
  }

  /************************ PRIVATE ******************************************/
  std::string Model::getwalleturl(CRYPTOTYPE t)
  {
    std::string url;
    switch (t) {
      case ETH: url = ETHURL; break;
      case VET: url = VETURL; break;
      case BTC: url = BTCURL; break;
      default: url = "";
    };
    return url;
  }

  std::string Model::createwalleturl(std::string urltemplate, std::string addr)
  {
    std::string delimiter = "{}";
    std::string token;
    std::stringstream url;
    size_t pos = urltemplate.find(delimiter);
    token = urltemplate.substr(0, pos);
    urltemplate.erase(0, pos + delimiter.length());
    url << token << addr << urltemplate;
    return url.str();
  }

  void Model::loaddatabasefile()
  {
    /* Get home directory and construct filepath */
    std::string line;
    std::stringstream pofile;
    std::stringstream sql;
    const char *homedir = getenv("HOME");
    pofile << homedir << "/.config/cwo/graph.db";
    int rc = sqlite3_open(pofile.str().c_str(), &_db);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error on opening\n");
    }
  }

  void Model::createdatabase()
  {
    std::stringstream sql;
    char *err;
    for (int i=ETH; i<LASTCRYPTO; ++i) {
      CRYPTOTYPE t = static_cast<CRYPTOTYPE>(i);
      sql.str(std::string());
      sql << "CREATE TABLE IF NOT EXISTS " << DBNAME.at(t) << "(" <<
        "DATA_TD DATETIME PRIMARY KEY NOT NULL,"
        "DATA_PRICE MONEY NOT NULL"
        ");" ;
      int rc = sqlite3_exec(_db, sql.str().c_str(), &Model::callbackempty, 0,
          &err);
      if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err);
        sqlite3_free(err);
      }
    }
  }

  void Model::insertpricedb(CRYPTOTYPE t, double price)
  {
    std::stringstream sql;
    char *err;
    sql << "INSERT INTO " << DBNAME.at(t) << "(DATA_TD, DATA_PRICE) "
      "VALUES (CURRENT_TIMESTAMP, " << price << "); "
      ;
    int rc = sqlite3_exec(_db, sql.str().c_str(), &Model::callbackempty, 0,
        &err);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", err);
      sqlite3_free(err);
    }
  }
};  // namespace cwo
