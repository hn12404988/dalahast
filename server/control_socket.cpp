#include <hast_web/wss_server.h>
#include <hast/client_core.h>
#include <dalahast/dalahast.h>

wss_server server;
std::string port;
const size_t amount {10};
da::IS location;
short int location_amount;
std::string root_prefix;

auto execute = [&](const short int index){
	client_core client;
	client.import_location(&location);
	client.set_error_node(0,__FILE__);
	short int i,j;
	char callback_index;
	std::string message,node;
	while(server.msg_recv(index)==true){
		message = server.raw_msg[index];
		callback_index = message.back();
		if(callback_index=='\0'){
			message = "0{\"Error\":\"No content\"}0";
			server.echo_back_msg(index,message);
			continue;
		}
		message.pop_back();
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
			message = "0{\"Error\":\"Fail on getting node\",\"message\":\""+message+"\"}0";
			server.echo_back_msg(index,message);
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
			message = "0{\"Error\":\"Node doesn't exist\",\"Node\":\""+node+"\"}0";
			server.echo_back_msg(index,message);
			continue;
		}
		/**
		 * 
		 **/
		if(client.fire(i,message)>0){
			message = "0{\"Error\":\"Fail on firing\"}";
			message.push_back(callback_index);
			server.echo_back_msg(index,message);
		}
		else{
			if(message==""){
				message = "0{\"Error\":\"Empty reply\"}";
			}
			message.push_back(callback_index);
			server.echo_back_msg(index,message);
		}
	}
	server.done(index);
	return;
};

bool init(){
	dalahast da(__FILE__);
	std::string str;
	int j;
	j = da.my_server_id();
	if(j==-1){
		return false;
	}
	port = da.get_port(j,"web_port");
	root_prefix = da.root+da::server_prefix;
	if(da.db_open(da.root+"/sqlite/info.db")==false){
		return false;
	}
	str = "select node from node WHERE private = 0 AND interface = 'FILE'";
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

void log(){
	dalahast da(__FILE__);
	short int i;
	i = da.my_server_id();
	port = da.get_port(i,"web_port");
	da.port_log(port);
}

int main (int argc, char* argv[]){
	if(argc>1){
		log();
		return 0;
	}
	if(init()==false){
		dalahast da(__FILE__);
		da.error_log("init() return false");
		return 0;
	}
	server.execute = execute;
	if(server.init("/home/pi/tls/nginx.crt","/home/pi/tls/nginx.key",port,3)==true){
		server.start_accept();
	}
	return 0;
}
