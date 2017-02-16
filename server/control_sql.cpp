#include <hast/tcp_server.h>
#include <hast/client_core.h>
#include <dalahast/dalahast.h>
#include <string.h>

tcp_server server;
std::string port;

auto execute = [&](const short int index){
	client_core client;
	client.import_location(&location,5);
	client.set_error_node(0,__FILE__);
	while(server.msg_to_args(index)==true){
	}
	server.done(index);
};

void init(){
}

int main(int argc, char* argv[]){
	server.set_args(nullptr);
	server.execute = execute;
	init();
	if(server.init(port)==true){
		server.start_accept();
	}
	return 0;
}
