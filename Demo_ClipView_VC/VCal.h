#pragma once
#include "GraphicEntity.h"
#include "stdafx.h"
#include<vector>
typedef CPoint Vector;
typedef fCPoint fVector;
//���ֵ
const double eps = 1e-10;
//ȡ��double�����ݵķ���
int dcmp(const double& x)
{
	if (fabs(x) < eps)
		return 0;
	return x < 0 ? -1 : 1;
}
//ȡ��int�����ݵķ���
int icmp(const int& x)
{
	if (x == 0)
		return 0;
	else
		return x < 0 ? -1 : 1;
}
//���
int Dot(const Vector& A, const Vector& B) { return A.x * B.x + A.y * B.y; }
//���
int Cross(const Vector& A, const Vector& B){ return A.x * B.y - A.y * B.x; }
//��������ؽ����int�ͣ��ķ���
int icmp_Cross(const Vector& A, const Vector& B)
{
	int temp = A.x * B.y - A.y * B.x;
	if (temp == 0)
		return 0;
	else
		return temp < 0 ? -1 : 1;
}
//��������ؽ����double�ͣ��ķ���
int dcmp_Cross(const fCPoint& A, const Vector& B)
{
	double temp = A.x * B.y - A.y * B.x;
	if (fabs(temp) < 0)
		return 0;
	return temp < 0 ? -1 : 1;
}
//����ֱ���ཻ�Ľ���
fCPoint GetLineIntersection(CPoint P, Vector v, CPoint Q, Vector w)
{
	Vector u = P - Q;
	double t = 1.0 * Cross(w, u) / Cross(v, w);
	return  fCPoint(v.x * t + P.x,v.y * t + P.y);
}
//�жϵ��Ƿ����߶���
bool OnSegment(const CPoint& p, const CPoint& a1,const CPoint& a2)
{
	return Cross(a1 - p, a2 - p) == 0 && Dot(a1 - p, a2 - p) < 0;
}
//�õ���P��ֱ��ab�ϵ�ͶӰ
fCPoint getFoot_point(const CPoint& p,const CPoint& a,const CPoint& b)
{
	fCPoint fp;
	Vector v1 = p - a, v2 = b - a;
	double t = 1.0 * Dot(v1, v2) / Dot(v2, v2);
	fp.x = (t * v2.x + eps) + a.x;
	fp.y = (t * v2.y + eps) + a.y;
	return fp;
}
//�õ�l���Բ��ֱ�߽���
fCPoint getLineCircleIntersection_clock(const CPoint l,const CPoint r,const Circle cir)
{
	double a = r.x - l.x, b = l.x - cir.center.x, c = r.y - l.y, d = l.y - cir.center.y;
	double e = a * a + c * c, f = 2 * (a*b + c*d), g = b*b + d*d - cir.radius * cir.radius;
	double delta = f * f - 4 * e * g;
	double t1 = (-f - sqrt(delta)) / (2 * e);
	double t2 = (-f + sqrt(delta)) / (2 * e);

	double x1 = l.x + t1 * (r.x - l.x);
	double x2 = l.x + t2 * (r.x - l.x);

	fCPoint p;
	if (fabs(x1 - l.x) < fabs(x2 - l.x))
	{
		p.x = x1;
		p.y = l.y + t1 * (r.y - l.y);
	}
	else
	{
		p.x = x2;
		p.y = l.y + t2 * (r.y - l.y);
	}
	return p;
}
//�õ�r���Բ��ֱ�߽���
fCPoint getLineCircleIntersection_anticlock(const CPoint& l,const CPoint& r,const Circle& cir)
{
	double a = r.x - l.x, b = l.x - cir.center.x, c = r.y - l.y, d = l.y - cir.center.y;
	double e = a * a + c * c, f = 2 * (a*b + c*d), g = b*b + d*d - cir.radius * cir.radius;
	double delta = f * f - 4 * e * g;
	double t1 = (-f - sqrt(delta)) / (2 * e);
	double t2 = (-f + sqrt(delta)) / (2 * e);

	double x1 = l.x + t1 * (r.x - l.x);
	double x2 = l.x + t2 * (r.x - l.x);

	fCPoint p;
	if (fabs(x1 - r.x) < fabs(x2 - r.x))
	{
		p.x = x1;
		p.y = l.y + t1 * (r.y - l.y);
	}
	else
	{
		p.x = x2;
		p.y = l.y + t2 * (r.y - l.y);
	}
	return p;
}
//�õ�Բ��ֱ�߽���
void getLineCircleIntersection(const CPoint& l,const CPoint& r,const Circle& cir, fCPoint &p1, fCPoint &p2)
{
	double a = r.x - l.x, b = l.x - cir.center.x, c = r.y - l.y, d = l.y - cir.center.y;
	double e = a * a + c * c, f = 2 * (a*b + c*d), g = b*b + d*d - cir.radius * cir.radius;
	double delta = f * f - 4 * e * g;
	if (delta < eps) return;
	double t1 = (-f - sqrt(delta)) / (2 * e);
	double t2 = (-f + sqrt(delta)) / (2 * e);

	double x1 = l.x + t1 * (r.x - l.x);
	double x2 = l.x + t2 * (r.x - l.x);

	if (fabs(x1 - l.x) < fabs(x2 - l.x))
	{
		p1.x = x1;
		p1.y = l.y + t1 * (r.y - l.y);

		p2.x = x2;
		p2.y = l.y + t2 * (r.y - l.y);
	}
	else
	{
		p1.x = x2;
		p1.y = l.y + t2 * (r.y - l.y);

		p2.x = x1;
		p2.y = l.y + t1 * (r.y - l.y);
	}
	
}
//�жϵ��Ƿ��ڶ�����ڲ�
int isPointInPolygon(const CPoint& p,const vector<CPoint> &poly)
{
	int w = 0;
	int n = poly.size();
	for (int i = 0; i < n - 1; i++)
	{
		CPoint a = poly[i], b = poly[i + 1];
		if (OnSegment(p, a, b)) return -1;
		int k = icmp_Cross(b - a, p - a);
		int d1 = icmp(a.y - p.y);
		int d2 = icmp(b.y - p.y);
		if (k > 0 && d1 <= 0 && d2 > 0) w++;
		if (k < 0 && d2 <= 0 && d1 > 0) w--;
	}
	if (w != 0) return 1;
	return 0;
}
