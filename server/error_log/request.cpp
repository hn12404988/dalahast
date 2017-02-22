#include <hast/unix_server.h>
#include <dalahast/dalahast.h>
#include <dalahast/tool/iss_tool.h>
#include <dalahast/tool/ii_tool.h>


unix_server server;
da::IS args {"up_tag"};
da::IS table {"dalahast","error","error_node_recv","request","socket"};
std::string up_string,database_location;
short int max {0};

auto execute = [&](const short int index){
	dalahast da(__FILE__);
	std::string str;
	da::II amount;
	da::SS send,param;
	iss_tool _iss;
	ss_tool *_ss {&_iss};
	ii_tool<da::II> _ii;
	short int i,target;
	send["up_tag"] = up_string;
	for(i=0;i<table.size();++i){
		amount.push_back(0);
	}
	_ss->import_fixed(&args);
	while(server.msg_recv(index)==true){
		/**
		 * 
		 **/
		if(_ss->json_to(param,server.raw_msg[index])==false){
			server.echo_back_error(server.socketfd[index],"Fail on parsing args");
			continue;
		}
		if(da.db_open(database_location)==false){
			server.echo_back_error(server.socketfd[index],"Fail on opening error_log database");
			continue;
		}
		/**
		 * 
		 **/
		if(param["up_tag"]=="init"){
			target = 0;
		}
		else{
			for(i=0;i<max;++i){
				if(param["up_tag"]==table[i]){
					target = i;
					break;
				}
			}
			if(i==max){
				server.echo_back_error(server.socketfd[index],"'up_tag' doesn't exist");
				continue;
			}
		}
		/**
		 * 
		 **/
		for(i=0;i<max;++i){
			if(i==target){
				str = "select * from "+table[i];
				if(da.db_iss_exec(str)==false){
					break;
				}
				amount[i] = da.iss.size();
				_iss.all_quote = true;
				_iss.escape_quote = true;
				_iss.to_array(da.iss);
				_iss.all_quote = false;
				_iss.escape_quote = false;
				send["content"] = _iss.outcome;
			}
			else{
				str = "select count(*) from "+table[i];
				if(da.db_is_exec(str)==false){
					break;
				}
				amount[i] = std::stoi(da.is[0]);
			}
		}
		if(i<max){
			server.echo_back_error(server.socketfd[index],"Fail on getting data from SQLite");
			continue;
		}
		_ii.to_array(amount);
		send["amount"] = _ii.outcome;
		_ss->to_json(send);
		server.echo_back_msg(server.socketfd[index],_ss->outcome);
	}
	server.done(index);
	return;
};

void log(){
	dalahast da(__FILE__);
}

void init(){
	dalahast da(__FILE__);
	is_tool _is;
	_is.set_array_separate('"');
	_is.to_array(table);
	up_string = _is.outcome;
	max = table.size();
	database_location = da.root+"/sqlite/error_log.db";
}

int main (int argc, char* argv[]){
	if(argc>1){
		log();
		return 0;
	}
	server.execute = execute;
	init();
	if(server.init(__FILE__)==true){
		server.start_accept();
		dalahast da(__FILE__);
		da.error_log("node crash");
	}
	else{
		dalahast da(__FILE__);
		da.error_log("server.init return false");
	}
	return 0;
}
