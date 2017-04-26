#include <hast/unix_server.hpp>
#include <hast/client_core.hpp>
#include <dalahast/dalahast.hpp>
#include <dalahast/tool/ss_tool.hpp>

unix_server server;
da::IS args {"node_id"}; // 'server_index' + '_' + 'node_name'
da::IS location;
da::IS server_index;
std::string my_server_id;
std::string database_location;

auto execute = [&](const short int index){
	dalahast da(__FILE__);
	client_core client;
	client.import_location(&location);
	client.set_error_node(0,__FILE__);
	ss_tool _ss;
	_ss.import_fixed(&args);
	_ss.all_quote = true;
	da::SS param,ss;
	std::string str,server_id,node;
	short int i;
	while(server.msg_recv(index)==true){
		if(_ss.json_to(param,server.raw_msg[index])==false){
			server.echo_back_error(index,"Fail on parsing args");
			continue;
		}
		i = param["node_id"].find("_");
		if(i==std::string::npos){
			server.echo_back_error(index,"Fail on parsing node_id");
			continue;
		}
		server_id = param["node_id"].substr(0,i);
		node = param["node_id"].substr(i+1);
		if(node[0]=='/'){
			node  = node.substr(1);
		}
		/**
		 * 
		 **/
		if(server_id==my_server_id){
			if(da.db_open(database_location)==false){
				server.echo_back_error(index,"Fail on opening database");
				continue;
			}
			str = "select * from node where node = '"+node+"' limit 1";
			if(da.db_iss_exec(str)==false){
				server.echo_back_error(index,"Fail on getting data");
				continue;
			}
			if(da.iss.size()!=1){
				server.echo_back_error(index,"Can't find node");
				continue;
			}
			_ss.to_json(*da.iss[0]);
			server.echo_back_msg(index,_ss.outcome);
			continue;
		}
		else{
			i = server_index.size()-1;
			for(;i>=0;--i){
				if(server_index[i]==server_id){
					break;
				}
			}
			if(i>=0){
				str = "topology/node_info"+server.raw_msg[index];
				if(client.fire(i,str)>0){
					server.echo_back_error(index,"Fail on fire");
					continue;
				}
				if(str==""){
					server.echo_back_error(index,"Empty reply");
				}
				else{
					server.echo_back_msg(index,str);
				}
				continue;
			}
			else{
				i = server_index.size()-1;
				for(;i>=0;--i){
					str = "topology/node_info"+server.raw_msg[index];
					if(client.fire(i,str)>0){
						server.echo_back_error(index,"Fail on fire");
						break;
					}
					if(str=="1"){
						continue;
					}
					if(str[0]=='0' || str==""){
						server.echo_back_error(index,"Empty reply");
						break;
					}
					else{
						if(str[0]=='{'){
							server.echo_back_msg(index,str);
							break;
						}
						else{
							server.echo_back_error(index,"Reply has wrong format");
							break;
						}
					}
				}
				if(i==-1){
					server.echo_back_msg(index,"1");
				}
			}
		}
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
		server_index.push_back((*da.iss[i])["server_id"]);
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
	my_server_id = std::to_string(da.my_server_id());
	build_location(da,&fire);
	da.location_log(location,fire);
}

bool init(){
	dalahast da(__FILE__);
	short int i;
	database_location = da.root+"/sqlite/info.db";
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

int main (int argc, char* argv[]){
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
