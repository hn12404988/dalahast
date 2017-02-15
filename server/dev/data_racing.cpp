#include <review/tcp_server.h>
#include <review/maria_wrapper.h>
#include <review/review_type.h>
#include <review/configure.h>
#include <review/review_time.h>
#include <review/ss_tool.h>
#include <poll.h>
#include <mutex>


tcp_server server;
review::IS args {"server","node","data"};
review_type rtype;
std::mutex mx;
std::mutex sql_mx;
std::string port;
short int timeout {0};
short int insert_error {0};
review::ISS iss;
maria_wrapper sql_global("error_log",__FILE__);

auto execute = [&](const short int index){
	review::SS ss;
	struct pollfd pol;
	ss_tool _ss;
	_ss.import_fixed(&args);
	int i,j,tmp;
	std::string str;
	pol.events = POLLIN;
	ss["server"] = "0";
	ss["node"] = "0";
	ss["data"] = "0";
	while(server.msg_to_args(index)==true){
		pol.fd = server.get_socket(index);
		if(_ss.json_to(ss,server.raw_msg[index])==false){
			server.echo_back_error(server.socketfd[index],"Fail on parsing args");
			break;
		}
		mx.lock();
		j = iss.size();
		tmp = -1;
		for(i=0;i<j;++i){
			if(tmp==-1){
				if((*iss[i])["server"]=="0" && (*iss[i])["node"]=="0" && (*iss[i])["data"]=="0"){
					tmp = i;
				}
			}
			if((*iss[i])["server"]==ss["server"] && (*iss[i])["node"]==ss["node"] && (*iss[i])["data"]==ss["data"]){
				i = -1;
			}
		}
		if(tmp==-1){
			iss.push_back(new review::SS);
			tmp = iss.size()-1;
			(*iss[tmp])["server"] = ss["server"];
			(*iss[tmp])["node"] = ss["node"];
			(*iss[tmp])["data"] = ss["data"];
		}
		else{
			(*iss[tmp])["server"] = ss["server"];
			(*iss[tmp])["node"] = ss["node"];
			(*iss[tmp])["data"] = ss["data"];
		}
		mx.unlock();
		server.echo_back_msg(server.socketfd[index],"1");
		j = poll(&pol, 1, timeout);
		if((j>0) && (pol.revents & POLLIN)){
		}
		else{
			sql_mx.lock();
			sql_global.pre[insert_error]->setString(1,ss["server"]);
			sql_global.pre[insert_error]->setString(2,ss["node"]);
			sql_global.pre[insert_error]->setString(3,ss["data"]);
			if(sql_global.update_execute(insert_error)!=1){
				std::cout << std::endl;
				std::cout << "!!!!!! [data_racing] Fail on inserting data!!!!!!" << std::endl;
				std::cout << "From Server: " << ss["server"] << std::endl;
				std::cout << "From Node: " << ss["node"] << std::endl;
				std::cout << "Data is: " << ss["data"] << std::endl;
			}
			sql_mx.unlock();
		}
		(*iss[tmp])["server"] = "0";
		(*iss[tmp])["node"] = "0";
		(*iss[tmp])["data"] = "0";
	}
	server.done(index);
	return;
};

bool init(){
	configure conf(__FILE__);
	port = conf.get_configuration("server","data_racing_port");
	timeout = std::stoi(conf.get_configuration("data_racing","timeout"))*1000;
	if(sql_global.prepare("insert into error_log.data_racing (server,node,data,time) values (?,?,?,NOW())")==-1){
		std::cout << "/*********** Init Fail ****************/" << std::endl;
		std::cout << "Server: " << conf.server << std::endl;
		std::cout << "Node: " << conf.node << std::endl;
		conf.node_crash_log();
		return false;
	}
	return true;
}


void log(){
	configure conf(__FILE__);
	conf.args_log(&args,"TCP_SOCKET",conf.get_configuration("server","data_racing_port"));
	conf.anti_data_racing(false);
	conf.private_node(true,"JSON");
	conf.sql_insert_log("[error_log]","[data_racing]");
}

int main(int argc, char* argv[]){
	if(argc>1){
		log();
		return 0;
	}
	server.execute = execute;
	server.set_args(nullptr);
	if(init()==true){
		if(server.init(port,1)==true){
			server.start_accept();
		}
	}
	rtype.clear_3d<review::ISS>(iss);
	return 0;
}
