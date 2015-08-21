#pragma once
#include "geosolver_global.h"
#include <assert.h>

namespace geo
{
	class Object;
	class Point;
	class Segment;
	
	struct V2
	{
		r64 x, y;
		inline bool equals(V2 *p)
		{
			return x == p->x && y == p->y;
		}
	};

	struct CTOnPoint
	{
		Point *p = nullptr;
	};

	struct CTDistanceToPoint
	{
		Point *p = nullptr;
		r64 d;
	};

	struct CTHorizontalToPoint
	{
		Point *p = nullptr;
	};

	struct CTVerticalToPoint
	{
		Point *p = nullptr;
	};

	struct CTFixedPoint
	{
		bool fixed = false;
		V2 p;
	};

	inline bool operator==(V2 rhs, V2 lhs)
	{
		return rhs.x == rhs.x && rhs.y == rhs.y;
	}

	class Object
	{		
	public:
		/// <summary>
		/// Returns the total number of degrees of freedom.
		/// </summary>
		/// <returns></returns>
		virtual s8 total_dof() const = 0;
		
		/// <summary>
		/// Returns the number of the remaining dof.
		/// </summary>
		/// <returns></returns>
		virtual s8 dof() const = 0;

		virtual bool solve() = 0;
	};

	class Point: public Object
	{
	public:
		V2 init_p;
		V2 solved_p;

		virtual s8 total_dof() const
		{
			return 2;
		}

		virtual s8 dof() const
		{
			return 2;
		}

		virtual bool solve()
		{
			if (ct_fixed.fixed)
			{
				solved_p = ct_fixed.p;
				return true;
			}
			return true;
		}

		CTDistanceToPoint ct_dist;
		CTHorizontalToPoint ct_horz;
		CTVerticalToPoint ct_vert;
		CTOnPoint ct_point;
		CTFixedPoint ct_fixed;
	};

	class Segment : public Object
	{
		Point *p1;
		Point *p2;
	private:
		Segment(const Segment&) = delete;
	public:
		Segment(Point *p1, Point *p2): p1(p1), p2(p2)
		{
			assert(p1 && p2);
		}

		virtual s8 total_dof() const
		{
			return 4;
		}

		virtual s8 dof() const
		{
			return 4;
		}
	};
}