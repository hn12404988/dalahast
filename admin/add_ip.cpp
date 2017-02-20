#include <dalahast/dalahast.h>

void get(std::string &str){
	str.clear();
	char input[100];
	std::cin.getline(input,100);
	str = input;
}

void echo(){
	std::cout << "SQL fail on processing, please check 'dalahast' table in error_log database" << std::endl;	
}

int main(){
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
				return 0;
			}
			else{
				std::cout << "What is the ip of the server?" << std::endl;
				get(ip);
				str = "select id from server where ip = '"+ip+"' limit 1";
				if(da.db_is_exec(str)==true){
					if(da.is.size()==1){
						std::cout << "ip is already registered by server_id: " << da.is[0] << std::endl;
						return 0;
					}
					else{
						str = "insert into server (id,ip) values ("+id+",'"+ip+"')";
						if(da.db_exec(str)==true){
							std::cout << "Success" << std::endl;
							return 0;
						}
					}
				}
			}
		}
	}
	echo();
	return 0;
}
