#include <hast/tcp_server.h>
#include <dalahast/dalahast.h>

/**
 * args [Raw Input]: A string in json form. 
 * args [message2]: It can be `flag` when type = `socket` or it can be `reply` when type = `request`.
 **/
tcp_server server;
std::string port {"8889"};
review::IS args {"type","from_server","from_node","to_node","message","message2"};
std::string this_server;
short int root_len;
short int error {-1};
short int request {-1};
short int client {-1};
short int tcp_server {-1};
short int tcp_node {-1};

bool sql_pre(maria_wrapper *sql,bool init = false){
	review::IS query {"insert into error_log.error_node_receive (server,message,reason,time) values (?,?,?,NOW())","insert into error_log.request (from_server,from_node,to_server,to_node,message,reply,time) values (?,?,?,?,?,?,NOW())","insert into error_log.client (from_server,from_node,to_server,to_node,message,flag,time) values (?,?,?,?,?,?,NOW())","select server from server.server where local_ip = ? OR outgoing_ip = ? limit 1","select node from node.info where server = ? and interface = ? limit 1"};
	short int i;
	for(i=0;i<query.size();++i){
		if(sql->prepare(query[i])==-1){
			return false;
		}
	}
	if(init==true){
		error = 0;
		request = 1;
		client = 2;
		tcp_server = 3;
		tcp_node = 4;
	}
	return true;
}

auto execute = [&](const short int index){
	maria_wrapper sql("error_log",__FILE__);
	ss_tool _ss;
	std::string to_server,ip,port;
	_ss.import_fixed(&args);
	review::SS ss;
	short int i;
	if(sql_pre(&sql)==false){
		server.echo_back_error(server.socketfd[index],"Thread fail on initiating");
		server.done(index);
		return;
	}
	while(server.msg_to_args(index)==true){
		sql.reset();
		if(_ss.json_to(ss,server.raw_msg[index])==false){
			sql.pre[error]->setString(1,this_server);
			sql.pre[error]->setString(2,server.raw_msg[index]);
			sql.pre[error]->setString(3,"Fail on parsing");
			sql.update_execute(error);
			server.echo_back_result(server.socketfd[index],false);
			continue;
		}
		if(ss["from_node"].length()<=root_len+4){
			sql.pre[error]->setString(1,this_server);
			sql.pre[error]->setString(2,server.raw_msg[index]);
			sql.pre[error]->setString(3,"Length of from_node string is too short");
			sql.update_execute(error);
			server.echo_back_result(server.socketfd[index],false);
			continue;
		}
		ss["from_node"] = ss["from_node"].substr(root_len);
		ss["from_node"].resize(ss["from_node"].length()-4);
		if(ss["to_node"]!="NULL"){
			i = ss["to_node"].find(":");
			if(i==std::string::npos){
				//unix
				to_server = this_server;
				if(ss["to_node"].length()<=root_len+7){
					sql.pre[error]->setString(1,this_server);
					sql.pre[error]->setString(2,server.raw_msg[index]);
					sql.pre[error]->setString(3,"Length of to_node string is too short");
					sql.update_execute(error);
					server.echo_back_result(server.socketfd[index],false);
					continue;
				}
				ss["to_node"] = ss["to_node"].substr(root_len);
				ss["to_node"].resize(ss["to_node"].length()-7);
			}
			else{
				//tcp
				ip = ss["to_node"].substr(0,i);
				port = ss["to_node"].substr(i+1);
				sql.pre[tcp_server]->setString(1,ip);
				sql.pre[tcp_server]->setString(2,ip);
				sql.res_execute(tcp_server);
				if(sql.res==nullptr || sql.res->next()==false){
					sql.pre[error]->setString(1,this_server);
					sql.pre[error]->setString(2,server.raw_msg[index]);
					sql.pre[error]->setString(3,"Can't find tcp_server");
					sql.update_execute(error);
					server.echo_back_result(server.socketfd[index],false);
					continue;
				}
				to_server = sql.res->getString(1);
				sql.pre[tcp_node]->setString(1,port);
				sql.res_execute(tcp_server);
				if(sql.res==nullptr || sql.res->next()==false){
					sql.pre[error]->setString(1,this_server);
					sql.pre[error]->setString(2,server.raw_msg[index]);
					sql.pre[error]->setString(3,"Can't find tcp_node");
					sql.update_execute(error);
					server.echo_back_result(server.socketfd[index],false);
					continue;
				}
				ss["to_node"] = sql.res->getString(1);
			}
		}
		if(ss["type"]=="request"){
			sql.pre[request]->setString(1,this_server);
			sql.pre[request]->setString(2,ss["from_node"]);
			sql.pre[request]->setString(3,to_server);
			sql.pre[request]->setString(4,ss["to_node"]);
			sql.pre[request]->setString(5,ss["message"]);
			sql.pre[request]->setString(6,ss["message2"]);
			sql.update_execute(request);
		}
		else if(ss["type"]=="client"){
			sql.pre[client]->setString(1,this_server);
			sql.pre[client]->setString(2,ss["from_node"]);
			sql.pre[client]->setString(3,to_server);
			sql.pre[client]->setString(4,ss["to_node"]);
			sql.pre[client]->setString(5,ss["message"]);
			sql.pre[client]->setString(6,ss["message2"]);
			sql.update_execute(client);
		}
		else{
			sql.pre[error]->setString(1,this_server);
			sql.pre[error]->setString(2,server.raw_msg[index]);
			sql.pre[error]->setString(3,"'type' doesn't exist");
			sql.update_execute(error);
			server.echo_back_result(server.socketfd[index],false);
			continue;
		}
		server.echo_back_result(server.socketfd[index],sql.something_wrong);
	}
	server.done(index);
	return;
};

void log(){
	configure conf(__FILE__);
	conf.args_log(&args);
	conf.anti_data_racing(false);
	conf.private_node(true,"JSON");
	conf.sql_insert_log("[error_log,error_log,error_log]","[request,client,error_node_receive]");
	conf.sql_select_log("[server,node]","[server,info]");
}

bool init(){
	configure conf(__FILE__);
	std::string str;
	this_server = conf.server;
	str = conf.get_server_root();
	root_len = str.length();
	if(sql_pre(&conf,true)==false){
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
