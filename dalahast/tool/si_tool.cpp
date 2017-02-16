template <class T>
void si_tool<T>::import_fixed(da::IS *im_fixed){
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

template <class T>
void si_tool<T>::to_json(T &target){
	outcome  = "{";
	it = target.begin();
	it_end = target.end();
	if(all_quote==true){
		for (;it!=it_end;++it){
			outcome = outcome+"\""+it->first+"\":\""+std::to_string(it->second)+"\",";
		}
	}
	else{
		for (;it!=it_end;++it){
			outcome = outcome+"\""+it->first+"\":"+std::to_string(it->second)+",";
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

template <class T>
bool si_tool<T>::json_to(T &target,std::string &json){
	if(fixed==nullptr){
		target.clear();
		if(json=="" || json=="{}"){
			return true;
		}
	}
	else{
		if(json=="" || json=="{}"){
			return false;
		}
		for(i=0;i<ceiling;++i){
			fixed_bool[i] = false;
		}
	}
	json.pop_back();
	//json = json.substr(1);
	i = 1;
	while(json[i]!='\0'){
		if(json[i]=='"'){
			++i;
			outcome.clear();
			while(json[i]!='"' && json[i]!='\0'){
				outcome += json[i];
				++i;
			}
			if(fixed!=nullptr){
				for(j=0;j<ceiling;++j){
					if((*fixed)[j]==outcome){
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
			i += 2;
			str.clear();
			if(json[i]=='"'){
				++i;
				while(json[i]!='\0'){
					if((json[i]=='"' && json[i+1]=='\0') || (json[i]=='"' && json[i+1]==',' && json[i+2]=='"')){
						break;
					}
					str += json[i];
					++i;
				}
				target[outcome] = std::stoi(str);
				++i;
				if(json[i]=='\0'){
					break;
				}
				else{
					++i;
				}
			}
			else{
				return false;
			}
		}
		else{
			return false;
		}
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
		else{
			return true;
		}
	}
	return true;
}

template <class T>
bool si_tool<T>::get_to(T &target,std::string &get){
	if(fixed==nullptr){
		target.clear();
		if(get=="" || get=="{}"){
			return true;
		}
	}
	else{
		if(get=="" || get=="{}"){
			return false;
		}
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
			while(get[i]!='\0'){
				if(get[i]=='&'){
					target[str] = std::stoi(outcome);
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
			++i;
		}
	}
	target[str] = std::stoi(outcome);
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
