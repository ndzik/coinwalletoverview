#define BOOST_TEST_MODULE cmc_test
#include <boost/test/unit_test.hpp>
#include "coingecko.hpp"

using namespace cwo;

BOOST_AUTO_TEST_CASE(request)
{
  CoinGecko cg;

  cg.crypto({ETH, VET, BTC})->currency(USD)->update();
}
