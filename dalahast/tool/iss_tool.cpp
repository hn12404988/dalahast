inline void iss_tool::is_use_sync_size(da::ISS &iss){
	i = iss.size();
	is_tool::i = is_use.size();
	while(i>is_tool::i){
		is_use.push_back("");
		++is_tool::i;
	}
}

inline void iss_tool::iss_sync_size(da::ISS &iss){
	i = iss.size();
	is_tool::i = is_use.size();
	while(i!=is_tool::i){
		if(i>is_tool::i){
			iss[i-1]->clear();
			delete iss[i-1];
			iss.pop_back();
			--i;
		}
		else{
			iss.push_back(new da::SS);
			++i;
		}
	}
}

void iss_tool::import_fixed(da::IS *im_fixed){
	ss_tool::import_fixed(im_fixed);
}

void iss_tool::to_json(da::ISS &iss){
	is_use_sync_size(iss);
	it = iss.begin();
	it_end = iss.end();
	for(i=0;it!=it_end;++it,++i){
		ss_tool::to_json(**it);
		is_use[i] = ss_tool::outcome;
	}
	if(i==0){
		this->outcome = "{}";
	}
	else{
		--i;
		is_tool::to_json(is_use,0,i);
		this->outcome = is_tool::outcome;
	}
}

void iss_tool::to_json(da::ISS &iss, std::size_t start, std::size_t end){
	i = iss.size();
	if(i==0){
		this->outcome = "{}";
		return;
	}
	else{
		this->outcome.clear();
	}
	if(end>=i){
		end = i-1;
	}
	if(start>end){
		this->outcome = "{}";
		return;
	}
	i = is_use.size();
	while(i<(end-start+1)){
		is_use.push_back("");
		++i;
	}
	it = iss.begin()+start;
	it_end = iss.begin()+end+1;
	for(i=0;it!=it_end;++it,++i){
		ss_tool::to_json(**it);
		is_use[i] = ss_tool::outcome;
	}
	--i;
	is_tool::to_json(is_use,0,i);
	this->outcome = is_tool::outcome;
}

void iss_tool::to_array(da::ISS &iss){
	is_use_sync_size(iss);
	it = iss.begin();
	it_end = iss.end();
	for(i=0;it!=it_end;++it,++i){
		ss_tool::to_json(**it);
		is_use[i] = ss_tool::outcome;
	}
	if(i==0){
		this->outcome = "[{}]";
	}
	else{
		--i;
		is_tool::to_array(is_use,0,i);
		this->outcome = is_tool::outcome;
	}
}

void iss_tool::to_array(da::ISS &iss, std::size_t start, std::size_t end){
	this->outcome.clear();
	i = iss.size();
	if(i==0){
		this->outcome = "[{}]";
		return;
	}
	else{
		this->outcome.clear();
	}
	if(end>=i){
		end = i-1;
	}
	if(start>end){
		this->outcome = "[{}]";
		return;
	}
	i = is_use.size();
	while(i<(end-start+1)){
		is_use.push_back("");
		++i;
	}
	it = iss.begin()+start;
	it_end = iss.begin()+end+1;
	for(i=0;it!=it_end;++it,++i){
		ss_tool::to_json(**it);
		is_use[i] = ss_tool::outcome;
	}
	--i;
	is_tool::to_array(is_use,0,i);
	this->outcome = is_tool::outcome;
}

bool iss_tool::json_to(da::ISS &iss, std::string &json){
	if(is_tool::json_to(is_use,json)==false){
		return false;
	}
	iss_sync_size(iss);
	is_tool::it = is_use.begin();
	is_tool::it_end = is_use.end();
	for(i=0;is_tool::it!=is_tool::it_end;++is_tool::it,++i){
		if(ss_tool::json_to(*iss[i],*is_tool::it)==false){
			return false;
		}
	}
	return true;
}

std::size_t iss_tool::fixed_json_to(da::ISS &iss, std::string &json){
	is_use_sync_size(iss);
	i = is_tool::fixed_json_to(is_use,json);
	if(i==0){
		return 0;
	}
	is_tool::it = is_use.begin();
	is_tool::it_end = is_use.begin()+i;
	for(i=0;is_tool::it!=is_tool::it_end;++is_tool::it,++i){
		if(ss_tool::json_to(*iss[i],*is_tool::it)==false){
			return 0;
		}
	}
	return i;
}

bool iss_tool::array_to(da::ISS &iss, std::string &array){
	if(is_tool::array_to(is_use,array)==false){
		return false;
	}
	iss_sync_size(iss);
	is_tool::it = is_use.begin();
	is_tool::it_end = is_use.end();
	for(i=0;is_tool::it!=is_tool::it_end;++is_tool::it,++i){
		std::cout << "ar: " << *is_tool::it << std::endl;
		if(ss_tool::json_to(*iss[i],*is_tool::it)==false){
			return false;
		}
		std::cout << "ar: " << __LINE__ << std::endl;
	}
	return true;
}

std::size_t iss_tool::fixed_array_to(da::ISS &iss, std::string &array){
	is_use_sync_size(iss);
	i = is_tool::fixed_array_to(is_use,array);
	if(i==0){
		return 0;
	}
	is_tool::it = is_use.begin();
	is_tool::it_end = is_use.begin()+i;
	for(i=0;is_tool::it!=is_tool::it_end;++is_tool::it,++i){
		if(ss_tool::json_to(*iss[i],*is_tool::it)==false){
			return 0;
		}
	}
	return i;
}

