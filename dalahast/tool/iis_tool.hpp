#ifndef iis_tool_hpp
#define iis_tool_hpp
#include <dalahast/tool/da_type.hpp>
#include <dalahast/tool/is_tool.hpp>

class iis_tool : public is_tool{
protected:
	da::IS is_use;
	da::IIS::iterator it;
	da::IIS::iterator it_end;
	std::size_t i;
	inline void is_use_sync_size(da::IIS &iis);
	inline void iis_sync_size(da::IIS &iis);
	char tmp_array_separate {'\0'};
public:
	std::string outcome;
	void to_array(da::IIS &iis);
	void to_array(da::IIS &iis, std::size_t start, std::size_t end);
	bool array_to(da::IIS &iis, std::string &array);
	std::size_t fixed_array_to(da::IIS &iis, std::string &array);
};
#include <dalahast/tool/iis_tool.cpp>
#endif /* iis_tool_hpp */
