#ifndef is_tool_h
#define is_tool_h
#include <dalahast/tool/da_type.h>

class is_tool{
protected:
	typename da::IS::iterator it;
	typename da::IS::iterator it_re;
	typename da::IS::iterator it_end;
	std::string str;
	std::size_t i,j,ceiling;
	short int count;
	inline void json_to_end(std::string &json);
	inline void array_to_end(std::string &array);
	char array_separate {'\0'};
public:
	std::string outcome;
	void error_log(da::IS &error,std::string &str,short int &count);
	std::size_t stack_up(da::IS &is);
	std::size_t stack_up(da::IS &is,std::size_t start, std::size_t end);
	void delete_repeat(da::IS &is);
	std::size_t delete_repeat(da::IS &is,std::size_t start, std::size_t end);
	void set_array_separate(char sep);
	void to_json(da::IS &is);
	void to_array(da::IS &is);
	bool json_to(da::IS &is, std::string &json);
	bool array_to(da::IS &is, std::string &array);
	size_t fixed_json_to(da::IS &is, std::string &json);
	size_t fixed_array_to(da::IS &is, std::string &array);
	void to_json(da::IS &is, std::size_t start, std::size_t end);
	void to_array(da::IS &is, std::size_t start, std::size_t end);
	void concat_array_string(std::string &main,std::string &sub);

};
#include <dalahast/tool/is_tool.cpp>
#endif
