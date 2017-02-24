/*
 * Copyright (c) 2015, Peter Thorson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the WebSocket++ Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <map>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
//#include <websocketpp/extensions/permessage_deflate/enabled.hpp>
#include <hast/client_core.h>
#include <dalahast/dalahast.h>
//#include <iostream>

struct testee_config : public websocketpp::config::asio {
    // pull default settings from our core config
    typedef websocketpp::config::asio core;

    typedef core::concurrency_type concurrency_type;
    typedef core::request_type request_type;
    typedef core::response_type response_type;
    typedef core::message_type message_type;
    typedef core::con_msg_manager_type con_msg_manager_type;
    typedef core::endpoint_msg_manager_type endpoint_msg_manager_type;

    typedef core::alog_type alog_type;
    typedef core::elog_type elog_type;
    typedef core::rng_type rng_type;
    typedef core::endpoint_base endpoint_base;

    static bool const enable_multithreading = true;

    struct transport_config : public core::transport_config {
        typedef core::concurrency_type concurrency_type;
        typedef core::elog_type elog_type;
        typedef core::alog_type alog_type;
        typedef core::request_type request_type;
        typedef core::response_type response_type;

        static bool const enable_multithreading = true;
    };

    typedef websocketpp::transport::asio::endpoint<transport_config>transport_type;

    static const websocketpp::log::level elog_level = websocketpp::log::elevel::none;
    static const websocketpp::log::level alog_level = websocketpp::log::alevel::none;
        
    /// permessage_compress extension
    //struct permessage_deflate_config {};
	//typedef websocketpp::extensions::permessage_deflate::enabled<permessage_deflate_config> permessage_deflate_type;

};

typedef websocketpp::server<testee_config> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::thread> thread_ptr;

std::string port;
const size_t amount {10};
da::IS location;
short int location_amount;
std::string root_prefix;
std::map<std::thread::id,client_core*> client_index;
typedef std::map<std::thread::id,client_core*> client_type;

// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
	client_core *client {client_index[std::this_thread::get_id()]};
	short int i,j;
	char callback_index;
	std::string message,node;
	/**
	 * 
	 **/
	message = msg->get_payload();
	callback_index = message.back();
	if(callback_index=='\0'){
		message = "0{\"Error\":\"No content\"}0";
		s->send(hdl, message.c_str(), msg->get_opcode());
		return;
	}
	message.pop_back();
	/**
	 * 
	 **/
	j = message.length();
	for(i=0;i<j;++i){
		if(message[i]=='{'){
			break;
		}
	}
	if(i==j){
		message = "0{\"Error\":\"Fail on getting node\",\"message\":\""+message+"\"}0";
		s->send(hdl, message.c_str(), msg->get_opcode());
		return;
	}
	/**
	 * 
	 **/
	node = message.substr(0,i);
	node = root_prefix+node+".socket";
	message = message.substr(i);
	for(i=1;i<location_amount;++i){
		if(location[i]==node){
			break;
		}
	}
	if(i==location_amount){
		message = "0{\"Error\":\"Node doesn't exist\",\"Node\":\""+node+"\"}0";
		s->send(hdl, message.c_str(), msg->get_opcode());
		return;
	}
	/**
	 * 
	 **/
	if(client->fire(i,message)>0){
		message = "0{\"Error\":\"Fail on firing\"}";
		message.push_back(callback_index);
		s->send(hdl, message.c_str(), msg->get_opcode());
	}
	else{
		if(message==""){
			message = "0{\"Error\":\"Empty reply\"}";
		}
		message.push_back(callback_index);
		s->send(hdl, message.c_str(), msg->get_opcode());
	}
}

void on_socket_init(websocketpp::connection_hdl, boost::asio::ip::tcp::socket & s) {
    boost::asio::ip::tcp::no_delay option(true);
    s.set_option(option);
}

void build_index(std::vector<thread_ptr> *ts){
	for (int i = 0; i < ts->size(); ++i) {
		client_index[(*ts)[i]->get_id()] = new client_core;
		client_index[(*ts)[i]->get_id()]->import_location(&location,5);
		client_index[(*ts)[i]->get_id()]->set_error_node(0,__FILE__);
	}
}

bool init(){
	dalahast da(__FILE__);
	std::string str;
	int j;
	j = da.my_server_id();
	if(j==-1){
		return false;
	}
	port = da.get_port(j,"web_port");
	root_prefix = da.root+da::server_prefix;
	if(da.db_open(da.root+"/sqlite/info.db")==false){
		return false;
	}
	str = "select node from node WHERE private = 0 AND interface = 'FILE'";
	if(da.db_is_exec(str)==false){
		return false;
	}
	location.push_back(da.root+da::server_prefix+"private/error_node.socket");
	j = da.is.size()-1;
	for(;j>=0;--j){
		location.push_back(da.root+da::server_prefix+da.is[j]+".socket");
	}
	location_amount = location.size();
	return true;
}

void log(){
	dalahast da(__FILE__);
	short int i;
	i = da.my_server_id();
	port = da.get_port(i,"web_port");
	da.port_log(port);
}

int main(int argc, char * argv[]) {
	if(argc>1){
		log();
		return 0;
	}
	if(init()==false){
		dalahast da(__FILE__);
		da.error_log("init() return false");
		return 0;
	}
    // Create a server endpoint
    server testee_server;

    short int control_port = std::stoi(port);
	short int i;
    try {
        // Total silence
        testee_server.clear_access_channels(websocketpp::log::alevel::all);
        testee_server.clear_error_channels(websocketpp::log::alevel::all);

        // Initialize ASIO
        testee_server.init_asio();
        testee_server.set_reuse_addr(true);

        // Register our message handler
        testee_server.set_message_handler(bind(&on_message,&testee_server,::_1,::_2));
        testee_server.set_socket_init_handler(bind(&on_socket_init,::_1,::_2));

        // Listen on specified port with extended listen backlog
        testee_server.set_listen_backlog(1000);
        testee_server.listen(control_port);

        // Start the server accept loop
        testee_server.start_accept();

        // Start the ASIO io_service run loop
		std::vector<thread_ptr> ts;
		for (i = 0; i < amount; i++) {
			ts.push_back(websocketpp::lib::make_shared<websocketpp::lib::thread>(&server::run, &testee_server));
		}
		build_index(&ts);
		for (i = 0; i < amount; i++) {
			ts[i]->join();
		}
    } catch (websocketpp::exception const & e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
	client_type::iterator it;
	client_type::iterator it_end;
	it = client_index.begin();
	it_end = client_index.end();
	for(;it!=it_end;++it){
		delete it->second;
	}
	return 0;
}
