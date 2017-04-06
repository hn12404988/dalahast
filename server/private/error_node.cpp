#include <hast/unix_server.h>
#include <dalahast/dalahast.h>
#include <dalahast/tool/ss_tool.h>

/**
 * args [Raw Input]: A string in json form. 
 * args [message2]: It is `error_flag` when type = `socket`, and it is `reply` when type = `request`.
 **/
unix_server server;
da::IS args {"type","from_node","to_node","message","message2"};
std::string root_prefix;
short int root_len;
std::string my_server_id;

auto execute = [&](const short int index){
	dalahast da(__FILE__);
	sqlite3 *db {nullptr};
	sqlite3_stmt *stmt {nullptr};
	ss_tool _ss;
	std::string str,to_server,to_node;
	_ss.import_fixed(&args);
	da::SS ss;
	short int i;
	int flag;
	while(server.msg_recv(index)==true){
		if(stmt!=nullptr){
			sqlite3_finalize(stmt);
			stmt = nullptr;
		}
		/**
		 * Parsing args
		 **/
		if(_ss.json_to(ss,server.raw_msg[index])==false){
			if(da.db_open(da.root+"/sqlite/error_log.db")==true){
				db = da.get_db();
				str = "insert into error_node_recv (message,time) values (?,datetime('now'))";
				flag = sqlite3_prepare(db,str.c_str(),str.length(),&stmt,nullptr);
				if(flag==SQLITE_OK){
					sqlite3_bind_text(stmt,1,server.raw_msg[index].c_str(),server.raw_msg[index].length(),nullptr);
					flag = sqlite3_step(stmt);
					if(flag==SQLITE_DONE){
						server.echo_back_error(index,"Fail on parsing args");
						continue;
					}
					std::cout << "Fail on inserting error_recv" << std::endl;
				}
				std::cout << "Fail on preparing to error_recv" << std::endl;
			}
			std::cout << "Fail on opening error_log database, so here are the messages" << std::endl;
			std::cout << "Fail on parsing args" << std::endl;
			std::cout << server.raw_msg[index] << std::endl;
			server.echo_back_error(index,"Fail on parsing args and open database");
			continue;
		}
		/**
		 * Deal with from_node
		 **/
		if(ss["from_node"].substr(0,root_len)==root_prefix){
			ss["from_node"] = ss["from_node"].substr(root_len);
		}
		i = ss["from_node"].length();
		if(ss["from_node"].substr(i-4)==".cpp"){
			ss["from_node"].resize(i-4);
		}
		/**
		 * Deal with to_node
		 **/
		i = ss["to_node"].find(":");
		if(i==std::string::npos){
			//unix
			if(ss["to_node"].substr(0,root_len)==root_prefix){
				ss["to_node"] = ss["to_node"].substr(root_len);
			}
			i = ss["to_node"].length();
			if(ss["to_node"].substr(i-4)==".cpp"){
				ss["to_node"].resize(i-4);
			}
			to_server = my_server_id;
			to_node = ss["to_node"];
		}
		else{
			//tcp
			to_server = ss["to_node"].substr(0,i);
			to_node = ss["to_node"].substr(i+1);
			i = da.get_server_id(to_server);
			if(i>=0){
				to_server = std::to_string(i);
				str = da.get_port_name(i,to_node);
				if(str!=""){
					to_node = str;
				}
			}
		}
		if(ss["type"]=="request"){
			str = "insert into request (from_node,to_server,to_node,message,reply,time) values (?,?,?,?,?,datetime('now'))";
		}
		else if(ss["type"]=="socket"){
			str = "insert into socket (from_node,to_server,to_node,message,error_flag,time) values (?,?,?,?,?,datetime('now'))";
		}
		else{
			server.echo_back_error(index,"type invalid");
			continue;
		}
		if(da.db_open(da.root+"/sqlite/error_log.db")==true){
			db = da.get_db();
			flag = sqlite3_prepare(db,str.c_str(),str.length(),&stmt,nullptr);
			if(flag==SQLITE_OK){
				sqlite3_bind_text(stmt,1,ss["from_node"].c_str(),ss["from_node"].length(),nullptr);
				sqlite3_bind_text(stmt,2,to_server.c_str(),to_server.length(),nullptr);
				sqlite3_bind_text(stmt,3,to_node.c_str(),to_node.length(),nullptr);
				sqlite3_bind_text(stmt,4,ss["message"].c_str(),ss["message"].length(),nullptr);
				sqlite3_bind_text(stmt,5,ss["message2"].c_str(),ss["message2"].length(),nullptr);
				flag = sqlite3_step(stmt);
				if(flag==SQLITE_DONE){
					server.echo_back_msg(index,"1");
					continue;
				}
				server.echo_back_error(index,"Fail on inserting error_log");
			}
			server.echo_back_error(index,"Fail on preparing error_log");
		}
		else{
			server.echo_back_error(index,"Fail on open error_log database");
		}
	}
	if(stmt!=nullptr){
		sqlite3_finalize(stmt);
		stmt = nullptr;
	}
	server.done(index);
	return;
};

void log(){
	dalahast da(__FILE__);
	da.private_node(true);
}

bool init(){
	dalahast da(__FILE__);
	short int i;
	root_prefix = da.root+da::server_prefix;
	root_len = root_prefix.length();
	i = da.my_server_id();
	if(i==-1){
		return false;
	}
	else{
		my_server_id = std::to_string(i);
		return true;
	}
}

int main (int argc,char* argv[]){
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
