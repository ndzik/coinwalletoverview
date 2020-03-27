#define BOOST_TEST_MODULE Wallettest
#include <chrono>
#include <thread>
#include <boost/test/unit_test.hpp>
#include "walletfactory.hpp"

using namespace cwo;

BOOST_AUTO_TEST_CASE(updatewallet)
{
  Walletfactory wf;
  Wallet* w = wf.cryptotype(VET)
    .address("<addr3>")
    .url("https://explore.vechain.org/accounts/<addr3>")
    .build();
}
