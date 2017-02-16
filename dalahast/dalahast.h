#ifndef dalahast_h
#define dalahast_h
#include <iostream>
#include <sqlite3.h>
#include <stdio.h>
#include <string>
#include <dalahast/tool/iss_tool.h>
#include <hast/client_core.h>

da_type dtype;
da::ISS iss;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	dtype.clear_3d<da::ISS>(iss);
	int i;
	for(i=0; i<argc; i++){
		iss.push_back(new da::SS);
		(*iss[i])[azColName[i]] = argv[i] ? argv[i] : "NULL";
		//printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	//printf("\n");
	return 0;
}

class dalahast : public client_core{
private:
	sqlite3 *db {nullptr};
	char *zErrMsg = 0;
	int i;
	da::IS location;
	bool center {true};
	void init_error();
public:
	std::string root;
	dalahast();
	~dalahast();
	bool db_exec(std::string query);
	bool db_res_exec(std::string query);
	bool db_open(std::string db_name);
	std::string command(std::string command);
	bool location_log(da::IS &location, da::IsH &fire);
};
#include <dalahast/dalahast.cpp>
#endif dalahast_h
