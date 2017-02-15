#include <vector>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <review/configure.h>

using namespace std;
string compiler {"g++"};

bool outside(string index,std::string root){
	bool all {false};
	string send;
	if(index=="all" || index=="auth"){
		send = compiler+" --std=c++11 -lfcgi -lfcgi++ -lmysqlcppconn -o "+root+"auth.o "+root+"auth.cpp";
		system(send.c_str());
		if(index!="all")
			return true;
	}
	if(index=="all" || index=="control"){
		send = compiler+" --std=c++11 -lmysqlcppconn -lfcgi -lfcgi++ -o "+root+"control.o "+root+"control.cpp";
		system(send.c_str());
		if(index!="all")
			return true;
	}
	if(index=="all" || index=="control_tcp"){
		send = compiler+" --std=c++11 -lmysqlcppconn -pthread -o "+root+"control_tcp.o "+root+"control_tcp.cpp";
		system(send.c_str());
		if(index!="all")
			return true;
	}
	if(index=="all" || index=="scan_node"){
		send = compiler+" --std=c++11 -lmysqlcppconn -o "+root+"scan_node.o "+root+"scan_node.cpp";
		system(send.c_str());
		if(index!="all")
			return true;
	}
	if(index=="all" || index=="data_racing"){
		send = compiler+" --std=c++11 -lmysqlcppconn -pthread -o "+root+"data_racing.o "+root+"data_racing.cpp";
		system(send.c_str());
		if(index!="all")
			return true;
	}
	/*
	if(index=="all" || index=="scan_node_sql"){
		send = compiler+" --std=c++11 -lmysqlcppconn -o "+root+"scan_node_sql.o "+root+"scan_node_sql.cpp";
		system(send.c_str());
		if(index!="all")
			return true;
	}
	*/
	if(index=="all" || index=="control_socket"){
		send = compiler+" --std=c++11 -lmysqlcppconn -lboost_system -pthread -o "+root+"control_socket.o "+root+"control_socket.cpp";
		system(send.c_str());
		if(index!="all")
			return true;
	}
	if(index=="all" || index=="start_socket"){
		send = compiler+" --std=c++11 -lmysqlcppconn -o "+root+"start_socket.o "+root+"start_socket.cpp";
		system(send.c_str());
		if(index!="all")
			return true;
	}
	if(index=="all" || index=="check_socket"){
		send = compiler+" --std=c++11 -lmysqlcppconn -o "+root+"check_socket.o "+root+"check_socket.cpp";
		system(send.c_str());
		if(index!="all")
			return true;
	}
	if(index=="all" || index=="stop_socket"){
		send = compiler+" --std=c++11 -lmysqlcppconn -o "+root+"stop_socket.o "+root+"stop_socket.cpp";
		system(send.c_str());
		if(index!="all")
			return true;
	}
	if(index=="all" || index=="reset_single_app"){
		send = compiler+" --std=c++11 -lmysqlcppconn -o "+root+"reset_single_app.o "+root+"reset_single_app.cpp";
		system(send.c_str());
		if(index!="all")
			return true;
	}
	if(index=="all" || index=="compile"){
		send = compiler+" --std=c++11 -lmysqlcppconn -o "+root+"compile.o "+root+"compile.cpp";
		system(send.c_str());
		if(index!="all")
			return true;
	}
	return all;
}

int main(int argc, char* argv[]){
	configure conf(__FILE__);
	vector<string> folder;
	FILE *in {nullptr};
	const int size {512};
	char buff[size];
	string send;
	string file;
	string root;
	string file_out;
	vector<string> escape_folder {"dev/"};
	int i {0};
	int j {0};

	root = conf.get_server_root();

	if(argc>1){
		for(i=1;i<argc;++i){
			send = argv[i];
			if(send.find('/')!=string::npos){
				file = send;
				if(file.substr(0,5)=="image"){
					if(file=="image/meme_pic"){
						send = compiler+" --std=c++11 `Magick++-config --cppflags --libs` -lfreetype -lmysqlcppconn -pthread -o "+root+file+".o "+root+file+".cpp";
					}
					else{
						send = compiler+" --std=c++11 `Magick++-config --cppflags --libs` -lmysqlcppconn -pthread -o "+root+file+".o "+root+file+".cpp";
					}
				}
				else{
					send = compiler+" --std=c++11 -lmysqlcppconn -pthread -o "+root+file+".o "+root+file+".cpp";
				}
				system(send.c_str());
			}
			else{
				if(send=="-gcc"){
					compiler = "g++";
					continue;
				}
				if(send=="-clang"){
					compiler = "clang++";
					continue;
				}
				if(send=="outside"){
					outside("all",root);
				}
				else{
					if(outside(send,root)==false){
						folder.push_back(root+argv[i]+"/");
					}
				}
			}
		}
	}
	if(argc==1 || (argc==2  && (send=="-gcc" || send=="-clang"))){
		send = "ls -d "+root+"*/";
		in = popen(send.c_str(),"r");
		while(fgets(buff, size, in)!=nullptr){
			if(strlen(buff)==1){
				break;
			}
			send = buff;
			send.pop_back();// \n
			folder.push_back(send);
		}
		outside("all",root);
	}
	
	for(i=0;i<folder.size();++i){
		j = escape_folder.size() - 1;
		for(;j>=0;--j){
			if(folder[i]==root+escape_folder[j]){
				break;
			}
		}
		if(j!=-1){
			continue;
		}
		send = "ls "+folder[i];
		if(in!=nullptr){
			pclose(in);
		}
		in = popen(send.c_str(),"r");
		while(fgets(buff, size, in)!=nullptr){
			if(strlen(buff)==1){
				break;
			}
			if(strstr(buff,".cpp")!=nullptr && strstr(buff,".cpp~")==nullptr){
				file = buff;
				file.pop_back();// \0
				file_out = file;
				file_out.pop_back();
				file_out.pop_back();
				file_out.pop_back();
				file_out.append("o");
				if(folder[i]==root+"image/"){
					if(file=="meme_pic.cpp"){
						send = compiler+" --std=c++11 `Magick++-config --cppflags --libs` -lfreetype -lmysqlcppconn -pthread -o "+folder[i]+file_out+" "+folder[i]+file;
					}
					else{
						send = compiler+" --std=c++11 `Magick++-config --cppflags --libs` -lmysqlcppconn -pthread -o "+folder[i]+file_out+" "+folder[i]+file;
					}
				}
				else{
					send = compiler+" --std=c++11 -lmysqlcppconn -pthread -o "+folder[i]+file_out+" "+folder[i]+file;
				}
				system(send.c_str());
			}
		}
	}
	if(in!=nullptr){
		pclose(in);
	}
	system("chown www:www *.o");
	system("chown -R www:www */*.o");
	system("chmod 770 *.o");
	system("chmod -R 770 */*.o");
	system("chown willy:willy *.cpp");
	system("chown -R willy:willy */*.cpp");
	system("chmod 700 *.cpp");
	system("chmod -R 700 */*.cpp");
	//system("chown willy:willy *.cpp~");
	//system("chown -R willy:willy */*.cpp~");
	//system("chmod 700 *.cpp~");
	//system("chmod -R 700 */*.cpp~");
	return 0;
}
