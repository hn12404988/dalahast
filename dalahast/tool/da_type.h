#ifndef da_type_h
#define da_type_h
#include <map>
#include <string>
#include <vector>
namespace da{
	typedef std::vector<std::string> IS;
	typedef std::vector<int> II;
	typedef std::vector<size_t> IsI;
	typedef std::vector<unsigned int> IuI;
	typedef std::vector<long> IL;
	typedef std::vector<double> ID;
	typedef std::vector<float> IF;
	typedef std::vector<short int> IsH;
	typedef std::map<std::string,std::string> SS;
	typedef std::map<std::string,int> SI;
	typedef std::map<std::string,unsigned int> SuI;
	typedef std::map<std::string,long> SL;
	typedef std::map<std::string,size_t> SsI;
	typedef std::map<std::string,double> SD;
	typedef std::map<std::string,float> SF;
	typedef std::map<std::string,short int> SsH;
	typedef std::vector<II*> III;
	typedef std::vector<IuI*> IIuI;
	typedef std::vector<IsI*> IIsI;
	typedef std::vector<IS*> IIS;
	typedef std::vector<IsH*> IIsH;
	typedef std::vector<SS*> ISS;
	typedef std::vector<SL*> ISL;
	typedef std::vector<SsI*> ISsI;
	typedef std::vector<SI*> ISI;
	typedef std::vector<SuI*> ISuI;
	typedef std::vector<SD*> ISD;
	typedef std::vector<SF*> ISF;
	typedef std::vector<SsH*> ISsH;
	short int fire {0};
	short int fireNstore {1};
	short int fireNforget {2};
	short int fireNclose {3};
	short int fireNfreeze {4};
	short int fireNcheck {5};
}

class da_type{
public:
	template<typename T>
		void clear_3d(T &target);
	template<typename T>
		void clear_3d(T &target, std::size_t start, std::size_t end);
	template<typename T>
		void clear_3d(T &target,std::size_t start);
	template<typename T>
		void concat_3d(T &main,T &sub);
};


template<typename T>
void da_type::clear_3d(T &target){
	typename T::iterator it_end;
	it_end = target.end();
	for(typename T::iterator it = target.begin();it!=it_end;++it){
		(*it)->clear();
		delete *it;
	}
	target.clear();
}

template<typename T>
void da_type::clear_3d(T &target,std::size_t start, std::size_t end){
	if(end > target.size()-1){
		end = target.size()-1;
	}
	if(start>end){
		clear_3d<T>(target);
		return;
	}
	typename T::iterator it;
	typename T::iterator it_end;
	it = target.begin()+start;
	it_end = target.begin()+end+1;
	for(;it!=it_end;++it){
		(*it)->clear();
		delete *it;
		*it = nullptr;
		target.erase(it);
	}
}

template<typename T>
void da_type::clear_3d(T &target,std::size_t start){
	if(start>target.size()-1){
		clear_3d<T>(target);
		return;
	}
	std::size_t i;
	i = target.size()-1;
	for(;i>=start;--i){
		target[i]->clear();
		delete target[i];
		target[i] = nullptr;
		target.pop_back();
	}
}

template<typename T>
void da_type::concat_3d(T &main,T &sub){
	std::size_t i,j,k,l;
	i = main.size();
	if(i!=0){
		--i;
		j = i;
		for(;;--i){
			if(main[i]!=nullptr){
				break;
			}
			if(i==0){
				break;
			}
		}
	}
	else{
		j = 0;
		i = 1;
	}
	l = sub.size();
	k = 0;
	if(j>i){
		for(;j>i,k<l;++i,++k){
			if(sub[k]==nullptr){
				--i;
				continue;
			}
			main[i] = sub[k];
			sub[k] = nullptr;
		}
	}
	for(;k<l;++k){
		if(sub[k]==nullptr){
			continue;
		}
		main.push_back(sub[k]);
	}
}
#endif
