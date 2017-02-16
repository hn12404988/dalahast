dalahast::dalahast(){
	i = 100;
	std::string str {"README.md"}, cd {"../"},out;
	if(command("ls | grep "+str)!=""){
		root = command("pwd");
		root.resize(root.length()-1);
		return;
	}
	for(;i>=0;--i){
		if(command("cd "+cd+" && ls | grep "+str)!=""){
			root = command("cd "+cd+" && pwd");
			break;
		}
		cd = cd + cd;
	}
	if(root!=""){
		root.resize(root.length()-1);
	}
}

dalahast::~dalahast(){
	if(db!=nullptr){
		sqlite3_close(db);
		db = nullptr;
	}
	dtype.clear_3d<da::ISS>(iss);
	sqlite3_free(zErrMsg);
}

bool dalahast::db_open(std::string db_name){
	if(db!=nullptr){
		sqlite3_close(db);
		db = nullptr;
	}
	i = sqlite3_open(db_name.c_str(), &db);
	if(i){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return false;
	}
	else{
		return true;
	}
}

bool dalahast::db_exec(std::string query){
	if(db==nullptr){
		return false;
	}
	i = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);
	if(i!=SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

bool dalahast::db_res_exec(std::string query){
	const char* data;
	if(db==nullptr){
		return false;
	}
	i = sqlite3_exec(db, query.c_str(), callback, (void*)data, &zErrMsg);
	if(i!=SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

std::string dalahast::command(std::string command){
	FILE *in {nullptr};
	const short int buff_size {300};
	char buff[buff_size];
	std::string str;
	in = popen(command.c_str(),"r");
	if(in==nullptr){
		return "";
	}
	while(fgets(buff, buff_size, in)!=nullptr){
		str.append(buff);
	}
	pclose(in);
	return str;
}

bool dalahast::location_log(da::IS &location, da::IsH &fire){
	db_open(root+"/sqlite/topology.db");
	if(location.size()!=fire.size()){
		db_open(root+"/sqlite/error_log.db");
		db_exec("insert into ");
	}
}
