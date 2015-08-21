#pragma once
#include "geosolver.h"
#include "geosolver_global.h"
#include <memory>
#include <string>
#include <list>

namespace geo {
	namespace equ {


		struct SymNode;
		using SymNodePtr = std::shared_ptr<SymNode>;
		using SymNodeList = std::list<SymNode>;

		GEOSOLVER_API void sym_kernel_init();
		GEOSOLVER_API s32 sym_kernel_get_id(std::string name);
		GEOSOLVER_API std::string sym_kernel_get_name(s32 id);

		struct SymNode
		{
			enum { OP_NONE, OP_ADD, OP_SUB, OP_MUL, /*OP_SQUARE, */ OP_DIV, OP_VAL_RATIONAL, OP_VAL_REAL, OP_VAR};
			SymNodeList nodes;
			int type;
			void *data;

			SymNode()
			{
				type = OP_NONE;
				data = nullptr;
			}

			SymNode(int type): type(type), data(nullptr){}

			SymNode(SymNode v1, SymNode v2, int type)
			{
				this->type = type;

				if (v1.type && v2.type)
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

				data = nullptr;
			}
		};

		std::ostream& operator<<(std::ostream& out, SymNode node)
		{
			size_t i = 0;
			auto it = node.nodes.begin();
			switch (node.type)
			{
			case SymNode::OP_VAR:
				out << sym_kernel_get_name(*(s32*)node.data) << " ";
				break;
			case SymNode::OP_VAL_REAL:
				out << *(r64*)node.data << " ";
				break;
			default:
				if (!node.nodes.empty())
					out << "( ";
				for (; it != node.nodes.end() && i < node.nodes.size()-1; it++)
				{
					out << (*it);
					
					switch (node.type)
					{
					case SymNode::OP_ADD:
						out << "+ ";
						break;
					case SymNode::OP_SUB:
						out << "- ";
						break;
					case SymNode::OP_MUL:
						out << "* ";
						break;
					case SymNode::OP_DIV:
						out << "/ ";
						break;
					}

					i++;
				}
				if (!node.nodes.empty())
				{
					out << (*(it++));
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
			return SymNode(lhs, rhs, SymNode::OP_MUL);
		}

		SymNode op_add(SymNode lhs, SymNode rhs)
		{
			return SymNode(lhs, rhs, SymNode::OP_ADD);
		}

		SymNode op_sub(SymNode lhs, SymNode rhs)
		{
			return SymNode(lhs, rhs, SymNode::OP_SUB);
		}

		SymNode op_div(SymNode lhs, SymNode rhs)
		{
			return SymNode(lhs, rhs, SymNode::OP_DIV);
		}

		SymNode op_val_real(r64 value)
		{
			r64 *v = new r64{ value };
			auto p = SymNode(SymNode::OP_VAL_REAL);
			p.data = v;
			return p;
		}

		SymNode op_var(std::string&& name)
		{
			s32 *v = new s32(0);
			*v = sym_kernel_get_id(name);
			auto p = SymNode();
			p.type = SymNode::OP_VAR;
			p.data = v;
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

		//-----------------------

		SymNode expand_mul_2(SymNode node1, SymNode node2)
		{
			SymNode result(SymNode::OP_ADD);

			SymNodeList::iterator it1;
			SymNodeList::iterator it2;

			if (node1.nodes.empty())
			{
				it2 = node2.nodes.begin();
				while (it2 != node2.nodes.end())
				{
					result = result + node1*(*it2);
					it2++;
				}
			}
			else
			{
				it1 = node1.nodes.begin();
				while(it1 != node1.nodes.end())
				{
					if (node2.nodes.empty())
					{
						result = result + node2*(*it1);
					}
					else
					{
						it2 = node2.nodes.begin();
						while (it2 != node2.nodes.end())
						{
							result = result + (*it1)*(*it2);
							it2++;
						}
					}
					it1++;
				}
			}

			return result;
		}

		SymNode expand_mul(SymNode node)
		{
			SymNodeList::iterator it1 = node.nodes.begin();
			SymNodeList::iterator it2 = it1;
			it2++;
			
			SymNode result = expand_mul_2(*it1, *it2);
			it2++;
			while (it2 != node.nodes.end())
			{
				result = expand_mul_2(result, *it2);
				it2++;
			}

			return result;
		}

		SymNode expand(SymNode node)
		{
			SymNode result;
			if (!node.nodes.empty())
			{
				switch (node.type)
				{
				case SymNode::OP_MUL:
					result = expand_mul(node);
					break;
				case SymNode::OP_DIV:
					break;
				case SymNode::OP_ADD:
					result = SymNode(SymNode::OP_ADD);
					for (auto it : node.nodes)
					{
						result = result + expand(it);
					}
					break;
				}
				return result;
			}
			else
				return node;
		}

		bool node_sort_pred(SymNode& lhs, SymNode& rhs)
		{
			if (lhs.type == rhs.type)
			{
				if (lhs.type == SymNode::OP_VAR)
				{
					return *(s32*)lhs.data < *(s32*)rhs.data;
				}
				else if (lhs.type == SymNode::OP_VAL_REAL)
				{
					return *(r64*)lhs.data < *(r64*)rhs.data;
				}
				else
					return false;
			}
			else
				return lhs.type < rhs.type;
		}

		void simplify(SymNode &node)
		{
			if (!node.nodes.empty())
			{
				node.nodes.sort(node_sort_pred);

				for (auto& n : node.nodes)
				{
					simplify(n);
				}
			}
		}

		struct Equation
		{
			SymNode lhs;
			SymNode rhs;
		};

		Equation solve(Equation equ, SymNodePtr var)
		{
			if (var->type != SymNode::OP_VAR)
			{
				return Equation{ 0,0 };
			}
			//TODO: Expand this to a list of variables
			// Find the highest degree of the var
			return Equation{ 0,0 };
		}
	}
}