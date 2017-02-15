#include <review_admin/staff_access_node.h>
#include <review/maria_wrapper.h>
#include <review/unix_server.h>
#include <review/configure.h>
#include <review/review_checker.h>

unix_server server;
staff_access_node check;
review::IS args {"up_tag","id","name","password"};
review::IS table {"maria","node_crash","client","request","scan_error","error_log","error_node_receive","staff_access","data_racing","execute_node","image_auth","image_upload"};
review::IS up_tag {"maria","node_crash","client","request","scan_node","configure","error_node","staff_access","data_racing","execute","image_auth","image_upload"};
review::IS database {"error_log","error_log","error_log","error_log","node","configuration","error_log","error_log","error_log","error_log","error_log","error_log"};
short int max {0};

short int staff_ck {-1};

bool sql_pre(maria_wrapper *sql, bool init = false){
	if(check.build_prepare(*sql)==-1){
		return false;
	}
	if(init==true){
		staff_ck = 0;
	}
	return true;
}

auto execute = [&](const short int index){
	std::string str;
	maria_wrapper sql("error_log");
	sql.build_statement();
	review_checker int_check;
	short int i;
	if(sql_pre(&sql)==false){
		server.echo_back_error(server.socketfd[index],"Thread fail on initiating");
		server.done(index);
		return;
	}
	while(server.msg_to_args(index)==true){
		sql.reset();
		if(check.check(sql,server.args[index])==false){
			server.echo_back_error(server.socketfd[index],"Access denied");
			continue;
		}
		if(int_check.int_filter(server.args[index]["id"])==false){
			server.echo_back_error(server.socketfd[index],"Wrong id");
			continue;
		}
		for(i=0;i<max;++i){
			if(server.args[index]["up_tag"]==up_tag[i]){
				sql.to_database(database[i].c_str());
				str = "delete from "+table[i]+" where id = "+server.args[index]["id"]+" limit 1";
				break;
			}
		}
		if(i==max){
			server.echo_back_error(server.socketfd[index],"'up_tag' doesn't exist");
			continue;
		}
		sql.update_send(str);
		if(sql.something_wrong==true){
			server.echo_back_error(server.socketfd[index],"SQL error on deleting data");
			continue;
		}
		else{
			str = "select count(*) from "+table[i];
			sql.res_send(str);
			if(sql.res==nullptr || sql.res->next()==false){
				server.echo_back_error(server.socketfd[index],"SQL error on getting amount");
				continue;
			}
			str = "{\"amount\":\""+sql.res->getString(1)+"\"}";
			server.echo_back_msg(server.socketfd[index],str);
		}
	}
	server.done(index);
	return;
};

void log(){
	configure conf(__FILE__);
	is_tool _is;
	std::string tmp;
	conf.args_log(&args);
	conf.anti_data_racing(false);
	conf.private_node(false);
	_is.to_array(database);
	tmp = _is.outcome;
	_is.to_array(table);
	conf.sql_delete_log(tmp,_is.outcome);
	conf.sql_select_log(tmp,_is.outcome);
	check.log(conf);
}

bool init(){
	configure conf(__FILE__);
	check.init(conf);
	max = up_tag.size();
	if(sql_pre(&conf,true)==false){
		std::cout << "/*********** Init Fail ****************/" << std::endl;
		std::cout << "Server: " << conf.server << std::endl;
		std::cout << "Node: " << conf.node << std::endl;
		conf.to_server(3,"error_log");
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
	server.set_args(&args);
	server.execute = execute;
	if(init()==true){
		if(server.init(__FILE__)==true){
			server.start_accept();
		}
	}
	return 0;
}
