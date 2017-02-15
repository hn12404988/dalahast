#include <review/maria_wrapper.h>
#include <review/unix_server.h>
#include <review/configure.h>

unix_server server;
review::IS args {"sql_access_init"};
std::string amount_str;

auto execute = [&](const short int index){
	std::string str;
	while(server.msg_to_args(index)==true){
		str = "{\"amount\":\""+amount_str+"\"}";
		server.echo_back_msg(server.socketfd[index],str);
	}
	server.done(index);
	return;
};

void init(){
	configure conf(__FILE__);
	amount_str = conf.get_configuration("server","amount");
}

void log(){
	configure conf(__FILE__);
	conf.args_log(&args);
	conf.anti_data_racing(false);
	conf.private_node(false);
}

int main (int argc, char* argv[]){
	if(argc>1){
		log();
		return 0;
	}
	server.execute = execute;
	server.set_args(nullptr);
	init();
	if(server.init(__FILE__)==true){
		server.start_accept();
	}
	configure conf(__FILE__);
	conf.node_crash_log();
	return 0;
}
