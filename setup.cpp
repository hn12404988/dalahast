#include <dalahast/dalahast.h>
#include <hast/client_core.h>

void get(std::string &str){
	str.clear();
	char input[100];
	std::cin.getline(input,100);
	str = input;
}

int main(){
	dalahast da;
	client_core client;
	std::string str,local_ip,server_index;
	int i;
	short int to_center {0};
	da::IS location;
	bool center {true};
	/**
	 * Init
	 **/
	std::cout << "Is this server the center server?(y/n)" << std::endl;
	get(server_index);
	if(server_index=="y" || server_index=="Y"){
		server_index = "0";
	}
	else if(server_index=="n" || server_index=="N"){
		center = false;
		/**
		 * 
		 **/
		std::cout << "What is the TCP/IP address of 'control_sql.cpp ' in the center server?(ex: 192.168.1.11:8888)" << std::endl;
		get(str);
		if(str==""){
			std::cout << "Abort" << std::endl;
			return 0;
		}
		/**
		 * 
		 **/
		location.push_back(str);
		client.import_location(&location);
		str = "test";
		if(client.fire(to_center,str)>0){
			std::cout << "Can't connect to 'control_sql.cpp' in center server" << std::endl;
			return 0;
		}
		/**
		 * 
		 **/
		std::cout << "What is the index of this server?(integer)" << std::endl;
		get(server_index);
		if(server_index==""){
			std::cout << "Abort" << std::endl;
			return 0;
		}
		i = std::stoi(server_index);
		server_index = std::to_string(i);
	}
	else{
		std::cout << "Abort" << std::endl;
		return 0;
	}
	std::cout << "What is the ip of this server for TCP/IP socket?(ex: 192.168.1.11)" << std::endl;
	get(local_ip);
	if(local_ip==""){
		std::cout << "Abort" << std::endl;
		return 0;
	}
	/**
	 * Build tables in self.db
	 **/
	if(da.db_open("sqlite/self.db")==false){
		std::cout << "Fail on opening database 'self'" << std::endl;
		return 0;
	}
	str = "create table if not exists self (key CHAR(100), value CHAR(100))";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'self'" << std::endl;
		return 0;
	}
	str = "delete from self";
	if(da.db_exec(str)==false){
		std::cout << "Fail on deleting 'self' table" << std::endl;
		return 0;
	}
	if(center==true){
		str = "insert into self (key,value) values ('center','1')";
	}
	else{
		str = "insert into self (key,value) values ('center','0')";
	}
	if(da.db_exec(str)==false){
		std::cout << "SQL fail on self table" << std::endl;
		return 0;
	}
	str = "insert into self (key,value) values ('local_ip','"+local_ip+"')";
	if(da.db_exec(str)==false){
		std::cout << "SQL fail on self table" << std::endl;
		return 0;
	}
	str = "insert into self (key,value) values ('server_index','"+server_index+"')";
	if(da.db_exec(str)==false){
		std::cout << "SQL fail on self table" << std::endl;
		return 0;
	}
	if(center==false){
		str = "{\"db\":\"info.db\",\"query\":\"delete from server_info where ip = '"+local_ip+"'\",\"res\":\"0\"}";
		if(client.fire(to_center,str)>0){
			std::cout << "Client fail on clearing server_info data" << std::endl;
			return 0;
		}
		else{
			if(str!="1"){
				std::cout << "Request fail on clearing server_info data" << std::endl;
				return 0;
			}
		}
		str = "{\"db\":\"info.db\",\"query\":\"insert into server_info (id,ip,root) values ("+server_index+",'"+local_ip+"','"+da.root+"')\",\"res\":\"0\"}";
		if(client.fire(to_center,str)>0){
			std::cout << "Client fail on inserting server_info data" << std::endl;
			return 0;
		}
		else{
			if(str!="1"){
				std::cout << "Request fail on inserting server_info data" << std::endl;
				return 0;
			}
		}
		str = "insert into self (key,value) values ('control_sql','"+location[0]+"')";
		if(da.db_exec(str)==false){
			std::cout << "SQL fail on self table" << std::endl;
			return 0;
		}
		return 0;
	}
	/**
	 * Center server start from here.
	 **/
	if(da.db_open("sqlite/topology.db")==false){
		std::cout << "Fail on opening database 'topology'" << std::endl;
		return 0;
	}
	/**
	 * Build tables in topology.db
	 **/
	str = "create table if not exists topology (from_server CHAR(50), from_node CHAR(100), to_server CHAR(50), to_node CHAR(100), type CHAR(50))";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'topology'" << std::endl;
		return 0;
	}
	/**
	 * Build tables in error_log.db
	 * dalahast, request, socket, error
	 **/
	if(da.db_open("sqlite/error_log.db")==false){
		std::cout << "Fail on opening database 'error_log'" << std::endl;
		return 0;
	}
	str = "create table if not exists dalahast (server INT, node CHAR(100), message TEXT, time DATETIME)";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'dalahast'" << std::endl;
		return 0;
	}
	str = "create table if not exists request (from_server INT, from_node CHAR(100), to_server INT, to_node CHAR(100), message TEXT, reply TEXT, time DATETIME)";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'request'" << std::endl;
		return 0;
	}
	str = "create table if not exists socket (from_server INT, from_node CHAR(100), to_server INT, to_node CHAR(100), message TEXT, error_flag INT, time DATETIME)";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'socket'" << std::endl;
		return 0;
	}
	str = "create table if not exists error (server INT, node CHAR(100), message TEXT, time DATETIME)";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'error'" << std::endl;
		return 0;
	}
	str = "create table if not exists error_node_recv (message TEXT, time DATETIME)";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'error'" << std::endl;
		return 0;
	}
	/**
	 * Build tables in info.db
	 * server_info, node_info
	 **/
	if(da.db_open("sqlite/info.db")==false){
		std::cout << "Fail on opening database 'info'" << std::endl;
		return 0;
	}
	str = "create table if not exists server_info (id INT,ip CHAR(50),root CHAR(100))";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'configuration'" << std::endl;
		return 0;
	}
	str = "create table if not exists node_info (server INT,name CHAR(100),'interface' CHAR(10) default 'FILE', 'private' TINYINT default 0, 'anti' TINYINT default 0, 'freeze' TINYINT default 0, 'check' TINYINT default 0)";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'node_info'" << std::endl;
		return 0;
	}
	/**
	 * local_ip into sql
	 **/
	str = "delete from server_info where ip = '"+local_ip+"'";
	if(da.db_exec(str)==false){
		std::cout << "SQLite process fail" << std::endl;
		return 0;
	}
	str = "insert into server_info (id,ip,root) values (0,'"+local_ip+"','"+da.root+"')";
	if(da.db_exec(str)==false){
		std::cout << "SQLite process fail" << std::endl;
		return 0;
	}
	return 0;
}
