# api-client
Native cpp based client for the API


## Building on macOS

macOS 10.11 and newer deliver openssl already, but cmake got trouble finding its path, so use the following commands to build:

```
mkdir -p build
cd build
OPENSSL_ROOT_DIR=/usr/local/opt/openssl/ cmake ..
```

This should work as expected.
