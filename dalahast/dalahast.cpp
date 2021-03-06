dalahast::dalahast(){
	get_root();
}

dalahast::dalahast(const char* node_name){
	short int i;
	std::string read;
	get_root();
	node = node_name;
	i = node.length()-1;
	for(;i>=0;--i){
		if(node[i]=='.'){
			break;
		}
	}
	if(i>=0){
		node = node.substr(0,i);
	}
	read = root+da::server_prefix;
	i = read.length();
	if(node.substr(0,i)==read){
		node = node.substr(i);
	}
	/**
	 * 
	 **/
	if(db_open(root+"/sqlite/info.db")==true){
		read = "select node from node where node = '"+node+"'";
		if(db_is_exec(read)==true){
			if(is.size()==0){
				read = "insert into node (node) values ('"+node+"')";
				db_exec(read);
			}
		}
	}
}

sqlite3* dalahast::get_db(){
	return db;
}

void dalahast::get_root(){
	short int i;
	i = 100;
	std::string read {".dalahast"}, cd {"../"};
	if(command("ls -a | grep "+read)!="1"){
		root = command("pwd");
		root.resize(root.length()-1);
	}
	else{
		for(;i>=0;--i){
			if(command("cd "+cd+" && ls -a | grep "+read)!="1"){
				root = command("cd "+cd+" && pwd");
				break;
			}
			cd = cd + cd;
		}
		if(root!=""){
			root.resize(root.length()-1);
		}
		else{
			std::cout << "Fail on detecting root" << std::endl;
			return;
		}
	}
}

dalahast::~dalahast(){
	if(db!=nullptr){
		sqlite3_close(db);
		db = nullptr;
	}
	clear_3d<da::ISS>(iss);
	is.clear();
}

void dalahast::error(std::string error_msg,std::string msg2){
	if(now_is_error_log==true){
		std::cout << "/******** Database error_log is dead, so here are the error message ********************/" << std::endl;
		std::cout << "Node: " << node << std::endl;
		std::cout << "Error_Message: " << error_msg << std::endl;
		std::cout << "Message: " << msg2 << std::endl;
		now_is_error_log = false;
	}
	else{
		now_is_error_log = true;
		if(db_open(root+"/sqlite/error_log.db")==false){
			return;
		}
		int flag;
		if(ppStmt!=nullptr){
			sqlite3_finalize(ppStmt);
			ppStmt = nullptr;
		}
		ss_tool::str = "insert into dalahast (node,message,message2,time) values (?,?,?,datetime('now'))";
		flag = sqlite3_prepare(db,ss_tool::str.c_str(),ss_tool::str.length(),&ppStmt,nullptr);
		if(flag!=SQLITE_OK){
			std::cout << "/******** Fail on preparing, so here are the error message ********************/" << std::endl;
			std::cout << "Node: " << node << std::endl;
			std::cout << "Error_Message: " << error_msg << std::endl;
			std::cout << "Message: " << msg2 << std::endl;
			now_is_error_log = false;
			return;
		}
		sqlite3_bind_text(ppStmt,1,node.c_str(),node.length(),nullptr);
		sqlite3_bind_text(ppStmt,2,error_msg.c_str(),error_msg.length(),nullptr);
		sqlite3_bind_text(ppStmt,3,msg2.c_str(),msg2.length(),nullptr);
		flag = sqlite3_step(ppStmt);
		sqlite3_finalize(ppStmt);
		ppStmt = nullptr;
		if(flag!=SQLITE_DONE){
			std::cout << "/******** Fail on step, so here are the error message ********************/" << std::endl;
			std::cout << "Node: " << node << std::endl;
			std::cout << "Error_Message: " << error_msg << std::endl;
			std::cout << "Message: " << msg2 << std::endl;
		}
		now_is_error_log = false;
	}
}

bool dalahast::db_open(std::string db_name){
	if(db!=nullptr){
		sqlite3_close(db);
		db = nullptr;
	}
	if(sqlite3_open(db_name.c_str(), &db)){
		//fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		error("Fail on opening database in db_open: "+db_name);
		return false;
	}
	else{
		return true;
	}
}

bool dalahast::db_exec(std::string query){
	if(db==nullptr){
		error("db==nullptr in db_exec");
		return false;
	}
	char *zErrMsg = 0;
	int flag {SQLITE_BUSY};
	while(flag==SQLITE_BUSY){
		flag = sqlite3_exec(db, query.c_str(), nullptr, 0, &zErrMsg);
	}
	if(flag!=SQLITE_OK){
		ss_tool::str = "sqlite3_exec fail in db_exec: ";
		ss_tool::str.append(zErrMsg);
		error(ss_tool::str,query);
		//fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

bool dalahast::db_iss_exec(std::string query,bool concat){
	if(concat==false){
		clear_3d<da::ISS>(iss);
	}
	if(db==nullptr){
		error("db==nullptr in db_res_exec");
		return false;
	}
	int flag {SQLITE_BUSY};
	char *zErrMsg = 0;
	while(flag==SQLITE_BUSY){
		flag = sqlite3_exec(db, query.c_str(), callback_iss, &iss, &zErrMsg);
	}
	if(flag!=SQLITE_OK){
		ss_tool::str = "sqlite3_exec fail in db_res_exec: ";
		ss_tool::str.append(zErrMsg);
		error(ss_tool::str,query);
		//fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

bool dalahast::db_is_exec(std::string query,bool concat){
	if(concat==false){
		is.clear();
	}
	if(db==nullptr){
		error("db==nullptr in db_res_exec");
		return false;
	}
	int flag {SQLITE_BUSY};
	char *zErrMsg = 0;
	while(flag==SQLITE_BUSY){
		flag = sqlite3_exec(db, query.c_str(), callback_is, &is, &zErrMsg);
	}
	if(flag!=SQLITE_OK){
		ss_tool::str = "sqlite3_exec fail in db_res_exec: ";
		ss_tool::str.append(zErrMsg);
		error(ss_tool::str,query);
		//fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

std::string dalahast::command(std::string command){
	FILE *in {nullptr};
	const short int buff_size {300};
	char buff[buff_size];
	in = popen(command.c_str(),"r");
	if(in==nullptr){
		error("command fail: "+command);
		return "";
	}
	ss_tool::str.clear();
	while(fgets(buff, buff_size, in)!=nullptr){
		ss_tool::str.append(buff);
	}
	pclose(in);
	if(ss_tool::str==""){
		ss_tool::str = "1";
	}
	return ss_tool::str;
}

bool dalahast::location_log(da::IS &location, da::IsH &fire){
	std::string to_ip,to_port,pre_root,my_index;
	int i,j,pre_len;
	i = my_server_id();
	if(i==-1){
		return false;
	}
	my_index = std::to_string(i);
	if(my_index==""){
		error_log("Can't get server_id in location_log()");
		return false;
	}
	pre_root = root+da::server_prefix;
	pre_len = pre_root.length();
	if(db_open(root+"/sqlite/topology.db")==false){
		return false;
	}
	if(location.size()!=fire.size()){
		error("location and fire are not even");
		return false;
	}
	ss_tool::str = "delete from topology where from_node = '"+node+"'";
	if(db_exec(ss_tool::str)==false){
		return false;
	}
	i = location.size()-1;
	for(;i>=0;--i){
		j = location[i].find(":");
		if(j==std::string::npos){
			j = location[i].length()-1;
			/**
			 * 
			 **/
			for(;j>=0;--j){
				if(location[i][j]=='.'){
					break;
				}
			}
			if(j>=0){
				ss_tool::str = location[i].substr(0,j);
			}
			else{
				ss_tool::str = location[i];
			}
			/**
			 * 
			 **/
			if(ss_tool::str.substr(0,pre_len)==pre_root){
				ss_tool::str = ss_tool::str.substr(pre_len);
			}
			/**
			 * 
			 **/
			ss_tool::str = "insert into topology (from_node,to_server,to_node,type) values ('"+node+"','"+my_index+"','"+ss_tool::str+"','";
		}
		else{
			to_ip = location[i].substr(0,j);
			to_port = location[i].substr(j+1);
			j = get_server_id(to_ip);
			if(j==-1){
				error_log("Can't get server_id from ip in location_log");
			}
			else{
				if(get_port_name(j,to_port)==""){
					error_log("Can't get port_name from port in location_log");
				}
			}
			ss_tool::str = "insert into topology (from_node,to_server,to_node,type) values ('"+node+"','"+to_ip+"','"+to_port+"','";
		}
		if(fire[i]==da::fire){
			ss_tool::str.append("fire')");
		}
		else if(fire[i]==da::fireNstore){
			ss_tool::str.append("fireNstore')");
		}
		else if(fire[i]==da::fireNclose){
			ss_tool::str.append("fireNclose')");
		}
		else if(fire[i]==da::fireNforget){
			ss_tool::str.append("fireNforget')");
		}
		else if(fire[i]==da::fireNfreeze){
			ss_tool::str.append("fireNfreeze')");
		}
		else if(fire[i]==da::fireNcheck){
			ss_tool::str.append("fireNcheck')");
		}
		else{
			error("fire_type invalid");
			return false;
		}
		if(db_open(root+"/sqlite/topology.db")==false){
			break;
		}
		db_exec(ss_tool::str);
	}
}

bool dalahast::anti(bool b){
	if(db_open(root+"/sqlite/info.db")==false){
		return false;
	}
	if(b==true){
		ss_tool::str = "update node set anti = 1 where node = '"+node+"'";
	}
	else{
		ss_tool::str = "update node set anti = 0 where node = '"+node+"'";
	}
	return db_exec(ss_tool::str);
}

bool dalahast::freeze(bool b){
	if(db_open(root+"/sqlite/info.db")==false){
		return false;
	}
	if(b==true){
		ss_tool::str = "update node set freeze = 1 where node = '"+node+"'";
	}
	else{
		ss_tool::str = "update node set freeze = 0 where node = '"+node+"'";
	}
	return db_exec(ss_tool::str);
}

bool dalahast::check(bool b){
	if(db_open(root+"/sqlite/info.db")==false){
		return false;
	}
	if(b==true){
		ss_tool::str = "update node set check = 1 where node = '"+node+"'";
	}
	else{
		ss_tool::str = "update node set check = 0 where node = '"+node+"'";
	}
	return db_exec(ss_tool::str);
}

bool dalahast::private_node(bool b){
	if(db_open(root+"/sqlite/info.db")==false){
		return false;
	}
	if(b==true){
		ss_tool::str = "update node set 'private' = 1 where node = '"+node+"'";
	}
	else{
		ss_tool::str = "update node set 'private' = 0 where node = '"+node+"'";
	}
	return db_exec(ss_tool::str);
}

bool dalahast::port_log(std::string port_name){
	ss_tool::str = get_port(my_server_id(),port_name);
	if(ss_tool::str!=""){
		port_name = ss_tool::str;
	}
	else{
		short int i;
		i = port_name.length()-1;
		for(;i>=0;--i){
			if(port_name[i]<48 || port_name[i]>57){
				error_log("Invalid port name in port_log(): "+port_name);
				return false;
			}
		}
	}
	if(db_open(root+"/sqlite/info.db")==false){
		return false;
	}
	ss_tool::str = "update node set interface = '"+port_name+"' where node = '"+node+"'";
	return db_exec(ss_tool::str);
}

bool dalahast::port_log(int port){
	if(db_open(root+"/sqlite/info.db")==false){
		return false;
	}
	ss_tool::str = "update node set interface = '"+std::to_string(port)+"' where node = '"+node+"'";
	return db_exec(ss_tool::str);
}

std::string dalahast::get_ip(short int i){
	short int tmp;
	tmp = my_server_id();
	if(tmp==-1){
		return "";
	}
	else{
		if(tmp==i){
			return "127.0.0.1";
		}
	}
	if(db_open(root+"/sqlite/tcp.db")==false){
		return "";
	}
	ss_tool::str = "select ip from server where id = "+std::to_string(i);
	if(db_is_exec(ss_tool::str)==false){
		return "";
	}
	else{
		if(is.size()==1){
			return is[0];
		}
		else{
			return "";
		}
	}
}

std::string dalahast::get_port(short int i,std::string name){
	if(db_open(root+"/sqlite/tcp.db")==false){
		return "";
	}
	ss_tool::str = "select port from port where server_id = "+std::to_string(i)+" and name = '"+name+"'";
	if(db_is_exec(ss_tool::str)==false){
		return "";
	}
	else{
		if(is.size()==1){
			return is[0];
		}
		else{
			return "";
		}
	}
}

short int dalahast::get_server_id(std::string ip){
	short int tmp;
	if(ip=="127.0.0.1"){
		return my_server_id();
	}
	if(db_open(root+"/sqlite/tcp.db")==false){
		return -1;
	}
	ss_tool::str = "select id from server where ip = '"+ip+"'";
	if(db_is_exec(ss_tool::str)==false){
		return -1;
	}
	else{
		if(is.size()==1){
			return (short int)std::stoi(is[0]);
		}
		else{
			return -1;
		}
	}
}

std::string dalahast::get_port_name(short int server_id,std::string port){
	if(db_open(root+"/sqlite/tcp.db")==false){
		return "";
	}
	ss_tool::str = "select name from port where server_id = "+std::to_string(server_id)+" and port = "+port;
	if(db_is_exec(ss_tool::str)==false){
		return "";
	}
	else{
		if(is.size()==1){
			return is[0];
		}
		else{
			return "";
		}
	}
}

short int dalahast::my_server_id(){
	if(db_open(root+"/sqlite/self.db")==false){
		return -1;
	}
	ss_tool::str = "select value from self where key = 'server_id'";
	if(db_is_exec(ss_tool::str)==false){
		return -1;
	}
	else{
		if(is.size()==1){
			return (short int)std::stoi(is[0]);
		}
		else{
			error_log("Can't get my_server_id");
			return -1;
		}
	}
}

bool dalahast::error_log(std::string msg){
	if(db_open(root+"/sqlite/error_log.db")==false){
		return false;
	}
	if(ppStmt!=nullptr){
		sqlite3_finalize(ppStmt);
		ppStmt = nullptr;
	}
	int flag;
	ss_tool::str = "insert into error (node,message,time) values (?,?,datetime('now'))";
	flag = sqlite3_prepare(db,ss_tool::str.c_str(),ss_tool::str.length(),&ppStmt,nullptr);
	if(flag!=SQLITE_OK){
		return false;
	}
	sqlite3_bind_text(ppStmt,1,node.c_str(),node.length(),nullptr);
	sqlite3_bind_text(ppStmt,2,msg.c_str(),msg.length(),nullptr);
	flag = sqlite3_step(ppStmt);
	sqlite3_finalize(ppStmt);
	ppStmt = nullptr;
	if(flag!=SQLITE_DONE){
		return false;
	}
	return true;
}

bool dalahast::all_main_port(){
	if(db_open(root+"/sqlite/tcp.db")==false){
		return false;
	}
	ss_tool::str = "select server_id,port from port where name = 'main_port'";
	if(db_iss_exec(ss_tool::str)==false){
		return false;
	}
	int i;
	i = iss.size()-1;
	for(;i>=0;--i){
		ss_tool::str = "select ip from server where id = "+(*iss[i])["server_id"];
		if(db_is_exec(ss_tool::str)==false){
			return false;
		}
		if(is.size()==0){
			error_log("Can't get ip from server in all_main_port");
			return false;
		}
		(*iss[i])["ip"] = is[0];
	}
	return true;
}
