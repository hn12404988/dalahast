#include <hast/unix_server.h>
#include <hast/client_core.h>
#include <dalahast/dalahast.h>
#include <dalahast/tool/iss_tool.h>

unix_server server;
da::IS args {"already"}; //It's a string a array form. Start from '[]'
da::IS location;
std::string my_server_id;
std::string database_location;

auto execute = [&](const short int index){
	dalahast da(__FILE__);
	client_core  client;
	client.import_location(&location);
	client.set_error_node(0,__FILE__);
	da::SS send,param,tmp;
	da::SS::iterator it;
	da::SS::iterator it_end;
	da::IS already;
	iss_tool _iss;
	ss_tool *_ss {&_iss};
	is_tool *_is {&_iss};
	std::string str;
	short int i,j;
	while(server.msg_recv(index)==true){
		/**
		 * 
		 **/
		_ss->import_fixed(&args);
		if(_ss->json_to(param,server.raw_msg[index])==false){
			server.echo_back_error(server.socketfd[index],"Fail on parsing args");
			continue;
		}
		_ss->import_fixed(nullptr);
		if(_is->array_to(already,param["already"])==false){
			server.echo_back_error(server.socketfd[index],"Fail on parsing already");
			continue;
		}
		send.clear();
		if(da.db_open(database_location)==false){
			server.echo_back_error(server.socketfd[index],"Fail on opening topology database");
			continue;
		}
		/**
		 * 
		 **/
		i = already.size()-1;
		for(;i>=0;--i){
			if(already[i]==my_server_id){
				break;
			}
		}
		if(i==-1){
			str = "select * from topology";
			if(da.db_iss_exec(str)==false){
				server.echo_back_error(server.socketfd[index],"Fail on getting topology info");
				continue;
			}
			if(da.iss.size()>0){
				_iss.to_array(da.iss);
				send[my_server_id] = _iss.outcome;
			}
			already.push_back(my_server_id);
		}
		/**
		 * 
		 **/
		i = location.size()-1;
		for(;i>0;--i){
			_is->to_array(already);
			str = "share_info/topology_info{\"already\":"+_is->outcome+"}";
			if(client.fire(i,str)>0){
				break;
			}
			if(str[0]=='0' || str==""){
				break;
			}
			if(_ss->json_to(tmp,str)==false){
				break;
			}
			it = tmp.begin();
			it_end = tmp.end();
			for(;it!=it_end;++it){
				j = already.size()-1;
				for(;j>=0;--j){
					if(already[j]==it->first){
						break;
					}
				}
				if(j==-1){
					already.push_back(it->first);
					send[it->first] = it->second;
				}
			}
		}
		if(i>0){
			server.echo_back_error(server.socketfd[index],"Fail on getting data from other server");
			continue;
		}
		/**
		 * 
		 **/
		_ss->to_json(send);
		server.echo_back_msg(server.socketfd[index],_ss->outcome);
	}
	server.done(index);
	return;
};

bool build_location(dalahast &da, da::IsH *fire = nullptr){
	int i;
	is_tool _is;
	if(da.all_main_port()==false){
		da.error_log("all_main_port() return false");
		return false;
	}
	location.push_back(da.root+da::server_prefix+"private/error_node.socket");
	if(fire!=nullptr){
		fire->push_back(da::fire);
	}
	i = da.iss.size()-1;
	for(;i>=0;--i){
		if((*da.iss[i])["server_id"]==my_server_id){
			continue;
		}
		location.push_back((*da.iss[i])["ip"]+":"+(*da.iss[i])["port"]);
		if(fire!=nullptr){
			fire->push_back(da::fireNstore);
		}
	}
	i = location.size();
	_is.delete_repeat(location);
	if(i!=location.size()){
		da.error_log("location repeat");
		return false;
	}
	return true;
}

void log(){
	dalahast da(__FILE__);
	da::IsH fire;
	if(build_location(da,&fire)==true){
		da.location_log(location,fire);
	}
}

bool init(){
	dalahast da(__FILE__);
	short int i;
	is_tool _is;
	database_location = da.root+"/sqlite/topology.db";
	i  = da.my_server_id();
	if(i==-1){
		return false;
	}
	my_server_id = std::to_string(i);
	if(build_location(da)==false){
		return false;
	}
	return true;
}

int main (int argc,char* argv[]){
	if(argc>1){
		log();
		return 0;
	}
	server.execute = execute;
	if(init()==true){
		if(server.init(__FILE__)==true){
			server.start_accept();
		}
		else{
			dalahast da(__FILE__);
			da.error_log("server.init return false");
			return 0;
		}
		dalahast da(__FILE__);
		da.error_log("server crash");
		return 0;
	}
	else{
		dalahast da(__FILE__);
		da.error_log("init() return false");
		return 0;
	}
	return 0;

}
