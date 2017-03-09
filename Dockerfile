FROM centos:7
#MAINTAINER "your_name" <your_mail@mail.com>
RUN yum install -y git sqlite-devel boost-devel gcc gcc-c++
RUN mkdir /tmp/dalahast
RUN git clone https://github.com/zaphoyd/websocketpp.git /tmp/dalahast/ws
RUN git clone https://github.com/hn12404988/dalahast.git /home/dalahast
RUN git clone https://github.com/hn12404988/hast.git /tmp/dalahast/hast
RUN mv /tmp/dalahast/ws/websocketpp /usr/include/websocketpp
RUN mv /tmp/dalahast/hast/hast /usr/include/hast
RUN mv /home/dalahast/dalahast /usr/include/dalahast
RUN rm -rf /tmp/dalahast
RUN g++ --std=c++11 -lsqlite3 -o /home/dalahast/admin/nodes.o /home/dalahast/admin/nodes.cpp
RUN g++ --std=c++11 -lsqlite3 -o /home/dalahast/admin/ip_and_port.o /home/dalahast/admin/ip_and_port.cpp
RUN g++ --std=c++11 -lsqlite3 -o /home/dalahast/admin/init.o /home/dalahast/admin/init.cpp
