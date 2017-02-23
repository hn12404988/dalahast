#ifndef dalahast_h
#define dalahast_h
#include <iostream>
#include <sqlite3.h>
#include <stdio.h>
#include <string>
#include <dalahast/tool/iss_tool.h>

namespace da{
	const std::string server_prefix {"/server/"};
	const bool CONCAT {true};
}

static int callback_iss(void *ptr, int argc, char **argv, char **azColName){
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

static int callback_is(void *ptr, int argc, char **argv, char **azColName){
	int i;
	da::IS *is = static_cast<da::IS*>(ptr);
	for(i=0; i<argc; i++){
		is->push_back(argv[i] ? argv[i] : "NULL");
	}
	return 0;
}

class dalahast : public da_type, public iss_tool{
private:
	sqlite3 *db {nullptr};
	std::string node;
	void error(std::string error_msg,std::string error_msg2 = "");
	void get_root();
	bool now_is_error_log {false};
public:
	std::string root;
	std::string server_index;
	da::ISS iss;
	da::IS is;
	dalahast(const char* node_name);
	dalahast();
	~dalahast();
	bool db_exec(std::string query);
	bool db_iss_exec(std::string query,bool concat = false);
	bool db_is_exec(std::string query,bool concat = false);
	bool db_open(std::string db_name);
	std::string command(std::string command);
	bool location_log(da::IS &location, da::IsH &fire);
	bool port_log(std::string port_name);
	bool port_log(int port);
	bool anti(bool b);
	bool private_node(bool b);
	bool freeze(bool b);
	bool check(bool b);
	std::string get_ip(short int i);
	std::string get_port(short int i,std::string name);
	short int get_server_id(std::string ip);
	std::string get_port_name(short int server_id,std::string port);
	short int my_server_id();
	bool error_log(std::string msg);
	bool all_main_port();
};
#include <dalahast/dalahast.cpp>
#endif dalahast_h
