#include <hast/tcp_server.h>
#include <dalahast/dalahast.h>
#include <dalahast/tool/ss_tool.h>
#include <string>

tcp_server server;
std::string port {"8888"};
da::IS args {"db","query","res"};

auto execute = [&](const short int index){
	dalahast da(__FILE__);
	iss_tool _iss;
	is_tool *_is {&_iss};
	ss_tool *_ss {&_iss};
	_ss->import_fixed(&args);
	da::SS param;
	while(server.msg_recv(index)==true){
		if(server.raw_msg[index]=="test"){
			server.echo_back_msg(server.socketfd[index],"1");
			continue;
		}
		if(_ss->json_to(param,server.raw_msg[index])==false){
			server.echo_back_msg(server.socketfd[index],"0{\"Error\":\"Fail on parsing args\"}");
			continue;
		}
		if(da.db_open(da.root+"/sqlite/"+param["db"])==false){
			server.echo_back_msg(server.socketfd[index],"0{\"Error\":\"Fail on opening database\"}");
			continue;
		}
		if(param["res"]=="iss"){
			if(da.db_iss_exec(param["query"])==false){
				server.echo_back_msg(server.socketfd[index],"0{\"Error\":\"Fail on executing res query\"}");
				continue;
			}
			_iss.to_array(da.iss);
			server.echo_back_msg(server.socketfd[index],_iss.outcome);
		}
		else if(param["res"]=="is"){
			if(da.db_is_exec(param["query"])==false){
				server.echo_back_msg(server.socketfd[index],"0{\"Error\":\"Fail on executing res query\"}");
				continue;
			}
			_is->to_array(da.is);
			server.echo_back_msg(server.socketfd[index],_is->outcome);
		}
		else{
			if(da.db_exec(param["query"])==false){
				server.echo_back_msg(server.socketfd[index],"0{\"Error\":\"Fail on executing query\"}");
				continue;
			}
			server.echo_back_msg(server.socketfd[index],"1");
		}
	}
	server.done(index);
};

void log(){
	dalahast da(__FILE__);
	da.port_log(port);
}

int main(int argc, char* argv[]){
	if(argc>1){
		log();
		return 0;
	}
	server.execute = execute;
	if(server.init(port)==true){
		server.start_accept();
	}
	return 0;
}
