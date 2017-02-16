#include <dalahast/dalahast.h>
#include <hast/client_core.h>

void get(std::string &str){
	str.clear();
	char input[100];
	cin.getline(input,100);
	str = input;
}

int main(){
	dalahast da;
	std::string str,local_ip,server_index;
	int i;
	short int to_center {0};
	da::IS location;
	bool center {true};
	/**
	 * Init
	 **/
	cout << "Is this server the center server?(y/n)" << endl;
	get(server_index);
	if(server_index=="y" || server_index=="Y"){
		server_index = "0";
	}
	else if(server_index=="n" || server_index=="N"){
		center = false;
		/**
		 * 
		 **/
		cout << "What is the TCP/IP address of 'control_sql.cpp ' in the center server?(ex: 192.168.1.11:8888)" << endl;
		get(str);
		if(str==""){
			cout << "Abort" << endl;
			return 0;
		}
		/**
		 * 
		 **/
		location.push_back(str);
		client.import_location(&location);
		str = "test";
		if(client.fire(to_center,str)>0){
			cout << "Can't connect to 'control_sql.cpp' in center server" << endl;
			return 0;
		}
		/**
		 * 
		 **/
		cout << "What is the index of this server?(integer)" << endl;
		get(server_index);
		if(server_index==""){
			cout << "Abort" << endl;
			return 0;
		}
		i = std::stoi(server_index);
		server_index = std::to_string(i);
		//to control_sql
	}
	else{
		cout << "Abort" << endl;
		return 0;
	}
	cout << "What is the ip of this server for TCP/IP socket?(ex: 192.168.1.11)" << endl;
	get(local_ip);
	if(local_ip==""){
		cout << "Abort" << endl;
		return 0;
	}
	//to control_sql
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
	str = "delete * from self";
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
	str = "create table if not exists topology (from_server INT, from_node CHAR(100), to_server INT, to_node CHAR(100), type CHAR(50))";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'topology'" << std::endl;
		return 0;
	}
	/**
	 * Build tables in error_log.db
	 * init, request, socket
	 **/
	if(da.db_open("sqlite/error_log.db")==false){
		std::cout << "Fail on opening database 'error_log'" << std::endl;
		return 0;
	}
	str = "create table if not exists init (server INT, node CHAR(100), message TEXT, time DATETIME)";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'init'" << std::endl;
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
	/**
	 * Build tables in server_info.db
	 * configuration, server_info, node_info
	 **/
	if(da.db_open("sqlite/info.db")==false){
		std::cout << "Fail on opening database 'info'" << std::endl;
		return 0;
	}
	str = "create table if not exists configuration (category CHAR(50),key CHAR(50),value CHAR(50))";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'configuration'" << std::endl;
		return 0;
	}
	str = "create table if not exists server_info (id INT,ip CHAR(50))";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'configuration'" << std::endl;
		return 0;
	}
	str = "create table if not exists node_info (server INT,name CHAR(100),'private' TINYINT default 0, 'anti' TINYINT default 0, 'freeze' TINYINT default 0, 'check' TINYINT default 0)";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'node_info'" << std::endl;
		return 0;
	}
	/**
	 * 127.0.0.1 into sql
	 **/
	str = "delete from server_info where ip = '127.0.0.1'";
	if(da.db_exec(str)==false){
		std::cout << "SQLite process fail" << std::endl;
		return 0;
	}
	str = "insert into server_info (id,ip) values (0,'127.0.0.1')";
	if(da.db_exec(str)==false){
		std::cout << "SQLite process fail" << std::endl;
		return 0;
	}
	/**
	 * Getting root
	 **/
	if(da.root==""){
		std::cout << "Fail on detecting root" << std::endl;
		return 0;
	}
	/**
	 * Insert root into sql
	 **/
	str = "delete from configuration where category = 'server' and key = 'root'";
	if(da.db_exec(str)==false){
		std::cout << "SQLite process fail" << std::endl;
		return 0;
	}
	str = "insert into configuration (category,key,value) values ('server','root','"+da.root+"')";
	if(da.db_exec(str)==false){
		std::cout << "SQLite process fail" << std::endl;
		return 0;
	}
	return 0;
}
