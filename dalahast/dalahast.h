#ifndef dalahast_h
#define dalahast_h
#include <iostream>
#include <sqlite3.h>
#include <stdio.h>
#include <string>
#include <dalahast/tool/iss_tool.h>
#include <hast/client_core.h>

static int callback(void *ptr, int argc, char **argv, char **azColName){
	int i,j;
	da::ISS *iss = static_cast<da::ISS*>(ptr);
	iss->push_back(new da::SS);
	j = iss->size()-1;
	for(i=0; i<argc; i++){
		(*(*iss)[j])[azColName[i]] = argv[i] ? argv[i] : "NULL";
		//printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	//printf("\n");
	return 0;
}

class dalahast : public client_core, public da_type, public iss_tool{
private:
	sqlite3 *db {nullptr};
	da::IS location;
	std::string remote_db;
	std::string node;
	void error(std::string error_msg);
public:
	std::string root;
	std::string server_index;
	da::ISS iss;
	dalahast(const char* node_name);
	dalahast();
	~dalahast();
	bool db_exec(std::string query);
	bool db_res_exec(std::string query);
	bool db_open(std::string db_name);
	std::string command(std::string command);
	bool location_log(da::IS &location, da::IsH &fire);
	bool public_node(std::string port);
	bool anti(bool b);
	bool freeze(bool b);
	bool check(bool b);
};
#include <dalahast/dalahast.cpp>
#endif dalahast_h
