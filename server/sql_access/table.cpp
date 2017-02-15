#include <review/maria_wrapper.h>
#include <review/unix_server.h>
#include <review/configure.h>
#include <review/is_tool.h>

unix_server server;
review::IS args {"server_index","database_name"};
short int select_data {-1};

bool sql_pre(maria_wrapper *sql, bool init = false){
	if(sql->prepare("select _table from node.sql_access where server = ? and _database = ?")==-1){
		return false;
	}
	if(init==true){
		select_data = 0;
	}
	return true;
}

auto execute = [&](const short int index){
	maria_wrapper sql("node",__FILE__);
	review::IS list;
	is_tool _is;
	short int i;
	std::string str;
	if(sql_pre(&sql)==false){
		server.echo_back_error(server.socketfd[index],"Thread fail on initiating");
		server.done(index);
		return;
	}
	while(server.msg_to_args(index)==true){
		sql.reset();
		list.clear();
		sql.pre[select_data]->setString(1,server.args[index]["server_index"]);
		sql.pre[select_data]->setString(2,server.args[index]["database_name"]);
		sql.res_execute(select_data);
		if(sql.res!=nullptr){
			while(sql.res->next()==true){
				list.push_back(sql.res->getString(1));
			}
			_is.delete_repeat(list);
			_is.to_array(list);
			str = "{\"list\":\""+_is.outcome+"\"}";
			server.echo_back_msg(server.socketfd[index],str);
		}
		else{
			server.echo_back_sql_error(server.socketfd[index]);
		}
	}
	server.done(index);
	return;
};

bool init(){
	configure conf(__FILE__);
	conf.sql_select_log("[node]","[sql_access]");
	if(sql_pre(&conf,true)==false){
		std::cout << "/*********** Init Fail ****************/" << std::endl;
		std::cout << "Server: " << conf.server << std::endl;
		std::cout << "Node: " << conf.node << std::endl;
		conf.node_crash_log();
		return false;
	}
	return true;
}

void log(){
	configure conf(__FILE__);
	conf.args_log(&args);
	conf.anti_data_racing(false);
	conf.private_node(false);
}

int main (int argc, char* argv[]){
	if(argc>1){
		log();
		return 0;
	}
	server.execute = execute;
	server.set_args(&args);
	if(init()==true){
		if(server.init(__FILE__)==true){
			server.start_accept();
		}
	}
	return 0;
}
