#include <dalahast/dalahast.h>
#include <string.h>

const short int buff_size {300};
std::string root;
short int root_len;

namespace da{
	const char start {'S'};
	const char stop {'s'};
	const char check {'c'};
	const char reset {'r'};
	const char compile {'C'};
}

void kill_pid(std::string name){
	FILE *in {nullptr};
	char buff[buff_size];
	int i;
	std::string send,send2;
	bool got_it {false};
	send = "ps axww | grep "+name+".o";
	in = popen(send.c_str(),"r");
	if(in==nullptr){
		std::cout << "Fail on 'ps axww' command" << std::endl;
		return;
	}
	send.clear();
	while(fgets(buff, buff_size, in)!=nullptr){
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
			std::cout << "Kill " << name.substr(root_len) << " while pid id: " << send2 << std::endl;
			send2 = "kill -9 "+send2;
			system(send2.c_str());
			got_it = true;
		}
	}
	if(got_it==false){
		std::cout << "Node " << name.substr(root_len) << " is not running." << std::endl;
	}
	pclose(in);
}

void grab_all(da::IS &list){
	FILE *in {nullptr};
	char buff[buff_size];
	std::string str;
	str = "find "+root+" -name \"*.cpp\"";
	in = popen(str.c_str(),"r");
	if(in==nullptr){
		return;
	}
	while(fgets(buff, buff_size, in)!=nullptr){
		if(strlen(buff)==1){
			break;
		}
		str = buff;
		str.pop_back();// \n
		str.pop_back();
		str.pop_back();
		str.pop_back();
		str.pop_back();
		list.push_back(str);
	}
	pclose(in);
}

void compile(std::string name){
	std::string str;
	if(name==root+"control_socket"){
		str = "g++ --std=c++11 -lsqlite3 -pthread -lboost_system -o "+name+".o "+name+".cpp";
	}
	else{
		str = "g++ --std=c++11 -lsqlite3 -pthread -o "+name+".o "+name+".cpp";
	}
	system(str.c_str());
}

void check_pid(std::string name){
	std::string send,send2;
	FILE *in {nullptr};
	int i;
	bool got_it {false};
	char buff[buff_size];
	send = "ps axww | grep "+name+".o";
	in = popen(send.c_str(),"r");
	while(fgets(buff, buff_size, in)!=nullptr){
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
			if(got_it==false){
				std::cout << "Node " << name.substr(root_len) << " running with pid: " << send2 << std::endl;
				got_it = true;
			}
			else{
				std::cout << "/&&&&&&&&&&&&  Replicated Nodes &&&&&&&&&&&/" << std::endl;
				std::cout << "Node " << name.substr(root_len) << " running with pid: " << send2 << std::endl;
			}
		}
	}
	if(got_it==false){
		std::cout << "!!!!!!!!!!! Node " << name.substr(root_len) << " is not running !!!!!!!" << std::endl;
	}
	pclose(in);
}

void help(){
	std::cout << "/***************** Manual *****************/" << std::endl;
	std::cout << "[--log] [--compile || --start || --stop || --reset || --check] [--node_name]" << std::endl;
	std::cout << "[--log]: Optional" << std::endl;
	std::cout << "[--node_name]: Multiple and Optional. Default is all nodes" << std::endl;
}

int main (int argc, char* argv[]){
	dalahast da;
	std::string str;
	int i,j;
	da::IS list;
	bool log {false};
	char execute {'n'};
	
	root = da.root+da::server_prefix;
	root_len = root.length();
	for(i=1;i<argc;++i){
		str = argv[i];
		if(i==1 && str=="--help"){
			help();
			break;
		}
		if(str=="--log"){
			log = true;
		}
		else if(str=="--start"){
			if(execute=='n'){
				execute = da::start;
			}
			else{
				std::cout << "Too much args" << std::endl;
				return 0;
			}
		}
		else if(str=="--stop"){
			if(execute=='n'){
				execute = da::stop;
			}
			else{
				std::cout << "Too much args" << std::endl;
				return 0;
			}
		}
		else if(str=="--check"){
			if(execute=='n'){
				execute = da::check;
			}
			else{
				std::cout << "Too much args" << std::endl;
				return 0;
			}
		}
		else if(str=="--reset"){
			if(execute=='n'){
				execute = da::reset;
			}
			else{
				std::cout << "Too much args" << std::endl;
				return 0;
			}
		}
		else if(str=="--compile"){
			if(execute=='n'){
				execute = da::compile;
			}
			else{
				std::cout << "Too much args" << std::endl;
				return 0;
			}
		}
		else{
			str = root+str;
			list.push_back(str);
		}
	}
	if(i<argc){
		return 0;
	}
	/**
	 * 
	 **/
	if(list.size()==0){
		grab_all(list);
		if(list.size()==0){
			std::cout << "Fail on getting all .cpp node" << std::endl;
			return 0;
		}
	}
	/**
	 * 
	 **/
	j = list.size();
	for(i=0;i<j;++i){
		if(execute==da::start){
			if(log==true){
				str = list[i]+".o log";
				system(str.c_str());
			}
			str = list[i]+".o &";
			system(str.c_str());
		}
		else if(execute==da::stop){
			kill_pid(list[i]);
		}
		else if(execute==da::check){
			check_pid(list[i]);
		}
		else if(execute==da::reset){
			kill_pid(list[i]);
			compile(list[i]);
			if(log==true){
				str = list[i]+".o log";
				system(str.c_str());
			}
			str = list[i]+".o";
			system(str.c_str());
		}
		else if(execute==da::compile){
			compile(list[i]);
			if(log==true){
				str = list[i]+".o log";
				system(str.c_str());
			}
		}
	}
	return 0;
}
