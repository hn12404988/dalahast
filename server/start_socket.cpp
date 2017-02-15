#include <iostream>
#include <chrono>
#include <thread>
#include <review/maria_wrapper.h>
#include <review/configure.h>

using namespace std;

void exec(std::string file,std::string root, bool ch = true){
	cout << "Starting Node: " << file << endl;
	string send;
	send = "sudo -u www  "+root+file+".o &";
	system(send.c_str());
	if(ch==true){
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		send = "chown www:www "+root+file+".socket";
		system(send.c_str());
		send = "chmod 770 "+root+file+".socket";
		system(send.c_str());
	}
	cout << "" << endl;
}

void fcgi(std::string file,std::string root){
	cout << "Starting Node: " << file << endl;
	string send;
	send = "sudo -u www spawn-fcgi -s "+root+file+".socket -f "+root+file+".o";
	system(send.c_str());
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	send = "chown www:www "+root+file+".socket";
	system(send.c_str());
	send = "chmod 770 "+root+file+".socket";
	system(send.c_str());
	cout << "" << endl;
}

int main(){
	const string server {"3"};
	int i;
	string root,send;
	maria_wrapper sql("server",__FILE__);
	sql.build_statement();
	configure conf(__FILE__);
	root = conf.get_server_root();
	fcgi("auth",root);
	fcgi("control",root);
	cout << "" << endl;
	send = "sudo -u www "+root+"control_socket.o &";
	system(send.c_str());
	cout << "control_socket run on port ";
	system("pidof control_socket.o");
	cout << "" << endl;
	for(i=0;i<2;++i){
		if(i==0){
			sql.send_and_error("select location from socket where server = "+server);
		}
		else{
			sql.send_and_error("select location from private_socket where server = "+server);
		}
		while(sql.res->next()){
			send = sql.res->getString(1);
			if(send.find("/")==std::string::npos){
				exec(sql.res->getString(1),root,false);
			}
			else{
				exec(sql.res->getString(1),root);
			}
		}
	}
	return 0;
}
