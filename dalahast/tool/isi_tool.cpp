template <class T>
inline void isi_tool<T>::is_use_sync_size(std::vector<T*> &isi){
	i = isi.size();
	is_tool::i = is_use.size();
	while(i>is_tool::i){
		is_use.push_back("");
		++is_tool::i;
	}
}

template <class T>
inline void isi_tool<T>::isi_sync_size(std::vector<T*> &isi){
	i = isi.size();
	is_tool::i = is_use.size();
	while(i!=is_tool::i){
		if(i>is_tool::i){
			isi[i-1]->clear();
			delete isi[i-1];
			isi.pop_back();
			--i;
		}
		else{
			isi.push_back(new T);
			++i;
		}
	}
}

template <class T>
void isi_tool<T>::import_fixed(da::IS *im_fixed){
	si_tool<T>::import_fixed(im_fixed);
}

template <class T>
void isi_tool<T>::to_json(std::vector<T*> &isi){
	is_use_sync_size(isi);
	it = isi.begin();
	it_end = isi.end();
	for(i=0;it!=it_end;++it,++i){
		si_tool<T>::to_json(**it);
		is_use[i] = si_tool<T>::outcome;
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

template <class T>
void isi_tool<T>::to_array(std::vector<T*> &isi){
	is_use_sync_size(isi);
	it = isi.begin();
	it_end = isi.end();
	for(i=0;it!=it_end;++it,++i){
		si_tool<T>::to_json(**it);
		is_use[i] = si_tool<T>::outcome;
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

template <class T>
void isi_tool<T>::to_json(std::vector<T*> &isi, std::size_t start, std::size_t end){
	i = isi.size();
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
	it = isi.begin()+start;
	it_end = isi.begin()+end+1;
	for(i=0;it!=it_end;++it,++i){
		si_tool<T>::to_json(**it);
		is_use[i] = si_tool<T>::outcome;
	}
	--i;
	is_tool::to_json(is_use,0,i);
	this->outcome = is_tool::outcome;
}

template <class T>
void isi_tool<T>::to_array(std::vector<T*> &isi, std::size_t start, std::size_t end){
	i = isi.size();
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
	it = isi.begin()+start;
	it_end = isi.begin()+end+1;
	for(i=0;it!=it_end;++it,++i){
		si_tool<T>::to_json(**it);
		is_use[i] = si_tool<T>::outcome;
	}
	--i;
	is_tool::to_array(is_use,0,i);
	this->outcome = is_tool::outcome;
}

template <class T>
bool isi_tool<T>::json_to(std::vector<T*> &isi, std::string &json){
	if(is_tool::json_to(is_use,json)==false){
		return false;
	}
	isi_sync_size(isi);
	is_tool::it = is_use.begin();
	is_tool::it_end = is_use.end();
	for(i=0;is_tool::it!=is_tool::it_end;++is_tool::it,++i){
		if(si_tool<T>::json_to(*isi[i],*is_tool::it)==false){
			return false;
		}
	}
	return true;
}

template <class T>
bool isi_tool<T>::array_to(std::vector<T*> &isi, std::string &array){
	if(is_tool::array_to(is_use,array)==false){
		return false;
	}
	isi_sync_size(isi);
	is_tool::it = is_use.begin();
	is_tool::it_end = is_use.end();
	for(i=0;is_tool::it!=is_tool::it_end;++is_tool::it,++i){
		if(si_tool<T>::json_to(*isi[i],*is_tool::it)==false){
			return false;
		}
	}
	return true;
}

template <class T>
std::size_t isi_tool<T>::fixed_json_to(std::vector<T*> &isi, std::string &json){
	is_use_sync_size(isi);
	i = is_tool::fixed_json_to(is_use,json);
	if(i==0){
		return 0;
	}
	is_tool::it = is_use.begin();
	is_tool::it_end = is_use.begin()+i;
	for(i=0;is_tool::it!=is_tool::it_end;++is_tool::it,++i){
		if(si_tool<T>::json_to(*isi[i],*is_tool::it)==false){
			return 0;
		}
	}
	return i;
}

template <class T>
std::size_t isi_tool<T>::fixed_array_to(std::vector<T*> &isi, std::string &array){
	is_use_sync_size(isi);
	i = is_tool::fixed_array_to(is_use,array);
	if(i==0){
		return 0;
	}
	is_tool::it = is_use.begin();
	is_tool::it_end = is_use.begin()+i;
	for(i=0;is_tool::it!=is_tool::it_end;++is_tool::it,++i){
		if(si_tool<T>::json_to(*isi[i],*is_tool::it)==false){
			return 0;
		}
	}
	return i;
}

