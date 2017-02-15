#include <string.h>
#include <string>
#include <review/local_ip.h>
#include <review/maria_wrapper.h>
#include <review/is_tool.h>

void error(maria_wrapper &sql, std::string &server, std::string &node, std::string &str){
	sql.to_database("node");
	sql.send_and_error("insert into scan_error (server,node,line,time) values ("+server+",'"+node+"','"+str+"',NOW())");
}

void find_fire(std::map<std::string,review::IS> &fires, std::string &str,std::string target){
	int i,j;
	i = target.length()+1;
	j = str.find(target+"(");
	str = str.substr(j+i);
	j = str.length();
	for(i=0;i<j;++i){
		if(str[i]==','){
			break;
		}
	}
	str.resize(i);
	fires[target].push_back(str);
}

void clear_SIS(std::map<std::string,review::IS> &sis){
	std::map<std::string,review::IS>::iterator it;
	std::map<std::string,review::IS>::iterator it_end;
	it = sis.begin();
	it_end = sis.end();
	for(;it!=it_end;++it){
		it->second.clear();
	}
	sis.clear();
}

int main(int argc ,char* argv[]){
	maria_wrapper sql("server",__FILE__);
	sql.build_statement();
	local_ip ip;
	std::string  root,server,str,send,str2;
	is_tool _is;
	review::IS node_list;
	review::IS name_list;
	review::IS init_block;
	review::IS unix_location;
	review::IS unix_name;
	review::IS fire_list {"fire","fireNstore","fireNforget","fireNclose","fireNfreeze","fireNcheck"};
	review::IS escape_folder {"dev/","bookmark/","comment/","crawler_socket/"};
	review::IS outside_nodes {"data_racing"};
	int i,j,k,l,m;
	short int fire_size;
	fire_size = fire_list.size();
	FILE *in {nullptr};
	FILE *in2 {nullptr};
	const int buff_size {500};
	char buff[buff_size];
	std::string compiler {"clang"};
	std::map<std::string,review::IS> fires;
	std::map<std::string,review::IS>::iterator it;
	std::map<std::string,review::IS>::iterator it_end;
	bool compile {false};
	bool is_init {false};
	bool got_it;
	bool echo {false};
	
	sql.send_and_error("select server,root from server where local_ip = '"+ip.get_ipv4()+"' limit 1");
	if(sql.res==nullptr || sql.res->next()==false){
		std::cout << "Can't find server index of this server" << std::endl;
		return 0;
	}
	server = sql.res->getString(1);
	root = sql.res->getString(2);
	sql.to_database("node");
	for(i=1;i<argc;++i){
		str = argv[i];
		if(str=="-clang" || str=="-gcc"){
			compile = true;
			compiler = str.substr(1);
			continue;
		}
		if(str=="-echo"){
			echo = true;
			continue;
		}
		if(str.find("/")==std::string::npos){
			for(j=0;j<outside_nodes.size();++j){
				if(str==outside_nodes[j]){
					node_list.push_back(root+str+".cpp");
					break;
				}
			}
			if(j<outside_nodes.size()){
				continue;
			}
			str = "ls -d "+root+str+"/*";
			if(in!=nullptr){
				pclose(in);
				in = nullptr;
			}
			in = popen(str.c_str(),"r");
			while(fgets(buff, buff_size, in)!=nullptr){
				if(strlen(buff)==1){
					break;
				}
				if(strstr(buff,".cpp")!=nullptr && strstr(buff,".cpp~")==nullptr){
					str = buff;
					str.pop_back();// \0
					node_list.push_back(str);
				}
			}
		}
		else{
			node_list.push_back(root+str+".cpp");
		}
	}
	if(in!=nullptr){
		pclose(in);
		in = nullptr;
	}
	if(node_list.size()==0){
		//scan all
		for(j=0;j<outside_nodes.size();++j){
			node_list.push_back(root+outside_nodes[j]+".cpp");
		}
		str = "ls -d "+root+"*/";
		in = popen(str.c_str(),"r");
		j = escape_folder.size();
		while(fgets(buff, buff_size, in)!=nullptr){
			if(strlen(buff)==1){
				break;
			}
			send = buff;
			send.pop_back();// \n
			for(i=0;i<j;++i){
				if(send==root+escape_folder[i]){
					break;
				}
			}
			if(i<j){
				continue;
			}
			str = "ls -d "+send+"*";
			if(in2!=nullptr){
				pclose(in2);
				in2 = nullptr;
			}
			in2 = popen(str.c_str(),"r");
			while(fgets(buff, buff_size, in2)!=nullptr){
				if(strlen(buff)==1){
					break;
				}
				if(strstr(buff,".cpp")!=nullptr && strstr(buff,".cpp~")==nullptr){
					str = buff;
					str.pop_back();// \0
					node_list.push_back(str);
				}
			}
		}
	}
	if(in!=nullptr){
		pclose(in);
		in = nullptr;
	}
	if(in2!=nullptr){
		pclose(in2);
		in2 = nullptr;
	}
	for(i=0;i<node_list.size();++i){
		str = node_list[i].substr(root.length());
		str = str.substr(0,str.length()-4);
		name_list.push_back(str);
		str = name_list[i];
		if(compile==true){
			std::cout << "/*******************************/" << std::endl;
			std::cout << "Compiling: " << str << std::endl;
			send = "./compile.o -"+compiler+" "+str;
			system(send.c_str());
			std::cout << "/*******************************/" << std::endl;
		}
	}
	if(in!=nullptr){
		pclose(in);
		in = nullptr;
	}
	for(i=0;i<name_list.size();++i){
		sql.send_and_error("delete from info where server = "+server+" and node = '"+name_list[i]+"'");
		if(sql.something_wrong==true){
			std::cout << "Fail on delete data from info table" << std::endl;
		}
		sql.send_and_error("delete from scan_error where server = "+server+" and node = '"+name_list[i]+"'");
		if(sql.something_wrong==true){
			std::cout << "Fail on delete data from scan_error table" << std::endl;
		}
		std::cout << "Initiate node: " << name_list[i] << std::endl;
		str = "./"+name_list[i]+".o log";
		system(str.c_str());
	}
	for(i=0;i<node_list.size();++i){
		sql.send_and_error("select input from info where server = "+server+" and node = '"+name_list[i]+"' limit 1");
		if(sql.res==nullptr || sql.res->next()==false){
			str = "Can not find input of node in info table";
			error(sql,server,name_list[i],str);
			continue;
		}
		if(in!=nullptr){
			fclose(in);
			in = nullptr;
		}
		in = fopen(node_list[i].c_str(),"r");
		str.clear();
		is_init = false;
		clear_SIS(fires);
		init_block.clear();
		unix_location.clear();
		unix_name.clear();
		while(fgets(buff, buff_size, in)!=nullptr){
			str.append(buff);
			//std::cout << str << std::endl;
			if(strstr(buff,"\n")!=nullptr){
				if(str.find("void init(")!=std::string::npos){
					is_init = true;
					str.clear();
					continue;
				}
				if(str.find("bool init(")!=std::string::npos){
					is_init = true;
					str.clear();
					continue;
				}
				if(str.find("int main(")!=std::string::npos || str.find("void log(")!=std::string::npos){
					is_init = false;
					str.clear();
					continue;
				}
				if(is_init==true){
					init_block.push_back(str);
					str.clear();
					continue;
				}
				for(k=0;k<fire_size;++k){
					if(str.find(fire_list[k]+"(")!=std::string::npos){
						send = str;
						j = send.find(",");
						if(j==std::string::npos){
							str.clear();
							continue;
						}
						send = send.substr(j+1);
						if(send.find(",")==std::string::npos){
							find_fire(fires,str,fire_list[k]);
						}
						break;
					}
				}
				str.clear();
			}
		}
		/**
		 * Delete repeat in fires.
		 **/
		it = fires.begin();
		it_end = fires.end();
		for(;it!=it_end;++it){
			_is.delete_repeat(it->second);
		}
		sql.send_and_error("delete from topology where from_server = "+server+" and from_node = '"+name_list[i]+"' and type!='EXECUTE'");
		if(sql.something_wrong==true){
			str = "SQL error when delete data in topology";
			error(sql,server,name_list[i],str);
		}
		k = init_block.size();
		got_it = false;
		for(j=0;j<k;++j){
			it = fires.begin();
			it_end = fires.end();
			for(;it!=it_end;++it){
				l = it->second.size()-1;
				for(;l>=0;--l){
					if(init_block[j].find(it->second[l])!=std::string::npos){
						m = init_block[j].find("=");
						if(m==std::string::npos){
							continue;
						}
						init_block[j] = init_block[j].substr(m+1);
						m = 0;
						str.clear();
						while(init_block[j][m]!=';' && init_block[j][m]!='\0'){
							if(init_block[j][m]==' '){
								++m;
								continue;
							}
							str += init_block[j][m];
							++m;
						}
						sql.to_database("tmp");
						sql.send_and_error("select to_node from unix_location where server = "+server+" and from_node = '"+name_list[i]+"' and location_index = "+str+" limit 1");
						if(sql.res==nullptr){
							str = "Wrong unix location_index: "+str;
							error(sql,server,name_list[i],str);
							break;
						}
						if(sql.res->next()==false){
							sql.send_and_error("select to_server, to_node from tcp_location where server = "+server+" and from_node = '"+name_list[i]+"' and location_index = "+str+" limit 1");
							if(sql.res==nullptr){
								str = "Wrong tcp location_index: "+str;
								error(sql,server,name_list[i],str);
								break;
							}
							if(sql.res->next()==false){
								str = "can not find location_index: "+str;
								error(sql,server,name_list[i],str);
								break;
							}
							send = sql.res->getString(2);
							str2 = sql.res->getString(1);
							str = "update tcp_location set got_it = true where server = "+server+" and from_node = '"+name_list[i]+"' and location_index = "+str+" limit 1";
							if(sql.update_send(str)!=1){
								str = "Fail on update tmp.tcp_location.";
								error(sql,server,name_list[i],str);
								break;
							}
						}
						else{
							send = sql.res->getString(1);
							str2 = "";
							str = "update unix_location set got_it = true where server = "+server+" and from_node = '"+name_list[i]+"' and location_index = "+str+" limit 1";
							if(sql.update_send(str)!=1){
								str = "Fail on update tmp.unix_location.";
								error(sql,server,name_list[i],str);
								break;
							}
						}
						sql.to_database("node");
						if(str2==""){
							str= "insert into topology (from_server,from_node,to_server,to_node,type) values ("+server+",'"+name_list[i]+"',"+server+",'"+send+"','"+it->first+"')";
						}
						else{
							str= "insert into topology (from_server,from_node,to_server,to_node,type) values ("+server+",'"+name_list[i]+"',"+str2+",'"+send+"','"+it->first+"')";
						}
						if(sql.update_send(str)!=1){
							str = "Fail when insert into topology table.";
							error(sql,server,name_list[i],str);
							break;
						}
					}
				}
				if(l>=0){
					break;
				}
			}
		}
		//And then tcp client.
		/**
		 * Check `got_it` is still false or not.
		 **/
		sql.to_database("tmp");
		sql.send_and_error("select server from unix_location where server = "+server+" and from_node = '"+name_list[i]+"' and got_it = false");
		if(sql.res!=nullptr && sql.res->next()==true){
			str = "There are still row which got_it = false in unix_location table.";
			error(sql,server,name_list[i],str);
		}
		sql.send_and_error("select server from tcp_location where server = "+server+" and from_node = '"+name_list[i]+"' and got_it = false");
		if(sql.res!=nullptr && sql.res->next()==true){
			str = "There are still row which got_it = false in tcp_location table.";
			error(sql,server,name_list[i],str);
		}
		sql.to_database("node");
		if(echo==true){
			//echo status
			it = fires.begin();
			it_end = fires.end();
			for(;it!=it_end;++it){
				std::cout << it->first << ": "<< std::endl;
				k = it->second.size();
				for(j=0;j<k;++j){
					std::cout << "Line: " << fires[it->first][j] << std::endl;
				}
			}
			std::cout << std::endl;
			for(j=0;j<unix_location.size();++j){
				std::cout << unix_name[j] << ":" << unix_location[j] << std::endl;
			}
		}
	}
	if(in!=nullptr){
		fclose(in);
		in = nullptr;
	}
	//std::cout << target << std::endl;
	return 0;
}
