#include <review/tcp_server.h>
#include <review/client_core.h>
#include <review/control_filter.h>
#include <review/control_index.h>
#include <review/configure.h>
#include <string.h>

tcp_server server;
std::string port {"8887"};
short int max {0};
review::IS location;
short int execute_node {0};

auto execute = [&](const short int index){
	client_core client;
	control_index get_index;
	client.import_location(&location,5);
	client.set_error_node(0,__FILE__);
	short int i {0};
	short int flag;
	char first_char;
	bool to_execute;
	while(server.msg_to_args(index)==true){
		/**
		 * 
		 **/
		to_execute = false;
		if(first_char=='{'){
			get_index.extract_index_JSON(server.raw_msg[index],i);
		}
		else if(first_char=='['){
			first_char = server.raw_msg[index].back();
			if(first_char>=48 && first_char<=57){
				get_index.extract_index_ARRAY(server.raw_msg[index],i);
			}
			else{
				i = 0;
				to_execute = true;
			}
		}
		else{
			get_index.extract_index_GET(server.raw_msg[index],i);
		}
		if(i>0 && i<=max){
			flag = client.fire(i,server.raw_msg[index]);
		}
		else if(i==0 && to_execute==true){
			flag = client.fire(execute_node,server.raw_msg[index]);
		}
		else{
			server.echo_back_error(server.socketfd[index],"index is wrong: "+std::to_string(i));
			continue;
		}
		if(flag>0){
			server.raw_msg[index] = "Fail on fire which flag is: "+std::to_string(flag);
			server.echo_back_error(server.socketfd[index],server.raw_msg[index]);
		}
		else{
			if(server.raw_msg[index]==""){
				server.raw_msg[index] = "0";
			}
			server.echo_back_msg(server.socketfd[index],server.raw_msg[index]);
		}
	}
	server.done(index);
};

void init(){
	configure conf(__FILE__);
	port = conf.get_configuration("server","control_tcp_port");
	control_filter filter;
	location = filter.location;
	location.push_back(conf.get_server_root()+conf.get_configuration("server","execute_node")+".socket");
	max = location.size()-2;
	execute_node = max+1;
}

int main(int argc, char* argv[]){
	server.execute = execute;
	server.set_args(nullptr);
	init();
	if(server.init(port)==true){
		server.start_accept();
	}
	return 0;
}
