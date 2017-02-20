#include <hast/tcp_server.h>
#include <hast/client_core.h>
#include <dalahast/dalahast.h>

tcp_server server;
std::string port;
short int location_amount;
da::IS location;
std::string root_prefix;

auto execute = [&](const short int index){
	client_core client;
	client.import_location(&location,5);
	client.set_error_node(0,__FILE__);
	int j;
	short int i;
	std::string node,message;
	while(server.msg_recv(index)==true){
		message = server.raw_msg[index];
		/**
		 * 
		 **/
		j = message.length();
		for(i=0;i<j;++i){
			if(message[i]=='{'){
				break;
			}
		}
		if(i==j){
			message = "Fail on getting node";
			server.echo_back_error(server.socketfd[index],message);
			continue;
		}
		/**
		 * 
		 **/
		node = message.substr(0,i);
		node = root_prefix+node+".socket";
		message = message.substr(i);
		for(i=1;i<location_amount;++i){
			if(location[i]==node){
				break;
			}
		}
		if(i==location_amount){
			message = "Node doesn't exist";
			server.echo_back_error(server.socketfd[index],message);
			continue;
		}
		/**
		 * 
		 **/
		if(client.fire(i,message)>0){
			message = "Fail on firing";
			server.echo_back_error(server.socketfd[index],message);
		}
		else{
			if(message==""){
				message = "Empty reply";
				server.echo_back_error(server.socketfd[index],message);
			}
			else{
				server.echo_back_msg(server.socketfd[index],message);
			}
		}
	}
	server.done(index);
};

void log(){
	dalahast da(__FILE__);
	da.port_log("main_port");
}

bool init(){
	dalahast da(__FILE__);
	std::string str;
	int j;
	j = da.my_server_id();
	if(j==-1){
		return false;
	}
	port = da.get_port(da.my_server_id(),"main_port");
	if(port==""){
		return false;
	}
	root_prefix = da.root+da::server_prefix;
	if(da.db_open(da.root+"/sqlite/info.db")==false){
		return false;
	}
	str = "select node from node where interface = 'FILE' and 'private' = 0";
	if(da.db_is_exec(str)==false){
		return false;
	}
	location.push_back(da.root+da::server_prefix+"private/error_node.socket");
	j = da.is.size()-1;
	for(;j>=0;--j){
		location.push_back(da.root+da::server_prefix+da.is[j]+".socket");
	}
	location_amount = location.size();
	return true;
}

int main(int argc, char* argv[]){
	if(argc>1){
		log();
		return 0;
	}
	server.execute = execute;
	if(init()==true){
		if(server.init(port)==true){
			server.start_accept();
		}
		dalahast da(__FILE__);
		da.error_log("Node crash");
	}
	else{
		dalahast da(__FILE__);
		da.error_log("init() return false");
	}
	return 0;
}
