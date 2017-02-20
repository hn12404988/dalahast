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
	short int i;
	if(da.db_open(da.root+"/sqlite/tcp.db")==false){
		echo();
		return 0;
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
	return 0;
}
