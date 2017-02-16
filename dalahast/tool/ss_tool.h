#ifndef ss_tool_h
#define ss_tool_h
#include <dalahast/tool/da_type.h>
class ss_tool{
protected:
	da::SS::iterator it;
	da::SS::iterator it_end;
	std::size_t i,j;
	short int ceiling {0};
	std::string str;
	da::IS *fixed {nullptr};
	std::vector<bool> fixed_bool;
	inline void json_to_end(std::string &json);
	inline void array_to_end(std::string &array);
	inline void escape(std::string &line);
public:
	std::string outcome;
	bool escape_quote {false};
	bool all_quote {false};
	void import_fixed(da::IS *im_fixed);
	void to_json(da::SS &ss);
	bool json_to(da::SS &ss,std::string &json);
	bool get_to(da::SS &ss,std::string &json);
};
#include <dalahast/tool/ss_tool.cpp>
#endif
