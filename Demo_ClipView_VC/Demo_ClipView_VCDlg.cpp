
// Demo_ClipView_VCDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "VCal.h"
#include "Demo_ClipView_VC.h"
#include "Demo_ClipView_VCDlg.h"
#include "afxdialogex.h"
#include <TlHelp32.h>
#include <queue>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDemo_ClipView_VCDlg 对话框

#define CANVAS_WIDTH	800
#define CANVAS_HEIGHT	600
#define INFO_HEIGHT		50
#define TESTDATA_XML1  "TestData1.xml"
#define TESTDATA_XML2  "TestData2.xml"

typedef std::pair<fCPoint, int> Pair;
typedef CPoint Vector;
typedef fCPoint fVector;

int cutted_line;
fCPoint cutted_circle;

CDemo_ClipView_VCDlg::CDemo_ClipView_VCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDemo_ClipView_VCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDemo_ClipView_VCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_CLIP, m_btn_clip);
	DDX_Control(pDX, IDC_STATIC_DRAWING, m_stc_drawing);
	DDX_Control(pDX, IDC_STATIC_INFO_1, m_stc_info1);
	DDX_Control(pDX, IDC_STATIC_INFO_2, m_stc_info2);
}

BEGIN_MESSAGE_MAP(CDemo_ClipView_VCDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CLIP, &CDemo_ClipView_VCDlg::OnBnClickedBtnClip)
	ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()


BOOL CDemo_ClipView_VCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	CRect clientRect;
	GetClientRect(&clientRect);
	CRect windowRect;
	GetWindowRect(&windowRect);
	int width = CANVAS_WIDTH + windowRect.Width() - clientRect.Width();
	int height = CANVAS_HEIGHT + INFO_HEIGHT + windowRect.Height() - clientRect.Height() + 10;
	SetWindowPos(&wndTopMost, 10, 10, width, height, SWP_NOZORDER);

	m_btn_clip.SetWindowPos(NULL, 50, CANVAS_HEIGHT + 15, 80, 30, SWP_NOZORDER);
	m_stc_drawing.SetWindowPos(NULL, 140, CANVAS_HEIGHT + 20, 200, 20, SWP_NOZORDER);
	m_stc_info1.SetWindowPos(NULL, 140, CANVAS_HEIGHT + 5, 450, 20, SWP_NOZORDER);
	m_stc_info2.SetWindowPos(NULL, 140, CANVAS_HEIGHT + 25, 650, 40, SWP_NOZORDER);

	hasOutCanvasData = FALSE;
	GetModuleFileName(NULL,curPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	curPath.ReleaseBuffer();
	curPath = curPath.Left(curPath.ReverseFind('\\') + 1);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDemo_ClipView_VCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CDemo_ClipView_VCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CDemo_ClipView_VCDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg=LOWORD(wParam);
	switch(uMsg)
	{
	case ID_TESTCASE1:
		{
			CString xmlPath = curPath + TESTDATA_XML1;
			DrawTestCase(xmlPath, "1");
		}
		break;
	case ID_TESTCASE2:
		{
			CString xmlPath = curPath + TESTDATA_XML1;
			DrawTestCase(xmlPath, "2");
		}
		break;
	case ID_TESTCASE3:
		{
			CString xmlPath = curPath + TESTDATA_XML1;
			DrawTestCase(xmlPath, "3");
		}
		break;
	case ID_TESTCASE4:
		{
			CString xmlPath = curPath + TESTDATA_XML1;
			DrawTestCase(xmlPath, "4");
		}
		break;
	case ID_TESTCASE5:
		{
			CString xmlPath = curPath + TESTDATA_XML2;
			DrawTestCase(xmlPath, "5");
		}
		break;
	default:
		break;
	}
	return CDialogEx::OnCommand(wParam, lParam);
}

void CDemo_ClipView_VCDlg::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	if (HTCAPTION == nHitTest) {
		return;
	}
	CDialogEx::OnNcLButtonDown(nHitTest, point);
}

struct CDemo_ClipView_VCDlg::cmp_Circle
{
	bool operator()(const Pair&a, const Pair&b)
	{	
		if (a.first.y < cutted_circle.y + eps && b.first.y < cutted_circle.y + eps)
		{
			return a.first.x > b.first.x + eps;
		}
		else if (a.first.y > cutted_circle.y + eps && b.first.y > cutted_circle.y + eps)
		{
			return a.first.x < b.first.x - eps;
		}
		else
		{
			return a.first.y > cutted_circle.y + eps;
		}
	}
};  
struct CDemo_ClipView_VCDlg::cmp_Line
{
	bool operator()(const fCPoint&a, const fCPoint&b)
	{
		if (cutted_line == 1)
			return fabs(a.x - b.x) < eps? a.y < b.y - eps : a.x > b.x + eps;
		else
			return fabs(a.x - b.x) < eps? a.y > b.y + eps : a.x > b.x + eps;
	}
};

//判断点P是否在线段上
int CDemo_ClipView_VCDlg::isOnLine(CPoint &p, CPoint &st, CPoint &ed)
{
	int left = min(st.x, ed.x), right = max(st.x, ed.x), top = min(st.y, ed.y), bottom = max(st.y, ed.y);
	return (left <= p.x && p.x <= right && p.y >= top && p.y <= bottom );
}
int CDemo_ClipView_VCDlg::isOnLine(fCPoint &p, CPoint &st, CPoint &ed)
{
	int left = min(st.x, ed.x), right = max(st.x, ed.x), top = min(st.y, ed.y), bottom = max(st.y, ed.y);
	return (left - eps < p.x && p.x < right + eps && p.y > top - eps && p.y < bottom + eps );
}
//裁剪线段
void CDemo_ClipView_VCDlg::DrawLine_Excute(CDC &memDC,COLORREF clr)
{
	CPen penUse;
	penUse.CreatePen(PS_SOLID, 1, clr);
	CPen* penOld = memDC.SelectObject(&penUse);

	int m = lines.size(), n = boundary.vertexs.size();
	std::priority_queue<fCPoint, vector<fCPoint>, cmp_Line> intersection;    //交点数组
	vector<int> vcode(n + 1, 0);                                           //顶点编码
	for (int i = 0; i < m; ++i)
	{
		CPoint st = lines[i].startpoint, ed = lines[i].endpoint;
		if (st.x > ed.x)
		{
			st.x ^= ed.x, st.y ^= ed.y;
			ed.x ^= st.x, ed.y ^= st.y;
			st.x ^= ed.x, st.y ^= ed.y;
		}
		if (max(st.x, ed.x) < left.x || min(st.x, ed.x) > right.x || max(st.y, ed.y) < top.y || min(st.y, ed.y) > bottom.y)
		{
			outside_boundary++;
			continue;
		}
		Vector dir = ed - st;                //得到线段的方向向量
		vcode[n] = 0;
		if (dir.y <= 0)                      
		{
			fCPoint anchor;
			if (-dir.y <= dir.x)             //斜率大于等于0且小于等于1
			{
				anchor = GetLineIntersection(st, ed - st, CPoint(left.x, 0), CPoint(0, 1));
				if (top.y > anchor.y + eps)
				{
					outside_boundary++;
					continue;
				}
				for (int j = 0; j < n - 1; ++j)
				{
					CPoint vex = boundary.vertexs[j];
					if (vex.y < (anchor.y + anchor.x) - vex.x)
						vcode[j] = 1;
					else if (vex.y > anchor.y)
						vcode[j] = -1;
					else
						vcode[j] = -icmp_Cross(ed - st, vex - st);
					if (vcode[j] == 0 && isOnLine(vex, st, ed));
					vcode[n] += vcode[j];
				}
			}
			else							//斜率大于1
			{
				anchor = GetLineIntersection(st, ed - st, CPoint(0, bottom.y), CPoint(1, 0));
				if (right.x < anchor.x - eps)
				{
					outside_boundary++;
					continue;
				}
				for (int j = 0; j < n - 1; ++j)
				{
					CPoint vex = boundary.vertexs[j];
					if (vex.x >(anchor.y + anchor.x) - vex.y)
						vcode[j] = 1;
					else if (vex.x < anchor.x)
						vcode[j] = -1;
					else
						vcode[j] = icmp_Cross(ed - st, vex - st);
					if (vcode[j] == 0 && isOnLine(vex, st, ed));
					vcode[n] += vcode[j];
				}
			}
		}
		else
		{
			fCPoint anchor;
			if (dir.y <= dir.x)				//斜率大于等于-1且小于0
			{
				anchor = GetLineIntersection(st, ed - st, CPoint(right.x, 0), CPoint(0, 1));
				if (top.y > anchor.y + eps)
				{
					outside_boundary++;
					continue;
				}
				for (int j = 0; j < n - 1; ++j)
				{
					CPoint vex = boundary.vertexs[j];
					if (vex.y < (anchor.y - anchor.x) + vex.x)
						vcode[j] = 1;
					else if (vex.y > anchor.y)
						vcode[j] = -1;
					else
						vcode[j] = -icmp_Cross(ed - st, vex - st);
					if (vcode[j] == 0 && isOnLine(vex, st, ed));
					vcode[n] += vcode[j];
				}
			}
			else							////斜率小于-1
			{
				anchor = GetLineIntersection(st, ed - st, CPoint(0, top.y), CPoint(1, 0));
				if (right.x < anchor.x - eps)
				{
					outside_boundary++;
					continue;
				}
				for (int j = 0; j < n - 1; ++j)
				{
					CPoint vex = boundary.vertexs[j];
					if (vex.x >vex.y - (anchor.y - anchor.x))
						vcode[j] = 1;
					else if (vex.x < anchor.x)
						vcode[j] = -1;
					else
						vcode[j] = -icmp_Cross(ed - st, vex - st);
					if (vcode[j] == 0 && isOnLine(vex, st, ed));
						vcode[n] += vcode[j];
				}
			}
		}
		vcode[n - 1] = vcode[0];
		if (abs(vcode[n]) == n - 1)
		{
			outside_boundary++;
			continue;
		}
		cutted_line = (ed.y - st.y) <= 0;

		int count(0);                             //标识位于起点逆时针方向的交点个数
		int isOverlapping(0);					  //标识是否完全重合与边界
		//计算交点
		for (int j = 0; j < n - 1; ++j)
		{
			CPoint v1 = boundary.vertexs[j], v2 = boundary.vertexs[j + 1];
			int res = vcode[j] * vcode[j + 1];
			if (res == -1)//端点位于直线异侧
			{
				fCPoint p = GetLineIntersection(st, ed - st, v1, v2 - v1);
				if (dir.y <= 0 && (p.x < st.x-eps || p.y > st.y+eps))
					count++;
				else if (dir.y > 0 && (p.x < st.x-eps || p.y < st.y-eps))
					count++;
				else if (isOnLine(p, st, ed))
				{
					intersection.push(p);
				}
			}
			else if (res == 0)
			{
				int pre = (j == 0) ? n - 2 : j - 1;
				int next = (j == n - 2) ? 1 : j + 2;
				if (vcode[j] == 0 && vcode[j + 1] == 0)//直线与边重合
				{
					if (isOnLine(st, v1, v2) && isOnLine(ed, v1, v2))
					{
						isOverlapping = 1;
						break;
					}
					if (Cross(v1 - boundary.vertexs[pre], v2 - boundary.vertexs[pre]) < 0)
					{
						if (dir.y <= 0 && (v1.x < st.x || v1.y > st.y))
							count++;
						else if (dir.y > 0 && (v1.x < st.x || v1.y < st.y))
							count++;
						else if (isOnLine(v1, st, ed))
						{
							intersection.push(fCPoint(v1.x,v1.y));
						}
					}
					if (Cross(v2 - v1, boundary.vertexs[next] - v2) < 0 && j != n - 2)
					{
						if (dir.y <= 0 && (v2.x < st.x || v2.y > st.y))
							count++;
						else if (dir.y > 0 && (v2.x < st.x || v2.y < st.y))
							count++;
						else if (isOnLine(v2, st, ed))
						{
							intersection.push(fCPoint(v2.x,v2.y));
						}
						j++;
					}
				}
				else if (vcode[j] == 0 && vcode[pre] * vcode[j + 1] == -1)//直线过v1点
				{
					if (dir.y <= 0 && (v1.x < st.x || v1.y > st.y))
						count++;
					else if (dir.y > 0 && (v1.x < st.x || v1.y < st.y))
						count++;
					else if (isOnLine(v1, st, ed))
					{
						intersection.push(fCPoint(v1.x,v1.y));
					}
				}
				else if (vcode[j + 1] == 0 && vcode[j] * vcode[next] == -1 && j != n - 2)//直线过v2点
				{
					if (dir.y <= 0 && (v2.x < st.x-eps|| v2.y > st.y+eps))
						count++;
					else if (dir.y > 0 && (v2.x < st.x-eps || v2.y < st.y-eps))
						count++;
					else if (isOnLine(v2, st, ed))
					{
						intersection.push(fCPoint(v2.x,v2.y));
					}
					j++;
				}
			}
		}
		//统计信息并绘制图形
		if (intersection.size())
		{
			intersect_boundary++;
			intersection_num += intersection.size();
		}
		if ((count & 1) == 1)
		{
			if (intersection.size() == 0)
				inside_boundary++;
			intersection.push(fCPoint(st.x,st.y));
		}
		if (intersection.size() == 0)
		{
			if (isOverlapping == 0)
				outside_boundary++;
			else
				overlapping_boundary++;
			continue;
		}
		while (intersection.size() > 1)
		{
			fCPoint line_st = intersection.top(); intersection.pop();
			fCPoint line_ed = intersection.top(); intersection.pop();
			
			DrawLine(line_st,line_ed,memDC);
		}
		if (!intersection.empty())
		{
			fCPoint line_st = intersection.top(); intersection.pop();
			fCPoint line_ed = fCPoint(ed.x,ed.y);
			DrawLine(line_st,line_ed,memDC);
		}
	}
	memDC.SelectObject(penOld);
	penUse.DeleteObject();

}
//裁剪圆
void CDemo_ClipView_VCDlg::DrawCircle_Excute(CDC &memDC,COLORREF clr)
{
	
	CPen penUse;
	penUse.CreatePen(PS_SOLID, 1, clr);
	CPen* penOld = memDC.SelectObject(&penUse);
	int m = circles.size(), n = boundary.vertexs.size();             
	std::priority_queue<Pair, vector<Pair>, cmp_Circle> intersection;			//交点数组
	vector<int> vcode(n+1);														//顶点编码
	for (int i = 0; i < m; ++i)
	{
		Circle cir = circles[i];
		CPoint top_line = CPoint(cir.center.x, cir.center.y - cir.radius);
		CPoint bottom_line = CPoint(cir.center.x, cir.center.y + cir.radius);
		CPoint left_line = CPoint(cir.center.x - cir.radius, cir.center.y);
		CPoint right_line = CPoint(cir.center.x + cir.radius, cir.center.y);
		if (right_line.x <= left.x || left_line.x >= right.x || top_line.y >= bottom.y || bottom_line.y <= top.y)
		{
			outside_boundary++;
			continue;
		}

		vcode[n] = 31;
		int rad2 = cir.radius * cir.radius;
		for (int j = 0; j < n - 1; j++)
		{
			vcode[j] = 0;
			CPoint p = boundary.vertexs[j];
			if (p.x <= left_line.x)
				vcode[j] |= 1;
			if (p.x >= right_line.x)
				vcode[j] |= 2;
			if (p.y >= bottom_line.y)
				vcode[j] |= 4;
			if (p.y <= top_line.y)
				vcode[j] |= 8;
			if (Dot(p - cir.center, p - cir.center) <= rad2)
				vcode[j] = 16;
			vcode[n] &= vcode[j];
		}
		vcode[n - 1] = vcode[0];
		if (vcode[n] == 16 || (vcode[n] > 0 && vcode[n] <= 8))
		{
			outside_boundary++;
			continue;
		}

		cutted_circle = fCPoint(cir.center.x,cir.center.y);
		//计算交点
		for (int j = 0; j < n-1; ++j)
		{
			CPoint v1 = boundary.vertexs[j], v2 = boundary.vertexs[j + 1];
			int res = vcode[j] | vcode[j + 1];
			if (vcode[j] == 16 && vcode[j + 1] == 16)//第一类位置关系
				continue;
			else if (res >= 16)//第三类位置关系
			{
				Pair pa;
				if (vcode[j] == 16)
				{
					int pre = (j == 0) ? n - 2 : j - 1;
					fCPoint p = getLineCircleIntersection_anticlock(v1, v2, cir);
					//防止重复计交点
					if (fabs(p.x - v1.x) < eps && fabs(p.y - v1.y) < eps && vcode[pre] != 16)
					{
						fCPoint temp = getLineCircleIntersection_clock(boundary.vertexs[pre], v1, cir);
						if (fabs(temp.x - v1.x) > eps || fabs(temp.y - v1.y) > eps)
						{
							pa = std::make_pair(p, 1);
							intersection.push(pa);
						}
					}
					else
					{
						pa = std::make_pair(p, 1);
						intersection.push(pa);
					}
				}
				else
				{
					int next = (j == n - 2) ? 1 : j + 2;
					fCPoint p = getLineCircleIntersection_clock(v1, v2, cir);
					//防止重复计交点
					if (fabs(p.x - v2.x) < eps && fabs(p.y - v2.y) < eps && vcode[next] != 16)
					{		
						fCPoint temp = getLineCircleIntersection_anticlock(v2, boundary.vertexs[next], cir);
						if (fabs(temp.x - v2.x) > eps || fabs(temp.y - v2.y) > eps)
						{
							pa = std::make_pair(p, 0);
							intersection.push(pa);
						}
						else
							j++;
					}
					else
					{
						pa = std::make_pair(p, 0);
						intersection.push(pa);
					}
				}
			}
			else if ((vcode[j] & vcode[j + 1]) > 0 && (vcode[j] & vcode[j + 1]) <= 8)//第二类位置关系
				continue;
			else if (res == 3 || res == 12 || res == 15)//第四类位置关系
			{
				Pair pa1, pa2;
				fCPoint p1(-1,-1), p2(-1,-1);
				getLineCircleIntersection(v1, v2, cir, p1,p2);
				if (fabs(p1.x + 1) < eps) continue;
				pa1 = std::make_pair(p1, 0);
				intersection.push(pa1);
				pa2 = std::make_pair(p2, 1);
				intersection.push(pa2);
			}
			else//第五类位置关系
			{			
				fCPoint fp = getFoot_point(cir.center, v1, v2);//fp代表圆心在边上的投影
				double d0 = (cir.center.x - fp.x) * (cir.center.x - fp.x) + (cir.center.y - fp.y) * (cir.center.y - fp.y);
				if (d0 < rad2 - eps)
				{
					int res = dcmp_Cross(fp - cutted_circle, v1 - cir.center) * dcmp_Cross(fp - cutted_circle, v2 - cir.center);
					if (res == -1 || (res == 0 && isOnLine(cir.center, v1, v2)))
					{
						Pair pa1, pa2;
						fCPoint p1, p2;
						getLineCircleIntersection(v1, v2, cir, p1, p2);
						pa1 = std::make_pair(p1, 0);
						intersection.push(pa1);
						pa2 = std::make_pair(p2, 1);
						intersection.push(pa2);
					}
				}
				
			}		
		}
		//统计信息并绘制图形
		if (intersection.size() == 0)
		{
			if (isPointInPolygon(cir.center, boundary.vertexs) == 1)
			{
				memDC.Arc(cir.center.x - cir.radius, cir.center.y - cir.radius, cir.center.x + cir.radius, cir.center.y + cir.radius, 0, 0, 0, 0);
				inside_boundary++;
			}
			else
				outside_boundary++;
			continue;
		}
		intersect_boundary++;
		intersection_num += intersection.size();
		fCPoint zero;
		if (intersection.top().second == 0)
		{
			zero = intersection.top().first; 
			intersection.pop();
		}
		while (intersection.size() > 1)
		{
			fCPoint ed = intersection.top().first; intersection.pop();
			fCPoint st = intersection.top().first; intersection.pop();
			DrawArc(left_line.x, top_line.y, right_line.x, bottom_line.y, st.x, st.y, ed.x, ed.y, memDC);
		}
		if (intersection.size() == 1)
		{
			DrawArc(left_line.x, top_line.y, right_line.x, bottom_line.y, zero.x, zero.y, intersection.top().first.x, intersection.top().first.y, memDC);
			intersection.pop();
		}
	}
	memDC.SelectObject(penOld);	
	penUse.DeleteObject();
}
void CDemo_ClipView_VCDlg::OnBnClickedBtnClip()
{
	BeginMonitor();
	//TODO 在此处完成裁剪算法和裁剪后显示程序，并修改需要显示的图形信息

	intersection_num = 0, outside_boundary = 0, intersect_boundary = 0, inside_boundary = 0, overlapping_boundary = 0;
	//默认顶点为顺时针描述
	top = CPoint(0, CANVAS_HEIGHT), bottom = CPoint(0, 0), left = CPoint(CANVAS_WIDTH, 0), right = CPoint(0, 0);
	//取得多边形边界包围盒
	for (int i = 0; i < boundary.vertexs.size(); i++)
	{
		if (top.y > boundary.vertexs[i].y)
			top = boundary.vertexs[i];
		if (bottom.y < boundary.vertexs[i].y)
			bottom = boundary.vertexs[i];
		if (left.x > boundary.vertexs[i].x)
			left = boundary.vertexs[i];
		if (right.x < boundary.vertexs[i].x)
			right = boundary.vertexs[i];
	}

	CDC memDC;
	CClientDC dc(this);
	CBitmap memBitmap;
	CBitmap *pOldBit;
	//创建兼容DC
	memDC.CreateCompatibleDC(&dc);
	memBitmap.CreateCompatibleBitmap(&dc, CANVAS_WIDTH, CANVAS_HEIGHT);
	pOldBit = memDC.SelectObject(&memBitmap);

	memDC.FillSolidRect(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, RGB(0, 0, 0));
	
	//执行裁剪函数
	DrawLine_Excute(memDC,RGB(0,255,0));
	DrawCircle_Excute(memDC,RGB(0,0,255));

	dc.BitBlt(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, &memDC, 0, 0, SRCCOPY);

	//释放资源
	memBitmap.DeleteObject();
	memDC.DeleteDC();
	dc.DeleteDC();

	DrawBoundary(boundary, RGB(255, 0, 0));
	//显示图形信息
	char str[500];
	sprintf(str,"共%d个图形,1个裁剪边界，其中%d个在边界内部，%d与边界相交(共计%d个交点)，%d个位于边界外部，%d个与边界重合",lines.size()+circles.size(),inside_boundary,intersect_boundary,intersection_num,outside_boundary,overlapping_boundary);
	m_stc_info2.SetWindowText(str);

	EndMonitor();
}

BOOL CDemo_ClipView_VCDlg::XmlNodeToPoint(pugi::xml_node node, CPoint& piont)
{
	pugi::xml_node pointNode = node.first_child();
	CString strValue(pointNode.value());
	int curPos = 0;
	int count = 0;
	while(TRUE)
	{
		CString resToken = strValue.Tokenize(",", curPos);
		if (resToken.IsEmpty())
			break;
		int coord = _ttoi(resToken);
		count++;
		if (count == 1)
		{
			piont.x = coord;
		}
		else if (count == 2)
		{
			piont.y = CANVAS_HEIGHT - coord; //调整CAD与窗口显示的坐标系一致
		}
	}
	if (count != 2)
	{
		return FALSE;
	}
	return TRUE;
}
int  CDemo_ClipView_VCDlg::SplitCStringToArray(CString str,CStringArray& strArray, CString split)
{
	int curPos = 0;
	while(TRUE)
	{
		CString resToken = str.Tokenize(split, curPos);
		if (resToken.IsEmpty())
			break;
		strArray.Add(resToken);
	}
	return strArray.GetSize();
}
BOOL CDemo_ClipView_VCDlg::IsPointOutCanvas(CPoint point)
{
	if (point.x < 0 || point.y < 0 || point.x > CANVAS_WIDTH || point.y > CANVAS_HEIGHT)
	{
		return TRUE;
	}
	return FALSE;
}
BOOL CDemo_ClipView_VCDlg::IsLineOutCanvas(Line line)
{
	if (IsPointOutCanvas(line.startpoint) || IsPointOutCanvas(line.endpoint))
	{
		return TRUE;
	}
	return FALSE;
}
BOOL CDemo_ClipView_VCDlg::IsCircleOutCanvas(Circle circle)
{
	if (IsPointOutCanvas(circle.center))
	{
		return TRUE;
	}
	if ((circle.center.x - circle.radius) < 0 || (circle.center.x + circle.radius) > CANVAS_WIDTH
		|| (circle.center.y - circle.radius) < 0 || (circle.center.y + circle.radius) > CANVAS_HEIGHT)
	{
		return TRUE;
	}
	return FALSE;
}
BOOL CDemo_ClipView_VCDlg::IsBoundaryOutCanvas(Boundary boundary)
{
	vector<CPoint>::iterator iter = boundary.vertexs.begin();
	for (;iter != boundary.vertexs.end(); iter++)
	{
		if (IsPointOutCanvas(*iter))
		{
			return TRUE;
		}
	}
	return FALSE;
}


BOOL CDemo_ClipView_VCDlg::LoadTestCaseData(CString xmlPath, CString caseID)
{
	pugi::xml_document doc;
	if (!doc.load_file(xmlPath)) return FALSE;
	pugi::xml_node root = doc.child("TestRoot");
	pugi::xml_node testCase = root.find_child_by_attribute("ID",caseID);
	if (!testCase) return FALSE;
	for (pugi::xml_node entity = testCase.first_child(); entity; entity = entity.next_sibling())
	{
		pugi::xml_attribute type = entity.attribute("Type");
		CString typeValue(type.value());
		if (typeValue.CompareNoCase("Line") == 0)
		{
			Line line;
			pugi::xml_node startNode = entity.child("StartPoint");
			if(!XmlNodeToPoint(startNode, line.startpoint)) return FALSE;
			pugi::xml_node endNode = entity.child("EndPoint");
			if(!XmlNodeToPoint(endNode, line.endpoint)) return FALSE;
			if (IsLineOutCanvas(line))
			{
				hasOutCanvasData = TRUE;
				continue;
			}
			lines.push_back(line);
		} 
		else if(typeValue.CompareNoCase("Circle") == 0)
		{
			Circle circle;
			pugi::xml_node centerNode = entity.child("CenterPoint");
			if(!XmlNodeToPoint(centerNode, circle.center)) return FALSE;
			pugi::xml_node radiusNode = entity.child("Radius");
			pugi::xml_node pointNode = radiusNode.first_child();
			circle.radius = _ttoi(pointNode.value());
			if (IsCircleOutCanvas(circle))
			{
				hasOutCanvasData = TRUE;
				continue;
			}
			circles.push_back(circle);
		}
		else if (typeValue.CompareNoCase("Polygon") == 0)
		{
			for (pugi::xml_node vertex = entity.first_child(); vertex; vertex = vertex.next_sibling())
			{
				CPoint point;
				if(!XmlNodeToPoint(vertex, point)) return FALSE;
				boundary.vertexs.push_back(point);
			}
			if (IsBoundaryOutCanvas(boundary))
			{
				hasOutCanvasData = TRUE;
				boundary.vertexs.clear();
			}
		}
	}
	return TRUE;
}
void CDemo_ClipView_VCDlg::ClearTestCaseData()
{
	hasOutCanvasData = FALSE;
	boundary.vertexs.clear();
	circles.clear();
	lines.clear();
	if (beginTIdList.GetSize() > 0)
	{
		beginTIdList.RemoveAll();
	}
	CClientDC dc(this);
	dc.FillSolidRect(0,0,CANVAS_WIDTH,CANVAS_HEIGHT, RGB(0,0,0));
}

void CDemo_ClipView_VCDlg::BeginMonitor()
{
	m_btn_clip.EnableWindow(FALSE);
	m_stc_drawing.SetWindowText("裁剪中...");
	m_stc_info1.ShowWindow(SW_HIDE);
	m_stc_info2.ShowWindow(SW_HIDE);
	GetThreadIdList(beginTIdList);
	startTime = clock();
}
void CDemo_ClipView_VCDlg::EndMonitor()
{
	endTime = clock();
	double useTimeS = (endTime - startTime)/1000;
	m_stc_drawing.ShowWindow(SW_HIDE);
	m_stc_info1.ShowWindow(SW_SHOW);
	CString strTime;
	strTime.Format("图形裁剪完毕，耗时：%.3lf 秒！", useTimeS);
	m_stc_info1.SetWindowText(strTime);
	m_stc_info2.ShowWindow(SW_SHOW);
	CList<int> endTIdList;
	GetThreadIdList(endTIdList);
	POSITION pos = endTIdList.GetHeadPosition();
	while(pos)
	{
		int tId = endTIdList.GetAt(pos);
		CString strId;
		strId.Format(_T("%d"), tId);
		if (!beginTIdList.Find(tId))
		{
			HANDLE th = OpenThread(THREAD_QUERY_INFORMATION, FALSE, tId);
			if (th)
			{
				DWORD exitCode = 0;
				GetExitCodeThread(th, &exitCode);
				CloseHandle(th);
				if (exitCode == STILL_ACTIVE)
				{
					MessageBox(_T("新建线程仍在运行:") + strId, _T("线程消息"), MB_OK);
					break;
				}
			}
			else
			{
				MessageBox(_T("新建线程无法访问:") + strId, _T("线程消息"), MB_OK);
				break;
			}
		}
		endTIdList.GetNext(pos);
	}
}
void CDemo_ClipView_VCDlg::DrawTestCase(CString xmlPath, CString caseID)
{
	m_stc_drawing.ShowWindow(SW_SHOW);
	m_stc_drawing.SetWindowText("图形绘制中...");
	m_stc_info1.ShowWindow(SW_HIDE);
	m_stc_info2.ShowWindow(SW_HIDE);
	ClearTestCaseData();
	if (!LoadTestCaseData(xmlPath, caseID))
	{
		MessageBox("读取数据失败!","Demo_CliView_VC",MB_OK);
		m_stc_drawing.SetWindowText("");
		return;
	}
	
	/*COLORREF clrLine = RGB(0,255,0);
	vector<Line>::iterator iterLine = lines.begin();
	for (;iterLine != lines.end(); iterLine++)
	{
		DrawLine(*iterLine, clrLine);
	}

	COLORREF clrCircle = RGB(0,0,255);
	vector<Circle>::iterator iterCircle = circles.begin();
	for (;iterCircle != circles.end(); iterCircle++)
	{
		DrawCircle(*iterCircle, clrCircle);
	}*/

	CDC memDC;
	CClientDC dc(this);
	CBitmap memBitmap;
	CBitmap *pOldBit;

	memDC.CreateCompatibleDC(&dc);
	memBitmap.CreateCompatibleBitmap(&dc, CANVAS_WIDTH, CANVAS_HEIGHT);
	pOldBit = memDC.SelectObject(&memBitmap);

	CPen penUse;
	penUse.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	CPen* penOld = memDC.SelectObject(&penUse);

	for(int i=0;i<lines.size();i++)
	{
		DrawLine(lines[i],memDC);
	}

	memDC.SelectObject(penOld);
	penUse.DeleteObject();

	penUse.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	penOld = memDC.SelectObject(&penUse);

	for(int i=0;i<circles.size();i++)
	{
		DrawCircle(circles[i],memDC);
	}
	memDC.SelectObject(penOld);

	dc.BitBlt(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, &memDC, 0, 0, SRCCOPY);

	penUse.DeleteObject();
	memBitmap.DeleteObject();
	memDC.DeleteDC();
	dc.DeleteDC();

	COLORREF clrBoundary = RGB(255,0,0);
	DrawBoundary(boundary, clrBoundary);

	if (hasOutCanvasData)
	{
		m_stc_drawing.SetWindowText("存在超出边界数据！！！");
	}
	else
	{
		m_btn_clip.EnableWindow(TRUE);
		m_stc_drawing.SetWindowText("图形绘制完毕！");
	}
}


void CDemo_ClipView_VCDlg::DrawLine(Line line, CDC &dc)
{
	dc.MoveTo(line.startpoint);
	dc.LineTo(line.endpoint);
}
void CDemo_ClipView_VCDlg::DrawLine(fCPoint &st,fCPoint &ed, CDC &dc)
{
	int dif;
	CPoint a,b;
	a.x  = st.x + eps, b.x = ed.x + eps;
	a.y  = st.y + eps, b.y = ed.y + eps;

	dif = 0;
	if (a.x == b.x)
		dif = dcmp(a.x - b.x);
	a.x += dif;

	dif = 0;
	if (a.y == b.y)
		dif = dif = dcmp(a.y - b.y);
	a.y += dif;

	dc.MoveTo(a);
	dc.LineTo(b);
}
void CDemo_ClipView_VCDlg::DrawCircle(Circle circle, CDC &dc)
{
	dc.Arc(circle.center.x - circle.radius, circle.center.y - circle.radius, circle.center.x + circle.radius, circle.center.y + circle.radius, 0, 0, 0, 0);
}
void CDemo_ClipView_VCDlg::DrawBoundary(Boundary boundary, COLORREF clr)
{
	CClientDC dc(this);
	CPen penUse;
	penUse.CreatePen(PS_SOLID, 1, clr);
	CPen* penOld = dc.SelectObject(&penUse);


	vector<CPoint>::iterator iter = boundary.vertexs.begin();
	if (iter == boundary.vertexs.end()) return;
	CPoint startPoint = *iter;
	dc.MoveTo(startPoint);
	for (;iter != boundary.vertexs.end(); iter++)
	{
		dc.LineTo(*iter);
	}
	dc.LineTo(startPoint);

	dc.SelectObject(penOld);
	penUse.DeleteObject();
}
void CDemo_ClipView_VCDlg::DrawArc(int x1, int y1, int x2, int y2, double x3, double y3, double x4, double y4, CDC &dc)
{
	int dif;
	int xx3 = x3 + eps, xx4 = x4 + eps;
	int yy3 = y3 + eps, yy4 = y4 + eps;

	dif = 0;
	if (xx3 == xx4)
		dif = dcmp(x3 - x4);
	xx3 += dif;

	dif = 0;
	if (yy3 == yy4)
		dif = dif = dcmp(y3 - y4);
	yy3 += dif;

	dc.Arc(x1, y1, x2, y2, xx3, yy3, xx4, yy4);
}

BOOL CDemo_ClipView_VCDlg::GetThreadIdList(CList<int>& tIdList)
{
	int pID = _getpid();
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pID);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	te32.dwSize = sizeof(THREADENTRY32);
	BOOL bGetThread = Thread32First(hThreadSnap, &te32);
	while(bGetThread)
	{
		if (te32.th32OwnerProcessID == pID)
		{
			tIdList.AddTail(te32.th32ThreadID);
		}
		bGetThread = Thread32Next(hThreadSnap, &te32);
	}
	CloseHandle(hThreadSnap);
	return TRUE;
}