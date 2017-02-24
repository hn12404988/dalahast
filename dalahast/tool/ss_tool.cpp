inline void ss_tool::json_to_end(std::string &json){
	//int tmp,k;
	//tmp = i;
	//k = i;
	++i;
	while(json[i]!='\0'){
		if(json[i]=='}'){
			//std::cout << "JSON" << std::endl;
			//std::cout << json.substr(tmp,i-tmp+1) << std::endl;
			return;
		}
		else if(json[i]=='['){
			array_to_end(json);
			//k = i;
		}
		else if(json[i]=='{'){
			json_to_end(json);
			//k = i;
		}
		++i;
	}
	/*
	std::cout << "tmp: " << tmp << std::endl;
	std::cout << "k: " << k << std::endl;
	std::cout << "*********JSON************" << std::endl;
	std::cout << json.substr(tmp,k-tmp+1) << std::endl;
	std::cout << "*********JSON************" << std::endl;
	*/
}

inline void ss_tool::array_to_end(std::string &array){
	//int tmp,k;
	//tmp = i;
	//k = i;
	++i;
	while(array[i]!='\0'){
		if(array[i]==']'){
			//std::cout << "Array" << std::endl;
			//std::cout << array.substr(tmp,i-tmp+1) << std::endl;
			return;
		}
		else if(array[i]=='['){
			array_to_end(array);
			//k = i;
		}
		else if(array[i]=='{'){
			json_to_end(array);
			//k = i;
		}
		++i;
	}
	/*
	std::cout << "tmp: " << tmp << std::endl;
	std::cout << "k: " << k << std::endl;
	std::cout << "*********Array************" << std::endl;
	std::cout << array.substr(tmp,k-tmp+1) << std::endl;
	std::cout << "*********Array************" << std::endl;
	*/
}

inline void ss_tool::escape(std::string &line){
	j = line.length();
	if(line[0]=='"'){
		line = '\\'+line;
		i = 2;
		++j;
	}
	else{
		i = 1;
	}
	for(;i<j;++i){
		if(line[i]=='"' && line[i-1]!='\\'){
			line.insert(i,"\\");
			++i;
			++j;
		}
	}
}

void ss_tool::import_fixed(da::IS *im_fixed){
	fixed = im_fixed;
	if(fixed==nullptr){
		ceiling = 0;
		fixed_bool.clear();
	}
	else{
		ceiling = fixed->size();
		i = fixed_bool.size();
		for(;i<ceiling;++i){
			fixed_bool.push_back(false);
		}
	}
}

void ss_tool::to_json(da::SS &ss){
	outcome  = "{";
	it = ss.begin();
	it_end = ss.end();
	if(escape_quote==true){
		for(;it!=it_end;++it){
			escape(it->second);
		}
		it = ss.begin();
	}
	if(all_quote==true){
		for(;it!=it_end;++it){
			outcome = outcome+"\""+it->first+"\":\""+it->second+"\",";
		}
	}
	else{
		for(;it!=it_end;++it){
			if(it->second[0]=='{' || it->second[0]=='['){
				outcome = outcome+"\""+it->first+"\":"+it->second+",";
			}
			else{
				outcome = outcome+"\""+it->first+"\":\""+it->second+"\",";
			}
		}
	}
	if(outcome!="{"){
		outcome.pop_back();
		outcome.append("}");
	}
	else{
		outcome = "{}";
	}
}

bool ss_tool::json_to(da::SS &ss,std::string &json){
	if(fixed==nullptr){
		ss.clear();
		if(json=="" || json=="{}"){
			return true;
		}
		std::cout << "ss: " << __LINE__ << std::endl;
	}
	else{
		if(json=="" || json=="{}"){
			return false;
		}
		for(i=0;i<ceiling;++i){
			fixed_bool[i] = false;
		}
	}
	i = 1;
	while(json[i]!='\0'){
		std::cout << "ss: " << __LINE__ << std::endl;
		if(json[i]!='"'){
			return false;
		}
		j = i+1;
		for(;json[i]!='\0';++i){
			if(json[i]==':'){
				break;
			}
		}
		str = json.substr(j,i-j-1);
		++i;
		j = i;
		std::cout << "ss: " << __LINE__ << std::endl;
		if(json[i]=='{'){
			std::cout << "ss: " << __LINE__ << std::endl;
			json_to_end(json);
			std::cout << "ss: " << __LINE__ << std::endl;
			if(json[i]!='}'){
				return false;
			}
			outcome = json.substr(j,i-j+1);
		}
		else if(json[i]=='['){
			std::cout << "ss: " << __LINE__ << std::endl;
			array_to_end(json);
			std::cout << "ss: " << __LINE__ << std::endl;
			if(json[i]!=']'){
				return false;
			}
			outcome = json.substr(j,i-j+1);
		}
		else if(json[i]=='"'){
			std::cout << "ss: " << __LINE__ << std::endl;
			++j;
			++i;
			for(;json[i]!='\0';++i){
				if(json[i]=='['){
					std::cout << "ss: " << __LINE__ << std::endl;
					array_to_end(json);
					std::cout << "ss: " << __LINE__ << std::endl;
					if(json[i]!=']'){
						return false;
					}
					continue;
				}
				if(json[i]=='{'){
					std::cout << "ss: " << __LINE__ << std::endl;
					json_to_end(json);
					std::cout << "ss: " << __LINE__ << std::endl;
					if(json[i]!='}'){
						return false;
					}
					continue;
				}
				if(json[i]=='"' && json[i-1]!='\\'){
					std::cout << "ss: " << __LINE__ << std::endl;
					break;
				}
				std::cout << "ss: " << __LINE__ << std::endl;
			}
			std::cout << "ss: " << __LINE__ << std::endl;
			outcome = json.substr(j,i-j);
			std::cout << "ss: " << __LINE__ << std::endl;
			if(json[i+1]=='}'){
				std::cout << "ss: " << __LINE__ << std::endl;
				if(fixed!=nullptr){
					for(j=0;j<ceiling;++j){
						if((*fixed)[j]==str){
							if(fixed_bool[j]==true){
								return false;
							}
							else{
								fixed_bool[j] = true;
							}
							break;
						}
					}
					if(j==ceiling){
						return false;
					}
				}
				std::cout << "ss: " << __LINE__ << std::endl;
				ss[str] = outcome;
				return true;
			}
			std::cout << "ss: " << __LINE__ << std::endl;
		}
		else{
			return false;
		}
		std::cout << "ss: " << __LINE__ << std::endl;
		if(fixed!=nullptr){
			for(j=0;j<ceiling;++j){
				if((*fixed)[j]==str){
					if(fixed_bool[j]==true){
						return false;
					}
					else{
						fixed_bool[j] = true;
					}
					break;
				}
			}
			if(j==ceiling){
				return false;
			}
		}
		std::cout << "ss: " << str << std::endl;
		std::cout << "ss: " << outcome << std::endl;
		ss[str] = outcome;
		std::cout << "ss: " << __LINE__ << std::endl;
		i += 2;
		std::cout << "ss: " << __LINE__ << std::endl;
	}
	if(fixed!=nullptr){
		for(i=0;i<ceiling;++i){
			if(fixed_bool[i]==false){
				break;
			}
		}
		if(i<ceiling){
			return false;
		}
	}
	std::cout << "ss: " << __LINE__ << std::endl;
	return true;
}

bool ss_tool::get_to(da::SS &ss,std::string &get){
	if(get==""){
		return false;
	}
	if(fixed==nullptr){
		ss.clear();
	}
	else{
		for(i=0;i<ceiling;++i){
			fixed_bool[i] = false;
		}
	}
	i = 0;
	outcome.clear();
	str.clear();
	while(get[i]!='\0'){
		if(get[i]==':'){
			++i;
			if(fixed!=nullptr){
				for(j=0;j<ceiling;++j){
					if((*fixed)[j]==str){
						if(fixed_bool[j]==true){
							return false;
						}
						else{
							fixed_bool[j] = true;
						}
						break;
					}
				}
				if(j==ceiling){
					return false;
				}
			}
			while(get[i]!='\0'){
				if(get[i]=='&'){
					ss[str] = outcome;
					outcome.clear();
					str.clear();
					++i;
					break;
				}
				else{
					outcome += get[i];
					++i;
				}
			}
		}
		else{
			str += get[i];
			++i;
		}
	}
	ss[str] = outcome;
	if(fixed!=nullptr){
		for(i=0;i<ceiling;++i){
			if(fixed_bool[i]==false){
				break;
			}
		}
		if(i<ceiling){
			return false;
		}
		else{
			return true;
		}
	}
	return true;
}
