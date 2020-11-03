# CoinWalletOverview

## Overview

![cwov1](https://user-images.githubusercontent.com/33512740/97795084-4cdd0e80-1c02-11eb-8c3e-8e61b9541003.png)

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
  * Now powered by **Coingecko-API** (awesome service)
  * **Blockchaininfo-API**

## Supported Cryptos (for now and to be extended on request)
* **Bitcoin** Mother of Cryptos
* **Ethereum**
* **VeChain**
* more to come on demand...
 
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
CWO is meant to be used on a Raspberry Pi but is not limited to that.
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

![cwov1eachownline](https://user-images.githubusercontent.com/33512740/97795087-4e0e3b80-1c02-11eb-84ff-933c1eaa2559.png)

**Switch timeintervals:**

![cwov1graphsingle5min](https://user-images.githubusercontent.com/33512740/97795092-4fd7ff00-1c02-11eb-8b55-8fa74b073d89.png)
![cwov1graphsingle1hour](https://user-images.githubusercontent.com/33512740/97795091-4f3f6880-1c02-11eb-92c0-22a036157ab7.png)
![cwov1graphsingle1day](https://user-images.githubusercontent.com/33512740/97795090-4ea6d200-1c02-11eb-8557-ca9ae3896c0e.png)

If enough datapoints are gathered and your display is wide enough, you can
optionally switch of graph filling:

![cwov1graphunfilled](https://user-images.githubusercontent.com/33512740/97795093-4fd7ff00-1c02-11eb-9a9e-f3d85733a237.png)
