#ifndef iss_tool_hpp
#define iss_tool_hpp
#include <dalahast/tool/da_type.hpp>
#include <dalahast/tool/is_tool.hpp>
#include <dalahast/tool/ss_tool.hpp>
class iss_tool : public is_tool , public ss_tool{
protected:
	da::IS is_use;
	typename da::ISS::iterator it;
	typename da::ISS::iterator it_end;
	std::size_t i;
	inline void is_use_sync_size(da::ISS &iss);
	inline void iss_sync_size(da::ISS &iss);
public:
	std::string outcome;
	void import_fixed(da::IS *im_fixed);
	void to_json(da::ISS &iss);
	void to_json(da::ISS &iss, std::size_t start, std::size_t end);
	void to_array(da::ISS &iss);
	void to_array(da::ISS &iss, std::size_t start, std::size_t end);
	bool json_to(da::ISS &iss, std::string &json);
	std::size_t fixed_json_to(da::ISS &iss, std::string &json);
	bool array_to(da::ISS &iss, std::string &array);
	std::size_t fixed_array_to(da::ISS &iss, std::string &array);
};
#include <dalahast/tool/iss_tool.cpp>
#endif /* iss_tool_hpp */
