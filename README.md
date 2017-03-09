# dalahast

An framework in early stage for [hast](https://github.com/hn12404988/hast), with simple UI based on web.

## Introduction Video

* Abstraction Layer
  - [English](https://www.youtube.com/watch?v=kjUw3OXX_gw)
  - [中文](https://www.youtube.com/watch?v=RftqUrRNDNw)
* API Layer (WIP)
* Code Layer (WIP)

## Before start using this project

Dalahast is mainly designed for understanding how to use [hast](https://github.com/hn12404988/hast) right now, so I do not recommend to use this project in serious project, not to mention production environment. It doesn't mean that dalahast is completed. I'm still trying to make it as a robust framework. 

## Run on Docker

* More information in [**Run on Docker**](https://github.com/hn12404988/dalahast/wiki/Run-on-Docker) wiki page.
* You can skip **Prerequisites** and **Installing** sections.

## Prerequisites

* hast is running well.
* `sqlite-devel` and `boost-devel` is required. You can install this via package manager like:
```
yum install sqlite-devel boost-devel
apt-get install sqlite-devel boost-devel
```
* A simple web server locate root at `html` folder. If you have `python` installed in your computer, you can simply run this command under `html` folder:
```
python -m SimpleHTTPServer 8888
```
* [websocketpp](https://github.com/zaphoyd/websocketpp) is required. This is a header only C++ library.

## Installing
(I'm still learning auto-configure and make, so right now the installation is quite "hand-made")
* Copy the `dalahast` folder to your system include path.

## Initiate dalahast

* Information in [**Initiate dalahast**](https://github.com/hn12404988/dalahast/wiki/Initiate-dalahast) wiki page.

## Acknowledgments

* This project use an excellent js library called [sigma.js](https://github.com/jacomyal/sigma.js) for web rendering.
* This project use [websocketpp](https://github.com/zaphoyd/websocketpp) to receive web socket from web front-end.

## Bugs and Issues

This project is still in developed and maintained. Open an issue if you discover some problems.
