template <class T>
inline void iii_tool<T>::is_use_sync_size(std::vector<T*> &iii){
	i = iii.size();
	is_tool::i = is_use.size();
	while(i>is_tool::i){
		is_use.push_back("");
		++is_tool::i;
	}
}

template <class T>
inline void iii_tool<T>::iii_sync_size(std::vector<T*> &iii){
	i = iii.size();
	is_tool::i = is_use.size();
	while(i!=is_tool::i){
		if(i>is_tool::i){
			iii[i-1]->clear();
			delete iii[i-1];
			iii.pop_back();
			--i;
		}
		else{
			iii.push_back(new T);
			++i;
		}
	}
}

template <class T>
void iii_tool<T>::to_array(std::vector<T*> &iii){
	if(iii.size()==0){
		this->outcome = "[[]]";
		return;
	}
	is_use_sync_size(iii);
	it = iii.begin();
	it_end = iii.end();
	for(i=0;it!=it_end;++it,++i){
		ii_tool<T>::to_array(**it);
		is_use[i] = ii_tool<T>::outcome;
	}
	--i;
	is_tool::to_array(is_use,0,i);
	this->outcome = is_tool::outcome;
}

template <class T>
void iii_tool<T>::to_array(std::vector<T*> &iii, std::size_t start, std::size_t end){
	i = iii.size();
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
	it = iii.begin()+start;
	it_end = iii.begin()+end+1;
	for(i=0;it!=it_end;++it,++i){
		ii_tool<T>::to_array(**it);
		is_use[i] = ii_tool<T>::outcome;
	}
	--i;
	is_tool::to_array(is_use,0,i);
	this->outcome = is_tool::outcome;
}

template <class T>
bool iii_tool<T>::array_to(std::vector<T*> &iii, std::string &array){
	if(is_tool::array_to(is_use,array)==false){
		return false;
	}
	iii_sync_size(iii);
	is_tool::it = is_use.begin();
	is_tool::it_end = is_use.end();
	for(i=0;is_tool::it!=is_tool::it_end;++is_tool::it,++i){
		if(ii_tool<T>::array_to(*iii[i],*is_tool::it)==false){
			return false;
		}
	}
	return true;
}

template <class T>
std::size_t iii_tool<T>::fixed_array_to(std::vector<T*> &iii, std::string &array){
	is_use_sync_size(iii);
	i = is_tool::fixed_array_to(is_use,array);
	if(i==0){
		return 0;
	}
	is_tool::it = is_use.begin();
	is_tool::it_end = is_use.begin()+i;
	for(i=0;is_tool::it!=is_tool::it_end;++is_tool::it,++i){
		if(ii_tool<T>::array_to(*iii[i],*is_tool::it)==false){
			return 0;
		}
	}
	return i;
}

