# My HTTP server

Simple HTTP server for science.

```
# Build or clean
$ make
$ make clean

# Run
$ ./my-http

# Test
$ curl -i 127.0.0.1:8080
# or
$ curl -v http://127.0.0.1:8080/index.html
# useful options:
--path-as-is
```