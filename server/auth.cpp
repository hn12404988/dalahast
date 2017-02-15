#include <stdio.h>
#include <iostream>
#include <string.h>
#include "fcgio.h"
#include <fstream>

#include <review/maria_wrapper.h>
#include <review/configure.h>
#include <exception>

using namespace std;

bool check_pw(std::string pw){
	short int i,j;
	j = pw.length();
	// 0~9 is 48~57
	// a~z is 97~122
	// A~Z is 65~90
	for(i=0;i<j;++i){
		if((pw[i]>=48 && pw[i]<=57) || (pw[i]>=97 && pw[i]<=122) || (pw[i]>=65 && pw[i]<=90)){
			
		}
		else{
			break;
		}
	}
	if(i==j){
		return true;
	}
	else{
		return false;
	}
}
int main(void) {
    // Backup the stdio streambufs
    streambuf * cin_streambuf  = cin.rdbuf();
    streambuf * cout_streambuf = cout.rdbuf();
    streambuf * cerr_streambuf = cerr.rdbuf();

    FCGX_Request request;
	
    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);

	std::string str,key,value,cookie,staff_name,password,ip;
	maria_wrapper sql("staff",__FILE__);
	str = "select id from staff.info where name = ? and password = ? limit 1";
	if(sql.prepare(str)==-1){
		std::cout << "Fail on init" << std::endl;
		std::cout << "Server: " << sql.server << std::endl;
		std::cout << "Node: " << sql.node<< std::endl;
		return 0;
	}
	short int i,j;
	short int s {0};
	bool change;
	char **env {nullptr};
	char **env_init {nullptr};

    while (FCGX_Accept_r(&request) == 0) {
        fcgi_streambuf cin_fcgi_streambuf(request.in);
        fcgi_streambuf cout_fcgi_streambuf(request.out);
        fcgi_streambuf cerr_fcgi_streambuf(request.err);

        cin.rdbuf(&cin_fcgi_streambuf);
        cout.rdbuf(&cout_fcgi_streambuf);
        cerr.rdbuf(&cerr_fcgi_streambuf);
		
        //const char * uri = FCGX_GetParam("REQUEST_URI", request.envp);
		// Note: the fcgi_streambuf destructor will auto flush
		env = request.envp;
		env_init = request.envp;
		cookie.clear();
		ip.clear();
		while (*(++env)){
			str = (*env);
			if(str.find("HTTP_COOKIE")!=std::string::npos){
				cookie = FCGX_GetParam("HTTP_COOKIE", env_init);
			}
			if(str.find("REMOTE_ADDR")!=std::string::npos){
				ip = FCGX_GetParam("REMOTE_ADDR", env_init);
			}
		}
		try{
			if(ip=="" || cookie==""){
				throw false;
			}
			if(ip.substr(0,10)!="192.168.1."){
				throw false;
			}
			sql.reset();
			//HTTP_COOKIE=lang_ui=1; bg_color=green; font_color=white; lang_read=1; browse=4
			j = cookie.length();
			change = false;
			staff_name.clear();
			password.clear();
			key.clear();
			value.clear();
			for(i=0;i<j;++i){
				if(cookie[i]=='='){
					change = true;
					continue;
				}
				if(cookie[i]==';'){
					if(change==false){
						throw false;
					}
					change = false;
					if(key=="staff_name"){
						staff_name = value;
					}
					else if(key=="password"){
						password = value;
					}
					key.clear();
					value.clear();
					++i;
					continue;
				}
				if(change==false){
					key += cookie[i];
				}
				else{
					value += cookie[i];
				}
			}
			if(key=="staff_name"){
				staff_name = value;
			}
			else if(key=="password"){
				password = value;
			}
			if(check_pw(password)==false){
				throw false;
			}
			sql.pre[s]->setString(1,staff_name);
			sql.pre[s]->setString(2,password);
			sql.res_execute(s);
			if(sql.res==nullptr){
				throw false;
			}
			if(sql.res->next()==true){
				cout << "Status: 200\r\n"
					 << "Content-type: text/plain\r\n"
					 << "\r\n";
			}
			else{
				throw false;
			}
		}
		catch(bool b){
			cout << "Status: 401\r\n"
				 << "Content-type: text/plain\r\n"
				 << "\r\n";
		}
	}
	// restore stdio streambufs
	cin.rdbuf(cin_streambuf);
	cout.rdbuf(cout_streambuf);
	cerr.rdbuf(cerr_streambuf);
	configure conf(__FILE__);
	conf.node_crash_log();
	return 0;
}
