#ifndef iii_tool_hpp
#define iii_tool_hpp
#include <dalahast/tool/da_type.hpp>
#include <dalahast/tool/is_tool.hpp>
#include <dalahast/tool/ii_tool.hpp>
template<class T>
class iii_tool : public is_tool , public ii_tool<T>{
protected:
	da::IS is_use;
	typename std::vector<T*>::iterator it;
	typename std::vector<T*>::iterator it_end;
	std::size_t i;
	inline void is_use_sync_size(std::vector<T*> &iii);
	inline void iii_sync_size(std::vector<T*> &iii);
public:
	std::string outcome;
	void to_array(std::vector<T*> &iii);
	void to_array(std::vector<T*> &iii, std::size_t start, std::size_t end);
	bool array_to(std::vector<T*> &iii, std::string &array);
	std::size_t fixed_array_to(std::vector<T*> &iii, std::string &array);
};
#include <dalahast/tool/iii_tool.cpp>
#endif /* iii_tool_hpp */
