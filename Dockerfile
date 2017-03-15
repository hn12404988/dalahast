FROM centos:7
#MAINTAINER "your_name" <your_mail@mail.com>
RUN yum update -y && \
yum install -y git sqlite-devel gcc gcc-c++ openssl-devel && \
mkdir /tmp/dalahast && \
git clone https://github.com/hn12404988/dalahast.git /home/dalahast && \
git clone https://github.com/hn12404988/hast_web.git /tmp/dalahast/hast_web && \
git clone https://github.com/hn12404988/hast.git /tmp/dalahast/hast && \
mv /tmp/dalahast/hast_web/hast_web /usr/include/hast_web && \
mv /tmp/dalahast/hast/hast /usr/include/hast && \
mv /home/dalahast/dalahast /usr/include/dalahast && \
rm -rf /tmp/dalahast && \
mkdir /home/dalahast/sqlite && \
g++ --std=c++11 -lsqlite3 -o /home/dalahast/admin/nodes.o /home/dalahast/admin/nodes.cpp && \
g++ --std=c++11 -lsqlite3 -o /home/dalahast/admin/ip_and_port.o /home/dalahast/admin/ip_and_port.cpp && \
g++ --std=c++11 -lsqlite3 -o /home/dalahast/admin/init.o /home/dalahast/admin/init.cpp
