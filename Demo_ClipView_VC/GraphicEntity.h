#pragma once
#include "stdafx.h"
#include <vector>
using std::vector;
//浮点型点坐标
struct fCPoint
{
	double x;
	double y;
	fCPoint(double x = 0, double y = 0) : x(x), y(y){}
	fCPoint(int val_x,int val_y)
	{
		x = val_x;
		y = val_y;
	}
	void operator = (const CPoint &val)
	{
		x = val.x;
		y = val.y;
	}
	void operator = (const fCPoint &val)
	{
		x = val.x;
		y = val.y;
	}
	fCPoint operator + (const fCPoint &val)
	{
		fCPoint temp = fCPoint(x, y);
		temp.x += val.x;
		temp.y += val.y;
		return temp;
	}
	fCPoint operator - (const fCPoint &val)
	{
		fCPoint temp = fCPoint(x, y);
		temp.x -= val.x;
		temp.y -= val.y;
		return temp;
	}
	fCPoint operator * (const double &val)
	{
		fCPoint temp = fCPoint(x, y);
		temp.x *= val;
		temp.y *= val;
		return temp;
	}
};
struct Line
{
public:
	CPoint startpoint;
	CPoint endpoint;
};

struct Circle
{
public:
	CPoint center;
	int radius;
};

struct Boundary
{
public:
	vector<CPoint> vertexs;
};