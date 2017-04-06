#include <hast/unix_server.h>
#include <hast/client_thread.h>
#include <dalahast/dalahast.h>
#include <dalahast/tool/iss_tool.h>

unix_server server;
da::IS args {"topology_init"};
da::IS location;
short int to_info {-1};
short int to_topology {-1};
da_type dtype;

auto execute = [&](const short int index){
	client_thread client;
	client.import_location(&location);
	client.set_error_node(0,__FILE__);
	std::string str,info,topology;
	da::ISS iss,iss2;
	da::SS ss;
	da::SS::iterator it;
	da::SS::iterator it_end;
	da::SS send;
	iss_tool _iss;
	ss_tool *_ss {&_iss};
	int i,j;
	while(server.msg_recv(index)==true){
		dtype.clear_3d<da::ISS>(iss2);
		info = "{\"already\":[]}";
		topology = "{\"already\":[]}";
		if(client.fireNstore(to_info,info)>0){
			server.echo_back_error(index,"Fail on firing to info");
			continue;
		}
		if(client.fireNstore(to_topology,topology)>0){
			server.echo_back_error(index,"Fail on firing to topology");
			continue;
		}
		/**
		 * 
		 **/
		if(client.join(to_info)==false){
			server.echo_back_error(index,"Fail on joining info");
			continue;
		}
		if(info[0]=='0' || info==""){
			server.echo_back_error(index,"info return error");
			continue;
		}
		if(_ss->json_to(ss,info)==false){
			server.echo_back_error(index,"Fail on parsing info");
			continue;
		}
		/**
		 * 
		 **/
		it = ss.begin();
		it_end = ss.end();
		dtype.clear_3d<da::ISS>(iss2);
		for(;it!=it_end;++it){
			dtype.clear_3d<da::ISS>(iss);
			str = it->first;
			if(_iss.array_to(iss,it->second)==false){
				break;
			}
			i = iss.size()-1;
			for(;i>=0;--i){
				(*iss[i])["server"] = str;
				j = (*iss[i])["node"].find("/");
				if(j==std::string::npos){
					(*iss[i])["folder"] = "";
				}
				else{
					(*iss[i])["folder"] = (*iss[i])["node"].substr(0,j);
					(*iss[i])["node"] = (*iss[i])["node"].substr(j+1);
				}
			}
			dtype.concat_3d<da::ISS>(iss2,iss);
		}
		if(it!=it_end){
			server.echo_back_error(index,"Fail on parsing iss_info");
			continue;
		}
		_iss.to_array(iss2);
		send["info"] = _iss.outcome;
		/**
		 * 
		 **/
		if(client.join(to_topology)==false){
			server.echo_back_error(index,"Fail on joining topology");
			continue;
		}
		if(topology[0]=='0' || topology==""){
			server.echo_back_error(index,"topology return error");
			continue;
		}
		if(_ss->json_to(ss,topology)==false){
			server.echo_back_error(index,"Fail on parsing topology");
			continue;
		}
		/**
		 * 
		 **/
		it = ss.begin();
		it_end = ss.end();
		dtype.clear_3d<da::ISS>(iss2);
		for(;it!=it_end;++it){
			dtype.clear_3d<da::ISS>(iss);
			str = it->first;
			if(_iss.array_to(iss,it->second)==false){
				break;
			}
			i = iss.size()-1;
			for(;i>=0;--i){
				(*iss[i])["from_server"] = str;
				/**
				 * 
				 **/
				j = (*iss[i])["from_node"].find("/");
				if(j==std::string::npos){
					(*iss[i])["from_folder"] = "";
				}
				else{
					(*iss[i])["from_folder"] = (*iss[i])["from_node"].substr(0,j);
					(*iss[i])["from_node"] = (*iss[i])["from_node"].substr(j+1);
				}
				/**
				 * 
				 **/
				j = (*iss[i])["to_node"].find("/");
				if(j==std::string::npos){
					(*iss[i])["to_folder"] = "";
				}
				else{
					(*iss[i])["to_folder"] = (*iss[i])["to_node"].substr(0,j);
					(*iss[i])["to_node"] = (*iss[i])["to_node"].substr(j+1);
				}
			}
			dtype.concat_3d<da::ISS>(iss2,iss);
		}
		if(it!=it_end){
			server.echo_back_error(index,"Fail on parsing iss_topology");
			continue;
		}
		_iss.to_array(iss2);
		send["fire"] = _iss.outcome;
		_ss->to_json(send);
		server.echo_back_msg(index,_ss->outcome);
	}
	dtype.clear_3d<da::ISS>(iss);
	dtype.clear_3d<da::ISS>(iss2);
	server.done(index);
	return;
};

void build_location(dalahast &da, da::IsH *fire = nullptr){
	location.push_back(da.root+da::server_prefix+"private/error_node.socket");
	location.push_back(da.root+da::server_prefix+"share_info/node_info.socket");
	
	location.push_back(da.root+da::server_prefix+"share_info/topology_info.socket");
	if(fire!=nullptr){
		fire->push_back(da::fire);
		fire->push_back(da::fireNstore);
		fire->push_back(da::fireNstore);
	}
}

void log(){
	dalahast da(__FILE__);
	da::IsH fire;
	build_location(da,&fire);
	da.location_log(location,fire);
}

void init(){
	dalahast da(__FILE__);
	build_location(da,nullptr);
	to_info = 1;
	to_topology = 2;
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
