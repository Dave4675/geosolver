#pragma once
#include "geosolver.h"
#include "geosolver_global.h"
#include <memory>
#include <string>
#include <list>
#include <algorithm>

using std::cout;
using std::endl;

namespace geo {
	namespace equ {


		struct SymNode;
		using SymNodePtr = std::shared_ptr<SymNode>;
		using SymNodeList = std::list<SymNode>;

		GEOSOLVER_API void sym_kernel_init();
		GEOSOLVER_API s64 sym_kernel_get_id(std::string name);
		GEOSOLVER_API std::string sym_kernel_get_name(s64 id);

		GEOSOLVER_API void display_sym_node(SymNode *node);
		int simplify(SymNode &node);

		enum class SYM_TYPE { OP_NONE, OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_VAL_RATIONAL, OP_VAR, OP_VAL_REAL};
		struct SymNode
		{
			static GEOSOLVER_API s32 total_ctor_calls;
			SymNodeList nodes;
			SYM_TYPE type;
			union
			{
				r64 real_value;
				struct
				{
					s32 id;
					r32 power;
				};
			};

			SymNode()
			{
				type = SYM_TYPE::OP_NONE;
				total_ctor_calls++;
			}

			SymNode(SYM_TYPE type): type(type) { total_ctor_calls++; }

			SymNode(SymNode v1, SymNode v2, SYM_TYPE type)
			{
				this->type = type;

#if 0
				if (type == SYM_TYPE::OP_ADD)
				{
					if (v1.is_var() && v2.is_var() && v1.id == v2.id)
					{
						this->type = SYM_TYPE::OP_MUL;
						nodes.p
							nodes.push_back(v1);
					}
				}

				else
#endif // 0
				if (type == SYM_TYPE::OP_MUL)
				{
					if (v1.is_real() && v1.real_value == 1)
					{
						this->type = v2.type;
						real_value = v2.real_value;
					}
					else if (v2.is_real() && v2.real_value == 1)
					{
						this->type = v1.type;
						real_value = v1.real_value;
					}
					else
					{
						if (!v1.is_none() && !v2.is_none())
						{
							if (v1.is_var() && v2.is_var() && v1.id == v2.id)
							{
								this->id = v1.id;
								this->power = v1.power + v2.power;
								this->type = SYM_TYPE::OP_VAR;
							}	
							else
							{
								if (type == v1.type)
								{
									nodes.splice(nodes.end(), v1.nodes);
								}
								else
								{
									nodes.push_back(v1);
								}

								if (type == v2.type)
								{
									nodes.splice(nodes.end(), v2.nodes);
								}
								else
								{
									nodes.push_back(v2);
								}
							}
						}
					}

				}
				else
				{
					if (!v1.is_none() && !v2.is_none())
					{
						if (v1.is_var() && v2.is_var() && v1.id == v2.id && type == SYM_TYPE::OP_MUL)
						{
							this->id = id;
							power = v1.power + v2.power;
						}
						else if (type == v1.type)
						{
							nodes.splice(nodes.end(), v1.nodes);
						}
						else
						{
							nodes.push_back(v1);
						}

						if (type == v2.type)
						{
							nodes.splice(nodes.end(), v2.nodes);
						}
						else
						{
							nodes.push_back(v2);
						}
					}
				}
				
				total_ctor_calls++;
			}

			

			bool is_final() const
			{
				return nodes.empty();
			}

			bool is_real() const
			{
				return type == SYM_TYPE::OP_VAL_REAL;
			}

			bool is_add() const
			{
				return type == SYM_TYPE::OP_ADD;
			}

			bool is_mul() const
			{
				return type == SYM_TYPE::OP_MUL;
			}

			bool is_var() const
			{
				return type == SYM_TYPE::OP_VAR;
			}

			bool is_none() const
			{
				return type == SYM_TYPE::OP_NONE;
			}
		};

		std::ostream& operator<<(std::ostream& out, SymNode node)
		{
			size_t i = 0;
			auto it = node.nodes.begin();
			switch (node.type)
			{
			case SYM_TYPE::OP_VAR:
				out << sym_kernel_get_name(node.id);
				if (node.power != 1)
				{
					out << "^" << node.power;
				}
				out << " ";

				break;
			case SYM_TYPE::OP_VAL_REAL:
				out << node.real_value << " ";
				break;
			default:
				if (!node.nodes.empty() && node.is_add())
					out << "( ";
				for (; it != node.nodes.end() && i < node.nodes.size()-1; it++)
				{
					out << (*it);
					
					switch (node.type)
					{
					case SYM_TYPE::OP_ADD:
						out << "+ ";
						break;
					case SYM_TYPE::OP_SUB:
						out << "- ";
						break;
					case SYM_TYPE::OP_MUL:
						//out << ".";
						break;
					case SYM_TYPE::OP_DIV:
						out << "/ ";
						break;
					}

					i++;
				}
				if (!node.nodes.empty())
				{
					out << (*(it++));
					if(node.is_add())
						out << ") ";
				}
				break;
			}
			return out;
		}

		struct ExpandResult
		{
			SymNodePtr node;
			int depth;
		};
		
		SymNode op_none()
		{
			return SymNode();
		}

		SymNode op_mul(SymNode lhs, SymNode rhs)
		{
			return SymNode(lhs, rhs, SYM_TYPE::OP_MUL);
		}

		SymNode op_add(SymNode lhs, SymNode rhs)
		{
			return SymNode(lhs, rhs, SYM_TYPE::OP_ADD);
		}

		SymNode op_sub(SymNode lhs, SymNode rhs)
		{
			return SymNode(lhs, rhs, SYM_TYPE::OP_SUB);
		}

		SymNode op_div(SymNode lhs, SymNode rhs)
		{
			return SymNode(lhs, rhs, SYM_TYPE::OP_DIV);
		}

		SymNode op_val_real(r64 value)
		{
			auto p = SymNode(SYM_TYPE::OP_VAL_REAL);
			p.real_value = value;
			return p;
		}

		SymNode op_var(std::string&& name)
		{
			auto p = SymNode();
			p.type = SYM_TYPE::OP_VAR;
			p.power = 1;
			p.id = sym_kernel_get_id(name);
			return p;
		}

		/*
		SymNodePtr operator+(SymNodePtr lhs, const char* rhs)
		{
			
		}*/

		SymNode operator+(SymNode lhs, SymNode rhs)
		{
			return op_add(lhs, rhs);
		}

		SymNode operator-(SymNode lhs, SymNode rhs)
		{
			return op_sub(lhs, rhs);
		}

		SymNode operator*(SymNode lhs, SymNode rhs)
		{
			return op_mul(lhs, rhs);
		}

		SymNode operator/(SymNode lhs, SymNode rhs)
		{
			return op_div(lhs, rhs);
		}

		// NODE - R64 SECTION
		SymNode operator+(SymNode lhs, r64 rhs)
		{
			return op_add(lhs, op_val_real(rhs));
		}

		SymNode operator-(SymNode lhs, r64 rhs)
		{
			return op_sub(lhs, op_val_real(rhs));
		}

		SymNode operator*(SymNode lhs, r64 rhs)
		{
			return op_mul(lhs, op_val_real(rhs));
		}

		SymNode operator/(SymNode lhs, r64 rhs)
		{
			return op_div(lhs, op_val_real(rhs));
		}

		SymNode operator+(r64 lhs, SymNode rhs)
		{
			return op_add(op_val_real(lhs), rhs);
		}

		SymNode operator-(r64 lhs, SymNode rhs)
		{
			return op_sub(op_val_real(lhs), rhs);
		}

		SymNode operator*(r64 lhs, SymNode rhs)
		{
			return op_mul(op_val_real(lhs), rhs);
		}

		SymNode operator/(r64 lhs, SymNode rhs)
		{
			return op_div(op_val_real(lhs), rhs);
		}

		// NODE - STRING SECTION
		SymNode operator+(const char* lhs, SymNode rhs)
		{
			return op_add(op_var(lhs), rhs);
		}

		SymNode operator-(const char* lhs, SymNode rhs)
		{
			return op_sub(op_var(lhs), rhs);
		}

		SymNode operator*(const char* lhs, SymNode rhs)
		{
			return op_mul(op_var(lhs), rhs);
		}

		SymNode operator/(const char* lhs, SymNode rhs)
		{
			return op_div(op_var(lhs), rhs);
		}

		SymNode operator+(SymNode lhs, const char* rhs)
		{
			return op_add(lhs, op_var(rhs));
		}

		SymNode operator-(SymNode lhs, const char* rhs)
		{
			return op_sub(lhs, op_var(rhs));
		}

		SymNode operator*(SymNode lhs, const char* rhs)
		{
			return op_mul(lhs, op_var(rhs));
		}

		SymNode operator/(SymNode lhs, const char* rhs)
		{
			return op_div(lhs, op_var(rhs));
		}

		bool operator==(const SymNode &lhs, const SymNode& rhs)
		{
			if (lhs.type == rhs.type)
			{
				if (lhs.is_var())
				{
					if (lhs.id == rhs.id)
					{
						if (lhs.power == rhs.power)
							return true;
					}
				}
			}
			return false;
		}

		//-----------------------

		SymNode expand_mul_sub(SymNode n1, SymNode n2)
		{
			SymNode result;
			
			if (n1.is_final() && n2.is_final())
			{
				result = n1*n2;
			}
			else if (n1.is_final() || n1.is_mul())
			{
				if (n2.is_add())
				{
					auto it = n2.nodes.begin();
					result = n1*(*it);
					it++;
					while (it != n2.nodes.end())
					{
						result = result + n1*(*it);
						it++;
					}
				}
				else if (n2.is_mul())
				{
					result = n1*n2;
				}
			}
			else if (n2.is_final() || n2.is_mul())
			{
				if (n1.is_add())
				{
					auto it = n1.nodes.begin();
					result = n2*(*it);
					it++;
					while (it != n1.nodes.end())
					{
						result = result + n2*(*it);
						it++;
					}
				}
				else if (n1.is_mul())
				{
					result = n1*n2;
				}
			}
			else
			{
				if (n1.is_add() && n2.is_add())
				{
					for (auto i1 : n1.nodes)
					{
						for (auto i2 : n2.nodes)
						{
							if (result.is_none())
							{
								result = i1*i2;
							}
							else
							{
								result = result + i1*i2;
							}
						}
					}
				}
			}

			return result;
		}

		SymNode expand_mul(SymNode node)
		{
			auto it = node.nodes.begin();
			SymNode result = *it;
			it++;
			
			while (it != node.nodes.end())
			{
				result = expand_mul_sub(result, *it);
				it++;
			}

			return result;
		}

		bool is_sym_node_flat(SymNode const& node)
		{
			if (node.is_mul())
			{
				for (auto& n : node.nodes)
				{
					if (!n.is_final())
					{
						return false;
					}
				}
			}
			else if (node.is_add())
			{
				for (auto& n : node.nodes)
				{
					if (!is_sym_node_flat(n))
					{
						return false;
					}
				}
			}
			return true;
		}

		SymNode expand(SymNode node)
		{
			//cout << "expanding [ " << node << "]"<<endl;
			SymNode result = node;
			if (!node.nodes.empty())
			{
				switch (node.type)
				{
				case SYM_TYPE::OP_MUL:
					result = expand_mul(node);
					break;
				case SYM_TYPE::OP_DIV:
					break;
				case SYM_TYPE::OP_ADD:
					result = SymNode(SYM_TYPE::OP_ADD);
					for (auto it : node.nodes)
					{
						result = result + expand(it);
					}
					break;
				case SYM_TYPE::OP_VAR:
					result = node;
					break;
				}
			}
			while (!is_sym_node_flat(result))
				result = expand(result);

			return result;
		}

		bool node_sort_pred(SymNode lhs, SymNode rhs)
		{
			if (lhs.type == rhs.type)
			{
				if (lhs.type == SYM_TYPE::OP_VAR)
				{
					if (lhs.power == rhs.power)
					{
						return lhs.id < rhs.id;
					}
					else
					{
						return lhs.power > rhs.power;
					}
				}
				else if (lhs.type == SYM_TYPE::OP_VAL_REAL)
				{
					return lhs.real_value > lhs.real_value;
				}
				else if (lhs.is_mul())
				{
					return node_sort_pred(lhs.nodes.front(), rhs.nodes.front());
				}
				else
					return false;
			}
			else
				return lhs.type > rhs.type;
		}

		bool is_flat_mul_node_equal(const SymNode& n1, const SymNode& n2)
		{
			if (n1.nodes.size() != n2.nodes.size())
				return false;

			auto it1 = n1.nodes.begin();
			auto it2 = n2.nodes.begin();

			while (it1 != n1.nodes.end())
			{
				if (!((*it1) == (*it2)))
					return false;
				it1++;
				it2++;
			}

			if (n1.is_final() && n2.is_final())
				return n1 == n2;

			return true;
		}

		int simplify(SymNode &node)
		{
			int n_op = 0;
			if (!node.nodes.empty())
			{
				if (!std::is_sorted(node.nodes.begin(), node.nodes.end(), node_sort_pred))
				{
					node.nodes.sort(node_sort_pred);
					n_op++;
				}
				
				for (auto &n : node.nodes)
				{
					n_op += simplify(n);
				}

				auto it1 = node.nodes.begin();
				auto it2 = it1;
				it2++;

				while (it2 != node.nodes.end())
				{
					if (it1->type == it2->type && it1->type == SYM_TYPE::OP_VAL_REAL && node.type == SYM_TYPE::OP_ADD)
					{
						it1->real_value += it2->real_value;
						node.nodes.erase(it2++);
						n_op++;
						if (it2 == node.nodes.end())
							break;
					}
					else if (it1->type == it2->type && it1->type == SYM_TYPE::OP_VAL_REAL && node.type == SYM_TYPE::OP_MUL)
					{
						it1->real_value *= it2->real_value;
						node.nodes.erase(it2++);
						n_op++;
						if (it2 == node.nodes.end())
							break;
					}
					else
					{
						it1++;
						if (it1 != node.nodes.end())
						{
							it2 = it1;
							it2++;
							if (it2 == node.nodes.end())
								break;
						}
					}
				}

				if (node.nodes.size() == 1)
				{
					node.type = node.nodes.front().type;
					if (node.type == SYM_TYPE::OP_VAR)
					{
						node.id = node.nodes.front().id;
						n_op++;
					}
					else if (node.type == SYM_TYPE::OP_VAL_REAL)
					{
						node.real_value = node.nodes.front().real_value;
						n_op++;
					}

					node.nodes.clear();
				}

#if 1
				if (node.is_mul() && node.nodes.front().is_var())
				{
					auto first_it = node.nodes.begin();
					auto last_it = first_it;

					while (first_it != node.nodes.end())
					{
						r32 power = 1;
						last_it++;
						if (last_it != node.nodes.end() && last_it->id == first_it->id)
						{
							while (last_it != node.nodes.end() && last_it->id == first_it->id)
							{
								power += last_it->power;
								last_it++;
							}

							SymNode power_node;
							power_node.power = power;
							power_node.type = SYM_TYPE::OP_VAR;
							power_node.id = first_it->id;

							node.nodes.erase(first_it, last_it);
							node.nodes.push_front(power_node);
							n_op++;
						}

						first_it = last_it;
					}
				}
#endif // 0

#if 1
				// Join MUL nodes with the same content: x*y + x*y = 2*x*y
				if (node.is_add() && is_sym_node_flat(node))
				{
					auto first_it = node.nodes.begin();
					auto last_it = first_it;

					while (first_it != node.nodes.end())
					{
						r64 times = 1;
						last_it++;
						if (last_it != node.nodes.end() && is_flat_mul_node_equal(*first_it, *last_it))
						{
							while (last_it != node.nodes.end() && is_flat_mul_node_equal(*first_it, *last_it))
							{
								times++;
								last_it++;
							}
							SymNode mul_node = op_mul(*first_it, op_val_real(times));

							node.nodes.erase(first_it, last_it);
							node.nodes.push_front(mul_node);
						}

						first_it = last_it;
					}
				}
#endif // 1

			}

			if (n_op > 0)
				simplify(node);

			return n_op;
		}

		struct Equation
		{
			SymNode lhs;
			SymNode rhs;
		};
	}
}