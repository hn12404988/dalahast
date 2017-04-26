#include <dalahast/dalahast.hpp>

void get(std::string &str){
	str.clear();
	char input[100];
	std::cin.getline(input,100);
	str = input;
}

void echo(){
	std::cout << "SQL fail on processing, please check 'dalahast' table in error_log database" << std::endl;	
}

void add_ip(){
	dalahast da;
	std::string id,ip,str;
	short int i;
	std::cout << "What is the server_id of the server?" << std::endl;
	get(id);
	i = std::stoi(id);
	id = std::to_string(i);
	if(da.db_open(da.root+"/sqlite/tcp.db")==true){
		str = "select ip from server where id = "+id+" limit 1";
		if(da.db_is_exec(str)==true){
			if(da.is.size()==1){
				std::cout << "server_id is already taken by ip: " << da.is[0] << std::endl;
				return;
			}
			else{
				std::cout << "What is the ip of the server?" << std::endl;
				get(ip);
				str = "select id from server where ip = '"+ip+"' limit 1";
				if(da.db_is_exec(str)==true){
					if(da.is.size()==1){
						std::cout << "ip is already registered by server_id: " << da.is[0] << std::endl;
						return;
					}
					else{
						str = "insert into server (id,ip) values ("+id+",'"+ip+"')";
						if(da.db_exec(str)==true){
							std::cout << "Success" << std::endl;
							return;
						}
					}
				}
			}
		}
	}
	echo();
	return;
}

void clear_ip(){
	dalahast da;
	std::string id,str;
	short int i;
	if(da.db_open(da.root+"/sqlite/tcp.db")==false){
		echo();
		return;
	}
	std::cout << "Clear all?(y/n)" << std::endl;
	get(str);
	if(str=="y" || str=="Y"){
		str = "delete from server";
		if(da.db_exec(str)==false){
			echo();
		}
		else{
			std::cout << "Success" << std::endl;
		}
	}
	else if(str=="n" || str=="N"){
		str = "select id,ip from server";
		if(da.db_iss_exec(str)==true){
			if(da.iss.size()>0){
				std::cout << "Following are all the data" << std::endl;
				for(i=0;i<da.iss.size();++i){
					std::cout << (*da.iss[i])["ip"] << " is server_id: " << (*da.iss[i])["id"] << std::endl;
				}
				std::cout << "What is the server_id of the data you want to remove?" << std::endl;
				get(id);
				i = std::stoi(id);
				id = std::to_string(i);
				str = "delete from server where id = "+id;
				if(da.db_exec(str)==true){
					std::cout << "Success" << std::endl;
					return;
				}
			}
			else{
				std::cout << "Success" << std::endl;
				return;
			}
		}
		echo();
	}
	else{
		std::cout << "Abort" << std::endl;
	}
	return;
}

void list_ip(){
	dalahast da;
	short int i;
	if(da.db_open(da.root+"/sqlite/tcp.db")==false){
		echo();
		return;
	}
	if(da.db_iss_exec("select id,ip from server")==true){
		if(da.iss.size()>0){
			std::cout << "Following are all the data" << std::endl;
			for(i=0;i<da.iss.size();++i){
				std::cout << (*da.iss[i])["id"] << " : " << (*da.iss[i])["ip"] << std::endl;
			}
		}
		else{
			std::cout << "Empty" << std::endl;
		}
	}
	else{
		echo();
	}
	return;
}

void add_port(){
	dalahast da;
	std::string id,port,str;
	short int i;
	std::cout << "What is the server_id of the port?" << std::endl;
	get(id);
	i = std::stoi(id);
	id = std::to_string(i);
	if(da.db_open(da.root+"/sqlite/tcp.db")==true){
		str = "select ip from server where id = "+id+" limit 1";
		if(da.db_is_exec(str)==true){
			if(da.is.size()==0){
				std::cout << "server_id is not registered yet" << std::endl;
				return;
			}
			else{
				std::cout << "What is the port?" << std::endl;
				get(port);
				str = "select name from port where server_id = "+id+" and port = "+port+" limit 1";
				if(da.db_is_exec(str)==true){
					if(da.is.size()==1){
						std::cout << "port is already registered by name: " << da.is[0] << std::endl;
						return;
					}
					else{
						std::cout << "What is the name of the port?" << std::endl;
						get(str);
						str = "insert into port (server_id,port,name) values ("+id+","+port+",'"+str+"')";
						if(da.db_exec(str)==true){
							std::cout << "Success" << std::endl;
							return;
						}
					}
				}
			}
		}
	}
	echo();
	return;
}

void clear_port(){
	dalahast da;
	std::string id,port,str;
	short int i;
	if(da.db_open(da.root+"/sqlite/tcp.db")==false){
		echo();
		return;
	}
	std::cout << "Clear all?(y/n)" << std::endl;
	get(str);
	if(str=="y" || str=="Y"){
		str = "delete from port";
		if(da.db_exec(str)==false){
			echo();
		}
		else{
			std::cout << "Success" << std::endl;
		}
	}
	else if(str=="n" || str=="N"){
		str = "select server_id,port,name from port";
		if(da.db_iss_exec(str)==true){
			if(da.iss.size()>0){
				std::cout << "Following are all the data" << std::endl;
				for(i=0;i<da.iss.size();++i){
					std::cout << (*da.iss[i])["name"] << " is port: " << (*da.iss[i])["port"] << "from server_id: " << (*da.iss[i])["server_id"] << std::endl;
				}
				std::cout << "What is the server_id of the data you want to remove?" << std::endl;
				get(id);
				i = std::stoi(id);
				id = std::to_string(i);
				std::cout << "What is the port of the data you want to remove?" << std::endl;
				get(port);
				i = std::stoi(port);
				port = std::to_string(i);
				str = "delete from port where server_id = "+id+" and port = "+port;
				if(da.db_exec(str)==true){
					std::cout << "Success" << std::endl;
					return;
				}
			}
			else{
				std::cout << "Success" << std::endl;
				return;
			}
		}
		echo();
	}
	else{
		std::cout << "Abort" << std::endl;
	}
	return;
}

void list_port(){
	dalahast da;
	short int i;
	if(da.db_open(da.root+"/sqlite/tcp.db")==false){
		echo();
		return;
	}
	if(da.db_iss_exec("select server_id,port,name from port")==true){
		if(da.iss.size()>0){
			std::cout << "Following are all the data" << std::endl;
			for(i=0;i<da.iss.size();++i){
				std::cout << (*da.iss[i])["server_id"] << " : " << (*da.iss[i])["port"] << " : " << (*da.iss[i])["name"] << std::endl;
			}
		}
		else{
			std::cout << "Empty" << std::endl;
		}
	}
	else{
		echo();
	}
	return;
}

void help(){
	std::cout << "/*************** Manual *****************/" << std::endl;
	std::cout << "--add-ip" << std::endl;
	std::cout << "--clear-ip" << std::endl;
	std::cout << "--list-ip" << std::endl;
	std::cout << "--add-port" << std::endl;
	std::cout << "--clear-port" << std::endl;
	std::cout << "--list-port" << std::endl;
}

int main(int argc,char* argv[]){
	if(argc==1){
		std::cout << "No args input" << std::endl;
		return 0;
	}
	short int i;
	std::string str;
	for(i=1;i<argc;++i){
		str = argv[i];
		if(i==1 && str=="--help"){
			help();
			break;
		}
		if(str=="--add-ip"){
			add_ip();
		}
		else if(str=="--clear-ip"){
			clear_ip();
		}
		else if(str=="--list-ip"){
			list_ip();
		}
		else if(str=="--add-port"){
			add_port();
		}
		else if(str=="--clear-port"){
			clear_port();
		}
		else if(str=="--list-port"){
			list_port();
		}
		break;
	}
	return 0;
}
