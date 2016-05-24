// Documents.cpp : 实现文件
//

#include "stdafx.h"
#include "Shadow.h"
#include "Documents.h"
#include "afxdialogex.h"

#include "msword9.h"

// CDocuments 对话框

IMPLEMENT_DYNAMIC(CDocuments, CDialogEx)

CDocuments::CDocuments(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DOCUMENT_DIALOG, pParent)
{

}

CDocuments::~CDocuments()
{
}

void CDocuments::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDocuments, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDocuments::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CDocuments::OnCancel)
END_MESSAGE_MAP()


// CDocuments 消息处理程序


BOOL CDocuments::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	AfxInitRichEdit();
	//CoInitialize(NULL);
	AfxEnableControlContainer();

	GetDlgItem(IDC_TEXT_EDIT)->MoveWindow(0, 0, 750, 481);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDocuments::OnOK()
{
}


void CDocuments::OnShowText(CString Path)
{
	_Application app;

	//初始化连接
	app.CreateDispatch(_T("word.Application"));

	Documents docs;
	CComVariant a(Path), b(false), c(0), d(true);

	_Document doc;
	docs.AttachDispatch(app.GetDocuments());
	doc.AttachDispatch(docs.Add(&a, &b, &c, &d));
	Range range;

	//求出文档的所选区域
	range = doc.GetContent();//取出文件内容
	CString str;
	str = range.GetText();

	GetDlgItem(IDC_TEXT_EDIT)->SetWindowText(str);

	//关闭
	app.Quit(&b, &c, &c);

	//释放环境
	range.ReleaseDispatch();
	doc.ReleaseDispatch();
	docs.ReleaseDispatch();
	app.ReleaseDispatch();
}


void CDocuments::OnCancel()
{
}
