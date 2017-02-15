#include <review/maria_wrapper.h>
#include <review/unix_server.h>
#include <review/configure.h>
#include <review/is_tool.h>

unix_server server;
review::IS args {"server_index"};
short int amount {0};

auto execute = [&](const short int index){
	maria_wrapper sql("node",__FILE__);
	sql.build_statement();
	review::IS list;
	is_tool _is;
	short int i;
	std::string str;
	while(server.msg_to_args(index)==true){
		sql.reset();
		list.clear();
		i = std::stoi(server.args[index]["server_index"]);
		str = std::to_string(i);
		if(i>0 && i<=amount){
			str = "select _database from sql_access where sql_server = "+str;
			sql.res_send(str);
			while(sql.res->next()==true){
				list.push_back(sql.res->getString(1));
			}
			_is.delete_repeat(list);
			_is.to_array(list);
			str = "{\"list\":\""+_is.outcome+"\"}";
			server.echo_back_msg(server.socketfd[index],str);
		}
		else{
			server.echo_back_error(server.socketfd[index],"Server doesn't exist");
		}
	}
	server.done(index);
	return;
};

void log(){
	configure conf(__FILE__);
	conf.args_log(&args);
	conf.anti_data_racing(false);
	conf.private_node(false);
	conf.sql_select_log("[node]","[sql_access]");
}

void init(){
	configure conf(__FILE__);
	amount = std::stoi(conf.get_configuration("server","amount"));
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
	configure conf(__FILE__);
	conf.node_crash_log();
	return 0;
}
