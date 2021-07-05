[![docker](https://img.shields.io/docker/pulls/gongt/alpine-init.svg)](https://hub.docker.com/r/gongt/alpine-init)
[![docker](https://img.shields.io/docker/pulls/gongt/glibc-init.svg)](https://hub.docker.com/r/gongt/glibc-init)

# init

Extreme simple `/sbin/init` for `alpine` (or anything) in a container.

Features:
* start multiple process
* ensure their stdout/stderr does not get jumbled together
* quit (or, "shutdown system") when:
	* any child process quit
	* receive SIGINT
* on quit, send signal to all process, and *(infinity)* wait then to quit
* **no and never have any other feature!!**

# Config file

> /etc/programs

```text
[php-fpm]
arg[]:/opt/prepare-and-run-php-fpm.sh
signal:int

[nginx]
arg[]:/usr/sbin/nginx
arg[]:-g
arg[]:daemon: off;
signal:int

# comment

```
