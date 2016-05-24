// Documents.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Shadow.h"
#include "Documents.h"
#include "afxdialogex.h"

#include "msword9.h"

// CDocuments �Ի���

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


// CDocuments ��Ϣ�������


BOOL CDocuments::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	AfxInitRichEdit();
	//CoInitialize(NULL);
	AfxEnableControlContainer();

	GetDlgItem(IDC_TEXT_EDIT)->MoveWindow(0, 0, 750, 481);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CDocuments::OnOK()
{
}


void CDocuments::OnShowText(CString Path)
{
	_Application app;

	//��ʼ������
	app.CreateDispatch(_T("word.Application"));

	Documents docs;
	CComVariant a(Path), b(false), c(0), d(true);

	_Document doc;
	docs.AttachDispatch(app.GetDocuments());
	doc.AttachDispatch(docs.Add(&a, &b, &c, &d));
	Range range;

	//����ĵ�����ѡ����
	range = doc.GetContent();//ȡ���ļ�����
	CString str;
	str = range.GetText();

	GetDlgItem(IDC_TEXT_EDIT)->SetWindowText(str);

	//�ر�
	app.Quit(&b, &c, &c);

	//�ͷŻ���
	range.ReleaseDispatch();
	doc.ReleaseDispatch();
	docs.ReleaseDispatch();
	app.ReleaseDispatch();
}


void CDocuments::OnCancel()
{
}
