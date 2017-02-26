# dalahast

An framework in early stage for [hast](https://github.com/hn12404988/hast), with simple UI based on web.

## Before start using this project

Dalahast is mainly designed for understanding how to use [hast](https://github.com/hn12404988/hast) right now, so I do not recommend to use dalahast in serious project, not to mention production environment. It doesn't mean that dalahast is completed. I'm still trying to make it as a robust framework. 

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

## Bugs and Issues thi si

This project is still in developed and maintained. Open an issue if you discover some problems.
