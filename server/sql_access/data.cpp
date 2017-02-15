#include <review/maria.h>
#include <review/unix_server.h>
#include <review/configure.h>
#include <review/iss_tool.h>

unix_server server;
review::IS args {"server_index","database_name","table_name"};
short int select_data {-1};

bool sql_pre(maria_wrapper *sql, bool init = false){
	if(sql->prepare("select server,node from node.sql_access where sql_server = ? and _database = ? and _table = ? and execution = ?")==-1){
		return false;
	}
	if(init==true){
		select_data = 0;
	}
	return true;
}

auto execute = [&](const short int index){
	maria sql("node",__FILE__);
	review_type *rtype = &sql;
	review::ISS list;
	review::SS send;
	review::IS loop {"SELECT","INSERT","UPDATE","DELETE"};
	iss_tool _iss;
	ss_tool *_ss {&_iss};
	short int i;
	std::string str;
	if(sql_pre(&sql)==false){
		server.echo_back_error(server.socketfd[index],"Thread fail on initiating");
		server.done(index);
		return;
	}
	while(server.msg_to_args(index)==true){
		sql.reset();
		for(i=0;i<loop.size();++i){
			sql.pre[select_data]->setString(1,server.args[index]["server_index"]);
			sql.pre[select_data]->setString(2,server.args[index]["database_name"]);
			sql.pre[select_data]->setString(3,server.args[index]["table_name"]);
			sql.pre[select_data]->setString(4,loop[i]);
			sql.ISS_send(select_data,list);
			_iss.to_array(list);
			send[loop[i]] = _iss.outcome;
		}
		if(sql.something_wrong==false){
			_ss->to_json(send);
			server.echo_back_msg(server.socketfd[index],_ss->outcome);
		}
		else{
			server.echo_back_sql_error(server.socketfd[index]);
		}
	}
	rtype->clear_3d<review::ISS>(list);
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

bool init(){
	configure conf(__FILE__);
	if(sql_pre(&conf,true)==false){
		std::cout << "/*********** Init Fail ****************/" << std::endl;
		std::cout << "Server: " << conf.server << std::endl;
		std::cout << "Node: " << conf.node << std::endl;
		conf.node_crash_log();
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
	server.set_args(&args);
	if(init()==true){
		if(server.init(__FILE__)==true){
			server.start_accept();
		}
	}
	return 0;
}
