# CoinWalletOverview

## Overview

![cwo1](/uploads/cd7ae033b0d4e302c435f944ff0bba82/cwo1censored.png)

CoinWalletOverview (CWO) is a program which runs in a command-line. It displays
graphs of price histories for selected cryptocurrencies tied to registered
crypto-wallets.
It also updates the estimated value for your coins in a currency of your choice.

## Used Libraries, Third-Party and Language

* Language: **C++**
* Tools:
  * **Ninja**
  * **CMake**
* Libraries and Third-Party:
  * **NCurses**
  * **Curl**
  * **SQLite3**
  * **https://github.com/nlohmann/json**
  * **CoinMarketCap-API**
  * **Blockchaininfo-API**

## Supported Cryptos (for now and to be extended on request)
* **Bitcoin** Mother of Cryptos
* **Ethereum**
* **VeChain**
* more to come...
 
## Build & Install
```
    git clone https://gitlab.com/nkdzeus/coinwalletoverview
    cd coinwalletoverview
```
For your default System do:
```
    mkdir -p build/default
    cd build/default
    cmake ../../ -GNinja
    ninja cwo
```
If you want to cross-compile CWO and have a working cross-compile toolchain
simply branch from above instruction:
```
    mkdir -p build/xcp
    cd build/xcp
    cmake -DCMAKE_TOOLCHAIN_FILE=../../toolchain-rpi.cmake ../../ -GNinja
    ninja cwo
```
Now copy resulting binary (cwo) to your rpi and let it run or boot, or however
you prefer (:

## Usage/Working
First a configuration-file has to be provided. The configuration should be
placed in '$HOME/.config/cwo/state.json'. Example template:
```json
{
  "wallets" : [
    {"crypto" : "ETH", "address" : "<your-addr>"},
    {"crypto" : "VET", "address" : "<your-addr>"},
    {"crypto" : "BTC", "address" : "<your-addr>"}

  ],
  "apikeys" : {
    "coinbase" : "your coinbase api-key"
  },
  "blockcypher" : {
    "token" : "your blockcypher api-key/token"
  },
  "investments" : [
    {"crypto" : "ETH", "amount" : 5000, "currency" : "EUR"},
    {"crypto" : "BTC", "amount" : 0, "currency" : "EUR"},
    {"crypto" : "VET", "amount" : 200, "currency" : "EUR"}
  ],
  "updateinterval" : 5
}

```
**Why do I have to provide my own API-Keys?**
**Now supporting Coingecko-API!**

Especially coinbase is very restrictive with their free api version. They
restrict you to ~300 API-Calls/Day. This is also the reason the maximum
frequency of api-queries for Coinbase is every 5 minutes.

CWO is meant to be used on a Raspberry Pi. CWO can obvioulsy also be used on a
desktop machine.
When being run, CWO will create a database in '$HOME/.config/cwo/graph.db'.
Each registered wallet will be updated in a frequency of 1 min.

## Future

* Upcoming features:
    * Custom graph.db location via configuration-file
    * More Cryptotypes...

* General:
    * Better cross-compile guide with every resource necessary

Working on it whenever there is time ;D

## Misc

**Adaptive in size:**

![cwoeachownline](/uploads/f29439c21465591a5c6893634fe7f7b6/cwoeachownlinecensored.png)

**Switch timeintervals:**

![cwov1graphsingle5min](/uploads/05efcadbca5f822526d41733c951adcc/cwov1graphsingle5min.png)
![cwov1graphsingle1hour](/uploads/dd56e5474cbf588322ed59bd5b973c92/cwov1graphsingle1hour.png)
![cwov1graphsingle1day](/uploads/c179c42f748eb8e0edbc5450baa101e5/cwov1graphsingle1day.png)

If enough datapoints are gathered and your display is wide enough, you can
optionally switch of graph filling:

![cwov1graphunfilled](/uploads/aee835c4bb901ecad244365b6eca3a11/cwov1graphunfilled.png)
