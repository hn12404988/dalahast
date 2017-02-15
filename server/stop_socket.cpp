#include <string.h>
#include <review/maria_wrapper.h>
#include <review/configure.h>

using namespace std;

void kill_pid(string &root,string name){
	FILE *in;
	char buff[200];
	int i;
	string send,send2;
	bool got_it {false};
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
			got_it = true;
		}
	}
	if(got_it==false){
		cout << "Node " << name << " is not running." << endl;
	}
	pclose(in);
}

int main(){
	maria_wrapper sql("server",__FILE__);
	sql.build_statement();
	configure conf(__FILE__);
	string name[] {"control.o","control_socket.o","auth.o"};
	size_t i;
	size_t j {sizeof(name)/sizeof(*name)};
	string root,str;
	root = conf.get_server_root();

	for(i=0;i<j;++i){
		kill_pid(root,name[i]);
	}
	for(i=0;i<2;++i){
		if(i==0){
			sql.send_and_error("select location from socket where server = "+conf.server);
		}
		else{
			sql.send_and_error("select location from private_socket where server = "+conf.server);
		}
		while(sql.res->next()){
			str = sql.res->getString(1)+".o";
			kill_pid(root,str);
		}
	}
	return 0;
}
