#include <review/maria_wrapper.h>
#include <review/configure.h>

using namespace std;

void check_pid(string &root, string name){
	string send,send2;
	FILE *in;
	int i;
	bool got_it {false};
	char buff[200];
	send = "ps axww | grep "+root+name;
	in = popen(send.c_str(),"r");
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
			if(got_it==false){
				cout << "Node " << name << " running with pid: " << send2 << endl;
				got_it = true;
			}
			else{
				cout << "/&&&&&&&&&&&&&&&&&&&&&&&  Replicated Nodes &&&&&&&&&&&&&&&&&&&&&&&&&&&&&/" << endl;
			}
		}
	}
	if(got_it==false){
		cout << "!!!!!!!!!!!!!!!!!!!!!!!!! Node " << name << " is not running!!!!!!!" << endl;
	}
	pclose(in);
}

int main(){
	configure conf(__FILE__);
	string root,str;
	root = conf.get_server_root();
	check_pid(root,"control.o");
	check_pid(root,"control_socket.o");
	check_pid(root,"auth.o");
	maria_wrapper sql("server",__FILE__);
	sql.build_statement();
	int i,j;
	for(j=0;j<2;++j){
		if(j==0){
			sql.send_and_error("select location from socket where server = "+conf.server);
		}
		else{
			sql.send_and_error("select location from private_socket where server = "+conf.server);
		}
		while(sql.res->next()){
			str = sql.res->getString(1);
			str = str+".o";
			check_pid(root,str);
		}
	}
	return 0;
}
