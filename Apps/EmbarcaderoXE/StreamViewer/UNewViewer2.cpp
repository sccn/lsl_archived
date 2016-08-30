//---------------------------------------------------------------------------



#include "useallegro4.h"

#include <vcl.h>


//#define __STDINT_H // trick to avoid error
//#include <SDL/SDL_thread.h>



#include "DrawView3.h"
//#include "UXML.h"
#include "UNewViewer2.h"

//#pragma hdrstop

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"





//////////////////////////////////////////////////////////////////////

#include <ddraw.h>
#include <stdio.h>
#include <math_util.h>

TForm1 *Form1;


void ShowVScale()
{
	char buf[20];
		sprintf(buf,"2^%d",Form1->UpDown1->Position);
		Form1->Edit1->Text        = buf;
}

//---------------------------------------------------------------------------
static AnsiString s;

//#include "UThreadQ.h"

//#include "drlib.h"
//#include "drinit.h"

#include "StreamThread.h"
TStreamThread *listenThread;

//void * hInStream = 0;
int CurrChNmb=0;


int GetAttribute(_di_IXMLNode Node, AnsiString attr)
{
	if (Node->HasAttribute(attr))
		return Node->GetAttribute(attr);
	else
		return 0; // see if 0 or NAN
}


TStringList * labellist = 0;
int nEegChans = 0;
int channelsPerMarker = 4;
double distanceScaleFactor = 1000.0;
bool motionTypeRecognized = false;

int sensor0, sensor1, sensor2, sensor3;

void * AcquireMMF(char * Id)
{         /*
		char nm[200] = "/tmp/";
		strcat(nm,Id);
		void * mmfHandle = ds_Open(nm);
		void * hw = mmfHandle;

		if (!labellist) labellist = new TStringList;

		Form1->xd ->Active = true;
		AnsiString sx = ds_XMLHeader(hw);
		Form1->xd->LoadFromXML(sx);

		labellist->Clear();
		_di_IXMLNode Node  = FindNode(Form1->xd->DocumentElement, "sample");

		int n  = Node->ChildNodes->Count;
		int nAnalog = 0;
		for (int i = 0; i < n; i++)
		{
			_di_IXMLNode pFN;
			pFN = Node->ChildNodes->Get(i);
			_di_IXMLNode NodeAnl  = FindNode(pFN,"data");

			if (NodeAnl)
			{
				String s = NodeAnl->GetAttribute("items");
				nAnalog += StrToInt(s);
			}
			if (i == 0) nEegChans = nAnalog; // for filtering


			int nn  = NodeAnl->ChildNodes->Count;
			for (int j = 0; j < nn; j++)    // browse the child nodes of data
			{
				_di_IXMLNode NodeType,NodeLbl;
				NodeType = NodeAnl->ChildNodes->Get(j);
				NodeLbl = FindNode(NodeType,"labels");
				if (NodeLbl)  // if have labels , use them,
				{
					int nLbl = NodeLbl->ChildNodes->Count;
					for (int k = 0; k < nLbl; k++)
					{
						_di_IXMLNode Node = NodeLbl->ChildNodes->Get(k);
						String s = Node->GetNodeName();
						labellist->Add(s);
					}
				}
				else   		// no labels, use local node label
				{
					_di_IXMLNode Node = NodeAnl->ChildNodes->Get(j);
					if (Node->HasAttribute("count"))
					{
						int count = Node->GetAttribute("count");
						int nItems = Node->ChildNodes->Count;
						for (int c = 0; c < count; c++)
						for (int k = 0; k < nItems; k++)
						{
							_di_IXMLNode NodeS = Node->ChildNodes->Get(k);
							if (NodeS->HasAttribute("label"))
							{
								AnsiString s = NodeS->GetAttribute("label");
								char * pLast = s.AnsiLastChar();
								if (*pLast =='%')
								{
									s.SetLength(s.Length()-1);
									s = s +IntToStr(c);
								}
								labellist->Add(s);
							}
						}
					}
					else
					{
						String ss = Node->GetNodeName();
						labellist->Add(ss);
					}
				}
			}

			CurrChNmb = nAnalog;
		}

		int banks = (CurrChNmb / 32) + bool(CurrChNmb % 32);
		Form1->PageControl1->ActivePageIndex = 0;
		Form1->TabSheet[0]->PageControl = Form1->PageControl1;
		for (int i = 1; i < 20; i++)
			if (i<banks)
				Form1->TabSheet[i]->PageControl = Form1->PageControl1;
			else
				Form1->TabSheet[i]->PageControl = 0;// Enabled = false;// Hide();// SetParentComponent(0);

		return mmfHandle;
		*/
		return NULL;
}

void OpenMmfStream(char * sName)
{
/*
	if (hInStream)
	{
		ds_Close(hInStream);
		hInStream = 0;
	}
	char * p=sName;
	do p++; while (*p !='.') ;
	*p = 0;
	hInStream =AcquireMMF(sName);
	_di_IXMLNode Node  = FindNode(Form1->xd->DocumentElement, "sample");
	Form1->udMoCh->Position = 0;
	Form1->leFirstMoCh->Text = Node->ChildNodes->Get(0)->GetAttribute("name") ;
	*/
}

__fastcall TForm1::TForm1(TComponent* Owner)        : TForm(Owner)
{
	for (int i=0;i<10;i++) ChanIsOn[i]=0xffffffff;
}
//---------------------------------------------------------------------------
extern bool zero;
#include "ufilter.h"

#define MAXBUFSIZE (256*8) // <8 s?
//TMaxArray buf[MAXBUFSIZE];


void HiPass(float  * pNewSample, int nChannels)
{
	//puts result in pNewSample
	CumulBuf.AddSample(pNewSample, nChannels);
};

//TMaxArray LocSample;

//#include "UThreadQ.h"
/*
inline int SStoDRiver(struct TDataRiver *dr, char *C)
{
	TStreamSample * ma = (TStreamSample *)C;

	dr->DataSample[0] = &ma->drf;
	dr->nStreams  = ma->nStreams;
	 for (int i = 1; i < dr->nStreams;i++)
	 {
		int items = dr->DataSample[i-1]->nItems;
		switch (dr->DataSample[i-1]->item_size)
		{
			case 1: dr->DataSample[i] = (TDataSample *)&dr->DataSample[i-1]->C[items];break;
			case 2: dr->DataSample[i] = (TDataSample *)&dr->DataSample[i-1]->S[items];break;
		default:
			dr->DataSample[i] = (TDataSample *)&dr->DataSample[i-1]->L[items];

		}
		int n = dr->DataSample[i]->nItems;
		int zz = 0;
	 }
	 return 1;
}
*/
int processDataCount = 0;

void ProcessDataString(char *data, int nChannels, double samplingRate) {
	HWND hWnd;
	HDC 	hDC;
	RECT r;
	if(processDataCount == 0 && Form1->cbParseData->Checked) {
		vector<UnicodeString> lines = splitString(UnicodeString(listenThread->xmlHeader), L'\n');
		Form1->Memo1->Clear();
		for(int i=0; i<lines.size(); i++) {
			Form1->Memo1->Lines->Add(lines[i]);
		}
	}
	if(samplingRate == 0.0) {
		Form1->Display->ShowString(lsl_local_clock(), data);
		hWnd =Form1->PageControl2->ActivePage->Handle;
		hDC = GetDC(hWnd);
		r=Form1->Image3->BoundsRect;
		draw_to_hdc (hDC,Form1->Display->bmpCanvas,r.left,r.top);
		r=Form1->ImageCode->BoundsRect;
		draw_to_hdc (hDC,Form1->Display->bmpCodeCanvas,r.left,r.top);
		ReleaseDC(hWnd,hDC);
	}
	processDataCount++;
}



void ProcessData(float *data, int nChannels, double samplingRate) {
   /*	Form1->Edit2->Text = nChannels/4; */
	if(processDataCount == 0 && Form1->cbParseData->Checked) {
		vector<UnicodeString> lines = splitString(UnicodeString(listenThread->xmlHeader), L'\n');
		Form1->Memo1->Clear();
		for(int i=0; i<lines.size(); i++) {
			Form1->Memo1->Lines->Add(lines[i]);
		}
	}
	//printf("sampling rate %g\n", samplingRate);
	if(samplingRate == 0.0) return;
	Form1->ProgressBar1->Position= ((int)((processDataCount++) * 100.0/samplingRate))% 125;
	HWND hWnd;
	HDC 	hDC;
	RECT r;


	bool isTime = Form1->PageControl2->ActivePageIndex==0;
	if (isTime)  //disply traces
	{


			if (Form1->CheckBox1->Checked) HiPass(data, nChannels);


			Form1->Display->DisplaySample(data, nChannels, samplingRate);

		if(((int)((processDataCount++) * 100.0/samplingRate))%5 == 0) {
			hWnd =Form1->PageControl2->ActivePage->Handle;
			hDC = GetDC(hWnd);
			r=Form1->Image3->BoundsRect;
			draw_to_hdc (hDC,Form1->Display->bmpCanvas,r.left,r.top);
			r=Form1->ImageCode->BoundsRect;
			draw_to_hdc (hDC,Form1->Display->bmpCodeCanvas,r.left,r.top);
			ReleaseDC(hWnd,hDC);
		}
	}
	else
	{
		bool ok;

		  Form1->Display3D->DisplaySample(data,nChannels,channelsPerMarker, samplingRate);

		if(((int)((processDataCount++) * 100.0/samplingRate)) % 5 == 0) {
			hWnd =Form1->PageControl2->ActivePage->Handle;
			hDC = GetDC(hWnd);
			r=Form1->Image4->BoundsRect;

			draw_to_hdc (hDC,Form1->Display3D->bmpCanvas,r.left,r.top);

			ReleaseDC(hWnd,hDC);

			if(sensor0 >= 0 && sensor0*channelsPerMarker < nChannels) {
				Form1->x0Edit->Text = FormatFloat("0.00",data[sensor0*channelsPerMarker]*distanceScaleFactor);
				Form1->y0Edit->Text = FormatFloat("0.00",data[sensor0*channelsPerMarker+1]*distanceScaleFactor);
				Form1->z0Edit->Text = FormatFloat("0.00",data[sensor0*channelsPerMarker+2]*distanceScaleFactor);
			}

			if(sensor1 >= 0 && sensor1*channelsPerMarker < nChannels) {
				Form1->x1Edit->Text = FormatFloat("0.00",data[sensor1*channelsPerMarker]*distanceScaleFactor);
				Form1->y1Edit->Text = FormatFloat("0.00",data[sensor1*channelsPerMarker+1]*distanceScaleFactor);
				Form1->z1Edit->Text = FormatFloat("0.00",data[sensor1*channelsPerMarker+2]*distanceScaleFactor);

			}
			if(sensor2 >= 0 && sensor2*channelsPerMarker < nChannels) {
				Form1->x2Edit->Text = FormatFloat("0.00",data[sensor2*channelsPerMarker]*distanceScaleFactor);
				Form1->y2Edit->Text = FormatFloat("0.00",data[sensor2*channelsPerMarker+1]*distanceScaleFactor);
				Form1->z2Edit->Text = FormatFloat("0.00",data[sensor2*channelsPerMarker+2]*distanceScaleFactor);

			}

			if(sensor3 >= 0 && sensor3*channelsPerMarker < nChannels) {
				Form1->x3Edit->Text = FormatFloat("0.00",data[sensor3*channelsPerMarker]*distanceScaleFactor);
				Form1->y3Edit->Text = FormatFloat("0.00",data[sensor3*channelsPerMarker+1]*distanceScaleFactor);
				Form1->z3Edit->Text = FormatFloat("0.00",data[sensor3*channelsPerMarker+2]*distanceScaleFactor);

			}

		}
	}
}

void ProcessDataInt(int *data, int nChannels, double samplingRate) {
	HWND hWnd;
	HDC 	hDC;
	RECT r;
	if(samplingRate == 0.0) {
		Form1->Display->ShowCode(lsl_local_clock(), data[0]);
		hWnd =Form1->PageControl2->ActivePage->Handle;
		hDC = GetDC(hWnd);
		r=Form1->Image3->BoundsRect;
		draw_to_hdc (hDC,Form1->Display->bmpCanvas,r.left,r.top);
		r=Form1->ImageCode->BoundsRect;
		draw_to_hdc (hDC,Form1->Display->bmpCodeCanvas,r.left,r.top);
		ReleaseDC(hWnd,hDC);
	} else {
		float * datafloat = new1D(nChannels, 0.0f);
		for(int i=0; i<nChannels; i++) {
			datafloat[i] = (float) data[i];
		}
		ProcessData(datafloat, nChannels,samplingRate);
	}

}

void TForm1::GetDDRect(TImage * pImg,RECT & rThis)
{
		rThis=pImg->BoundsRect;
		rThis.left  +=ClientOrigin.x;
		rThis.right +=ClientOrigin.x;
		rThis.top   +=ClientOrigin.y;
		rThis.bottom+=ClientOrigin.y;

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void __fastcall TForm1::UpDown1MouseUp(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
		ShowVScale();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "FClose.h"
#include <process.h>

extern  AnsiString par;
extern  bool ok;



void __fastcall TForm1::FormCreate(TObject *Sender)
{

	Form1->Caption = UnicodeString("Stream Viewer, version ") + getVersion();
	bool bMMF = false;
	bool bIndy = false;

	ListBox1->ItemHeight=20;
	RECT r=Image3->BoundsRect;
	Bevel1->SetBounds(r.left-1,r.top-1,r.right-r.left+3,r.bottom-r.top+3);
	r=ImageCode->BoundsRect;
	Bevel2->SetBounds(r.left-1,r.top-1,r.right-r.left+3,r.bottom-r.top+3);
	Button2Click(0);

	Display = new CChannelN(Image3,ImageCode,32);
	ShowVScale();
	Display->PaintBkgr();


	Display3D = new TDisplay3D(Image4,32);
	Display3D->PaintBkgr();

  	TabSheet [0] =TabSheet3;
	TabSheet [1] =TabSheet4;
	TabSheet [2] =TabSheet5;
	TabSheet [3] =TabSheet6;
	TabSheet [4] =TabSheet7;
	TabSheet [5] =TabSheet8;
	TabSheet [6] =TabSheet9;
	TabSheet [7] =TabSheet10;
	TabSheet [8] =TabSheet11;
	TabSheet [9] =TabSheet12;
	TabSheet [10] =TabSheet13;
	TabSheet [11] =TabSheet14;
	TabSheet [12] =TabSheet15;
	TabSheet [13] =TabSheet16;
	TabSheet [14] =TabSheet17;
	TabSheet [15] =TabSheet18;
	TabSheet [16] =TabSheet19;
	TabSheet [17] =TabSheet20;
	TabSheet [18] =TabSheet21;
	TabSheet [19] =TabSheet22;

	TimerDisp->Enabled=true;

	sensor0EditChange(this);
	sensor1EditChange(this);
	sensor2EditChange(this);
	sensor3EditChange(this);

	RefreshStreamsButtonClick(this);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	if(listenThread) {
		listenThread->Terminate();
		delete listenThread; //will not delete till terminated, by VCL design.
		listenThread = NULL;
	}


	delete Display;
	delete Display3D;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::PageControl1Change(TObject *Sender)
{
	  /*
		int bank = PageControl1->ActivePageIndex;

			int FromChannel = PageControl1->ActivePageIndex*32;


			for (int i = 0; i < 32; i++)
			{
				int ch = FromChannel+i;

				if (ch<labellist->Count)
					ListBox1->Items->Strings[i]= labellist->Strings[ch];
				else
					ListBox1->Items->Strings[i]= "";


				if (ChanIsOn[bank] & (1 << i))
					ListBox2->Items->Strings[i]="o";
				else
					ListBox2->Items->Strings[i]="x";
			}

		   */
  /*		_di_IXMLNode Node  = FindNode(xd->DocumentElement, "stream");
		float sr = Node->GetAttribute("sampling_rate");

		AnsiString str;
		str.sprintf("%.1f Hz",sr);
		LabeledEdit1->Text =str;
	*/
}
//---------------------------------------------------------------------------





void __fastcall TForm1::ListBox1Click(TObject *Sender)
{
	int    bank = PageControl1->ActivePageIndex;
	ChanIsOn[bank] ^= (1 << ListBox1->ItemIndex);
	if (ChanIsOn[bank] & (1 << ListBox1->ItemIndex))
		ListBox2->Items->Strings[ListBox1->ItemIndex]="o";
	else
		ListBox2->Items->Strings[ListBox1->ItemIndex]="x";
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button2Click(TObject *Sender)
{
	if (Button2->Caption == "<<")
	{
		Button2->Caption = ">>";
		ClientWidth = BorderWidth + Memo1->Left;
	}
	else
	{
		Button2->Caption = "<<";
		ClientWidth = BorderWidth + Memo1->Left + Memo1->Width+2;
	}

}
//---------------------------------------------------------------------------




void __fastcall TForm1::TrackBarRotChange(TObject *Sender)
{
	Display3D->PaintBkgr();

}
//---------------------------------------------------------------------------



#include "ULeds.h"
//---------------------------------------------------------------------------

void __fastcall TForm1::Panel1MouseEnter(TObject *Sender)
{
	FormLEDs->Left =  Panel1->ClientOrigin.x + Panel1->Width;
	FormLEDs->Top =   Panel1->ClientOrigin.y;
	int n = UpDown2->Position;
	if (n<30)
	{
		FormLEDs->Height = FormLEDs->CheckBoxLED[n]->Top+2;
		FormLEDs->Width  = FormLEDs->CheckBoxLED[0]->Left + FormLEDs->CheckBoxLED[0]->Width -8;
	}
	else
	{
		if (n<60)
		{
			FormLEDs->Width  = FormLEDs->CheckBoxLED[30]->Left+FormLEDs->CheckBoxLED[30]->Width -8;
		}
		else
		if (n<90)
		{
			FormLEDs->Width  = FormLEDs->CheckBoxLED[60]->Left+FormLEDs->CheckBoxLED[60]->Width -8;
		}
		else
		{
			FormLEDs->Width  = FormLEDs->CheckBoxLED[90]->Left+FormLEDs->CheckBoxLED[90]->Width -8;
		}
		FormLEDs->Height = FormLEDs->CheckBoxLED[29]->Top+20;
	}
	FormLEDs->Show();
}
//---------------------------------------------------------------------------




void __fastcall TForm1::Button3Click(TObject *Sender)
{
	zero = true;
}
//---------------------------------------------------------------------------


bool bDrag = false;
int X0,Y0;
void __fastcall TForm1::Image4MouseDown(TObject *Sender, TMouseButton Button,
	  TShiftState Shift, int X, int Y)
{
	 bDrag = true;
	 X0=X;
	 Y0=Y;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Image4MouseUp(TObject *Sender, TMouseButton Button,
	  TShiftState Shift, int X, int Y)
{
	 bDrag = false;
}
//---------------------------------------------------------------------------
#include <math.h>
#define CENTER (Image4->Width/2)
void __fastcall TForm1::Image4MouseMove(TObject *Sender, TShiftState Shift,
	  int X, int Y)
{
	if (bDrag)
	{
		AnsiString s;s.sprintf("x=%d,y=%d",X,Y);
		Edit3->Text=s;
		Shape1->Left = X +Image4->Left;
		Shape1->Top  = Y +Image4->Top;
		float dx = (X - CENTER);
		float dy = (Y - CENTER);

		if (dx>0 && dy> -10)  // lower left quadrant
		{
			bool noRot = TrackBarTilt->Position >85;
			if (noRot) // do only gain
			{
				int deg =TrackBarRot->Position;
				if (deg)
				{
					double RotAngle= TO_RAD(deg);
					double RotSin = sin(RotAngle);
					int gain = dx / RotSin / 2;
					TrackBarGain->Position = gain;
				}
			}
			else
			{
				double angt = TO_RAD(TrackBarTilt->Position);
				double cosT = cos(angt);
				float dy = (Y - CENTER)/ cosT  ;
				if (dy>=0)
				{
					if (dx>20 || dy>20)
					{
						float Rot;
						if (dy)
							Rot = atan(dx/dy);
						else
							Rot = PI/2;
						int deg = TO_DEG(Rot);
						TrackBarRot->Position = deg;
						int gain = sqrt(dx*dx + dy*dy) /2;
						TrackBarGain->Position = gain; // /3
					}
				}
			}
		}
		else
		if (dy<0 &&  (dx> -20 && dx< 20))  // upper half
		{
			float sinu = -dy / TrackBarGain->Position;
			if (sinu <=1)
			{
				float Tilt= asin(sinu);
				int deg = TO_DEG(Tilt);
				TrackBarTilt->Position = deg;
			}
		}
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ListBox2Click(TObject *Sender)
{
	int bank = PageControl1->ActivePageIndex;
	ChanIsOn[bank] ^= (1 << ListBox2->ItemIndex);
	if (ChanIsOn[bank] & (1 << ListBox2->ItemIndex))
		ListBox2->Items->Strings[ListBox1->ItemIndex]="o";
	else
		ListBox2->Items->Strings[ListBox1->ItemIndex]="x";

}
//---------------------------------------------------------------------------



void __fastcall TForm1::ComboBox1Select(TObject *Sender)
{
	if(listenThread) {
		listenThread->Terminate();
		delete listenThread;//will not delete till terminated, by VCL design.
		listenThread = NULL;
   }

   std::vector<UnicodeString> strs = splitString(ComboBox1->Text, L';');

	if(strcmp( ((AnsiString) strs[0]).c_str(), "PhaseSpace") == 0) {
		channelsPerMarker = 4;
		distanceScaleFactor = 1000.0;
		motionTypeRecognized = true;
	} else if (strcmp( ((AnsiString) strs[0]).c_str(), "OptiTrack") == 0) {
		channelsPerMarker = 3;
		distanceScaleFactor = 1.0;
		motionTypeRecognized = true;
	} else {
		channelsPerMarker = 3;
		distanceScaleFactor = 1.0;
		motionTypeRecognized = false;
	}

   char temp[512];
   sprintf(temp, "uid='%s'", AnsiString(strs[1]).c_str());
   processDataCount = 0;

   lsl_streaminfo info;
   int streamsFound = lsl_resolve_bypred(&info,1, temp, 1,1.0);
   if(streamsFound < 1) return;
   lsl_channel_format_t format = lsl_get_channel_format(info);

   if(format == cft_float32) {
		listenThread = new TStreamThread(temp, ProcessData);
   } else if (format == cft_int32) {

		listenThread = new TStreamThread(temp, ProcessDataInt);
   } else if (format == cft_string) {
		listenThread = new TStreamThread(temp, ProcessDataString);
   } else {
		free(temp);
		lsl_destroy_streaminfo(info);
		Application->MessageBoxA(L"LSL Stream type is not implemented.", L"Error", MB_OK);
		return;
   }
   listenThread->SetResamplingRate(200.0);
   listenThread->Resume();

//	AnsiString s = ComboBox1->Text;
	//	OpenMmfStream(s.c_str());
	CumulBuf.ClearBuf();
	PageControl1->ActivePageIndex=0;
	PageControl1Change(0);
 //	free(temp);
	lsl_destroy_streaminfo(info);
}
//---------------------------------------------------------------------------




#define MAX_STREAMS 256



void __fastcall TForm1::udMoChChangingEx(TObject *Sender, bool &AllowChange, short NewValue,
		  TUpDownDirection Direction)
{
 /*	_di_IXMLNode Node  = FindNode(xd->DocumentElement, "sample");
	udMoCh->Max  = Node->ChildNodes->Count-1;
	if (
		((Direction == updUp) && (NewValue<=udMoCh->Max))
			||
		((Direction == updDown) && (NewValue>=0))
		)
		leFirstMoCh->Text = Node->ChildNodes->Get(NewValue)->GetAttribute("name") ;     */
}

//---------------------------------------------------------------------------

//#include "uplay2.h"

void __fastcall TForm1::cbPlayClick(TObject *Sender)
{
   //	 PlayThread	 ("/tmp/audio", (void *)Form1);
}
//---------------------------------------------------------------------------






void __fastcall TForm1::sensor0EditChange(TObject *Sender)
{
	bool ex = false;
	try {
		sensor0Edit->Text.ToInt();
	} catch (...) {
		ex = true;
	}

	if(!ex) sensor0 = sensor0Edit->Text.ToInt();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::sensor1EditChange(TObject *Sender)
{
	bool ex = false;
	try {
		sensor1Edit->Text.ToInt();
	} catch (...) {
		ex = true;
	}

	if(!ex) sensor1 = sensor1Edit->Text.ToInt();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::sensor2EditChange(TObject *Sender)
{
	bool ex = false;
	try {
		sensor2Edit->Text.ToInt();
	} catch (...) {
		ex = true;
	}

	if(!ex) sensor2 = sensor2Edit->Text.ToInt();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::sensor3EditChange(TObject *Sender)
{
	bool ex = false;
	try {
		sensor3Edit->Text.ToInt();
	} catch (...) {
		ex = true;
	}

	if(!ex) sensor3 = sensor3Edit->Text.ToInt();
}
//---------------------------------------------------------------------------




void __fastcall TForm1::RefreshStreamsButtonClick(TObject *Sender)
{

	lsl_streaminfo infos[MAX_STREAMS];
	int streamsFound = lsl_resolve_all(infos, MAX_STREAMS, 0.1);
	ComboBox1->Items->Clear();
	for (int i = 0; i < streamsFound; i++) {
		char temp[256];
		sprintf(temp,"%s;%s", lsl_get_name(infos[i]), lsl_get_uid(infos[i]));
		ComboBox1->Items->Append(temp);
		lsl_destroy_streaminfo(infos[i]);
	}



}
//---------------------------------------------------------------------------




void __fastcall TForm1::PageControl2Change(TObject *Sender)
{
	if(PageControl2->ActivePage==TabSheet32 && !motionTypeRecognized) {
		Application->MessageBoxA(L"Motion data type not recognized. Assuming three channels per marker and distances measured in millimeters.", L"Warning", MB_OK);

	}
}
//---------------------------------------------------------------------------


