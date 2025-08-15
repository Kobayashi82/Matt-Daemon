# TODO

- Mejores logs
- Cliente
- Compile .hpp

## Client

- client user@host -> usa "user", "host" y puerto por defecto (4242)
- client host -> usa usuario actual, "host" y puerto por defecto (4242)
- client -u user -p port host -> usa "user", "port" y "host"
- client -u user -p port user@host -> error multiple user (solo si user es diferente a user@)

- -l user --login=user (default usuario actual)
- -p port --port=num (default 4242)
- -k --insecure (permita transferencias en texto plano)

- -h, -? --help
- --usage
- -V --version
