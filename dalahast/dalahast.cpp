dalahast::dalahast(){
	server_index = "0";
	short int i;
	i = 100;
	std::string read {"README.md"}, cd {"../"};
	if(command("ls | grep "+read)!=""){
		root = command("pwd");
		root.resize(root.length()-1);
	}
	else{
		for(;i>=0;--i){
			if(command("cd "+cd+" && ls | grep "+read)!=""){
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

dalahast::dalahast(const char* node_name){
	short int i;
	std::string read;
	dalahast();
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
	if(node.find(read)!=std::string::npos){
		i = read.length();
		if(node.length()>=i){
			node = node.substr(i);
		}
	}
	/**
	 * 
	 **/
	if(db_open(root+"/sqlite/self.db")==false){
		std::cout << "Fail on opening self.db" << std::endl;
		return;
	}
	if(db_is_exec("select value from self where key = 'center' limit 1")==true){
		if(is.size()==1){
			if(is[0]=="1"){
				server_index = "0";
			}
		}
		else{
			std::cout << "Can't find 'center' in self table" << std::endl;
			return;
		}
	}
	else{
		std::cout << "SQL fail on self table" << std::endl;
		return;
	}
	/**
	 * 
	 **/
	if(db_is_exec("select value from self where key = 'server_index' limit 1")==true){
		if(is.size()==1){
			server_index = is[0];
		}
		else{
			std::cout << "Can't find 'server_index' in self table" << std::endl;
		}
	}
	else{
		std::cout << "SQL fail on self table" << std::endl;
	}
	/**
	 * 
	 **/
	if(db_is_exec("select value from self where key = 'control_sql' limit 1")==true){
		if(is.size()==1){
			location.push_back(is[0]);
			import_location(&location);
		}
		else{
			std::cout << "Can't find 'control_sql' in self table" << std::endl;
		}
	}
	else{
		std::cout << "SQL fail on self table" << std::endl;
	}
	/**
	 * 
	 **/
	if(db_open(root+"/sqlite/info.db")==true){
		read = "select server from node_info where server = "+server_index+" and name = '"+node+"' limit 1";
		if(db_is_exec(read)==true){
			if(is.size()==0){
				read = "insert into node_info (server,name) values ("+server_index+",'"+node+"')";
				if(db_exec(read)==false){
					std::cout << "SQL fail on inserting data to node_info" << std::endl;
				}
			}
		}
		else{
			std::cout << "SQL fail on getting data from node_info" << std::endl;
		}
	}
	else{
		std::cout << "SQL fail on opening info database" << std::endl;
	}
}

dalahast::~dalahast(){
	if(db!=nullptr){
		sqlite3_close(db);
		db = nullptr;
	}
	clear_3d<da::ISS>(iss);
}

void dalahast::error(std::string error_msg){
	error_msg = "insert into dalahast (server,node,message,time) values ("+server_index+",'"+node+"','"+error_msg+"',NOW())";
	if(db_open(root+"/sqlite/error_log.db")==true){
		db_exec(error_msg);
	}
}

bool dalahast::db_open(std::string db_name){
	short int i;
	remote_db.clear();
	if(server_index!="0"){
		i = db_name.length()-7; // 7 is the string length of 'self.db'
		if(db_name.substr(i)!="self.db"){
			i += 6;
			for(;i>=0;--i){
				if(db_name[i]=='/'){
					break;
				}
			}
			if(i<0){
				if(db_name.find("error_log.db")==std::string::npos){
					error("Can't detect root in db_open: "+db_name);
				}
				else{
					std::cout << "Can't detect root in db_open" << std::endl;
				}
				return false;
			}
			remote_db = db_name.substr(i+1);
			return true;
		}
	}
	if(db!=nullptr){
		sqlite3_close(db);
		db = nullptr;
	}
	i = sqlite3_open(db_name.c_str(), &db);
	if(i){
		if(db_name.find("error_log.db")==std::string::npos){
			error("Fail on opening database in db_open: "+db_name);
		}
		else{
			fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		}
		return false;
	}
	else{
		return true;
	}
}

bool dalahast::db_exec(std::string query){
	short int i;
	//std::string str;
	if(remote_db!=""){
		i = 0;
		ss_tool::str = "{\"db\":\""+remote_db+"\",\"query\":\""+query+"\",\"res\":\"0\"}";
		i = fire(i,ss_tool::str);
		if(i==0){
			if(ss_tool::str=="1"){
				return true;
			}
			else{
				ss_tool::str = "{\"message\":\"{\"db\":\""+remote_db+"\",\"query\":\""+query+"\",\"res\":\"0\"}\",\"reply\":\""+ss_tool::str+"\"}";
				error("db_exec request fail: "+ss_tool::str);
				return false;
			}
		}
		else{
			ss_tool::str = "{\"message\":\"{\"db\":\""+remote_db+"\",\"query\":\""+query+"\",\"res\":\"0\"}\",\"error_flag\":\""+std::to_string(i)+"\"}";
			error("db_exec socket fail: "+ss_tool::str);
			return false;
		}
	}
	if(db==nullptr){
		error("db==nullptr in db_exec");
		return false;
	}
	char *zErrMsg = 0;
	i = sqlite3_exec(db, query.c_str(), nullptr, 0, &zErrMsg);
	if(i!=SQLITE_OK){
		ss_tool::str = "sqlite3_exec fail in db_exec: ";
		ss_tool::str.append(zErrMsg);
		error(ss_tool::str);
		//fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

bool dalahast::db_iss_exec(std::string query){
	short int i;
	clear_3d<da::ISS>(iss);
	if(remote_db!=""){
		//std::string str;
		i = 0;
		ss_tool::str = "{\"db\":\""+remote_db+"\",\"query\":\""+query+"\",\"res\":\"iss\"}";
		i = fire(i,ss_tool::str);
		if(i==0){
			if(ss_tool::str[0]=='0'){
				ss_tool::str = "{\"message\":\"{\"db\":\""+remote_db+"\",\"query\":\""+query+"\",\"res\":\"iss\"}\",\"reply\":\""+ss_tool::str+"\"}";
				error("db_res_exec request fail: "+ss_tool::str);
				return false;
			}
			else{
				if(array_to(iss,ss_tool::str)==false){
					error("db_res_exec fail on parsing reply: "+ss_tool::str);
					return false;
				}
				else{
					return true;
				}
			}
		}
		else{
			ss_tool::str = "{\"message\":\"{\"db\":\""+remote_db+"\",\"query\":\""+query+"\",\"res\":\"iss\"}\",\"error_flag\":\""+std::to_string(i)+"\"}";
			error("db_res_exec socket fail: "+ss_tool::str);
			return false;
		}
	}
	if(db==nullptr){
		error("db==nullptr in db_res_exec");
		return false;
	}
	char *zErrMsg = 0;
	i = sqlite3_exec(db, query.c_str(), callback_iss, &iss, &zErrMsg);
	if(i!=SQLITE_OK){
		ss_tool::str = "sqlite3_exec fail in db_res_exec: ";
		ss_tool::str.append(zErrMsg);
		error(ss_tool::str);
		//fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

bool dalahast::db_is_exec(std::string query){
	short int i;
	is.clear();
	if(remote_db!=""){
		//std::string str;
		i = 0;
		ss_tool::str = "{\"db\":\""+remote_db+"\",\"query\":\""+query+"\",\"res\":\"is\"}";
		i = fire(i,ss_tool::str);
		if(i==0){
			if(ss_tool::str[0]=='0'){
				ss_tool::str = "{\"message\":\"{\"db\":\""+remote_db+"\",\"query\":\""+query+"\",\"res\":\"is\"}\",\"reply\":\""+ss_tool::str+"\"}";
				error("db_res_exec request fail: "+ss_tool::str);
				return false;
			}
			else{
				if(is_tool::array_to(is,ss_tool::str)==false){
					error("db_res_exec fail on parsing reply: "+ss_tool::str);
					return false;
				}
				else{
					return true;
				}
			}
		}
		else{
			ss_tool::str = "{\"message\":\"{\"db\":\""+remote_db+"\",\"query\":\""+query+"\",\"res\":\"is\"}\",\"error_flag\":\""+std::to_string(i)+"\"}";
			error("db_res_exec socket fail: "+ss_tool::str);
			return false;
		}
	}
	if(db==nullptr){
		error("db==nullptr in db_res_exec");
		return false;
	}
	char *zErrMsg = 0;
	i = sqlite3_exec(db, query.c_str(), callback_is, &is, &zErrMsg);
	if(i!=SQLITE_OK){
		ss_tool::str = "sqlite3_exec fail in db_res_exec: ";
		ss_tool::str.append(zErrMsg);
		error(ss_tool::str);
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
	//std::string str;
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
	return ss_tool::str;
}

bool dalahast::location_log(da::IS &location, da::IsH &fire){
	std::string to_ip,to_port;
	int i,j;
	if(db_open(root+"/sqlite/topology.db")==false){
		return false;
	}
	if(location.size()!=fire.size()){
		error("location and fire are not even");
		return false;
	}
	ss_tool::str = "delete from topology where from_server = "+server_index+" and from_node = '"+node+"'";
	if(db_exec(ss_tool::str)==false){
		error("Fail on clear data in topology table during location_log");
		return false;
	}
	i = location.size();
	for(;i>=0;--i){
		j = location[i].find(":");
		if(j==std::string::npos){
			j = location[i].length()-1;
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
			ss_tool::str = "insert into topology (from_server,from_node,to_server,to_node,type) values ('"+server_index+"','"+node+"','"+server_index+"','"+ss_tool::str+"','";
		}
		else{
			to_ip = location[i].substr(0,j);
			to_port = location[i].substr(j+1);
			ss_tool::str = "insert into topology (from_server,from_node,to_server,to_node,type) values ('"+server_index+"','"+node+"','"+to_ip+"','"+to_port+"','";
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
		if(db_exec(ss_tool::str)==false){
			error("Fail on inserting data in location_log: "+ss_tool::str);
		}
	}
}

bool dalahast::anti(bool b){
	if(db_open(root+"/sqlite/info.db")==false){
		return false;
	}
	if(b==true){
		ss_tool::str = "update node_info set anti = 1 where server = "+server_index+" and name = '"+node+"' limit 1";
	}
	else{
		ss_tool::str = "update node_info set anti = 0 where server = "+server_index+" and name = '"+node+"' limit 1";
	}
	return db_exec(ss_tool::str);
}

bool dalahast::freeze(bool b){
	if(db_open(root+"/sqlite/info.db")==false){
		return false;
	}
	if(b==true){
		ss_tool::str = "update node_info set freeze = 1 where server = "+server_index+" and name = '"+node+"' limit 1";
	}
	else{
		ss_tool::str = "update node_info set freeze = 0 where server = "+server_index+" and name = '"+node+"' limit 1";
	}
	return db_exec(ss_tool::str);
}

bool dalahast::check(bool b){
	if(db_open(root+"/sqlite/info.db")==false){
		return false;
	}
	if(b==true){
		ss_tool::str = "update node_info set check = 1 where server = "+server_index+" and name = '"+node+"' limit 1";
	}
	else{
		ss_tool::str = "update node_info set check = 0 where server = "+server_index+" and name = '"+node+"' limit 1";
	}
	return db_exec(ss_tool::str);
}

bool dalahast::private_node(bool b){
	if(db_open(root+"/sqlite/info.db")==false){
		return false;
	}
	if(b==true){
		ss_tool::str = "update node_info set 'private' = 1 where server = "+server_index+" and name = '"+node+"' limit 1";
	}
	else{
		ss_tool::str = "update node_info set 'private' = 0 where server = "+server_index+" and name = '"+node+"' limit 1";
	}
	return db_exec(ss_tool::str);
}

bool dalahast::port_log(std::string port){
	if(db_open(root+"/sqlite/info.db")==false){
		return false;
	}
	ss_tool::str = "update node_info set interface = '"+port+"' where server = "+server_index+" and name = '"+node+"' limit 1";
	return db_exec(ss_tool::str);
}

std::string dalahast::get_center_ip(){
	if(db_open(root+"/sqlite/self.db")==false){
		return "";
	}
	ss_tool::str = "select value from self where key = 'control_sql' limit 1";
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

bool dalahast::error_log(std::string msg){
	if(db_open(root+"/sqlite/error_log.db")==false){
		return false;
	}
	ss_tool::str = "insert into error (server,node,message,time) values ("+server_index+",'"+node+"','"+msg+"',NOW())";
	if(db_exec(ss_tool::str)==false){
		return false;
	}
	else{
		return true;
	}
}
