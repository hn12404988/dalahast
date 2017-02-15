#include <review/maria_wrapper.h>
#include <review/unix_server.h>
#include <review/configure.h>
#include <review/ss_tool.h>

unix_server server;
review::IS args {"node_id"};
short int get_info {0};

auto execute = [&](const short int index){
	maria_wrapper sql("node",__FILE__);
	if(sql.prepare("select node_type,interface,input,input_form,anti_data_racing from node.info where server = ? and node = ? limit 1")==-1){
		server.echo_back_error(server.socketfd[index],"Thread fail on initiating");
		server.done(index);
		return;
	}
	ss_tool _ss;
	_ss.all_quote = true;
	review::SS ss;
	std::string server_index,node;
	short int i;
	while(server.msg_to_args(index)==true){
		sql.reset();
		i = server.args[index]["node_id"].find("_");
		if(i==std::string::npos){
			server.echo_back_error(server.socketfd[index],"Fail on parsing args");
			continue;
		}
		server_index = server.args[index]["node_id"].substr(0,i);
		node = server.args[index]["node_id"].substr(i+1);
		sql.pre[0]->setString(1,server_index);
		sql.pre[0]->setString(2,node);
		sql.res_execute(get_info);
		if(sql.res==nullptr){
			server.echo_back_error(server.socketfd[index],"SQL fail on getting info");
			continue;
		}
		if(sql.res->next()==false){
			server.echo_back_msg(server.socketfd[index],"1");
			continue;
		}
		ss["node"] = node;
		ss["node_type"] = sql.res->getString(1);
		ss["interface"] = sql.res->getString(2);
		ss["input"] = sql.res->getString(3);
		ss["input_form"] = sql.res->getString(4);
		ss["anti_data_racing"] = sql.res->getString(5);
		_ss.to_json(ss);
		server.echo_back_msg(server.socketfd[index],_ss.outcome);
	}
	server.done(index);
	return;
};

void log(){
	configure conf(__FILE__);
	conf.args_log(&args);
	conf.anti_data_racing(false);
	conf.private_node(false);
	conf.sql_select_log("[node]","[info]");
}

void init(){
}

int main (int argc, char* argv[]){
	if(argc>1){
		log();
		return 0;
	}
	server.execute = execute;
	server.set_args(&args);
	init();
	if(server.init(__FILE__)==true){
		server.start_accept();
	}
	return 0;
}
