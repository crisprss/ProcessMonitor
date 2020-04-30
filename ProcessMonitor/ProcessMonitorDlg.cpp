
// ProcessMonitorDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ProcessMonitor.h"
#include "ProcessMonitorDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
	
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CProcessMonitorDlg 对话框



CProcessMonitorDlg::CProcessMonitorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROCESSMONITOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(CProcessMonitorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CProcessMonitorDlg::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_BT_Refresh, &CProcessMonitorDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BT_closeProcess, &CProcessMonitorDlg::OnBnClickedBtcloseprocess)
END_MESSAGE_MAP()


// CProcessMonitorDlg 消息处理程序

BOOL CProcessMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_list.InsertColumn(0, _T("编号"), LVCFMT_LEFT, 150); //LVCFMT_LEFT表示左对齐
	m_list.InsertColumn(0, _T("进程名"), LVCFMT_LEFT, 250);
	m_list.InsertColumn(0, _T("PID"), LVCFMT_LEFT, 150);
	m_list.SetExtendedStyle(LVS_EX_GRIDLINES);
	m_list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	BrowserTravle();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CProcessMonitorDlg::BrowserTravle()
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return;
	}
	int i = 0;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	BOOL bMore = Process32First(hProcessSnap, &pe32);
	CString str;
	while (bMore)
	{
		str.Format(_T("%d"), i+1);
		m_list.InsertItem(i, str);
		m_list.SetItemText(i, 1, pe32.szExeFile);
		str.Format(_T("%d"), pe32.th32ProcessID);
		m_list.SetItemText(i, 2, str);
		bMore = Process32Next(hProcessSnap, &pe32);
		i++;
	}

}







void CProcessMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CProcessMonitorDlg::OnPaint()
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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CProcessMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CProcessMonitorDlg::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CProcessMonitorDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通5知处理程序代码
	m_list.DeleteAllItems();
	BrowserTravle();
}

//结束进程
void CProcessMonitorDlg::OnBnClickedBtcloseprocess()
{
	int table = m_list.GetSelectionMark();
	if (table < 0)
		return;
	CString strPID =  m_list.GetItemText(table, 2);
	DWORD dwPID = _wtoi(strPID);

	//打开进程
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (hProcess == NULL)
	{
		MessageBox(L"打开进程失败");
		return;
	}
	//关闭进程
	TerminateProcess(hProcess, 0);
	Sleep(1000); 
	OnBnClickedButton1();
}
