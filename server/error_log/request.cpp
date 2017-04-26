#include <hast/unix_server.hpp>
#include <hast/client_core.hpp>
#include <dalahast/dalahast.hpp>
#include <dalahast/tool/ss_tool.hpp>
#include <dalahast/tool/ii_tool.hpp>


unix_server server;
da::IS args {"server","up_tag"}; //server can be 'all' or an index
da::IS location;
da::IS server_index;
short int to_info {-1};

auto execute = [&](const short int index){
	dalahast da(__FILE__);
	client_core client;
	client.import_location(&location);
	client.set_error_node(0,__FILE__);
	ss_tool _ss;
	ii_tool<da::II> _ii;
	da::SS param,tmp,send;
	da::II ii,ii2;
	da::SS::iterator it;
	da::SS::iterator it_end;
	std::string str,already;
	short int i,j;
	while(server.msg_recv(index)==true){
		_ss.import_fixed(&args);
		if(_ss.json_to(param,server.raw_msg[index])==false){
			server.echo_back_error(index,"Fail on parsing args");
			continue;
		}
		_ss.import_fixed(nullptr);
		if(param["server"]=="all"){
			already = "[]";
		}
		else{
			already.clear();
			already = "[";
			i = server_index.size()-1;
			for(;i>=0;--i){
				if(server_index[i]==param["server"]){
					continue;
				}
				already = already+server_index[i]+",";
			}
			if(already.back()==','){
				already.pop_back();
			}
			already.push_back(']');
		}
		/**
		 * 
		 **/
		str = "{\"up_tag\":\""+param["up_tag"]+"\",\"already\":\""+already+"\"}";

		if(client.fire(to_info,str)>0){
			server.echo_back_error(index,"Fail on fire");
			continue;
		}
		if(str==""){
			server.echo_back_error(index,"Empty reply");
			continue;
		}
		else if(str[0]=='0'){
			server.echo_back_error(index,"info return error");
			continue;
		}
		if(_ss.json_to(tmp,str)==false){
			server.echo_back_error(index,"Fail on parsing tmp");
			continue;
		}
		/**
		 * 
		 **/
		send.clear();
		send["up_tag"] = tmp["up_tag"];
		it = tmp.begin();
		it_end = tmp.end();
		if(param["server"]=="all"){
			ii.clear();
			for(;it!=it_end;++it){
				if(it->first=="up_tag"){
					tmp.erase(it);
					it_end = tmp.end();
					it = tmp.begin();
				}
				else if((it->first).back()=='t'){//_amount
					if(_ii.array_to(ii2,it->second)==false){
						break;
					}
					if(ii.size()==0){
						j = ii2.size();
						for(i=0;i<j;++i){
							ii.push_back(ii2[i]);
						}
					}
					else{
						j = ii2.size();
						for(i=0;i<j;++i){
							ii[i] += ii2[i];
						}
					}
					tmp.erase(it);
					it_end = tmp.end();
					it = tmp.begin();
				}
			}
			if(it!=it_end){
				server.echo_back_error(index,"Fail on process data return from info");
				continue;
			}
			_ss.to_json(tmp);
			send["content"] = _ss.outcome;
			_ii.to_array(ii);
			send["amount"] = _ii.outcome;
		}
		else{
			for(;it!=it_end;++it){
				if(it->first==param["server"]){
					break;
				}
			}
			if(it==it_end){
				server.echo_back_error(index,"Can not get data from client");
				continue;
			}
			send["content"] = "{\""+param["server"]+"\":\""+it->second+"\"}";
			send["amount"] = tmp[param["server"]+"_amount"];
		}
		_ss.to_json(send);
		server.echo_back_msg(index,_ss.outcome);
	}
	server.done(index);
	return;
};

void log(){
	dalahast da(__FILE__);
	da::IsH fire;
	location.push_back(da.root+da::server_prefix+"private/error_node.socket");
	fire.push_back(da::fire);
	location.push_back(da.root+da::server_prefix+"share_info/error_log.socket");
	fire.push_back(da::fire);
	da.location_log(location,fire);
}

bool init(){
	dalahast da(__FILE__);
	int i;
	location.push_back(da.root+da::server_prefix+"private/error_node.socket");
	location.push_back(da.root+da::server_prefix+"share_info/error_log.socket");
	to_info = 1;
	if(da.all_main_port()==false){
		da.error_log("all_main_port() return false");
		return false;
	}
	i = da.iss.size()-1;
	for(;i>=0;--i){
		server_index.push_back((*da.iss[i])["server_id"]);
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
