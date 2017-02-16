template<class T>
void ii_tool<T>::delete_repeat(T &ii){
	it = ii.begin();
	it_end = ii.end();
	for(;it!=it_end;++it){
		it_re = it+1;
		for(;it_re!=it_end;++it_re){
			if(*it==*it_re){
				--it_end;
				if(it_re==it_end){
					ii.pop_back();
					it_end = ii.end();
					break;
				}
				else{
					ii.erase(it_re);
					it_end = ii.end();
					--it_re;
				}
			}
		}
	}
}

template<class T>
std::size_t ii_tool<T>::delete_repeat(T &ii, std::size_t start, std::size_t over){
	i = ii.size();
	if(i==0){
		return 0;
	}
	if(over>=i){
		over = i-1;
	}
	if(start>over){
		return 0;
	}
	it = ii.begin()+start;
	it_end = ii.begin()+over+1;
	j = over-start+1;
	for(;it!=it_end;++it){
		it_re = it+1;
		for(;it_re!=it_end;++it_re){
			if(*it==*it_re){
				ii.erase(it_re);
				--j;
				--it_end;
				--it_re;
			}
		}
	}
	return j;
}

template<class T>
void ii_tool<T>::to_array(T &ii){
	outcome  = "[";
	it = ii.begin();
	it_end = ii.end();
	for(;it!=it_end;++it){
		outcome = outcome+std::to_string(*it)+",";
	}
	if(outcome!="["){
		outcome.pop_back();
		outcome.append("]");
	}
	else{
		outcome = "[]";
	}
}

template<class T>
void ii_tool<T>::to_array(T &ii, std::size_t start, std::size_t end){
	i = ii.size();
	if(i==0){
		outcome  = "[]";
		return;
	}
	else{
		outcome  = "[";
	}
	if(end>=i){
		end = i-1;
	}
	if(start>end){
		outcome  = "[]";
		return;
	}
	it = ii.begin()+start;
	it_end = ii.begin()+end+1;
	for(;it!=it_end;++it){
		outcome = outcome+std::to_string(*it)+",";
	}
	outcome.pop_back();
	outcome.append("]");
}

template<class T>
bool ii_tool<T>::array_to(T &ii, std::string &array){
	if(array[0]!='['){
		return false;
	}
	if(array=="[]"){
		ii.clear();
		return true;
	}
	ceiling = ii.size();
	j = 0;
	k = array.length();
	str.clear();
	for(i=1;i<k;++i){
		if(array[i]==','){
			if(j<ceiling){
				ii[j] = std::stoi(str);
				++j;
			}
			else{
				ii.push_back(std::stoi(str));
			}
			str.clear();
		}
		else if(array[i]==']'){
			if(j<ceiling){
				ii[j] = std::stoi(str);
				++j;
			}
			else{
				ii.push_back(std::stoi(str));
			}
			str.clear();
		}
		else{
			str += array[i];
		}
	}
	while(j<ceiling){
		ii.pop_back();
		--ceiling;
	}
	return true;
}

template<class T>
std::size_t ii_tool<T>::fixed_array_to(T &ii, std::string &array){
	if(array[0]!='[' || array=="[]"){
		return 0;
	}
	ceiling = ii.size();
	j = 0;
	k = array.length();
	str.clear();
	for(i=1;i<k;++i){
		if(array[i]==','){
			if(j<ceiling){
				ii[j] = std::stoi(str);
				++j;
			}
			else{
				return 0;
			}
			str.clear();
		}
		else if(array[i]==']'){
			if(j<ceiling){
				ii[j] = std::stoi(str);
				++j;
			}
			else{
				return 0;
			}
			str.clear();
		}
		else{
			str += array[i];
		}
	}
	return j;
}
