#ifndef ii_tool_hpp
#define ii_tool_hpp
#include <dalahast/tool/da_type.hpp>

template< class T>
class ii_tool{
protected:
	typename T::iterator it;
	typename T::iterator it_re;
	typename T::iterator it_end;
	std::string str;
	std::size_t i,j,k,ceiling;
public:
	std::string outcome;
	void delete_repeat(T &ii);
	std::size_t delete_repeat(T &ii, std::size_t start, std::size_t end);
	void to_array(T &ii);
	void to_array(T &ii, std::size_t start, std::size_t end);
	bool array_to(T &ii, std::string &array);
	std::size_t fixed_array_to(T &ii, std::string &array);

};
#include <dalahast/tool/ii_tool.cpp>
#endif /* ii_tool_hpp */
