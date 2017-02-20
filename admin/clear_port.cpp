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
	if(da.db_open(da.root+"/sqlite/tcp.db")==false){
		echo();
		return 0;
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
					return 0;
				}
			}
			else{
				std::cout << "Success" << std::endl;
				return 0;
			}
		}
		echo();
	}
	else{
		std::cout << "Abort" << std::endl;
	}
	return 0;
}
