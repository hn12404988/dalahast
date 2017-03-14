FROM centos:7
#MAINTAINER "your_name" <your_mail@mail.com>
RUN yum update -y && \
yum install -y git sqlite-devel boost-devel gcc gcc-c++ && \
mkdir /tmp/dalahast && \
git clone https://github.com/zaphoyd/websocketpp.git /tmp/dalahast/ws && \
git clone https://github.com/hn12404988/dalahast.git /home/dalahast && \
git clone https://github.com/hn12404988/hast.git /tmp/dalahast/hast && \
mv /tmp/dalahast/ws/websocketpp /usr/include/websocketpp && \
mv /tmp/dalahast/hast/hast /usr/include/hast && \
mv /home/dalahast/dalahast /usr/include/dalahast && \
rm -rf /tmp/dalahast && \
g++ --std=c++11 -lsqlite3 -o /home/dalahast/admin/nodes.o /home/dalahast/admin/nodes.cpp && \
g++ --std=c++11 -lsqlite3 -o /home/dalahast/admin/ip_and_port.o /home/dalahast/admin/ip_and_port.cpp && \
g++ --std=c++11 -lsqlite3 -o /home/dalahast/admin/init.o /home/dalahast/admin/init.cpp
