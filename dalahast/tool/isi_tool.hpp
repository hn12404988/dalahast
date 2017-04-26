#ifndef isi_tool_hpp
#define isi_tool_hpp
#include <dalahast/tool/da_type.hpp>
#include <dalahast/tool/is_tool.hpp>
#include <dalahast/tool/si_tool.hpp>
template<class T>
class isi_tool : public is_tool , public si_tool<T>{
protected:
	da::IS is_use;
	typename std::vector<T*>::iterator it;
	typename std::vector<T*>::iterator it_end;
	std::size_t i;
	inline void is_use_sync_size(std::vector<T*> &isi);
	inline void isi_sync_size(std::vector<T*> &isi);
public:
	std::string outcome;
	void import_fixed(da::IS *im_fixed);
	void to_json(std::vector<T*> &isi);
	void to_json(std::vector<T*> &isi, std::size_t start, std::size_t end);
	void to_array(std::vector<T*> &isi);
	void to_array(std::vector<T*> &isi, std::size_t start, std::size_t end);
	bool json_to(std::vector<T*> &isi, std::string &json);
	std::size_t fixed_json_to(std::vector<T*> &isi, std::string &json);
	bool array_to(std::vector<T*> &isi, std::string &array);
	std::size_t fixed_array_to(std::vector<T*> &isi, std::string &array);
};
#include <dalahast/tool/isi_tool.cpp>
#endif /* isi_tool_hpp */
