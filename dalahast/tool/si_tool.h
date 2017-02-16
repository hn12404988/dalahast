#ifndef si_tool_h
#define si_tool_h
#include <dalahast/tool/da_type.h>

template <class T>
class si_tool{
protected:
	typename T::iterator it;
	typename T::iterator it_end;
	std::size_t i,j;
	short int ceiling {0};
	std::string str;
	da::IS *fixed {nullptr};
	std::vector<bool> fixed_bool;
public:
	bool all_quote {false};
	std::string outcome;
	void import_fixed(da::IS *im_fixed);
	void to_json(T &target);
	bool json_to(T &target,std::string &json);
	bool get_to(T &target,std::string &json);
};

#include <dalahast/tool/si_tool.cpp>
#endif
