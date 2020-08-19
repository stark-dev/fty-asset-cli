# fty-asset-test

A simple tool to perform CRUD operations on assets.

The tool prints both the message sent and the response from `fty-asset`.

## Build
```
mkdir build && cd build
cmake ..
make
```

## Run
```
./asset-test <OPERATION> [PARAMS, ...]
```

## Available operations
```
CREATE      : ./fty-asset-cli CREATE asset.json                  # create new asset
UPDATE      : ./fty-asset-cli UDPATE asset.json                  # update existing asset
DELETE      : ./fty-asset-cli DELETE asset-name [asset-name ...] # delete one or more assets
GET         : ./fty-asset-cli GET asset-name                     # get asset
GET_BY_UUID : ./fty-asset-cli GET uuid                           # get asset
LIST        : ./fty-asset-cli LIST                               # list all asset names
```

## Dependencies
Package `fty-cmake` must be installed before running `cmake`

```
cxxtools
fty_asset
fty_common_logging
fty_common_messagebus
pthread
stdc++fs
```

__NOTE__: `stdc++fs` library is available only on Debian 10
