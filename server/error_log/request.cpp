#include <review/maria.h>
#include <review/unix_server.h>
#include <review/configure.h>
#include <review/iss_tool.h>
#include <review/is_tool.h>
#include <review/ii_tool.h>
#include <review/review_type.h>

unix_server server;
review::IS args {"up_tag"};
review_type rtype;
review::IS table {"maria","node_crash","client","request","scan_error","error_log","error_node_receive","staff_access","data_racing","execute_node","image_auth","image_upload"};
review::IS up_tag {"maria","node_crash","client","request","scan_node","configure","error_node","staff_access","data_racing","execute","image_auth","image_upload"};
review::IS database {"error_log","error_log","error_log","error_log","node","configuration","error_log","error_log","error_log","error_log","error_log","error_log"};
std::string up_string;
short int max {0};

auto execute = [&](const short int index){
	std::string str;
	maria sql("error_log");
	sql.build_statement();
	review::ISS iss;
	review::II amount;
	review::SS send;
	iss_tool _iss;
	ss_tool *_ss {&_iss};
	ii_tool<review::II> _ii;
	short int i,target;
	send["up_tag"] = up_string;
	for(i=0;i<up_tag.size();++i){
		amount.push_back(0);
	}
	while(server.msg_to_args(index)==true){
		sql.reset();
		if(server.args[index]["up_tag"]=="init"){
			target = 0;
		}
		else{
			for(i=0;i<max;++i){
				if(server.args[index]["up_tag"]==up_tag[i]){
					target = i;
					break;
				}
			}
			if(i==max){
				server.echo_back_error(server.socketfd[index],"'up_tag' doesn't exist");
				continue;
			}
		}
		for(i=0;i<max;++i){
			sql.to_database(database[i].c_str());
			if(i==target){
				str = "select * from "+table[i];
				sql.ISS_send(str,iss);
				if(sql.something_wrong==true){
					break;
				}
				amount[i] = iss.size();
				_iss.all_quote = true;
				_iss.escape_quote = true;
				_iss.to_array(iss);
				_iss.all_quote = false;
				_iss.escape_quote = false;
				send["content"] = _iss.outcome;
			}
			else{
				str = "select count(*) from "+table[i];
				sql.res_send(str);
				if(sql.something_wrong==true || sql.res->next()==false){
					break;
				}
				amount[i] = sql.res->getInt(1);
			}
		}
		if(i<max){
			server.echo_back_error(server.socketfd[index],"Fail on getting data from SQ");
			continue;
		}
		_ii.to_array(amount);
		send["amount"] = _ii.outcome;
		_ss->to_json(send);
		server.echo_back_msg(server.socketfd[index],_ss->outcome);
	}
	rtype.clear_3d<review::ISS>(iss);
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
	conf.sql_select_log(tmp,_is.outcome);
}

void init(){
	configure conf(__FILE__);
	is_tool _is;
	_is.set_array_separate('"');
	_is.to_array(up_tag);
	up_string = _is.outcome;
	max = up_tag.size();
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
