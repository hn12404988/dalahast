#include <review/unix_server.h>
#include <review/maria_wrapper.h>
#include <review/configure.h>

unix_server server;
review::IS args {"name","msg"};
review::IS name_list;
int max {0};
std::string root;
const short int buff_size {100};

auto execute = [&](const short int index){
	std::string name;
	int i;
	char index_char;
	bool forget;
	FILE *in {nullptr};
	char buff[buff_size];
	while(server.msg_to_args(index)==true){
		/**
		 * 
		 **/
		index_char = server.raw_msg[index].back();
		if(index_char=='&'){
			forget = true;
			server.raw_msg[index].pop_back();
			
		}
		else{
			forget = false;
		}
		/**
		 * 
		 **/
		i = server.raw_msg[index].length()-1;
		for(;i>=1;--i){
			if(server.raw_msg[index][i]==']'){
				break;
			}
		}
		if(i<=0){
			server.echo_back_error(server.socketfd[index],"Fail on getting name");
			continue;
		}
		++i;
		name = server.raw_msg[index].substr(i);
		server.raw_msg[index].resize(i);
		/**
		 * 
		 **/
		for(i=0;i<max;++i){
			if(name==name_list[i]){
				break;
			}
		}
		if(i==max){
			server.echo_back_error(server.socketfd[index],"name doesn't exist");
			continue;
		}
		/**
		 * 
		 **/
		i = server.raw_msg[index].length();
		if(i>1){
			--i;
			for(;i>=0;--i){
				index_char = server.raw_msg[index][i];
				if((index_char>=48 && index_char<=57) || (index_char>=97 && index_char<=122) || (index_char>=65 && index_char<=90) || index_char==91 || index_char==93 || index_char==44){
				}
				else{
					break;
				}
			}
			if(i>=0){
				server.echo_back_error(server.socketfd[index],"msg contain illegal character");
				continue;
			}
			if(forget==true){
				name = root+name+".o "+server.raw_msg[index]+" &";
				server.echo_back_msg(server.socketfd[index],"1");
				popen(name.c_str(),"r");
			}
			else{
				name = root+name+".o "+server.raw_msg[index];
				if(in!=nullptr){
					pclose(in);
					in = nullptr;
				}
				in = popen(name.c_str(),"r");
				if(in==nullptr){
					server.echo_back_error(server.socketfd[index],"in is nullptr");
				}
				else{
					name.clear();
					while(fgets(buff, buff_size, in)!=nullptr){
						name.append(buff);
					}
					server.echo_back_msg(server.socketfd[index],name);
				}
			}
		}
		else{
			server.echo_back_error(server.socketfd[index],"msg is too short");
		}
	}
	server.done(index);
};

bool init(){
	configure conf(__FILE__);
	std::string str;
	str = "select node from info where server = "+conf.server+" and node_type = 'EXECUTE'";
	conf.to_database("node");
	conf.res_send(str);
	if(conf.res==nullptr){
		std::cout << "/*********** Init Fail ****************/" << std::endl;
		std::cout << "Server: " << conf.server << std::endl;
		std::cout << "Node: " << conf.node << std::endl;
		conf.node_crash_log();
		return false;
	}
	while(conf.res->next()==true){
		name_list.push_back(conf.res->getString(1));
	}
	max = name_list.size();
	root = conf.get_server_root();
	return true;
}

void log(){
	configure conf(__FILE__);
	conf.args_log(&args);
	conf.anti_data_racing(false);
	conf.private_node(true,"STRING");
}

int main(int argc, char* argv[]){
	if(argc>1){
		log();
		return 0;
	}
	server.execute = execute;
	server.set_args(nullptr);
	if(init()==true){
		if(server.init(__FILE__)==true){
			server.start_accept();
		}
	}
	return 0;
}
