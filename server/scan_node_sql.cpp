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
	if(str.find("join_")!=std::string::npos){
		return;
	}
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

std::string find_table(review::IS &query_component,std::string target){
	bool keep {false};
	bool got_it {false};
	int j,k,count;
	std::string sql_table,str;
	count = 0;
	for(j=0;j<query_component.size();++j){
		if(got_it==true){
			break;
		}
		if(keep==false){
			if(query_component[j].find(target)!=std::string::npos && query_component[j].front()=='"'){
				keep = true;
				k = query_component[j].find(target);
				k += target.length();
				if(query_component[j][k]==' '){
					++k;
				}
				else{
					continue;
				}
				while(query_component[j][k]!='"' && query_component[j][k]!='\0'){
					if(query_component[j][k]==' '){
						got_it = true;
						break;
					}
					if(query_component[j][k]!='`'){
						sql_table += query_component[j][k];
					}
					++k;
				}
			}
		}
		else{
			if(query_component[j][0]=='"'){
				k = 1;
				while(query_component[j][k]!='"' && query_component[j][k]!='\0'){
					if(query_component[j][k]==' '){
						got_it = true;
						break;
					}
					if(query_component[j][k]!='`'){
						sql_table += query_component[j][k];
					}
					++k;
				}
			}
			else{
				++count;
				str = "["+std::to_string(count)+"]";
				sql_table.append(str);
			}
		}
	}
	if(keep==false && got_it==false){
		return "ERROR:none";
	}
	else{
		if(count==0){
			return sql_table;
		}
		else if(count==1){
			j = sql_table.find("[1]");
			if(j==std::string::npos){
				return "ERROR:fail";
			}
			str = sql_table.substr(j+3);
			sql_table = sql_table.substr(0,j);
			sql_table = sql_table+"[YDATE]"+str;
			if(sql_table.find("[YDATE]")==1){
				sql_table = "[TYPE]"+sql_table.substr(1);
			}
			return sql_table;
		}
		else if(count==2){
			j = sql_table.find("[1]");
			if(j==std::string::npos){
				return "ERROR:fail";
			}
			str = sql_table.substr(j+3);
			sql_table = sql_table.substr(0,j);
			sql_table = sql_table+"[TYPE]"+str;
			j = sql_table.find("[2]");
			if(j==std::string::npos){
				return "ERROR:fail";
			}
			str = sql_table.substr(j+3);
			sql_table = sql_table.substr(0,j);
			sql_table = sql_table+"[YDATE]"+str;
			return sql_table;
		}
		else{
			str = "ERROR:"+std::to_string(count);
			return str;
		}
	}
}

void query_decode(review::IS &is, std::string &str){
	str.pop_back();
	is.clear();
	std::size_t i {0};
	std::string part;
	while(str[i]!='\0'){
		part.clear();
		if(str[i]=='"'){
			part = "\"";
			++i;
			while(str[i]!='"'){
				part += str[i];
				++i;
			}
			part.append("\"");
			i += 2;
		}
		else{
			while(str[i]!='+' && str[i]!='\0'){
				part += str[i];
				++i;
			}
			if(str[i]=='\0'){
				is.push_back(part);
				break;
			}
			i += 1;
		}
		is.push_back(part);
	}
	/*
	  std::cout << std::endl;
	  std::cout << "Query Decode" << std::endl;
	  for(i=0;i<is.size();++i){
	  std::cout << is[i] << std::endl;
	  }
	  std::cout << "Query Decode Finish" << std::endl;
	  std::cout << std::endl;
	*/
}

int main(int argc ,char* argv[]){
	if(argc<2){
		return 0;
	}
	maria_wrapper sql("server",__FILE__);
	sql.build_statement();
	local_ip ip;
	std::string  root,server,str,send,str2;
	is_tool _is;
	review::IS node_list;
	review::IS name_list;
	review::IS init_block;
	review::IS query_component;
	review::IS unix_location;
	review::IS unix_name;
	int i,j,k,l,m;
	FILE *in {nullptr};
	const int buff_size {500};
	char buff[buff_size];
	std::string compiler {"clang"};
	std::string sql_server,sql_database,sql_table;
	std::map<std::string,review::IS> database_list;
	std::map<std::string,review::IS> table_list;
	std::map<std::string,review::IS> sql_type_list;
	std::map<std::string,review::IS> fires;
	std::map<std::string,review::IS>::iterator it;
	std::map<std::string,review::IS>::iterator it_end;
	bool compile {false};
	bool is_init {false};
	bool got_it;
	bool echo {false};
	
	std::map<std::string,bool> info;
	info["unix_server"] = false;
	info["tcp_server"] = false;
	info["tcp_client"] = false;
	info["unix_client"] = false;
	
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
			str = "Can not find input in sql";
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
		clear_SIS(database_list);
		clear_SIS(sql_type_list);
		clear_SIS(table_list);
		init_block.clear();
		unix_location.clear();
		unix_name.clear();
		sql_server = server;
		while(fgets(buff, buff_size, in)!=nullptr){
			str.append(buff);
			//std::cout << str << std::endl;
			if(strstr(buff,"\n")!=nullptr){
				if(str.find("void init()")!=std::string::npos){
					is_init = true;
					str.clear();
					continue;
				}
				if(str.find("int main")!=std::string::npos){
					is_init = false;
					str.clear();
					continue;
				}
				if(is_init==true){
					init_block.push_back(str);
					str.clear();
					continue;
				}
				if(str.find("maria_wrapper ")!=std::string::npos || str.find("maria ")!=std::string::npos){
					j = str.find("(");
					if(str[j+1]!='"'){
						++j;
						sql_server.clear();
						while(str[j]!=','){
							sql_server += str[j];
							++j;
						}
					}
					j = str.find("\"");
					++j;
					sql_database = "";
					while(str[j]!='"'){
						sql_database += str[j];
						++j;
					}
					str.clear();
					continue;
					//std::cout << "sql_database: " << sql_database << std::endl;
				}
				if(str.find("to_server(")!=std::string::npos){
					j = str.find("(");
					++j;
					sql_server = "";
					while(str[j]!=','){
						sql_server += str[j];
						++j;
					}
					//std::cout << "sql_server: " << sql_server << std::endl;
					j = str.find("\"");
					++j;
					sql_database = "";
					while(str[j]!='"'){
						sql_database += str[j];
						++j;
					}
					str.clear();
					continue;
					//std::cout << "sql_database: " << sql_database << std::endl;
				}
				/**
				 * 
				 **/
				if(str.find("to_database(")!=std::string::npos){
					j = str.find("\"");
					++j;
					sql_database = "";
					while(str[j]!='"'){
						sql_database += str[j];
						++j;
					}
					str.clear();
					continue;
					//std::cout << "sql_database: " << sql_database << std::endl;
				}
				if(str.find("\"insert into ")!=std::string::npos){
					sql_table.clear();
					j = str.find("\"insert into ");
					str = str.substr(j);
					str2 = str;
					query_decode(query_component,str);
					sql_table = find_table(query_component,"insert into");
					if(sql_table=="ERROR:fail"){
						str = "Can not find query component:"+str2;
						error(sql,server,name_list[i],str);
					}
					else if(sql_table=="ERROR:none" || sql_table==""){
						str = "got_it = false in:"+str2;
						error(sql,server,name_list[i],str);
					}
					else if(sql_table.substr(0,5)=="ERROR"){
						str = "More than 2 query component find:"+str2;
						error(sql,server,name_list[i],str);
					}
					else{
						sql_type_list[sql_server].push_back("INSERT");
						j = sql_table.find(".");
						if(j==std::string::npos){
							table_list[sql_server].push_back(sql_table);
							database_list[sql_server].push_back(sql_database);
						}
						else{
							table_list[sql_server].push_back(sql_table.substr(j+1));
							database_list[sql_server].push_back(sql_table.substr(0,j));
						}
					}
					if(str2.find(" values ")==std::string::npos){
						j = str2.find(" select ");
						if(j==std::string::npos){
							str = "Can not find select part in a insert from select query";
							error(sql,server,name_list[i],str);
						}
						else{
							str = "\""+str2.substr(j+1);
							str = str.substr(0,str.length()-1);
						}
					}
				}
				if(str.find("\"select ")!=std::string::npos){
					if(str.find("from")==std::string::npos){
						str.clear();
						continue;
					}
					j = str.find("\"select ");
					str = str.substr(j);
					str2 = str;
					query_decode(query_component,str);
					sql_table = find_table(query_component,"from");
					if(sql_table=="ERROR:fail"){
						str = "Can not find query component:"+str2;
						error(sql,server,name_list[i],str);
					}
					else if(sql_table=="ERROR:none" || sql_table==""){
						str = "got_it = false in:"+str2;
						error(sql,server,name_list[i],str);
					}
					else if(sql_table.substr(0,5)=="ERROR"){
						str = "More than 2 query component find:"+str2;
						error(sql,server,name_list[i],str);
					}
					else{
						sql_type_list[sql_server].push_back("SELECT");
						j = sql_table.find(".");
						if(j==std::string::npos){
							table_list[sql_server].push_back(sql_table);
							database_list[sql_server].push_back(sql_database);
						}
						else{
							table_list[sql_server].push_back(sql_table.substr(j+1));
							database_list[sql_server].push_back(sql_table.substr(0,j));
						}
					}
				}
				if(str.find("\"update ")!=std::string::npos){
					sql_table.clear();
					j = str.find("\"update ");
					str = str.substr(j);
					str2 = str;
					query_decode(query_component,str);
					sql_table = find_table(query_component,"update");
					if(sql_table=="ERROR:fail"){
						str = "Can not find query component:"+str2;
						error(sql,server,name_list[i],str);
					}
					else if(sql_table=="ERROR:none" || sql_table==""){
						str = "got_it = false in:"+str2;
						error(sql,server,name_list[i],str);
					}
					else if(sql_table.substr(0,5)=="ERROR"){
						str = "More than 2 query component find:"+str2;
						error(sql,server,name_list[i],str);
					}
					else{
						sql_type_list[sql_server].push_back("UPDATE");
						j = sql_table.find(".");
						if(j==std::string::npos){
							table_list[sql_server].push_back(sql_table);
							database_list[sql_server].push_back(sql_database);
						}
						else{
							table_list[sql_server].push_back(sql_table.substr(j+1));
							database_list[sql_server].push_back(sql_table.substr(0,j));
						}
					}
				}
				if(str.find("\"delete from ")!=std::string::npos){
					sql_table.clear();
					j = str.find("\"delete from ");
					str = str.substr(j);
					str2 = str;
					query_decode(query_component,str);
					sql_table = find_table(query_component,"from");
					if(sql_table=="ERROR:fail"){
						str = "Can not find query component:"+str2;
						error(sql,server,name_list[i],str);
					}
					else if(sql_table=="ERROR:none" || sql_table==""){
						str = "got_it = false in:"+str2;
						error(sql,server,name_list[i],str);
					}
					else if(sql_table.substr(0,5)=="ERROR"){
						str = "More than 2 query component find:"+str2;
						error(sql,server,name_list[i],str);
					}
					else{
						sql_type_list[sql_server].push_back("DELETE");
						j = sql_table.find(".");
						if(j==std::string::npos){
							table_list[sql_server].push_back(sql_table);
							database_list[sql_server].push_back(sql_database);
						}
						else{
							table_list[sql_server].push_back(sql_table.substr(j+1));
							database_list[sql_server].push_back(sql_table.substr(0,j));
						}
					}
				}
				/**
				 * 
				 **/
				if(str.find("fire(")!=std::string::npos){
					find_fire(fires,str,"fire");
				}
				if(str.find("fireNstore(")!=std::string::npos){
					find_fire(fires,str,"fireNstore");
				}
				if(str.find("fireNforget(")!=std::string::npos){
					find_fire(fires,str,"fireNforget");
				}
				if(str.find("fireNclose(")!=std::string::npos){
					find_fire(fires,str,"fireNclose");
				}
				if(str.find("fireNfreeze(")!=std::string::npos){
					find_fire(fires,str,"fireNfreeze");
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
		/**
		 * Insert data to `sql_access` table.
		 **/
		sql.send_and_error("delete from sql_access where server = "+server+" and node = '"+name_list[i]+"'");
		it = database_list.begin();
		it_end = database_list.end();
		for(;it!=it_end;++it){
			sql_server = it->first;
			k = it->second.size();
			for(j=0;j<k;++j){
				str = sql_type_list[it->first][j];
				sql_table = table_list[it->first][j];
				sql_database = it->second[j];
				sql.send_and_error("select server from sql_access where server = "+server+" and node = '"+name_list[i]+"' and sql_server = "+sql_server+" and _database = '"+sql_database+"' and _table = '"+sql_table+"' and execution = '"+str+"' limit 1");
				if(sql.res!=nullptr){
					if(sql.res->next()==false){
						sql.send_and_error("insert into sql_access (server,node,sql_server,_database,_table,execution) values ("+server+",'"+name_list[i]+"',"+sql_server+",'"+sql_database+"','"+sql_table+"','"+str+"')");
					}
				}
				else{
					str = "Select fail in sql_access table.";
					error(sql,server,name_list[i],str);
				}
			}
		}
		sql.send_and_error("delete from topology where from_server = "+server+" and from_node = '"+name_list[i]+"'");
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
						if(sql.something_wrong==true || sql.res->next()==false){
							sql.to_database("node");
							str = "Wrong location_index: "+str;
							error(sql,server,name_list[i],str);
							break;
						}
						send = sql.res->getString(1);
						sql.send_and_error("update unix_location set got_it = true where server = "+server+" and from_node = '"+name_list[i]+"' and location_index = "+str+" limit 1");
						if(sql.something_wrong==true || sql.update_count()==0){
							sql.to_database("node");
							str = "Fail on update tmp.unix_location.";
							error(sql,server,name_list[i],str);
							break;
						}
						sql.to_database("node");
						sql.send_and_error("insert into topology (from_server,from_node,to_server,to_node,type) values ("+server+",'"+name_list[i]+"',"+server+",'"+send+"','"+it->first+"')");
						if(sql.something_wrong==true){
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
		sql.to_database("node");
		if(echo==true){
			//echo status
			it = database_list.begin();
			it_end = database_list.end();
			for(;it!=it_end;++it){
				std::cout << "SQL Server: " << it->first << std::endl;
				sql_server = it->first;
				k = it->second.size();
				for(j=0;j<k;++j){
					std::cout << "SQL Table: " << table_list[it->first][j] << std::endl;
					std::cout << "SQL Database: " << it->second[j] << std::endl;
					std::cout << "SQL Type: " << sql_type_list[it->first][j] << std::endl;
					std::cout << std::endl;
				}
			}
			std::cout << std::endl;
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
