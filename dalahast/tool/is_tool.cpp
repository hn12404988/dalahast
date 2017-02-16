inline void is_tool::json_to_end(std::string &json){
	++i;
	while(json[i]!='\0'){
		if(json[i]=='}'){
			return;
		}
		else if(json[i]=='['){
			array_to_end(json);
		}
		else if(json[i]=='{'){
			json_to_end(json);
		}
		++i;
	}
}

inline void is_tool::array_to_end(std::string &array){
	++i;
	while(array[i]!='\0'){
		if(array[i]==']'){
			return;
		}
		else if(array[i]=='['){
			array_to_end(array);
		}
		else if(array[i]=='{'){
			json_to_end(array);
		}
		++i;
	}
}

void is_tool::error_log(da::IS &error,std::string &str,short int &count){
	++count;
	if(error.size()>count){
		error[count] = str;
	}
	else{
		error.push_back(str);
	}
}

std::size_t is_tool::stack_up(da::IS &is){
	it_end = is.end();
	it = is.begin();
	for(;it!=it_end;++it){
		if(*it==""){
			for(it_re=it+1;it_re<it_end;++it_re){
				if(*it_re!=""){
					*it = *it_re;
					it_re->clear();
					break;
				}
			}
			if(it_re==it_end){
				break;
			}
		}
	}
	it_end = is.end();
	it = is.begin();
	for(i=0;it!=it_end;++it,++i){
		if(*it==""){
			break;
		}
	}
	return i;
}


std::size_t is_tool::stack_up(da::IS &is,std::size_t start, std::size_t end){
	i = is.size();
	if(i==0){
		return 0;
	}
	if(end>=i){
		end = i-1;
	}
	if(start>end){
		return 0;
	}
	it_end = is.begin()+end+1;
	it = is.begin()+start;
	for(;it!=it_end;++it){
		if(*it==""){
			for(it_re=it+1;it_re<it_end;++it_re){
				if(*it_re!=""){
					*it = *it_re;
					it_re->clear();
					break;
				}
			}
			if(it_re==it_end){
				break;
			}
		}
	}
	it_end = is.begin()+end+1;
	it = is.begin()+start;
	for(i=0;it!=it_end;++it,++i){
		if(*it==""){
			break;
		}
	}
	return i;
}

void is_tool::delete_repeat(da::IS &is){
	it = is.begin();
	it_end = is.end();
	for(;it!=it_end;++it){
		it_re = it+1;
		for(;it_re!=it_end;++it_re){
			if(*it==*it_re){
				--it_end;
				if(it_re==it_end){
					is.pop_back();
					it_end = is.end();
					break;
				}
				else{
					is.erase(it_re);
					it_end = is.end();
					--it_re;
				}
			}
		}
	}
}

std::size_t is_tool::delete_repeat(da::IS &is, std::size_t start, std::size_t over){
	i = is.size();
	if(i==0){
		return 0;
	}
	if(over>=i){
		over = i-1;
	}
	if(start>over){
		return 0;
	}
	it = is.begin()+start;
	it_end = is.begin()+over+1;
	j = over-start+1;
	for(;it!=it_end;++it){
		it_re = it+1;
		for(;it_re!=it_end;++it_re){
			if(*it==*it_re){
				is.erase(it_re);
				--j;
				--it_end;
				--it_re;
			}
		}
	}
	return j;
}

void is_tool::set_array_separate(char sep){
	array_separate = sep;
}

void is_tool::to_json(da::IS &is){
	outcome  = "{";
	it = is.begin();
	it_end = is.end();
	for(i=0;it!=it_end;++it,++i){
		if((*it)[0]=='{'){
			outcome = outcome+"\""+std::to_string(i)+"\":"+*it+",";
		}
		else{
			outcome = outcome+"\""+std::to_string(i)+"\":\""+*it+"\",";
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

void is_tool::to_array(da::IS &is){
	outcome  = "[";
	it = is.begin();
	it_end = is.end();
	if(array_separate=='\0'){
		for(;it!=it_end;++it){
			outcome = outcome+*it+",";
		}
	}
	else{
		for(;it!=it_end;++it){
			outcome = outcome+array_separate+*it+array_separate+",";
		}
	}
	if(outcome!="["){
		outcome.pop_back();
		outcome.append("]");
	}
	else{
		outcome = "[]";
	}
}

void is_tool::to_json(da::IS &is, std::size_t start, std::size_t end){
	i = is.size();
	if(i==0){
		outcome  = "{}";
		return;
	}
	else{
		outcome  = "{";
	}
	if(end>=i){
		end = i - 1;
	}
	if(start>end){
		outcome  = "{}";
		return;
	}
	it = is.begin()+start;
	it_end = is.begin()+end+1;
	for(i=0;it!=it_end;++it,++i){
		if((*it)[0]=='{'){
			outcome = outcome+"\""+std::to_string(i)+"\":"+*it+",";
		}
		else{
			outcome = outcome+"\""+std::to_string(i)+"\":\""+*it+"\",";
		}
	}
	outcome.pop_back();
	outcome.append("}");
}

void is_tool::to_array(da::IS &is, std::size_t start, std::size_t end){
	i = is.size();
	if(i==0){
		outcome  = "[]";
		return;
	}
	else{
		outcome  = "[";
	}
	if(end>=i){
		end = i - 1;
	}
	if(start>end){
		outcome  = "[]";
		return;
	}
	it = is.begin()+start;
	it_end = is.begin()+end+1;
	if(array_separate=='\0'){
		for(;it!=it_end;++it){
			outcome = outcome+*it+",";
		}
	}
	else{
		for(;it!=it_end;++it){
			outcome = outcome+array_separate+*it+array_separate+",";
		}
	}
	outcome.pop_back();
	outcome.append("]");
}

bool is_tool::json_to(da::IS &is, std::string &json){
	if(json[0]!='{'){
		return false;
	}
	if(json=="{}"){
		is.clear();
		return false;
	}
	ceiling = is.size();
	count = 0;
	i = 1;
	while(json[i]!='\0'){
		if(json[i]!='"'){
			return false;
		}
		for(;json[i]!='\0';++i){
			if(json[i]==':'){
				break;
			}
		}
		++i;
		j = i;
		if(json[i]=='{'){
			json_to_end(json);
			if(json[i]!='}'){
				return false;
			}
			outcome = json.substr(j,i-j+1);
		}
		else if(json[i]=='['){
			array_to_end(json);
			if(json[i]!=']'){
				return false;
			}
			outcome = json.substr(j,i-j+1);
		}
		else if(json[i]=='"'){
			++j;
			++i;
			for(;json[i]!='\0';++i){
				if(json[i]=='['){
					array_to_end(json);
					if(json[i]!=']'){
						return false;
					}
					continue;
				}
				if(json[i]=='{'){
					json_to_end(json);
					if(json[i]!='}'){
						return false;
					}
					continue;
				}
				if(json[i]=='"' && json[i-1]!='\\'){
					break;
				}
			}
			outcome = json.substr(j,i-j);
			if(json[i+1]=='}'){
				if(count<ceiling){
					is[count] = outcome;
					++count;
				}
				else{
					is.push_back(outcome);
				}
				while(count<ceiling){
					is.pop_back();
					--ceiling;
				}
				return true;
			}
		}
		else{
			return false;
		}
		if(count<ceiling){
			is[count] = outcome;
			++count;
		}
		else{
			is.push_back(outcome);
		}
		i += 2;
	}
	while(count<ceiling){
		is.pop_back();
		--ceiling;
	}
	return true;
}

std::size_t is_tool::fixed_json_to(da::IS &is, std::string &json){
	if(json[0]!='{' || json=="{}"){
		return 0;
	}
	ceiling = is.size();
	count = 0;
	i = 1;
	while(json[i]!='\0'){
		if(json[i]!='"'){
			return 0;
		}
		for(;json[i]!='\0';++i){
			if(json[i]==':'){
				break;
			}
		}
		++i;
		j = i;
		if(json[i]=='{'){
			json_to_end(json);
			if(json[i]!='}'){
				return 0;
			}
			outcome = json.substr(j,i-j+1);
		}
		else if(json[i]=='['){
			array_to_end(json);
			if(json[i]!=']'){
				return 0;
			}
			outcome = json.substr(j,i-j+1);
		}
		else if(json[i]=='"'){
			++j;
			++i;
			for(;json[i]!='\0';++i){
				if(json[i]=='['){
					array_to_end(json);
					if(json[i]!=']'){
						return false;
					}
					continue;
				}
				if(json[i]=='{'){
					json_to_end(json);
					if(json[i]!='}'){
						return false;
					}
					continue;
				}
				if(json[i]=='"' && json[i-1]!='\\'){
					break;
				}
			}
			outcome = json.substr(j,i-j);
			if(json[i+1]=='}'){
				if(count<ceiling){
					is[count] = outcome;
					++count;
				}
				else{
					return 0;
				}
				return count;
			}
		}
		else{
			return 0;
		}
		if(count<ceiling){
			is[count] = outcome;
			++count;
		}
		else{
			return 0;
		}
		i += 2;
	}
	return count;
}

bool is_tool::array_to(da::IS &is, std::string &array){
	if(array[0]!='['){
		return false;
	}
	if(array=="[]"){
		is.clear();
		return true;
	}
	ceiling = is.size();
	count = 0;
	i = 1;
	j = 0;
	while(array[i]!='\0'){
		if(j==0){
			j = i;
		}
		if(array[i]=='{'){
			json_to_end(array);
			if(array[i]!='}'){
				return false;
			}
			if(array[i+1]!=','){
				++i;
				continue;
			}
		}
		else if(array[i]=='['){
			array_to_end(array);
			if(array[i]!=']'){
				return false;
			}
			if(array[i+1]!=','){
				++i;
				continue;
			}
		}
		else{
			while(array[i]!='\0'){
				if(array[i]==','){
					--i;
					break;
				}
				if(array[i]=='['){
					array_to_end(array);
					if(array[i]!=']'){
						return false;
					}
					++i;
					continue;
				}
				if(array[i]=='{'){
					json_to_end(array);
					if(array[i]!='}'){
						return false;
					}
					++i;
					continue;
				}
				if(array[i]==']' && array[i+1]=='\0'){
					--i;
					outcome = array.substr(j,i-j+1);
					if(count<ceiling){
						is[count] = outcome;
						++count;
					}
					else{
						is.push_back(outcome);
					}
					while(count<ceiling){
						is.pop_back();
						--ceiling;
					}
					return true;
				}
				++i;
			}
		}
		outcome = array.substr(j,i-j+1);
		i += 2;
		if(count<ceiling){
			is[count] = outcome;
			++count;
		}
		else{
			is.push_back(outcome);
		}
		j = 0;
	}
	while(count<ceiling){
		is.pop_back();
		--ceiling;
	}
	return true;
}

std::size_t is_tool::fixed_array_to(da::IS &is, std::string &array){
	if(array[0]!='[' || array=="[]"){
		return 0;
	}
	ceiling = is.size();
	count = 0;
	i = 1;
	j = 0;
	while(array[i]!='\0'){
		if(j==0){
			j = i;
		}
		if(array[i]=='{'){
			json_to_end(array);
			if(array[i]!='}'){
				return 0;
			}
			if(array[i+1]!=','){
				++i;
				continue;
			}
		}
		else if(array[i]=='['){
			array_to_end(array);
			if(array[i]!=']'){
				return 0;
			}
			if(array[i+1]!=','){
				++i;
				continue;
			}
		}
		else{
			while(array[i]!='\0'){
				if(array[i]==','){
					--i;
					break;
				}
				if(array[i]=='['){
					array_to_end(array);
					if(array[i]!=']'){
						return false;
					}
					++i;
					continue;
				}
				if(array[i]=='{'){
					json_to_end(array);
					if(array[i]!='}'){
						return false;
					}
					++i;
					continue;
				}
				if(array[i]==']' && array[i+1]=='\0'){
					--i;
					outcome = array.substr(j,i-j+1);
					if(count<ceiling){
						is[count] = outcome;
						++count;
					}
					else{
						return 0;
					}
					return count;
				}
				++i;
			}
		}
		outcome = array.substr(j,i-j+1);
		i += 2;
		if(count<ceiling){
			is[count] = outcome;
			++count;
		}
		else{
			return 0;
		}
		j = 0;
	}
	return count;
}

void is_tool::concat_array_string(std::string &main,std::string &sub){
	if(sub=="[]"){
		return;
	}
	if(main=="[]"){
		main = sub;
		return;
	}
	main.pop_back();
	main.append(",");
	main.append(sub.substr(1));
}
