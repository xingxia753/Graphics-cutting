
// Demo_ClipView_VCDlg.h : 头文件
//
#pragma once
#include <vector>
using std::vector;
#include "pugiconfig.hpp"
#include "pugixml.hpp"
#include "GraphicEntity.h"
#include <time.h>
#include <Windows.h>
#include <queue>
#include <Psapi.h>
#include "afxwin.h"
#pragma comment(lib,"psapi.lib")

// CDemo_ClipView_VCDlg 对话框
class CDemo_ClipView_VCDlg : public CDialogEx
{
// 构造
public:
	CDemo_ClipView_VCDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DEMO_CLIPVIEW_VC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CButton m_btn_clip;
	CStatic m_stc_drawing;
	CStatic m_stc_info1;
	CStatic m_stc_info2;
	afx_msg void OnBnClickedBtnClip();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	virtual virtual virtual virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) final final final;

private:
	CString curPath;
	double startTime;
	double endTime;
	BOOL hasOutCanvasData;
	CList<int> beginTIdList;

	void ClearTestCaseData();
	BOOL LoadTestCaseData(CString xmlPath, CString caseID);
	void DrawTestCase(CString xmlPath, CString caseID);
	void BeginMonitor();
	void EndMonitor();
	BOOL XmlNodeToPoint(pugi::xml_node node, CPoint& piont);
	int  SplitCStringToArray(CString str,CStringArray& strArray, CString split);
	BOOL IsPointOutCanvas(CPoint point);
	BOOL IsLineOutCanvas(Line line);
	BOOL IsCircleOutCanvas(Circle circle);
	BOOL IsBoundaryOutCanvas(Boundary boundary);
	BOOL GetThreadIdList(CList<int>& tIdList);

	int isOnLine(CPoint &p, CPoint &st, CPoint &ed);
	int isOnLine(fCPoint &p, CPoint &st, CPoint &ed);
	int intersection_num;
	int outside_boundary;
	int intersect_boundary;
	int inside_boundary;
	int overlapping_boundary;

	CPoint top;
	CPoint bottom;
	CPoint left;
	CPoint right;

	struct cmp_Line;
	struct cmp_Circle;

public:
	vector<Line> lines;
	vector<Circle> circles;
	Boundary boundary;
	void DrawLine(Line line, CDC &dc);
	void DrawLine(fCPoint &st,fCPoint &ed, CDC &dc);
	void DrawCircle(Circle circle,CDC &dc);
	void DrawBoundary(Boundary boundary, COLORREF clr);
	void DrawArc(int x1, int y1, int x2, int y2, double x3, double y3, double x4, double y4, CDC &dc);

	void DrawLine_Excute(CDC &memDC,COLORREF clr);
	void DrawCircle_Excute(CDC &memDC,COLORREF clr);


};
