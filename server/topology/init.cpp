#include <review/maria.h>
#include <review/unix_server.h>
#include <review/configure.h>
#include <review/iss_tool.h>
#include <review/review_type.h>

unix_server server;
review::IS args {"topology_init"};
review_type rtype;

auto execute = [&](const short int index){
	std::string str;
	maria sql("node",__FILE__);
	sql.build_statement();
	review::ISS info;
	review::ISS fire;
	review::ISS::iterator it;
	review::ISS::iterator it_end;
	review::SS send;
	iss_tool _iss;
	ss_tool *_ss {&_iss};
	short int i;
	while(server.msg_to_args(index)==true){
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
	rtype.clear_3d<review::ISS>(info);
	rtype.clear_3d<review::ISS>(fire);
	server.done(index);
	return;
};

void log(){
	configure conf(__FILE__);
	conf.args_log(&args);
	conf.anti_data_racing(false);
	conf.private_node(false);
	conf.sql_select_log("[node,node]","[info,topology]");
}

void init(){
}

int main (int argc, char* argv[]){
	if(argc>1){
		log();
		return 0;
	}
	server.execute = execute;
	server.set_args(nullptr);
	init();
	if(server.init(__FILE__)==true){
		server.start_accept();
	}
	configure conf(__FILE__);
	conf.node_crash_log();
	return 0;
}
