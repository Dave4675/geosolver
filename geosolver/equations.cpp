#include "stdafx.h"
#include "geosolver.h"
#include "equations.hpp"
#include <map>

using std::cout;
using std::endl;

namespace geo {
	namespace equ {

		std::map<std::string, s64> name_id_map;
		std::map<s64, std::string> id_name_map;
		s64 id = 0;
		GEOSOLVER_API s32 SymNode::total_ctor_calls = 0;

		GEOSOLVER_API void sym_kernel_init()
		{
			id = 0;
			name_id_map.clear();
		}

		GEOSOLVER_API void display_sym_node(SymNode *node)
		{
			cout << *node << endl;
		}

		GEOSOLVER_API s64 sym_kernel_get_id(std::string name)
		{
			auto it = name_id_map.find(name);
			if (it != name_id_map.end())
			{
				return it->second;
			}
			else
			{
				name_id_map[name] = id;
				id_name_map[id] = name;
				//cout << "added symvar: [" << name << "] with id: " << id << endl;
				return id++;
			}
		}

		GEOSOLVER_API std::string sym_kernel_get_name(s64 id)
		{
			//cout << "sym_kernel_get_name: " << id << endl;
			auto it = id_name_map.find(id);
			if (it != id_name_map.end())
			{
				return it->second;
			}
			else
			{
				return "<unknown id>";
			}
		}

	}
}