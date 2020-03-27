#include <boost/test/tools/old/interface.hpp>
#include <chrono>
#include <thread>
#define BOOST_TEST_MODULE Modeltest
#include <boost/test/unit_test.hpp>
#include "model.hpp"

using namespace cwo;

const std::string eth01 = "<addr1>";
const std::string eth02 = "<addr2>";
const std::string vet01 = "<addr3>";

struct fix {
  fix()
  {
    m.apikey("<api-key>");
    m.registerwallet(VET, vet01);
    m.registerwallet(ETH, eth01);
    m.registerwallet(ETH, eth02);
  }
  Model m;
};

BOOST_AUTO_TEST_CASE(ctors)
{
  Model m;
}

BOOST_FIXTURE_TEST_CASE(registerwallets, fix)
{
  std::vector<Wallet*> v = m.walletsof(ETH);
  BOOST_CHECK_EQUAL(v[0]->address(), eth01);
  BOOST_CHECK_EQUAL(v[1]->address(), eth02);
  v = m.walletsof(VET);
  BOOST_CHECK_EQUAL(v[0]->address(), vet01);
}

BOOST_FIXTURE_TEST_CASE(unregisterwallets, fix)
{
  std::vector<Wallet*> v = m.walletsof(VET);
  BOOST_CHECK_EQUAL(v[0]->address(), vet01);
  m.unregisterwallet(VET, vet01);
  v = m.walletsof(VET);
  BOOST_CHECK_EQUAL(v.empty(), true);

  /* unregister nonexisting wallet */
  m.unregisterwallet(BTC, "0xdeadbeef");
  v = m.walletsof(ETH);
  BOOST_CHECK_EQUAL(v[0]->address(), eth01);
  BOOST_CHECK_EQUAL(v[1]->address(), eth02);
}

BOOST_FIXTURE_TEST_CASE(runinanotherthreadandregisterunregisterwallets, fix)
{
  std::thread th([&] () {
        m.run();
      });
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  m.stop();
  th.join();
}

BOOST_FIXTURE_TEST_CASE(setinvestmentwhilerunning, fix)
{
  std::thread th([&] () {
      m.run();
      });
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  m.investment(ETH, 5300);
  m.investment(VET, 350);
  m.stop();
  th.join();
}

BOOST_FIXTURE_TEST_CASE(updateintervalwhilerunning, fix)
{
  std::thread th([&] () {
      m.run();
      });
  m.updateinterval(200);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  m.updateinterval(100);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  m.stop();
  th.join();
}
