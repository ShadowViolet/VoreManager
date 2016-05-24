// Manager.cpp : 实现文件
//

#include "stdafx.h"
#include "Shadow.h"
#include "Manager.h"
#include "afxdialogex.h"


// CManager 对话框

IMPLEMENT_DYNAMIC(CManager, CDialogEx)

CManager::CManager(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MANAGER_DIALOG, pParent)
{
	IsRunning = IsFind = FALSE;
	m_hOperate = NULL;
}

CManager::~CManager()
{
}

void CManager::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RESOURCE_LIST, m_Resource_List);
}


BEGIN_MESSAGE_MAP(CManager, CDialogEx)
	ON_BN_CLICKED(IDOK, &CManager::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CManager::OnCancel)
	ON_BN_CLICKED(IDC_ADD_RESOURCE, &CManager::OnAddResource)
	ON_BN_CLICKED(IDC_MODIFY_RESOURCE, &CManager::OnModifyResource)
	ON_BN_CLICKED(IDC_DELETE_RESOURCE, &CManager::OnDeleteResource)
	ON_WM_DROPFILES()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_RESOURCE_LIST, &CManager::OnLvnItemchangedResourceList)
	ON_NOTIFY(NM_CLICK, IDC_RESOURCE_LIST, &CManager::OnNMClickResourceList)
	ON_NOTIFY(NM_RCLICK, IDC_RESOURCE_LIST, &CManager::OnNMRClickResourceList)
END_MESSAGE_MAP()


// CManager 消息处理程序


BOOL CManager::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Resource_List.SetExtendedStyle(LVS_EX_FULLROWSELECT  //允许整行选中
		| LVS_EX_HEADERDRAGDROP				            //允许整列拖动
		| LVS_EX_SUBITEMIMAGES			                //单击选中项
		| LVS_EX_GRIDLINES);				            //画出网格线

	m_Resource_List.SetHeadings(_T("图片,80;名称,100;热度,70;类型,70;路径,200"));
	m_Resource_List.LoadColumnInfo();

	OnRefresh();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CManager::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CManager::OnOK()
{
}


void CManager::OnDropFiles(HDROP hDropInfo)
{
	if (theApp.Class == "Record")
		return;

	CManager::hDropInfo = hDropInfo;
	ParameterType = 1;

	if (m_hOperate == NULL)
		m_hOperate = AfxBeginThread(Operate, this);

	CDialogEx::OnDropFiles(hDropInfo);
}


//用于计算拖放文件夹内的游戏数量
int CManager::CountFile(CString Path)
{
	int count = 0;
	CFileFind finder;
	BOOL working = finder.FindFile(Path + "./*.*");

	while (working)
	{
		working = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())
			count += CountFile(finder.GetFilePath());
		else
		{
			CString Ext = PathFindExtension(finder.GetFilePath());
			if (Ext.MakeLower() != _T(".jpg") && Ext.MakeLower() != _T(".ico") && Ext.MakeLower() != _T(".png") && Ext.MakeLower() != _T(".gif") && Ext.MakeLower() != _T(".bmp") && Ext.MakeLower() != _T(".jpeg")
				&& Ext.MakeLower() != _T(".tif") && Ext.MakeLower() != _T(".pcx") && Ext.MakeLower() != _T(".tga") && Ext.MakeLower() != _T(".psd") && Ext.MakeLower() != _T(".svg") && Ext.MakeLower() != _T(".tiff")
				&& Ext.MakeLower() != _T(".avi") && Ext.MakeLower() != _T(".flv") && Ext.MakeLower() != _T(".mp4") && Ext.MakeLower() != _T(".mpg") && Ext.MakeLower() != _T(".wmv") && Ext.MakeLower() != _T(".mpeg")
				&& Ext.MakeLower() != _T(".mov") && Ext.MakeLower() != _T(".3gp") && Ext.MakeLower() != _T(".ram") && Ext.MakeLower() != _T(".mkv") && Ext.MakeLower() != _T(".vob") && Ext.MakeLower() != _T(".rmvb")
				&& Ext.MakeLower() != _T(".mp3") && Ext.MakeLower() != _T(".aac") && Ext.MakeLower() != _T(".ape") && Ext.MakeLower() != _T(".wmv") && Ext.MakeLower() != _T(".mmf") && Ext.MakeLower() != _T(".flac")
				&& Ext.MakeLower() != _T(".amr") && Ext.MakeLower() != _T(".m4a") && Ext.MakeLower() != _T(".m4r") && Ext.MakeLower() != _T(".ogg") && Ext.MakeLower() != _T(".wav") && Ext.MakeLower() != _T(".blend")
				&& Ext.MakeLower() != _T(".swf") && Ext.MakeLower() != _T(".exe") && Ext.MakeLower() != _T(".doc") && Ext.MakeLower() != _T(".txt") && Ext.MakeLower() != _T(".pdf") && Ext.MakeLower() != _T(".docx"))
				continue;

			count++;
		}
	}
	return count;
}


//拖放文件夹则执行此代码，用于添加文件夹内游戏
void CManager::OnAddFloder(CString Path)
{
	CString Name, Type;
	CFileFind Finder;
	BOOL IsFind = Finder.FindFile(Path + _T("./*.*"));
	while (IsFind)
	{
		IsFind = Finder.FindNextFile();
		Path = Finder.GetFilePath();
		Name = Path.Right(Path.GetLength() - Path.ReverseFind('\\') - 1);
		Type = Name.Right(Name.GetLength() - Name.ReverseFind('.') - 1);

		if (Finder.IsDots())
			continue;
		if (Finder.IsDirectory())
			OnAddFloder(Path);
		else
		{
			CString Ext = PathFindExtension(Path);
			if (Ext.MakeLower() != _T(".jpg") && Ext.MakeLower() != _T(".ico") && Ext.MakeLower() != _T(".png") && Ext.MakeLower() != _T(".gif") && Ext.MakeLower() != _T(".bmp") && Ext.MakeLower() != _T(".jpeg")
				&& Ext.MakeLower() != _T(".tif") && Ext.MakeLower() != _T(".pcx") && Ext.MakeLower() != _T(".tga") && Ext.MakeLower() != _T(".psd") && Ext.MakeLower() != _T(".svg") && Ext.MakeLower() != _T(".tiff")
				&& Ext.MakeLower() != _T(".avi") && Ext.MakeLower() != _T(".flv") && Ext.MakeLower() != _T(".mp4") && Ext.MakeLower() != _T(".mpg") && Ext.MakeLower() != _T(".wmv") && Ext.MakeLower() != _T(".mpeg")
				&& Ext.MakeLower() != _T(".mov") && Ext.MakeLower() != _T(".3gp") && Ext.MakeLower() != _T(".ram") && Ext.MakeLower() != _T(".mkv") && Ext.MakeLower() != _T(".vob") && Ext.MakeLower() != _T(".rmvb")
				&& Ext.MakeLower() != _T(".mp3") && Ext.MakeLower() != _T(".aac") && Ext.MakeLower() != _T(".ape") && Ext.MakeLower() != _T(".wmv") && Ext.MakeLower() != _T(".mmf") && Ext.MakeLower() != _T(".flac")
				&& Ext.MakeLower() != _T(".amr") && Ext.MakeLower() != _T(".m4a") && Ext.MakeLower() != _T(".m4r") && Ext.MakeLower() != _T(".ogg") && Ext.MakeLower() != _T(".wav") && Ext.MakeLower() != _T(".blend")
				&& Ext.MakeLower() != _T(".swf") && Ext.MakeLower() != _T(".exe") && Ext.MakeLower() != _T(".doc") && Ext.MakeLower() != _T(".txt") && Ext.MakeLower() != _T(".pdf") && Ext.MakeLower() != _T(".docx"))
				continue;

			if (OnAddData(Name, Path))
				AfxMessageBox(_T("已跳过重复的资源: " + Name + " !"));

			//theApp.Complete++;
		}
	}
}


//启动游戏线程，读取参数并开始游戏
UINT CManager::FullScreen(LPVOID pParam)
{
	//从配置文件读取模拟器数据
	char Type[MAX_PATH], Emulator[MAX_PATH];
	GetPrivateProfileStringA("Emulator", (CStringA)theApp.Type, NULL, Type, sizeof(Type), "./Config.ini");
	GetPrivateProfileStringA(Type, (CStringA)theApp.Type, NULL, Emulator, sizeof(Emulator), "./Config.ini");

	//处理模拟器数据
	CString EmulatorPath(Emulator), Name = EmulatorPath.Right(EmulatorPath.GetLength() - EmulatorPath.ReverseFind('\\') - 1),
		Directory = EmulatorPath.Left(EmulatorPath.GetLength() - Name.GetLength());

	//检查传入参数
	//theApp.Parameters = ((CManager*)pParam)->ReadParameters();
	if (theApp.Parameters.IsEmpty())
		return FALSE;

	//调用对应模拟器
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.lpDirectory = Directory;
	ShExecInfo.lpFile = Name;
	ShExecInfo.lpParameters = theApp.Parameters;
	ShExecInfo.nShow = SW_SHOW;
	ShellExecuteEx(&ShExecInfo);

	//等待模拟器关闭
	AfxGetApp()->BeginWaitCursor();
	AfxGetApp()->GetMainWnd()->ShowWindow(SW_MINIMIZE);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	AfxGetApp()->GetMainWnd()->ShowWindow(SW_RESTORE);
	AfxGetApp()->EndWaitCursor();

	//主窗口置顶
	::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	if (!GetPrivateProfileIntA("Config", "KeepTop", 1, "./Config.ini"))
		::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	return TRUE;
}


//该方法用于向资源列表添加图片
void CManager::OnSetImg(CString ImgPath)
{
	Bitmap bmp(ImgPath);              //传入图片路径
	Bitmap* pThumbnail = (Bitmap*)bmp.GetThumbnailImage(80, 68, NULL, NULL); //设定缩略图的大小

	HBITMAP hBitmap;
	pThumbnail->GetHBITMAP(Color(255, 255, 255), &hBitmap);
	CBitmap *pImage = CBitmap::FromHandle(hBitmap);                 //转换成CBitmap格式位图

	m_Imagelist.Add(pImage, RGB(0, 0, 0));

	// 设置CImageList图像列表与CListCtrl控件关联 LVSIL_SMALL小图标列表
	m_Resource_List.SetImageList(&m_Imagelist, LVSIL_SMALL);
}


//该方法用于向游戏数据库添加游戏数据
BOOL CManager::OnAddData(CString Name, CString Path)
{
	if (Path.Replace(_T("'"), _T("")))
	{
		CString ErrMsg;
		ErrMsg.Format(_T("资源路径: %s 不可以带有单引号!"), Path);

		AfxMessageBox(ErrMsg);
		return FALSE;
	}
	sqlite3_prepare(theApp.db, "select * from " + (CStringA)theApp.Class + " where Path = '" + (CStringA)Path + "' and Type = '" + (CStringA)theApp.Type + "' ", -1, &theApp.stmt, NULL);
	if (sqlite3_step(theApp.stmt) != SQLITE_ROW)
	{
		int rc = sqlite3_exec(theApp.db, "INSERT INTO " + (CStringA)theApp.Class + " VALUES( '" + (CStringA)Path + "', '" + (CStringA)Name + "', '" + _T("1 ☆") + "' , '" + (CStringA)theApp.Type + "','" + (CStringA)Path + "')", NULL, NULL, &theApp.errMsg);
		if (rc)
		{
			MessageBox((CString)theApp.errMsg);
			return 1;
		}
		else
			IsFind = 0;
	}
	else
		IsFind = 1;

	sqlite3_finalize(theApp.stmt);

	return IsFind;
}


//启动工作者线程并刷新游戏列表
void CManager::OnRefresh()
{
	ParameterType = 0;
	if (m_hOperate == NULL)
		m_hOperate = AfxBeginThread(Operate, this);
}


//搜索本地游戏数据库中的游戏数据
void CManager::OnSearch()
{
	//CString SearchText;
	//GetDlgItem(IDC_SEARCH_TEXT)->GetWindowText(SearchText);

	//sqlite3_prepare(theApp.db, "select * from " + (CStringA)theApp.Class + " where Name like '%" + (CStringA)SearchText + "%' and Type = '" + (CStringA)theApp.Type + "'", -1, &stmt, NULL);
	//m_Games_List.DeleteAllItems();
	//m_Imagelist.Create(75, 73, ILC_COLORDDB | ILC_COLOR32, 0, 1);   //创建图像序列CImageList对象

	//while (sqlite3_step(stmt) == SQLITE_ROW)
	//{
	//	OnSetImg((CString)sqlite3_column_text(stmt, 0));
	//	m_Games_List.AddItem((CString)sqlite3_column_text(stmt, 0), (CString)sqlite3_column_text(stmt, 1), (CString)sqlite3_column_text(stmt, 2), (CString)sqlite3_column_text(stmt, 3), (CString)sqlite3_column_text(stmt, 4));
	//}

	//sqlite3_finalize(stmt);
}


//工作者线程，执行所有耗时操作
UINT CManager::Operate(LPVOID lpParameter)
{
	CManager *pDlg = (CManager*)lpParameter;
	if (pDlg->IsRunning)
		return FALSE;

	pDlg->IsRunning = TRUE;
	switch (pDlg->ParameterType)
	{
	case 0: //刷新游戏列表
	{
	Refresh:
		/*char * errMsg;
		char **dbResult;
		int nRow, nColumn;*/

		/*if (sqlite3_get_table(theApp.db, (CStringA)Sql, &dbResult, &nRow, &nColumn, &errMsg) == SQLITE_OK)
		{
			theApp.Complete = 0;
			theApp.Total = nRow;
			if (theApp.m_hTip == NULL)
				theApp.m_hTip = AfxBeginThread(theApp.OperateTip, (LPVOID)_T("刷新ROM数据"));

			sqlite3_free_table(dbResult);
		}*/

		if (sqlite3_prepare(theApp.db, "select * from " + (CStringA)theApp.Class + " where Type = '" + (CStringA)theApp.Type + "'", -1, &theApp.stmt, NULL) == SQLITE_OK)
		{
			pDlg->m_Resource_List.DeleteAllItems();
			pDlg->m_Imagelist.Create(80, 68, ILC_COLORDDB | ILC_COLOR32, 0, 1);   //创建图像序列CImageList对象

			while (sqlite3_step(theApp.stmt) == SQLITE_ROW)
			{
				pDlg->OnSetImg((CString)sqlite3_column_text(theApp.stmt, 0));
				pDlg->m_Resource_List.AddItem((CString)sqlite3_column_text(theApp.stmt, 0), (CString)sqlite3_column_text(theApp.stmt, 1), (CString)sqlite3_column_text(theApp.stmt, 2), (CString)sqlite3_column_text(theApp.stmt, 3), (CString)sqlite3_column_text(theApp.stmt, 4));
				//theApp.Complete++;
			}

			sqlite3_finalize(theApp.stmt);
		}
	}break;

	case 1: //拖放文件或文件夹
	{
		char FilePath[300];
		CString Path, Name;
		UINT count = DragQueryFile(pDlg->hDropInfo, 0xFFFFFFFF, NULL, 0);
		if (count)
		{
			//for (UINT i = 0; i < count; i++)
			//{
			//	theApp.Complete = theApp.Total = 0;
			//	int pathLen = DragQueryFileA(pDlg->hDropInfo, i, FilePath, sizeof(FilePath));
			//	Path = FilePath;

			//	CFileStatus Status;
			//	if (CFile::GetStatus(Path, Status))
			//	{
			//		if ((Status.m_attribute & 0x10) == 0x10)//文件夹
			//		{
			//			theApp.Total = pDlg->CountFile(Path);
			//			continue;
			//		}
			//		else
			//		{
			//			CString Ext = PathFindExtension(Path);
			//			if (Ext.MakeLower() != _T(".jpg") && Ext.MakeLower() != _T(".jpeg") && Ext.MakeLower() != _T(".png") && Ext.MakeLower() != _T(".gif") && Ext.MakeLower() != _T(".bmp") && Ext.MakeLower() != _T(".ico")
			//				&& Ext.MakeLower() != _T(".mp4") && Ext.MakeLower() != _T(".flv") && Ext.MakeLower() != _T(".avi") && Ext.MakeLower() != _T(".zip") && Ext.MakeLower() != _T(".7z")  && Ext.MakeLower() != _T(".gba"))
			//				continue;

			//			theApp.Total++;
			//		}
			//	}


			/*if (theApp.m_hTip == NULL)
				theApp.m_hTip = AfxBeginThread(theApp.OperateTip, (LPVOID)_T("添加游戏ROM"));*/

			for (UINT i = 0; i < count; i++)
			{
				int pathLen = DragQueryFileA(pDlg->hDropInfo, i, FilePath, sizeof(FilePath));
				Path = FilePath;
				Name = Path.Right(Path.GetLength() - Path.ReverseFind('\\') - 1);

				CFileStatus Status;
				if (CFile::GetStatus(Path, Status))
				{
					if ((Status.m_attribute & 0x10) == 0x10)
					{
						pDlg->OnAddFloder(Path);
						continue;
					}
				}

				CString Ext = PathFindExtension(Path);
				if (Ext.MakeLower()    != _T(".jpg") && Ext.MakeLower() != _T(".ico") && Ext.MakeLower() != _T(".png") && Ext.MakeLower() != _T(".gif") && Ext.MakeLower() != _T(".bmp") && Ext.MakeLower() != _T(".jpeg")
					&& Ext.MakeLower() != _T(".tif") && Ext.MakeLower() != _T(".pcx") && Ext.MakeLower() != _T(".tga") && Ext.MakeLower() != _T(".psd") && Ext.MakeLower() != _T(".svg") && Ext.MakeLower() != _T(".tiff")
					&& Ext.MakeLower() != _T(".avi") && Ext.MakeLower() != _T(".flv") && Ext.MakeLower() != _T(".mp4") && Ext.MakeLower() != _T(".mpg") && Ext.MakeLower() != _T(".wmv") && Ext.MakeLower() != _T(".mpeg")
					&& Ext.MakeLower() != _T(".mov") && Ext.MakeLower() != _T(".3gp") && Ext.MakeLower() != _T(".ram") && Ext.MakeLower() != _T(".mkv") && Ext.MakeLower() != _T(".vob") && Ext.MakeLower() != _T(".rmvb") 
					&& Ext.MakeLower() != _T(".mp3") && Ext.MakeLower() != _T(".aac") && Ext.MakeLower() != _T(".ape") && Ext.MakeLower() != _T(".wmv") && Ext.MakeLower() != _T(".mmf") && Ext.MakeLower() != _T(".flac") 
					&& Ext.MakeLower() != _T(".amr") && Ext.MakeLower() != _T(".m4a") && Ext.MakeLower() != _T(".m4r") && Ext.MakeLower() != _T(".ogg") && Ext.MakeLower() != _T(".wav") && Ext.MakeLower() != _T(".blend")
					&& Ext.MakeLower() != _T(".swf") && Ext.MakeLower() != _T(".exe") && Ext.MakeLower() != _T(".doc") && Ext.MakeLower() != _T(".txt") && Ext.MakeLower() != _T(".pdf") && Ext.MakeLower() != _T(".docx"))
					continue;

				if (pDlg->OnAddData(Name, Path))
					AfxMessageBox(_T("已跳过重复的资源: " + Name + " !"));
			}

			if (count > 0)
				goto Refresh;

			//theApp.Complete++;
			/*if (theApp.Total > 0)
				goto Refresh;*/
		}

		DragFinish(pDlg->hDropInfo);
	}break;

	case 2: //添加游戏
	{
		CFileDialog FileDlg(TRUE, NULL, NULL, OFN_NOCHANGEDIR | OFN_ENABLEHOOK | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING,
			_T("游戏资源(*.*)|*.cdi;*.mds;*.nrg;*.gdi;*.chd;*.nes;*.fds;*.nsf;*.zip;*.7z;*.smc;*.smd;*.gen;*.zsg;*.32x;*.raw;*.gba;*.agb;*.bin;*.elf;*.mb;*.z;*.gz;*.iso;*.sms;*.sg;*.sc;*.mv;*.n64;*.rom;*.usa;*.jap;*.pal;*.nds;*.ds.gba;*.srl;*.rar;*.dol;*.gcm;*.ciso;.gcz;*.wad;*.ngp;*.ngc;*.npc;*.pce;*.hes;*.mdf;*.img;*.dump;*.cso;*.pdp;*.prx;*.ws;*.wsc;*.wbfs;*.fdi;*.a26;*.gb;*.gbc;||"), pDlg);
		char fileBuffer[900000] = { 0 };
		FileDlg.m_ofn.lpstrFile = (LPWSTR)fileBuffer;
		FileDlg.m_ofn.nMaxFile = 50000;
		if (FileDlg.DoModal() != IDOK)
		{
			pDlg->m_hOperate = NULL;
			pDlg->IsRunning = FALSE;
			return FALSE;
		}

		//theApp.Complete = theApp.Total = 0;
		POSITION Pos = FileDlg.GetStartPosition();
		while (Pos)
		{
			FileDlg.GetNextPathName(Pos);
			//theApp.Total++;
		}

		/*if (theApp.m_hTip == NULL)
			theApp.m_hTip = AfxBeginThread(theApp.OperateTip, (LPVOID)_T("添加游戏ROM"));*/

		Pos = FileDlg.GetStartPosition();
		while (Pos)
		{
			CString Path = FileDlg.GetNextPathName(Pos);
			CString Name = Path.Right(Path.GetLength() - Path.ReverseFind('\\') - 1);

			if (pDlg->OnAddData(Name, Path))
				AfxMessageBox(_T("已跳过重复的资源: " + Name + " !"));
		}

		/*if (theApp.Total > 0)
			goto Refresh;*/
	}break;

	case 3: //删除游戏
	{
		int Count = pDlg->m_Resource_List.GetSelectedCount();
		//theApp.Complete = 0;
		//theApp.Total = Count;

		/*if (theApp.m_hTip == NULL)
			theApp.m_hTip = AfxBeginThread(theApp.OperateTip, (LPVOID)_T("删除游戏ROM"));*/

		for (int i = 0; i < Count; i++)
		{
			int Row = pDlg->m_Resource_List.GetNextItem(i - 1, LVIS_SELECTED);
			CString Path = pDlg->m_Resource_List.GetItemText(Row, 4);
			
			//theApp.Complete++;

			sqlite3_exec(theApp.db, "delete From " + (CStringA)theApp.Class + " where Path = '" + (CStringA)Path + "' and Type = '" + (CStringA)theApp.Type + "' ", NULL, NULL, &theApp.errMsg);
		}

		if (Count > 0)
			goto Refresh;
	}break;

	default:
		break;
	}

	pDlg->m_hOperate = NULL;
	pDlg->IsRunning = FALSE;
	return TRUE;
}


void CManager::OnAddResource()
{
	ParameterType = 2;
	if (m_hOperate == NULL)
		m_hOperate = AfxBeginThread(Operate, this);
}


void CManager::OnModifyResource()
{
	
}


void CManager::OnDeleteResource()
{
	ParameterType = 3;
	if (m_hOperate == NULL)
		m_hOperate = AfxBeginThread(Operate, this);
}


void CManager::OnLvnItemchangedResourceList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	CString Path = m_Resource_List.GetItemText(m_Resource_List.GetNextItem(-1, LVIS_SELECTED), 4);
	if (!Path.IsEmpty() && m_Resource_List.GetNextItem(-1, LVIS_SELECTED) != -1)
		::SendMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), WM_CHILDMESSAGE, theApp.ChildType, *(LPARAM*)&Path);

	*pResult = 0;
}


void CManager::OnNMClickResourceList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
}


void CManager::OnNMRClickResourceList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CManager::OnCancel()
{
}
