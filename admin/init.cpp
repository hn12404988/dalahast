#include <dalahast/dalahast.hpp>

void get(std::string &str){
	str.clear();
	char input[100];
	std::cin.getline(input,100);
	str = input;
}

int main(){
	dalahast da;
	std::string str,local_ip,server_index,local_port,web_port;
	int i;
	bool tcp {true};
	bool to_center {true}, web {true};
	/**
	 * Init
	 **/
	std::cout << "Is this server need to set TCP/IP port for other servers to connect?(y/n)" << std::endl;
	get(str);
	if(str=="y" || str=="Y"){
		std::cout << "What is the ip of this server for TCP/IP port?(ex: 192.168.1.11)" << std::endl;
		get(local_ip);
		if(local_ip==""){
			std::cout << "Abort" << std::endl;
			return 0;
		}
		/**
		 * 
		 **/
		std::cout << "What is the port of this server for TCP/IP port?(ex: 8888)" << std::endl;
		get(local_port);
		if(local_port==""){
			std::cout << "Abort" << std::endl;
			return 0;
		}
		tcp = true;
	}
	else if(str=="n" || str=="N"){
		tcp = false;
		local_port = "-1";
		local_ip = "127.0.0.1";
	}
	else{
		std::cout << "Abort" << std::endl;
		return 0;
	}
	/**
	 * 
	 **/
	std::cout << "Is this server need to receive websocket call from web front-end?(y/n)" << std::endl;
	get(str);
	if(str=="y" || str=="Y"){
		std::cout << "What is the port of this server for websocket port?(ex: 8889)" << std::endl;
		get(web_port);
		if(web_port==""){
			std::cout << "Abort" << std::endl;
			return 0;
		}
		web = true;
	}
	else if(str=="n" || str=="N"){
		web = false;
	}
	else{
		std::cout << "Abort" << std::endl;
		return 0;
	}
	/**
	 * 
	 **/
	std::cout << "What is the id of this server?(integer)" << std::endl;
	get(server_index);
	if(server_index==""){
		std::cout << "Abort" << std::endl;
		return 0;
	}
	i = std::stoi(server_index);
	server_index = std::to_string(i);
	/**
	 * Build tables in self.db
	 * self
	 **/
	if(da.db_open(da.root+"/sqlite/self.db")==false){
		std::cout << "Fail on opening database 'self'" << std::endl;
		return 0;
	}
	str = "create table if not exists self (key CHAR(100), value CHAR(100))";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'self'" << std::endl;
		return 0;
	}
	/**
	 * 
	 **/
	str = "delete from self";
	if(da.db_exec(str)==false){
		std::cout << "Fail on deleting 'self' table" << std::endl;
		return 0;
	}
	if(tcp==true){
		str = "insert into self (key,value) values ('tcp','1')";
	}
	else{
		str = "insert into self (key,value) values ('tcp','0')";
	}
	if(da.db_exec(str)==false){
		std::cout << "SQL fail on self table: tcp" << std::endl;
		return 0;
	}
	if(web==true){
		str = "insert into self (key,value) values ('websocket','1')";
	}
	else{
		str = "insert into self (key,value) values ('websocket','0')";
	}
	if(da.db_exec(str)==false){
		std::cout << "SQL fail on self table: websocket" << std::endl;
		return 0;
	}
	str = "insert into self (key,value) values ('local_ip','"+local_ip+"')";
	if(da.db_exec(str)==false){
		std::cout << "SQL fail on self table: local_ip" << std::endl;
		return 0;
	}
	str = "insert into self (key,value) values ('server_id','"+server_index+"')";
	if(da.db_exec(str)==false){
		std::cout << "SQL fail on self table: server_id" << std::endl;
		return 0;
	}
	str = "insert into self (key,value) values ('local_port','"+local_port+"')";
	if(da.db_exec(str)==false){
		std::cout << "SQL fail on self table: local_port" << std::endl;
		return 0;
	}
	/**
	 * General data start from here.
	 **/
	if(da.db_open(da.root+"/sqlite/topology.db")==false){
		std::cout << "Fail on opening database 'topology'" << std::endl;
		return 0;
	}
	/**
	 * Build tables in topology.db
	 **/
	str = "create table if not exists topology (from_node CHAR(100), to_server CHAR(50), to_node CHAR(100), type CHAR(50))";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'topology'" << std::endl;
		return 0;
	}
	/**
	 * Build tables in error_log.db
	 * dalahast, request, socket, error
	 **/
	if(da.db_open(da.root+"/sqlite/error_log.db")==false){
		std::cout << "Fail on opening database 'error_log'" << std::endl;
		return 0;
	}
	str = "create table if not exists dalahast (node CHAR(100), message TEXT, message2 TEXT, time DATETIME)";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'dalahast'" << std::endl;
		return 0;
	}
	str = "create table if not exists request (from_node CHAR(100), to_server INT, to_node CHAR(100), message TEXT, reply TEXT, time DATETIME)";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'request'" << std::endl;
		return 0;
	}
	str = "create table if not exists socket (from_node CHAR(100), to_server INT, to_node CHAR(100), message TEXT, error_flag INT, time DATETIME)";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'socket'" << std::endl;
		return 0;
	}
	str = "create table if not exists error (node CHAR(100), message TEXT, time DATETIME)";
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
	 * node
	 **/
	if(da.db_open(da.root+"/sqlite/info.db")==false){
		std::cout << "Fail on opening database 'info'" << std::endl;
		return 0;
	}
	str = "create table if not exists node (node CHAR(300),'interface' CHAR(10) default 'FILE', 'private' INT default 0, 'anti' INT default 0, 'freeze' INT default 0, 'check' INT default 0)";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'node'" << std::endl;
		return 0;
	}
	/**
	 * Build tables in tcp.db
	 * server,port
	 **/
	if(da.db_open(da.root+"/sqlite/tcp.db")==false){
		std::cout << "Fail on opening database 'tcp'" << std::endl;
		return 0;
	}
	str = "create table if not exists 'server' ('id' INT, ip CHAR(30))";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'server'" << std::endl;
		return 0;
	}
	/**
	 * 
	 **/
	str = "delete from 'server' where id = "+server_index;
	if(da.db_exec(str)==false){
		std::cout << "SQL fail on table 'server' deleting" << std::endl;
		return 0;
	}
	if(tcp==true){
		str = "insert into 'server' (id,ip) values ("+server_index+",'"+local_ip+"')";
		if(da.db_exec(str)==false){
			std::cout << "SQL fail on table 'server' inserting" << std::endl;
			return 0;
		}
	}
	/**
	 * 
	 **/
	str = "create table if not exists 'port' (server_id INT, port INT, name CHAR(100))";
	if(da.db_exec(str)==false){
		std::cout << "Fail on building table 'port'" << std::endl;
		return 0;
	}
	/**
	 * 
	 **/
	str = "delete from 'port' where server_id = "+server_index+" and name = 'main_port'";
	if(da.db_exec(str)==false){
		std::cout << "SQL fail on table 'port' deleting" << std::endl;
		return 0;
	}
	if(tcp==true){
		str = "insert into 'port' (server_id,port,name) values ("+server_index+","+local_port+",'main_port')";
		if(da.db_exec(str)==false){
			std::cout << "SQL fail on table 'tcp_port' inserting" << std::endl;
			return 0;
		}
	}
	if(web==true){
		str = "insert into 'port' (server_id,port,name) values ("+server_index+","+web_port+",'web_port')";
		if(da.db_exec(str)==false){
			std::cout << "SQL fail on table 'web_port' inserting" << std::endl;
			return 0;
		}
	}
	return 0;
}
