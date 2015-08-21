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
		using SymNodeList = std::list<SymNodePtr>;

		GEOSOLVER_API void sym_kernel_init();
		GEOSOLVER_API s32 sym_kernel_get_id(std::string name);
		GEOSOLVER_API std::string sym_kernel_get_name(s32 id);

		struct SymNode
		{
			enum { OP_ADD, OP_SUB, OP_MUL, /*OP_SQUARE, */ OP_DIV, OP_VAL_RATIONAL, OP_VAL_REAL, OP_VAR};
			SymNodeList nodes;
			int type;
			void *data;
			SymNode(SymNodePtr v1, SymNodePtr v2, int type)
			{
				this->type = type;

				if (v1 && v2)
				{
					if (type == v1->type)
					{
						nodes.splice(nodes.end(), v1->nodes);
					}
					else
					{
						nodes.push_back(v1);
					}

					if (type == v2->type)
					{
						nodes.splice(nodes.end(), v2->nodes);
					}
					else
					{
						nodes.push_back(v2);
					}
				}

				data = nullptr;
			}
		};

		std::ostream& operator<<(std::ostream& out, SymNodePtr node)
		{
			size_t i = 0;
			auto it = node->nodes.begin();
			switch (node->type)
			{
			case SymNode::OP_VAR:
				out << sym_kernel_get_name(*(s32*)node->data) << " ";
				break;
			case SymNode::OP_VAL_REAL:
				out << *(r64*)node->data << " ";
				break;
			default:
				if (!node->nodes.empty())
					out << "( ";
				for (; it != node->nodes.end() && i < node->nodes.size()-1; it++)
				{
					out << (*it);
					
					switch (node->type)
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
				if (!node->nodes.empty())
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

		SymNodePtr op_mul(SymNodePtr lhs, SymNodePtr rhs)
		{
			return std::make_shared<SymNode>(lhs, rhs, SymNode::OP_MUL);
		}

		SymNodePtr op_add(SymNodePtr lhs, SymNodePtr rhs)
		{
			return std::make_shared<SymNode>(lhs, rhs, SymNode::OP_ADD);
		}

		SymNodePtr op_sub(SymNodePtr lhs, SymNodePtr rhs)
		{
			return std::make_shared<SymNode>(lhs, rhs, SymNode::OP_SUB);
		}

		SymNodePtr op_div(SymNodePtr lhs, SymNodePtr rhs)
		{
			return std::make_shared<SymNode>(lhs, rhs, SymNode::OP_DIV);
		}

		SymNodePtr op_val_real(r64 value)
		{
			r64 *v = new r64{ value };
			auto p = std::make_shared<SymNode>(nullptr, nullptr, SymNode::OP_VAL_REAL);
			p->data = v;
			return p;
		}

		SymNodePtr op_var(std::string&& name)
		{
			s32 *v = new s32(0);
			*v = sym_kernel_get_id(name);
			auto p = std::make_shared<SymNode>(nullptr, nullptr, SymNode::OP_VAR);
			p->data = v;
			return p;
		}

		/*
		SymNodePtr operator+(SymNodePtr lhs, const char* rhs)
		{
			
		}*/

		SymNodePtr operator+(SymNodePtr lhs, SymNodePtr rhs)
		{
			return op_add(lhs, rhs);
		}

		SymNodePtr operator-(SymNodePtr lhs, SymNodePtr rhs)
		{
			return op_sub(lhs, rhs);
		}

		SymNodePtr operator*(SymNodePtr lhs, SymNodePtr rhs)
		{
			return op_mul(lhs, rhs);
		}

		SymNodePtr operator/(SymNodePtr lhs, SymNodePtr rhs)
		{
			return op_div(lhs, rhs);
		}

		// NODE - R64 SECTION
		SymNodePtr operator+(SymNodePtr lhs, r64 rhs)
		{
			return op_add(lhs, op_val_real(rhs));
		}

		SymNodePtr operator-(SymNodePtr lhs, r64 rhs)
		{
			return op_sub(lhs, op_val_real(rhs));
		}

		SymNodePtr operator*(SymNodePtr lhs, r64 rhs)
		{
			return op_mul(lhs, op_val_real(rhs));
		}

		SymNodePtr operator/(SymNodePtr lhs, r64 rhs)
		{
			return op_div(lhs, op_val_real(rhs));
		}

		SymNodePtr operator+(r64 lhs, SymNodePtr rhs)
		{
			return op_add(op_val_real(lhs), rhs);
		}

		SymNodePtr operator-(r64 lhs, SymNodePtr rhs)
		{
			return op_sub(op_val_real(lhs), rhs);
		}

		SymNodePtr operator*(r64 lhs, SymNodePtr rhs)
		{
			return op_mul(op_val_real(lhs), rhs);
		}

		SymNodePtr operator/(r64 lhs, SymNodePtr rhs)
		{
			return op_div(op_val_real(lhs), rhs);
		}

		// NODE - STRING SECTION
		SymNodePtr operator+(const char* lhs, SymNodePtr rhs)
		{
			return op_add(op_var(lhs), rhs);
		}

		SymNodePtr operator-(const char* lhs, SymNodePtr rhs)
		{
			return op_sub(op_var(lhs), rhs);
		}

		SymNodePtr operator*(const char* lhs, SymNodePtr rhs)
		{
			return op_mul(op_var(lhs), rhs);
		}

		SymNodePtr operator/(const char* lhs, SymNodePtr rhs)
		{
			return op_div(op_var(lhs), rhs);
		}

		SymNodePtr operator+(SymNodePtr lhs, const char* rhs)
		{
			return op_add(lhs, op_var(rhs));
		}

		SymNodePtr operator-(SymNodePtr lhs, const char* rhs)
		{
			return op_sub(lhs, op_var(rhs));
		}

		SymNodePtr operator*(SymNodePtr lhs, const char* rhs)
		{
			return op_mul(lhs, op_var(rhs));
		}

		SymNodePtr operator/(SymNodePtr lhs, const char* rhs)
		{
			return op_div(lhs, op_var(rhs));
		}

		//-----------------------

		SymNodePtr expand_mul_2(SymNodePtr node1, SymNodePtr node2)
		{
			SymNodePtr result;

			result = op_add(nullptr, nullptr);

			SymNodeList::iterator it1;
			SymNodeList::iterator it2;

			if (node1->nodes.empty())
			{
				it2 = node2->nodes.begin();
				while (it2 != node2->nodes.end())
				{
					result = result + node1*(*it2);
					it2++;
				}
			}
			else
			{
				it1 = node1->nodes.begin();
				while(it1 != node1->nodes.end())
				{
					if (node2->nodes.empty())
					{
						result = result + node2*(*it1);
					}
					else
					{
						it2 = node2->nodes.begin();
						while (it2 != node2->nodes.end())
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

		SymNodePtr expand_mul(SymNodePtr node)
		{
			SymNodeList::iterator it1 = node->nodes.begin();
			SymNodeList::iterator it2 = it1;
			it2++;
			
			SymNodePtr result = expand_mul_2(*it1, *it2);
			it2++;
			while (it2 != node->nodes.end())
			{
				result = expand_mul_2(result, *it2);
				it2++;
			}

			return result;
		}

		SymNodePtr expand(SymNodePtr node)
		{
			SymNodePtr result;
			if (!node->nodes.empty())
			{
				switch (node->type)
				{
				case SymNode::OP_MUL:
					result = expand_mul(node);
					break;
				case SymNode::OP_DIV:
					break;
				case SymNode::OP_ADD:
					result = op_add(nullptr, nullptr);
					for (auto it : node->nodes)
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

		struct Equation
		{
			SymNodePtr lhs;
			SymNodePtr rhs;
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