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
				return 0;
			}
			else{
				std::cout << "What is the port?" << std::endl;
				get(port);
				str = "select name from port where server_id = "+id+" and port = "+port+" limit 1";
				if(da.db_is_exec(str)==true){
					if(da.is.size()==1){
						std::cout << "port is already registered by name: " << da.is[0] << std::endl;
						return 0;
					}
					else{
						std::cout << "What is the name of the port?" << std::endl;
						get(str);
						str = "insert into port (server_id,port,name) values ("+id+","+port+",'"+str+"')";
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
