# dalahast

An framework in early stage for [hast](https://github.com/hn12404988/hast), with simple UI based on web.

## Before start using this project

Dalahast is mainly designed for understanding how to use [hast](https://github.com/hn12404988/hast) right now, so I do not recommend to use this project in serious project, not to mention production environment. It doesn't mean that dalahast is completed. I'm still trying to make it as a robust framework. 

## Prerequisites

* hast is running well.
* `sqlite-devel` is required. You can install this via package manager like:
```
yum install sqlite-devel
apt-get install sqlite-devel
```
* A simple web server locate root at `html` folder. If you have `php` installed in your computer, you can simply run this command under `html` folder:
```
php -S localhost:8889
```
* [websocketpp](https://github.com/zaphoyd/websocketpp) is required. This is a header only C++ library.

## Installing
(I'm still learning auto-configure and make, so right now the installation is quite "hand-made")
* Copy the `dalahast` folder to your system include path.
* Compile three admin program in `admin` folder. Running command like this:
```
g++ --std=c++11 -lsqlite3 -o init.o init.cpp && g++ --std=c++11 -lsqlite3 -o nodes.o nodes.cpp && g++ --std=c++11 -lsqlite3 -o ip_and_port.o ip_and_port.cpp
```
* Running `./init.o`, and follow the instructions. It will ask you whether this server have a `main_port`, which is the port for dalahast servers communicate with each other.
* You need a port for web-socket from browser to connect, so running `./ip_and_port.o --add-port`. It will ask you the name of the port, and the name should be `web_port`.
* Running `./nodes.o --compile` to compile all the `.cpp` file.

## Test Run for Installation

* Running `./nodes.o --log --start` to log all the nodes, and activate all the nodes.
* Make sure your web server is running under `html` folder, and open the browser with url: `http://your-url/main.html`. If your are using `php -S localhost:8889`, url is `http://localhost:8889/main.html`.
* Stop the server with `./nodes.o --stop`. Running `./nodes.o --help` and `./ip_and_port.o --help` for more information.

## Details about Dalahast

**Working in Progress**

## Acknowledgments

* This project use an excellent js library called [sigma.js](https://github.com/jacomyal/sigma.js) for web rendering.

## Bugs and Issues

This project is still in developed and maintained. Open an issue if you discover some problems.
