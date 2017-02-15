#include <review/maria_wrapper.h>
#include <review/tcp_server.h>
#include <review/configure.h>
#include <review/ss_tool.h>


/**
 * args [Raw Input]: A string in json form. 
 * args [message2]: It can be `flag` when type = `client` or it can be `reply` when type = `request`.
 **/
tcp_server server;
review::IS args {"type","server","node","to_ip","to_port","message","message2"};
review::IsH server_root_len;
std::string port;

short int insert_error {-1};
short int insert_request {-1};
short int insert_client {-1};

bool sql_pre(maria_wrapper *sql, bool init = false){
	review::IS query {"insert into error_log.error_node_receive (server,message,time) values (0,?,NOW())","insert into error_log.tcp_request (server,node,to_ip,to_port,message,reply,time) values (?,?,?,?,?,?,NOW())","insert into error_log.tcp_client (server,node,to_ip,to_port,message,flag,time) values (?,?,?,?,?,?,NOW())"};
	short int i;
	for(i=0;i<query.size();++i){
		if(sql->prepare(query[i])==-1){
			return false;
		}
	}
	if(init==true){
		insert_error = 0;
		insert_request = 1;
		insert_client = 2;
	}
	return true;
}

auto execute = [&](const short int index){
	maria_wrapper sql("error_log",__FILE__);
	ss_tool _ss;
	_ss.import_fixed(&args);
	review::SS ss;
	std::string str;
	short int server_index;
	if(sql_pre(&sql)==false){
		server.echo_back_error(server.socketfd[index],"Thread fail on initiating");
		server.done(index);
		return;
	}
	while(server.msg_to_args(index)==true){
		sql.reset();
		if(_ss.json_to(ss,server.raw_msg[index])==false){
			sql.pre[insert_error]->setString(1,server.raw_msg[index]);
			sql.update_execute(insert_error);
			server.echo_back_msg(server.socketfd[index],"1");
			continue;
		}
		server_index = std::stoi(ss["server"]);
		if(server_index>=server_root_len.size()){
			server.echo_back_error(server.socketfd[index],"Server doesn't exist");
			continue;
		}
		if(ss["node"].length()<=server_root_len[server_index]+4){
			server.echo_back_error(server.socketfd[index],"Name of 'node' is too short");
			continue;
		}
		ss["node"] = ss["node"].substr(server_root_len[server_index]);
		if(ss["type"]=="request"){
			sql.pre[insert_request]->setString(1,ss["server"]);
			sql.pre[insert_request]->setString(2,ss["node"]);
			sql.pre[insert_request]->setString(3,ss["to_ip"]);
			sql.pre[insert_request]->setString(4,ss["to_port"]);
			sql.pre[insert_request]->setString(5,ss["message"]);
			sql.pre[insert_request]->setString(6,ss["message2"]);
			sql.update_execute(insert_request);
		}
		else if(ss["type"]=="client"){
			sql.pre[insert_client]->setString(1,ss["server"]);
			sql.pre[insert_client]->setString(2,ss["node"]);
			sql.pre[insert_client]->setString(3,ss["to_ip"]);
			sql.pre[insert_client]->setString(4,ss["to_port"]);
			sql.pre[insert_client]->setString(5,ss["message"]);
			sql.pre[insert_client]->setString(6,ss["message2"]);
			sql.update_execute(insert_client);
		}
		else{
			server.echo_back_error(server.socketfd[index],"'type' doesn't exist");
			continue;
		}
		server.echo_back_result(server.socketfd[index],sql.something_wrong);
	}
	server.done(index);
	return;
};

void log(){
	configure conf(__FILE__);
	conf.args_log(&args,"TCP_SOCKET",conf.get_configuration("server","error_node_port"));
	conf.anti_data_racing(false);
	conf.private_node(true,"JSON");
	conf.sql_insert_log("[error_log,error_log,error_log]","[tcp_client,tcp_request,error_node_receive]");
}

bool init(){
	configure conf(__FILE__);
	maria_wrapper *sql {&conf};
	short int i;
	std::string str;
	port = conf.get_configuration("server","error_node_port");
	server_root_len.push_back(0);
	sql->to_database("server");
	sql->send_and_error("select server, root from server order by server ASC");
	while(sql->res->next()==true){
		i = sql->res->getInt(1);
		while(server_root_len.size()<i){
			server_root_len.push_back(0);
		}
		str = sql->res->getString(2);
		server_root_len.push_back(str.length());
	}
	if(sql_pre(sql,true)==false){
		std::cout << "/*********** Init Fail ****************/" << std::endl;
		std::cout << "Server: " << conf.server << std::endl;
		std::cout << "Node: " << conf.node << std::endl;
		conf.node_crash_log();
		return false;
	}
	return true;
}

int main (int argc,char* argv[]){
	if(argc>1){
		log();
		return 0;
	}
	server.execute = execute;
	server.set_args(nullptr);
	if(init()==true){
		if(server.init(__FILE__)==true){
			server.start_accept();
		}
	}
	return 0;
}
