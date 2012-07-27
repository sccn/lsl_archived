//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RiverView.h"
#include "UParseLine.h"
//---------------------------------------------------------------------------

#include "GetMMFId.h"
#include "ListenToMMF.h"
#include "AdaptMMF.h"

#include "DataSuite.h"

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
//TMMFhandle * thrMmfAdapt = 0;
HANDLE handleRd1;
//	thrMmfAdapt = InitMMFthread(LabeledEdit1->Text.c_str(),ProcessData,sizeof(TMaxArray),0);//,tpNormal); // buffer must already exist
HANDLE handleWr1;

extern AnsiString par;
int  firstSensorCh = 0;

class TPtrShape: public TShape
{
public:
	THotspot * p[2];
	TPtrShape(void * AOwner): TShape((TComponent *)AOwner) {p[0]=0;p[1]=0;};
};

TPtrShape * _3DHotspots[20] = {
	0,0,0,0,0,
	0,0,0,0,0,
	0,0,0,0,0,
	0,0,0,0,0
};
int nHotspots=0;

int nSensors=0;

TMaxArray MaxArrayCapture;
bool Capture = false;
union TVector
{
	short sh[2];
	TObject * pO;
};

int count = 0;
HANDLE hMutex;

typedef TDataArray<21> THotspotSample;   //EvChannel (at 0)
THotspotSample HotspotSample;

void ProcessData(char * data, int size)
{
int res = WaitForSingleObject(hMutex,100);
if (res!=WAIT_OBJECT_0) return;
__try
{
	HotspotSample.nItems = nHotspots +1;
	TMaxArray MaxArray;
	memcpy(&MaxArray, data,size);
	count++;
	if (Capture)
	{
		MaxArrayCapture = MaxArray;
		Capture = false;
	}
	HotspotSample[0] = 0;
	for (int i = 0; i< nHotspots; i++)
	{
		TPtrShape * pS = _3DHotspots[i];
		THotspot * pHs = pS->p[1];
		bool isTouch = pHs->sensor[1]<0;
		bool isPointTo = !isTouch;
		if (isTouch)
		{
			int ch = 1 + pHs->sensor[0]*3;
			TPoint3D pt3D(MaxArray[ch],MaxArray[ch+1],MaxArray[ch+2]);
			if (pHs->Includes(pt3D))
			{
				pS->Brush->Color = clGreen;
				HotspotSample[0] = pHs->value;
			}
			else
				pS->Brush->Color = clRed;
			HotspotSample[i+1] = pHs->Distance(pt3D);

		}
		else // point to
		{
			int ch1 = 1 + pHs->sensor[0]*3;
			TPoint3D pt1(MaxArray[ch1],MaxArray[ch1+1],MaxArray[ch1+2]);
			int ch2 = 1 + pHs->sensor[1]*3;
			TPoint3D pt2(MaxArray[ch2],MaxArray[ch2+1],MaxArray[ch2+2]);
			int sz = pHs->r;
			TVectorToHotspot PointingTo(pt1,pt2,sz );
			TVectorToHotspot TargetIs(pt1,*pHs,sz);
			if (PointingTo.Includes(TargetIs))
			{
				pS->Brush->Color = clGreen;
				HotspotSample[0] = pHs->value;
			}
			else
				pS->Brush->Color = clRed;
			HotspotSample[i+1] = PointingTo.Distance(TargetIs);;
		}
	}
	PushData(handleWr1,(char *)&HotspotSample, sizeof HotspotSample);
}
__finally
{
		ReleaseMutex(hMutex);
}
}

void __fastcall TForm11::FormCreate(TObject *Sender)
{
	hMutex = CreateMutex(0,false,0);
	TStringList * pStrPar = ParseCmdLine(par,' ');
	int nPar = pStrPar->Count;
	bool bMMF = false;
	bool bIndy = false;
	Timer1->Enabled = true;

	LabeledEdit1->Text =  "PhaseSpace";//GetMMFBufferID();
	handleRd1 = InitThread("PhaseSpace", ProcessData,sizeof(TMaxArray));
//	thrMmfAdapt = InitMMFthread(LabeledEdit1->Text.c_str(),ProcessData,sizeof(TMaxArray),0);//,tpNormal); // buffer must already exist
	handleWr1 = InitDataBuffer("Hotspots");
}
//---------------------------------------------------------------------------
#include "UEdit3D.h"

class TPtrButton: public TButton
{
public:
	void * p;
	TPtrButton(void * PWin): TButton(PWin) {p=0;};
};


void __fastcall TForm11::FormDestroy(TObject *Sender)
{
	ReleaseThread(handleRd1);
//	FreeMMFthread(thrMmfAdapt);
	CloseHandle(hMutex);

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------







void __fastcall TForm11::GridPanel1Click(TObject *Sender)
{
	TPtrButton * pC = (TPtrButton *) Sender;
	String txt = pC->Name;
	bool remove = txt.SubString(0,1)=="r";
	bool touch = txt.SubString(0,1)=="t";
	bool point = txt.SubString(0,1)=="p";

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
		TPtrShape * pSh = new TPtrShape(GridPanel2);
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
}
//---------------------------------------------------------------------------

void __fastcall TForm11::Button5Click(TObject *Sender)
{
	int res = Form3->ShowModal();
	if (res == mrOk)
	{
		TPoint3D * p3D = (TPoint3D *)
			Form3->StringGrid1->Objects[0][0];
		Form3->StringGrid1->Objects[0][0]=0;
		Form3->ClientOrigin.x = Left + Width;

		TPtrButton * pB = new TPtrButton(GridPanel1);
		pB->p = p3D;
		pB->Caption = "Remove";

		static int nId=1;
		pB->Name.sprintf("r%d",nId);
		pB->OnClick = GridPanel1->OnClick;
		pB->Width = 200;
		pB->Height = 20;
		p3D->name = nId;
		pB->Parent = GridPanel1;

		GridPanel1->Height += pB->Height;
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
		pB->Name.sprintf("t%d",nId);
		pB->OnClick = GridPanel1->OnClick;
		pB->Width = 200;
		pB->Height = 20;
		pB->Parent = GridPanel1;

		pB = new TPtrButton(GridPanel1);
		pB->p = p3D;
		pB->Caption = "Point to";
		pB->Name.sprintf("p%d",nId);
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
		String s;
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
		String s;
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
}
__finally
{
	ReleaseMutex(hMutex);
}
	int zz=0;
}
//---------------------------------------------------------------------------

void __fastcall TForm11::Timer1Timer(TObject *Sender)
{
	Memo1->Lines->Add(count);
}
//---------------------------------------------------------------------------

