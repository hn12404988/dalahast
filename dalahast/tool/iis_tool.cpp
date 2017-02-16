inline void iis_tool::is_use_sync_size(da::IIS &iis){
	i = iis.size();
	is_tool::i = is_use.size();
	while(i>is_tool::i){
		is_use.push_back("");
		++is_tool::i;
	}
}

inline void iis_tool::iis_sync_size(da::IIS &iis){
	i = iis.size();
	is_tool::i = is_use.size();
	while(i!=is_tool::i){
		if(i>is_tool::i){
			iis[i-1]->clear();
			delete iis[i-1];
			iis.pop_back();
			--i;
		}
		else{
			iis.push_back(new da::IS);
			++i;
		}
	}
}

void iis_tool::to_array(da::IIS &iis){
	if(iis.size()==0){
		this->outcome = "[[]]";
		return;
	}
	is_use_sync_size(iis);
	it = iis.begin();
	it_end = iis.end();
	for(i=0;it!=it_end;++it,++i){
		is_tool::to_array(**it);
		is_use[i] = is_tool::outcome;
	}
	tmp_array_separate = array_separate;
	set_array_separate('\0');
	--i;
	is_tool::to_array(is_use,0,i);
	set_array_separate(tmp_array_separate);
	this->outcome = is_tool::outcome;
}

void iis_tool::to_array(da::IIS &iis, std::size_t start, std::size_t end){
	i = iis.size();
	if(i==0){
		this->outcome = "[[]]";
		return;
	}
	else{
		this->outcome.clear();
	}
	if(end>=i){
		end = i-1;
	}
	if(start>end){
		this->outcome = "[[]]";
		return;
	}
	i = is_use.size();
	while(i<(end-start+1)){
		is_use.push_back("");
		++i;
	}
	it = iis.begin()+start;
	it_end = iis.begin()+end+1;
	for(i=0;it!=it_end;++it,++i){
		is_tool::to_array(**it);
		is_use[i] = is_tool::outcome;
	}
	--i;
	tmp_array_separate = array_separate;
	set_array_separate('\0');
	is_tool::to_array(is_use,0,i);
	set_array_separate(tmp_array_separate);
	this->outcome = is_tool::outcome;
}

bool iis_tool::array_to(da::IIS &iis, std::string &array){
	if(is_tool::array_to(is_use,array)==false){
		return false;
	}
	iis_sync_size(iis);
	is_tool::it = is_use.begin();
	is_tool::it_end = is_use.end();
	for(i=0;is_tool::it!=is_tool::it_end;++is_tool::it,++i){
		if(is_tool::array_to(*iis[i],*is_tool::it)==false){
			return false;
		}
	}
	return true;
}

std::size_t iis_tool::fixed_array_to(da::IIS &iis, std::string &array){
	is_use_sync_size(iis);
	i = is_tool::fixed_array_to(is_use,array);
	if(i==0){
		return 0;
	}
	is_tool::it = is_use.begin();
	is_tool::it_end = is_use.begin()+i;
	for(i=0;is_tool::it!=is_tool::it_end;++is_tool::it,++i){
		if(is_tool::array_to(*iis[i],*is_tool::it)==false){
			return 0;
		}
	}
	return i;
}

