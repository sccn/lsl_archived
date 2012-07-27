//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "U3dHotspots.h"
#include "UParseLine.h"
//---------------------------------------------------------------------------

#include "GetMMFId.h"

//#include "DataSuite.h"
//#include "DataRiver.h"
#include "DataArraysx.h"

#pragma package(smart_init)
#pragma resource "*.dfm"
TForm11 *Form11;
//---------------------------------------------------------------------------
__fastcall TForm11::TForm11(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

#include "UEdit3D.h"
HANDLE handleRd1 = 0;
HANDLE handleWr1 = 0;

extern AnsiString par;

#define MAXHOTSPOTS 20
class TPtrShape: public TShape
{
public:
	THotspot * p[2];
	TPtrShape(void * AOwner): TShape((TComponent *)AOwner) {p[0]=0;p[1]=0;};
};

TPtrShape * _3DHotspots[MAXHOTSPOTS] = {
	0,0,0,0,0,
	0,0,0,0,0,
	0,0,0,0,0,
	0,0,0,0,0
};

int nHotspots=0;
TMaxArray MaxArrayCapture;

HANDLE hMutex;
HANDLE hCapture;
HANDLE hCaptureDone;

typedef TDataArray<MAXHOTSPOTS+1> THotspotSample;   //EvChannel (at 0)
THotspotSample HotspotSample;


int LowPass(int bufsize, TMaxArray   * pNewSample)
{

	static int oldbufsize=bufsize;
	static __int64 cumulBuf[400]; // need 289
	static __int64 n=-1;
	if (oldbufsize != bufsize) n = -1;
	if (n==-1)
	{
		memset(cumulBuf,0,sizeof cumulBuf);
		n=0;
	}

	static TMaxArray  buf[100];

	if (bufsize==1) return pNewSample->nItems-1;

	int indexin = n%bufsize;
	for (int i=1;i<pNewSample->nItems;i++)   // code is at 0
	{
		buf[indexin][i] = (*pNewSample)[i]/bufsize;
		cumulBuf[i] += buf[indexin][i];
	}

	n++;
	int indexout = n%bufsize;
	int nch = 0;
	for (int i=1;i<pNewSample->nItems;i++)   // code is at 0
	{
		nch++;
		if (n>bufsize)
			cumulBuf[i] -= buf[indexout][i];
		(*pNewSample)[i]   = cumulBuf[i];
	}
	oldbufsize = bufsize;
	return nch;
};

void DoTrackBars(TMaxArray * pMA)
{
	Form11->ProgressBar2-> Position = (*pMA)[1];
	Form11->ProgressBar3-> Position = (*pMA)[2];
	Form11->ProgressBar4-> Position = (*pMA)[3];
}


#include "drlib.h"
//static int tail;

#include "UListen.h"
TListenThread *thListen;
TQueue * pCodeQ = 0;


#include <XMLDoc.hpp>
#include "UXML.h"

enum HS_TYPE {IS_UNDEF,IS_TOUCH,IS_DIR,IS_POINTTO};

void UpdateInfo()
{
		HS_TYPE * hsTemp;
		hsTemp  = (HS_TYPE *)malloc(sizeof(*hsTemp)*nHotspots);
		int n = nHotspots;
		for (int i = 0; i< n ; i++)
		{
			TPtrShape * pS = _3DHotspots[i];
			THotspot * pHs = pS->p[1];
			bool isTouch = pHs->sensor[1]<0;    // no "to" sensor
			if (isTouch)
				hsTemp[i] = IS_TOUCH;
			else // is point-to or dist
			{
				bool isDir = pHs->dist_mm > 0;
				if (isDir)
					hsTemp[i] = IS_DIR;
				else
					hsTemp[i] = IS_POINTTO;
			}
		}


	TXMLDocument * xml =CreateXML(Form11);
	int nitems = n+1;
	for (int i = 0; i< n ; i++)
	switch (hsTemp[i])
	{
		case IS_TOUCH:
		{
			TPtrShape * pS = _3DHotspots[i];
			THotspot * pHs = pS->p[1];
			int sensor = pHs->sensor[0];
			AddHeaderNode		(xml,handleWr1,"data","touch"+ IntToStr(i));       	// create atributes
			SetHeaderAttribute(xml,handleWr1,"data","touch"+ IntToStr(i),"sensor", sensor);       	// create atributes
			int r = pHs->r;
			SetHeaderAttribute(xml,handleWr1,"data","touch"+ IntToStr(i),"r", r);       	// create atributes
			int code = pHs->value;
			SetHeaderAttribute(xml,handleWr1,"data","touch"+ IntToStr(i),"ev.code", code);       	// create atributes
		}
		break;
		case IS_DIR:
			AddHeaderNode		(xml,handleWr1,"data","dir");//+IntToStr(i));       	// create atributes
		break;
		case IS_POINTTO:
			AddHeaderNode		(xml,handleWr1,"data","pointto");//+IntToStr(i));       	// create atributes
		break;

	default:
		;
	}
	SetHeaderAttribute(xml,handleWr1,"","data_packet","bytes", nitems*4 +128);       	// create atributes
	SetHeaderAttribute(xml,handleWr1,"data_packet","sample_data","bytes", nitems*4);       	// create atributes
	SetHeaderAttribute(xml,handleWr1,"data_packet","sample_data","items", nitems);       	// create atributes

	SetHeaderAttribute(xml,handleWr1,"sample_data","stream","name", "hotspots");       	// create atributes
	SetHeaderAttribute(xml,handleWr1,"sample_data","stream","items", nitems);       	// create atributes
	SetHeaderAttribute(xml,handleWr1,"stream","data","items", nitems-1);       	// create atributes

	if (FileExists("/tmp/hotspots.xml"))
		DeleteFile("/tmp/hotspots.xml");
	WriteXMLtoFile(handleWr1,"/tmp/hotspots.xml");
	delete xml;



	free(hsTemp);
}



void ProcessData(char * data, int size)
{
	if (!pCodeQ) pCodeQ = new TQueue;
	static int count =0;
	int res = WaitForSingleObject(hMutex,100);
	if (res!=WAIT_OBJECT_0) return;
	__try
	{
		HotspotSample.nItems = nHotspots +1;
		TMaxArray MaxArray;
		memcpy(&MaxArray, data,size);
		int nch = LowPass(Form11->UpDown1->Position, &MaxArray);
		count++;
		if ((count % 10) ==0)
			DoTrackBars(&MaxArray);

		HotspotSample.TimeStampOrig = MaxArray.TimeStampOrig ;
		HotspotSample.TimeStampDest = MaxArray.TimeStampDest;
		int resc = WaitForSingleObject(hCapture,0);
		if (resc==WAIT_OBJECT_0) //signaled
		{
			MaxArrayCapture = MaxArray;
			ResetEvent(hCapture);
			SetEvent(hCaptureDone);
			return;
		}
		static bool is_changed[MAXHOTSPOTS] = {
					0,0,0,0,0,
					0,0,0,0,0,
					0,0,0,0,0,
					0,0,0,0,0
					};
		static int t_unchgd[MAXHOTSPOTS] = {
					0,0,0,0,0,
					0,0,0,0,0,
					0,0,0,0,0,
					0,0,0,0,0
					};
		int codes[MAXHOTSPOTS] = {
					0,0,0,0,0,
					0,0,0,0,0,
					0,0,0,0,0,
					0,0,0,0,0
					};
		bool is_in[MAXHOTSPOTS] = {
					0,0,0,0,0,
					0,0,0,0,0,
					0,0,0,0,0,
					0,0,0,0,0
					};
		HotspotSample[0] = 0;
		for (int i = 0; i< nHotspots; i++)
		{
			TPtrShape * pS = _3DHotspots[i];
			THotspot * pHs = pS->p[1];
			int ch1 = 1 + pHs->sensor[0]*3;
			bool isTouch = pHs->sensor[1]<0;    // no "to" sensor
			bool in;
			if (isTouch)
			{
				TPoint3D pt3D(MaxArray[ch1],MaxArray[ch1+1],MaxArray[ch1+2]);
				float f = pHs->Distance(pt3D);
				HotspotSample[i+1] = f;
				in = pHs->Includes(pt3D);
			}
			else // is point-to or dist
			{
				bool isDir = pHs->dist_mm > 0;
				if (isDir)
				{
					TPoint3D pt1(MaxArray[ch1],MaxArray[ch1+1],MaxArray[ch1+2]);
					int ch2 = 1 + pHs->sensor[1]*3;
					TPoint3D pt2(MaxArray[ch2],MaxArray[ch2+1],MaxArray[ch2+2]);
					int sz = pHs->r;
					TVectorToHotspot PointingTo(pt1,pt2,sz );
					TVectorToHotspot TargetIs(pt1,*pHs,sz);
					float deg = PointingTo.Distance(TargetIs);
					HotspotSample[i+1] = deg*10;
					in =   PointingTo.Includes(TargetIs);
				}
				else
				{	// isPointTo:
					TPoint3D pt1(MaxArray[ch1],MaxArray[ch1+1],MaxArray[ch1+2]);
					int ch2 = 1 + pHs->sensor[1]*3;
					TPoint3D pt2(MaxArray[ch2],MaxArray[ch2+1],MaxArray[ch2+2]);
					int sz = pHs->r;
					TVectorToHotspot PointingTo(pt1,pt2,sz );
					TVectorToHotspot TargetIs(pt1,*pHs,sz);
					float deg = PointingTo.Distance(TargetIs);
					HotspotSample[i+1] = deg*10;
					in =   PointingTo.Includes(TargetIs);
				}
			}
			if (in)
			{
				pS->Brush->Color = clGreen;
				codes[i] = pHs->value;
			}
			else
				pS->Brush->Color = clRed;
			is_changed[i] = is_in[i]!=in;
			is_in[i] = in;
			if (is_changed[i])
			{
				if (t_unchgd[i]>4 && is_in[i]) // only moving into the hotspot after being out for 4 samples!
				{
					HotspotSample[0] = codes[i];
					Form11->Memo1->Lines->Add(codes[i]);
				}
				t_unchgd[i] = 0;
			}
			else
			{
				t_unchgd[i]++;  // count time unchanged
			}
		}
		ds_Write(handleWr1,(char *)&HotspotSample, sizeof HotspotSample);
		int PosIn =  ds_TailPos(handleRd1)/40.96;// pp->GetPosition(pH->TailN ); // -1:head

		Form11->ProgressBar1->Position = PosIn;
	}
	__finally
	{
		ReleaseMutex(hMutex);
	}
}

//class TListenThread: public TThread
//{
//public:
//	HANDLE hRd;
//	int Tail;
//		TListenThread(HANDLE hr, int tn);
//		__fastcall ~TListenThread();
//		void __fastcall Execute();
//};
//
//TListenThread::TListenThread(HANDLE hr, int tn):TThread(true)
//{
//		FreeOnTerminate=false;
//		hRd=hr;
//		Tail = tn;
//};
//
//__fastcall TListenThread::~TListenThread()
//{
//	int zz = 0;
//};
//
//char buf[65536];
//void __fastcall TListenThread::Execute()
//{
//	while (!Terminated)
//	{
//		int posr = ds_NodesToRead(hRd);
//		if (posr)
//		{
//			int sz=ds_Read(hRd,buf);
//			ProcessData(buf,sz);
//		}
//	}
//}


#include "drlib.h"
void __fastcall TForm11::FormCreate(TObject *Sender)
{
	bool ok = InitDRlib();
	hCapture = CreateEvent(0,true,false,0);// InitEvent(0,false); // manual, non-signaled
	hCaptureDone = CreateEvent(0,true,false,0);//InitEvent(0,false); // manual, non-signaled
	hMutex = CreateMutex(0,false,0);
	TStringList * pStrPar = ParseCmdLine(par,' ');
	int nPar = pStrPar->Count;
	bool bMMF = false;
	bool bIndy = false;
	Timer1->Enabled = true;

//	if (nPar) {
//		LabeledEdit1->Text = pStrPar->Strings[0];
//	}
//	else
//	{
//		InitializeMMFReadForm(""); //
//		MMFReadShowModal();
//		LabeledEdit1->Text = GetMMFBufferID();
//
//		int n = FindMmfBuffer();
//		if (n!=ComboBox1->Items->Count)
//		{
//			ComboBox1->Items->Clear();
//			for (int i = 0; i <n; i++)
//				ComboBox1->Items->Append(DirMMF[i]);
//		}
//	}
//	AnsiString s = LabeledEdit1->Text;
//	handleRd1 = ds_OpenRead(s.c_str());
//	thListen = new TListenThread(handleRd1,ProcessData);
//

//
//	PageControl1->ActivePageIndex=0;
//	thListen->Resume();
}
//------------------------------------------                                        ---------------------------------
#include "UEdit3D.h"

class TPtrButton: public TButton
{
public:
	void * p;
	TPtrButton(void * PWin): TButton(PWin) {p=0;};
};


void __fastcall TForm11::FormDestroy(TObject *Sender)
{
	ds_CloseRead(handleRd1);
	CloseHandle(hMutex);
	CloseHandle(hCapture);

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void __fastcall TForm11::GridPanel1Click(TObject *Sender)
{
	TPtrButton * pC = (TPtrButton *) Sender;
	AnsiString txt = pC->Name;
	bool remove = txt.SubString(0,1)=="r";
	bool touch  = txt.SubString(0,1)=="t";
	bool point  = txt.SubString(0,1)=="p";
	bool dir    = txt.SubString(0,1)=="d";

	if (remove)
	{
		int cc1 = GridPanel1->ControlCollection->Count ;
		int z;
		for (z=0;z<cc1;z++)
		{
			TControl * pC = GridPanel1->Controls[z];
			if (pC->Name == txt)
				break;
		}
		GridPanel1->Height -= pC->Height;
		for (int i = 0; i <GridPanel1->ColumnCollection->Count; i++)
		{
			TControl * pC = GridPanel1->Controls[z];
			GridPanel1->RemoveControl(pC);
		}
	}
	else
	if (touch)
	{
		TPoint3D * p3D = (TPoint3D *)pC->p;
		THotspot * pHs = new THotspot(
				p3D->x,
				p3D->y,
				p3D->z,
				p3D->name,
				10, //size
				0,	//value
				1	//sensor
				);


		TPtrShape * pSh = new TPtrShape(GridPanel2);
		pSh->Name = "Tsh" + p3D->name;
		pSh->p[0] = pHs;
		pSh->Shape = stCircle;
		pSh->Brush->Color=clRed;
		pSh->Height = 12;
		pSh->Parent =  GridPanel2;

		TEdit * pE =new TEdit(GridPanel2);
		pE->Name = "Te1" + p3D->name;
		GridPanel2->Height += pE->Height+2;
		pE->Text = pHs->name;
		pE->Parent = GridPanel2;

		pE =new TEdit(GridPanel2);
		pE->Name = "Te2" + p3D->name;
		pE->Text = pHs->r;
		pE->Parent = GridPanel2;

		pE =new TEdit(GridPanel2);
		pE->Name = "Te3" + p3D->name;
		pE->Text = pHs->sensor[0];
		pE->Parent = GridPanel2;

		pE =new TEdit(GridPanel2);
		pE->Name = "Te4" + p3D->name;
		pE->Text = pHs->value;
		pE->Parent = GridPanel2;
		int zz=0;
	}
	else
	if (point)
	{
		TPoint3D * p3D = (TPoint3D *)pC->p;
		THotspot * pHs = new THotspot(
				p3D->x,
				p3D->y,
				p3D->z,
				p3D->name,
				10, //size
				0,	//value
				1,	//sensor1
				2	//sensor2
				);
		TPtrShape * pSh = new TPtrShape(GridPanel3);
		pSh->p[0] = pHs;
		pSh->Name = "Psh" + p3D->name;
		pSh->Shape = stCircle;
		pSh->Brush->Color=clRed;
		pSh->Height = 12;
		pSh->Parent =  GridPanel3;


		TEdit * pE =new TEdit(GridPanel3);
		GridPanel3->Height += pE->Height+2;
		pE->Name = "Pe1" + p3D->name;
		pE->Text = pHs->name;
		pE->Parent = GridPanel3;

		pE =new TEdit(GridPanel3);
		pE->Name = "Pe2" + p3D->name;
		pE->Text = pHs->r;
		pE->Parent = GridPanel3;

		pE =new TEdit(GridPanel3);
		pE->Name = "Pe3" + p3D->name;
		pE->Text = pHs->sensor[0];
		pE->Parent = GridPanel3;

		pE =new TEdit(GridPanel3);
		pE->Name = "Pe4" + p3D->name;
		pE->Text = pHs->sensor[1];
		pE->Parent = GridPanel3;

		pE =new TEdit(GridPanel3);
		pE->Name = "Pe5" + p3D->name;
		pE->Text = pHs->value;
		pE->Parent = GridPanel3;
		int zz=0;
	}
	else
	if (dir)
	{
		TPoint3D * p3D = (TPoint3D *)pC->p;
		THotspot * pHs = new THotspot(
				p3D->x,
				p3D->y,
				p3D->z,
				p3D->name,
				10, //size
				0,	//value
				1,	//sensor1
				2,	//sensor2
				1000 // dist_mm
				);

		pHs->x*=pHs->dist_mm;
		pHs->y*=pHs->dist_mm;
		pHs->z*=pHs->dist_mm;

		TPtrShape * pSh = new TPtrShape(GridPanel4);
		pSh->p[0] = pHs;
		pSh->Name = "Dsh" + p3D->name;
		pSh->Shape = stCircle;
		pSh->Brush->Color=clRed;
		pSh->Height = 12;
		pSh->Parent =  GridPanel4;


		TEdit * pE =new TEdit(GridPanel4);
		GridPanel4->Height += pE->Height+2;
		pE->Name = "De1" + p3D->name;
		pE->Text = pHs->name;
		pE->Parent = GridPanel4;

		pE =new TEdit(GridPanel4);
		pE->Name = "De2" + p3D->name;
		pE->Text = pHs->r;
		pE->Parent = GridPanel4;

		pE =new TEdit(GridPanel4);
		pE->Name = "De3" + p3D->name;
		pE->Text = pHs->sensor[0];
		pE->Parent = GridPanel4;

		pE =new TEdit(GridPanel4);
		pE->Name = "De4" + p3D->name;
		pE->Text = pHs->sensor[1];
		pE->Parent = GridPanel4;

		pE =new TEdit(GridPanel4);
		pE->Name = "De5" + p3D->name;
		pE->Text = pHs->value;
		pE->Parent = GridPanel4;
		int zz=0;
	}
}
//---------------------------------------------------------------------------
void TForm11::AddHotspot(TPoint3D * p3D)
{
		Form3->StringGrid1->Objects[0][0]=0;
		Form3->ClientOrigin.x = Left + Width;

		TPtrButton * pB = new TPtrButton(GridPanel1);
		pB->p = p3D;
		pB->Caption = "Remove";

		static int nId=1;
		AnsiString s;
		s.sprintf("r%d",nId);
		pB->Name = s;
		pB->OnClick = GridPanel1->OnClick;
		pB->Width = 200;
		pB->Height = 20;
		p3D->name = nId;
		pB->Parent = GridPanel1;

		GridPanel1->Height += pB->Height+3;
		int c =GridPanel1->CellCount;
		c =GridPanel1->ColumnCollection->Count ;

		for (int i=0; i < 4;i++)
		{
			TEdit * pE =new TEdit(GridPanel1);
			pE->Parent = GridPanel1;
			switch (i)
			{
				case 0: pE->Text = p3D->x; break;
				case 1: pE->Text = p3D->y; break;
				case 2: pE->Text = p3D->z; break;
				case 3: pE->Text = p3D->name; break;
			}
		}

		pB = new TPtrButton(GridPanel1);
		pB->p = p3D;
		pB->Caption = "Touch";
		s.sprintf("t%d",nId);
		pB->Name = s;
		pB->OnClick = GridPanel1->OnClick;
		pB->Width = 100;
		pB->Height = 20;
		pB->Parent = GridPanel1;

		pB = new TPtrButton(GridPanel1);
		pB->p = p3D;
		pB->Caption = "Point to";
		s.sprintf("p%d",nId);
		pB->Name = s;
		pB->OnClick = GridPanel1->OnClick;
		pB->Width = 100;
		pB->Height = 20;
		pB->Parent = GridPanel1;

		pB = new TPtrButton(GridPanel1);
		pB->p = p3D;
		pB->Caption = "Direction";
		s.sprintf("d%d",nId);
		pB->Name = s;
		pB->OnClick = GridPanel1->OnClick;
		pB->Width = 100;
		pB->Height = 20;
		pB->Parent = GridPanel1;

		nId++;
	}

void __fastcall TForm11::Button5Click(TObject *Sender)
{
	int res = Form3->ShowModal();
	if (res == mrOk)
	{
		TPoint3D * p3D = (TPoint3D *)
			Form3->StringGrid1->Objects[0][0];
		AnsiString s;
		s.sprintf( "%d,%d,%d",p3D->x,p3D->y,p3D->z);
		Memo2->Lines->Add(s);
//		AddHotspot(p3D);
		return;


		Form3->StringGrid1->Objects[0][0]=0;
		Form3->ClientOrigin.x = Left + Width;

		TPtrButton * pB = new TPtrButton(GridPanel1);
		pB->p = p3D;
		pB->Caption = "Remove";

		static int nId=1;

		s.sprintf("r%d",nId);
		pB->Name = s;
		pB->OnClick = GridPanel1->OnClick;
		pB->Width = 200;
		pB->Height = 20;
		p3D->name = nId;
		pB->Parent = GridPanel1;

		GridPanel1->Height += pB->Height+3;
		int c =GridPanel1->CellCount;
		c =GridPanel1->ColumnCollection->Count ;

		for (int i=0; i < 4;i++)
		{
			TEdit * pE =new TEdit(GridPanel1);
			pE->Parent = GridPanel1;
			switch (i)
			{
				case 0: pE->Text = p3D->x; break;
				case 1: pE->Text = p3D->y; break;
				case 2: pE->Text = p3D->z; break;
				case 3: pE->Text = p3D->name; break;
			}
		}

		pB = new TPtrButton(GridPanel1);
		pB->p = p3D;
		pB->Caption = "Touch";
		s.sprintf("t%d",nId);
		pB->Name = s;
		pB->OnClick = GridPanel1->OnClick;
		pB->Width = 200;
		pB->Height = 20;
		pB->Parent = GridPanel1;

		pB = new TPtrButton(GridPanel1);
		pB->p = p3D;
		pB->Caption = "Point to";
		s.sprintf("p%d",nId);
		pB->Name = s;
		pB->OnClick = GridPanel1->OnClick;
		pB->Width = 100;
		pB->Height = 20;
		pB->Parent = GridPanel1;
		nId++;
	}
}

//---------------------------------------------------------------------------

void __fastcall TForm11::Button1Click(TObject *Sender)
{
	int res = WaitForSingleObject(hMutex,100);
	if (res!=WAIT_OBJECT_0) return;
	__try
	{
		int nn =	nHotspots;
		nHotspots =0;
		for (int i = 0; i < nn; i++)
			_3DHotspots[i] = 0;

		int nCol =	GridPanel2->ColumnCollection->Count;
		int nRow =	GridPanel2->RowCollection->Count;
		int offsShape = nCol;
		int nObj = nRow -1;
		int offsEdit = offsShape +nObj;
		for (int i=offsShape; i<offsEdit; i++)
		{
			AnsiString s;
			TPtrShape * pSh = (TPtrShape *)               //Controls
				GridPanel2->Controls[i];
			s.sprintf("TShapes: %s", pSh->Name);
			Memo1->Lines->Add(s);

			THotspot * pHS = pSh->p[0];// ->p); //(THotspot *)

			TEdit * pESize   = (TEdit *) GridPanel2->Controls[offsEdit + (i-offsShape)*(nCol-1) + 1];
			s.sprintf("TEdits %s",pESize->Name);
			Memo1->Lines->Add(s);

			TEdit * pESensor = (TEdit *) GridPanel2->Controls[offsEdit + (i-offsShape)*(nCol-1) + 2];
			s.sprintf("TEdits %s",pESensor->Name);
			Memo1->Lines->Add(s);

			TEdit * pEDevice = (TEdit *) GridPanel2->Controls[offsEdit + (i-offsShape)*(nCol-1) + 3];
			s.sprintf("TEdits %s",pEDevice->Name);
			Memo1->Lines->Add(s);

			THotspot * pHS2 = new THotspot(pHS->x,pHS->y,pHS->z,pHS->name,
			pESize->Text.ToInt(),
			pEDevice->Text.ToInt(),
			pESensor->Text.ToInt());
			if (pSh->p[1])
				delete pSh->p[1];
			pSh->p[1] = pHS2;
			_3DHotspots[nHotspots] = pSh;// pHS2;
			nHotspots++;
		}

		nCol =	GridPanel3->ColumnCollection->Count;
		nRow =	GridPanel3->RowCollection->Count;
		offsShape = nCol;
		nObj = nRow -1;
		offsEdit = offsShape +nObj;
		for (int i=offsShape; i<offsEdit; i++)
		{
			AnsiString s;
			TPtrShape * pSh = (TPtrShape *)               //Controls
				GridPanel3->Controls[i];
			s.sprintf("PShapes: %s", pSh->Name);
			Memo1->Lines->Add(s);

			THotspot * pHS = pSh->p[0];// ->p); //(THotspot *)

			TEdit * pESize   = (TEdit *) GridPanel3->Controls[offsEdit + (i-offsShape)*(nCol-1) + 1];
			s.sprintf("PEdits %s",pESize->Name);
			Memo1->Lines->Add(s);

			TEdit * pESensor1= (TEdit *) GridPanel3->Controls[offsEdit + (i-offsShape)*(nCol-1) + 2];
			s.sprintf("PEdits %s",pESensor1->Name);
			Memo1->Lines->Add(s);

			TEdit * pESensor2 = (TEdit *) GridPanel3->Controls[offsEdit + (i-offsShape)*(nCol-1) + 3];
			s.sprintf("PEdits %s",pESensor2->Name);
			Memo1->Lines->Add(s);

			TEdit * pEDevice = (TEdit *) GridPanel3->Controls[offsEdit + (i-offsShape)*(nCol-1) + 4];
			s.sprintf("PEdits %s",pEDevice->Name);
			Memo1->Lines->Add(s);

			THotspot * pHS2 = new THotspot(pHS->x,pHS->y,pHS->z,pHS->name,
			pESize->Text.ToInt(),
			pEDevice->Text.ToInt(),
			pESensor1->Text.ToInt(),
			pESensor2->Text.ToInt());
			if (pSh->p[1])
				delete pSh->p[1];
			pSh->p[1] = pHS2;
			_3DHotspots[nHotspots] = pSh;// pHS2;
			nHotspots++;
		}
		nCol =	GridPanel4->ColumnCollection->Count;
		nRow =	GridPanel4->RowCollection->Count;
		offsShape = nCol;
		nObj = nRow -1;
		offsEdit = offsShape +nObj;
		for (int i=offsShape; i<offsEdit; i++)
		{
			AnsiString s;
			TPtrShape * pSh = (TPtrShape *)               //Controls
				GridPanel4->Controls[i];
			s.sprintf("PShapes: %s", pSh->Name);
			Memo1->Lines->Add(s);

			THotspot * pHS = pSh->p[0];// ->p); //(THotspot *)

			TEdit * pESize   = (TEdit *) GridPanel4->Controls[offsEdit + (i-offsShape)*(nCol-1) + 1];
			s.sprintf("PEdits %s",pESize->Name);
			Memo1->Lines->Add(s);

			TEdit * pESensor1= (TEdit *) GridPanel4->Controls[offsEdit + (i-offsShape)*(nCol-1) + 2];
			s.sprintf("PEdits %s",pESensor1->Name);
			Memo1->Lines->Add(s);

			TEdit * pESensor2 = (TEdit *) GridPanel4->Controls[offsEdit + (i-offsShape)*(nCol-1) + 3];
			s.sprintf("PEdits %s",pESensor2->Name);
			Memo1->Lines->Add(s);

			TEdit * pEDevice = (TEdit *) GridPanel4->Controls[offsEdit + (i-offsShape)*(nCol-1) + 4];
			s.sprintf("PEdits %s",pEDevice->Name);
			Memo1->Lines->Add(s);

			THotspot * pHS2 = new THotspot(pHS->x,pHS->y,pHS->z,pHS->name,
			pESize->Text.ToInt(),
			pEDevice->Text.ToInt(),
			pESensor1->Text.ToInt(),
			pESensor2->Text.ToInt(),
			1000
			);
			if (pSh->p[1])
				delete pSh->p[1];
			pSh->p[1] = pHS2;
			_3DHotspots[nHotspots] = pSh;// pHS2;
			nHotspots++;
		}
	}
	__finally
	{
		ReleaseMutex(hMutex);
	if (!handleWr1)
		ds_CloseWrite(handleWr1);
	if (FileExists("/tmp/hotspots.#"))
		DeleteFile("/tmp/hotspots.#");
	handleWr1 = ds_OpenWrite("/tmp/hotspots",0,480);
		UpdateInfo();
	}
	int zz=0;
}
//---------------------------------------------------------------------------



void __fastcall TForm11::Button2Click(TObject *Sender)
{
	if (SaveDialog1->Execute())
	{
		Memo2->Lines->SaveToFile(SaveDialog1->FileName+".hs");
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm11::Button3Click(TObject *Sender)
{
	if (OpenDialog1->Execute())
	{
		Memo2->Lines->LoadFromFile(OpenDialog1->FileName);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm11::Button4Click(TObject *Sender)
{
	char buf[40][101], pt[8] = {0,0,0,0,0,0,0,0};
	int nPar =0;
	TPoint3D * p3D;
	memset(buf,0,sizeof buf);
	int x,y,z;

	for (int i = 0; i < Memo2->Lines->Count; i++)
	{
		AnsiString s = Memo2->Lines->Strings[i];
		strcpy(buf[i], s.c_str());
		char * p = buf[i];
		char * pz = strchr(p,',');
		*pz = 0;
		x = atoi(p);
		p = pz+1;
		pz = strchr(p,',');
		y = atoi(p);
		p = pz+1;
		pz = strchr(p,',');
		z = atoi(p);
		p3D =  new TPoint3D(x,y,z);
		AddHotspot(p3D);
	}
}
//---------------------------------------------------------------------------




AnsiString DirMMF[100];
int TForm11::FindMmfBuffer()
{
  AnsiString s ="/tmp/*.#";
  int Count =0;
  TSearchRec Info;
  if (FindFirst (s,faAnyFile,Info) ==0)
  {
	do
	  DirMMF[Count++]=Info.Name;
	while (FindNext(Info)==0);
	FindClose(Info);
  }
  return Count;
}


void __fastcall TForm11::Timer1Timer(TObject *Sender)
{
	int n = FindMmfBuffer();
	if (n!=ComboBox1->Items->Count)
	{
		ComboBox1->Items->Clear();
		for (int i = 0; i <n; i++)
			ComboBox1->Items->Append(DirMMF[i]);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm11::ComboBox1Change(TObject *Sender)
{
	LabeledEdit1->Text = ComboBox1->Text;
	ComboBox1->Visible = false;
	Timer1->Enabled = false;

	AnsiString s = LabeledEdit1->Text;
	s[strlen(s.c_str())-1] = 0;
	AnsiString fn = "/tmp/" + s;

	if (handleRd1)
		ds_CloseRead(handleRd1);
	handleRd1 = ds_OpenRead(fn.c_str());

	if (FileExists("/tmp/hotspots.#"))
		DeleteFile("/tmp/hotspots.#");
	if (!handleWr1)
		handleWr1 = ds_OpenWrite("/tmp/hotspots",0,480);

//	UpdateInfo();
	thListen = new TListenThread(handleRd1,ProcessData);
	PageControl1->ActivePageIndex=0;
	PageControl1->Visible = true;
	thListen->Resume();
}
//---------------------------------------------------------------------------

