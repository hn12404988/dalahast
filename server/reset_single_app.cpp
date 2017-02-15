#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <review/configure.h>

using namespace std;

void kill_pid(string &root,string &name){
	FILE *in;
	char buff[200];
	int i;
	string send,send2;
	send = "ps axww | grep "+root+name;
	in = popen(send.c_str(),"r");
	send.clear();
	while(fgets(buff, 200, in)!=nullptr){
		send = buff;
		i = 0;
		if(send.find(" sudo ")==std::string::npos && send.find(" grep ")==std::string::npos){
			send2.clear();
			while(send[0]==' '){
				send = send.substr(1);
			}
			while(send[i]!=' '){
				send2 += send[i];
				++i;
			}
			cout << "Kill " << name << " while pid id: " << send2 << endl;
			send2 = "kill -9 "+send2;
			system(send2.c_str());
		}
	}
	pclose(in);
}

void after_compile_exec(std::string file,std::string root, bool ch = true){
	string send;
	send = "chown www:www "+root+file+".o";
	system(send.c_str());
	send = "chmod 770 "+root+file+".o";
	system(send.c_str());
	send = "sudo -u www "+root+file+".o &";
	system(send.c_str());
	if(ch==true){
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		send = "chown www:www "+root+file+".socket";
		system(send.c_str());
		send = "chmod 770 "+root+file+".socket";
		system(send.c_str());
	}
}

void after_compile_fcgi(std::string file, std::string root){
	string send;
	send = "chown www:www "+root+file+".o";
	system(send.c_str());
	send = "chmod 770 "+root+file+".o";
	system(send.c_str());
	send = "sudo -u www spawn-fcgi -F 1 -s "+root+file+".socket -f "+root+file+".o";
	system(send.c_str());
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	send = "chown www:www "+root+file+".socket";
	system(send.c_str());
	send = "chmod 770 "+root+file+".socket";
	system(send.c_str());
}

void outside(std::string target, std::string root){
	string send;
	if(target=="auth"){
		send = "g++ --std=c++11 -lfcgi -lfcgi++ -lmysqlcppconn -o "+root+"auth.o "+root+"auth.cpp";
		system(send.c_str());
		after_compile_fcgi(target,root);
	}
	else if(target=="control"){
		send = "g++ --std=c++11 -lmysqlcppconn -lfcgi -lfcgi++ -o "+root+"control.o "+root+"control.cpp";
		system(send.c_str());
		after_compile_fcgi(target,root);
	}
	else if(target=="control_socket"){
		send = "g++ --std=c++11 -lmysqlcppconn -lboost_system -pthread -o "+root+"control_socket.o "+root+"control_socket.cpp";
		system(send.c_str());
		send = "chown www:www "+root+"control_socket.o";
		system(send.c_str());
		send = "chmod 770 "+root+"control_socket.o";
		system(send.c_str());
		send = "sudo -u www "+root+"control_socket.o &";
		system(send.c_str());
		cout << "control_socket run on port ";
		system("pidof control_socket.o");
	}
	else if(target=="data_racing"){
		send = "g++ --std=c++11 -lmysqlcppconn -pthread -o "+root+"data_racing.o "+root+"data_racing.cpp";
		system(send.c_str());
		after_compile_exec(target,root,false);
	}
}

int main(int argc, char* argv[]){
	string file;
	string file_out;
	string send;
	string root;
	int i {0};
	configure conf(__FILE__);
	root = conf.get_server_root();
	if(argc>1){
		file = argv[1];
		i = file.find("/");
		if(i!=std::string::npos){
			file_out = file+".o";
			kill_pid(root,file_out);
			send = "g++ --std=c++11 -lmysqlcppconn -pthread -o "+root+file_out+" "+root+file+".cpp";
			system(send.c_str());
			after_compile_exec(file,root);
		}
		else{
			file_out = file+".o";
			kill_pid(root,file_out);
			outside(file,root);
		}
	}
	return 0;
}
