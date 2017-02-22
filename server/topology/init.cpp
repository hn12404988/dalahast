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
	client.import_location(&args);
	client.set_error_node(0,__FILE__);
	std::string str,info,topology;
	da::ISS iss,iss_info,iss_topology;
	da::SS ss;
	da::ISS::iterator it;
	da::ISS::iterator it_end;
	da::SS send;
	iss_tool _iss;
	ss_tool *_ss {&_iss};
	int i;
	while(server.msg_recv(index)==true){
		info = "[]";
		topology = "[]";
		if(client.fireNstore(to_info,info)>0){
			server.echo_back_error(server.socketfd[index],"Fail on firing to info");
			continue;
		}
		if(client.fireNstore(to_topology,topology)>0){
			server.echo_back_error(server.socketfd[index],"Fail on firing to topology");
			continue;
		}
		/**
		 * 
		 **/
		if(client.join(to_info)==false){
			server.echo_back_error(server.socketfd[index],"Fail on joining info");
			continue;
		}
		if(info[0]=='0' || info==""){
			server.echo_back_error(server.socketfd[index],"info return error");
			continue;
		}
		if(_ss->json_to(ss,info)==false){
			server.echo_back_error(server.socketfd[index],"Fail on parsing info");
			continue;
		}
		it = ss.begin();
		it_end = ss.end();
		for(;it!=it_end;++it){
			
		}
		












		
		sql.reset();
		str = "select * from info";
		sql.ISS_send(str,info);
		it = info.begin();
		it_end = info.end();
		for(;it!=it_end;++it){
			if((**it)["input"][0]=='['){
				(**it)["input"] = (**it)["input"].substr(1);
				(**it)["input"].pop_back();
			}
			i = (**it)["node"].find("/");
			if(i==std::string::npos){
				(**it)["folder"] = "";
			}
			else{
				(**it)["folder"] = (**it)["node"].substr(0,i);
				(**it)["node"] = (**it)["node"].substr(i+1);
			}
		}
		str = "select * from topology";
		sql.ISS_send(str,fire);
		it = fire.begin();
		it_end = fire.end();
		for(;it!=it_end;++it){
			i = (**it)["from_node"].find("/");
			if(i==std::string::npos){
				(**it)["from_folder"] = "";
			}
			else{
				(**it)["from_folder"] = (**it)["from_node"].substr(0,i);
				(**it)["from_node"] = (**it)["from_node"].substr(i+1);
			}
		}
		it = fire.begin();
		it_end = fire.end();
		for(;it!=it_end;++it){
			i = (**it)["to_node"].find("/");
			if(i==std::string::npos){
				(**it)["to_folder"] = "";
			}
			else{
				(**it)["to_folder"] = (**it)["to_node"].substr(0,i);
				(**it)["to_node"] = (**it)["to_node"].substr(i+1);
			}
		}
		_iss.to_array(info);
		send["info"] = _iss.outcome;
		_iss.to_array(fire);
		send["fire"] = _iss.outcome;
		_ss->to_json(send);
		if(sql.something_wrong==false){
			server.echo_back_msg(server.socketfd[index],_ss->outcome);
		}
		else{
			server.echo_back_sql_error(server.socketfd[index]);
		}
	}
	dtype.clear_3d<da::ISS>(info);
	dtype.clear_3d<da::ISS>(fire);
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
	build_location(da,&fire);
	to_info = 1;
	to_topology = 2;
}

int main (int argc, char* argv[]){
	if(argc>1){
		log();
		return 0;
	}
	server.execute = execute;
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
