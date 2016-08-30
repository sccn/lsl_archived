//---------------------------------------------------------------------------

#ifndef U3dHotspotsH
#define U3dHotspotsH




#include "UEdit3D.h"
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <msxmldom.hpp>
#include <StdCtrls.hpp>
#include <XMLDoc.hpp>
#include <xmldom.hpp>
#include <XMLIntf.hpp>
#include<list>
#include<vector>
#include<map>
#include "GazeUtil.h"
#include "pugixml.hpp"

#ifdef _DEBUG
	#undef _DEBUG  //prevent loading of debug version of library.
	#include "lsl_c.h" //must be "", not <>, otherwise get data corruption in console apps when using dll functions
	#define _DEBUG
#else
	#include "lsl_c.h"
#endif



class TForm11 : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TTabSheet *TabSheet3;
	TGridPanel *GridPanel2;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label15;
	TButton *Button5;
	TGridPanel *GridPanel3;
	TLabel *Label17;
	TLabel *Label18;
	TLabel *Label19;
	TLabel *Label20;
	TLabel *Label16;
	TLabel *Label21;
	TSaveDialog *SaveDialog1;
	TOpenDialog *OpenDialog1;
	TTabSheet *TabSheet4;
	TGridPanel *GridPanel4;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label22;
	TLabel *Label23;
	TLabel *Label24;
	TLabel *Label25;
	TPanel *Panel1;
	TGridPanel *GridPanel1;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label1;
	TPanel *Panel2;
	TButton *Button4;
	TMemo *Memo1;
	TMemo *Memo2;
	TButton *LoadLoc;
	TButton *SaveLoc;
	TPanel *Panel3x;
	TScrollBox *ScrollBox1;
	TScrollBox *ScrollBox2;
	TScrollBox *ScrollBox3;
	TScrollBox *ScrollBox4;
	TXMLDocument *xdoc_out;
	TButton *SaveConfigButton;
	TButton *LoadConfigButton;
	TSaveDialog *SaveDialog2;
	TOpenDialog *OpenDialog2;
	TPanel *TrashPanel;
	TLabel *Label26;
	TTabSheet *TabSheet5;
	TScrollBox *ScrollBox5;
	TGridPanel *GridPanel5;
	TLabel *Label27;
	TLabel *Label28;
	TLabel *Label29;
	TLabel *Label30;
	TLabel *Label31;
	TLabel *Label33;
	TLabel *Label35;
	TLabel *Label32;
	TLabel *Label37;
	TComboBox *GazeComboBox;
	TProgressBar *GazeProgressBar;
	TOpenDialog *GazeOpenDialog;
	TXMLDocument *xdoc_in;
	TButton *LoadCalibration;
	TTabSheet *Screen;
	TScrollBox *ScrollBox6;
	TGridPanel *GridPanel6;
	TLabel *Label34;
	TLabel *Label38;
	TLabel *Label39;
	TLabel *Label40;
	TLabel *Label41;
	TLabel *Label42;
	TLabel *Label43;
	TLabel *Label44;
	TLabel *Label45;
	TButton *OpenDisplaysButton;
	TButton *CloseDisplaysButton;
	TProgressBar *ProgressBar1;
	TComboBox *PhaseComboBox;
	TLabel *Label36;
	TLabeledEdit *xOffsetEdit;
	TLabeledEdit *yOffsetEdit;
	TButton *RefreshStreamsButton;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);

	/**
	 * When the gridpanel is clicked, determine which button what clicked.
	 * Create a hotpsot of the appropriate type.
	 *
	 */
	void __fastcall GridPanel1Click(TObject *Sender);

	/**
	 * Add new location using phasespace.
	 *
	 */
	void __fastcall Button5Click(TObject *Sender);

	/**
	 * Save contents of Memo2 to location file.
	 */
	void __fastcall SaveLocClick(TObject *Sender);

	/**
	 * Load contents of Memo2 from location file.
	 */
	void __fastcall LoadLocClick(TObject *Sender);

	/**
	 * Create locations from points in Memo2.
	 * locations should be of form
	 * x1,y1,z1
	 * x2,y2,z2
	 */
	void __fastcall Button4Click(TObject *Sender);


	/**
	 * Save hotspots configuration to to xml file.
	 */
	void __fastcall SaveConfigButtonClick(TObject *Sender);

	/**
	 * Load hotspots configuration from xml file.
	 */
	void __fastcall LoadConfigButtonClick(TObject *Sender);

	/**
	 * Called when one of the dynamic buttons is clicked. Caused internal representation
	 * of hotspots to be updated from UI.
	 */
	void __fastcall ApplyChange(TObject *Sender);
	void __fastcall GazeComboBoxChange(TObject *Sender);
	void __fastcall LoadCalibrationClick(TObject *Sender);
	void __fastcall OpenDisplaysButtonClick(TObject *Sender);
	void __fastcall CloseDisplaysButtonClick(TObject *Sender);
	void __fastcall PhaseComboBoxChange(TObject *Sender);
	void __fastcall xOffsetEditChange(TObject *Sender);
	void __fastcall yOffsetEditChange(TObject *Sender);
	void __fastcall RefreshStreamsButtonClick(TObject *Sender);
private:	// User declarations
	int FindMmfBuffer();
	pugi::xml_document doc;
	AnsiString streamIdentifier;

public:		// User declarations

	/**
	 * Update internal representation of hotspots with data from UI.
	 * Update data stream xml.
	 */
	void ApplyChanges();

	/**
	 * Clear all hotspot and location gui elements.
	 */
	void clear();
	void AddHotspot(TPoint3D * p3D, int id);


	void GenerateXMLHeader();

	/**
	 * Load configuration from XML file.
	 */
	void LoadConfig(const System::UnicodeString FileName);

	/**
	 * Add touch hotspot
	 */
	void addTouch(TPoint3D * p3D, int size, int device, int sensor);

	/**
	 * Add Point To hotspot.
	 */
	void addPointTo(TPoint3D * p3D, int size, int device, int sensor1, int sensor2);

	/**
	 * Add Direction Hotspot. This feature has not been used lately. It has something
	 * to do with the optitrack system.
	 */
	void addDirection(TPoint3D * p3D,  int r, int device, int sensor1, int sensor2, int dist_mm);

	/**
	 * Add rectangular hotspot. This is done when a hotgrid is made.
	 */
	void addRectangular(THotspotGrid *hotspotGrid, double x, double y, double z, double Xthickness, double Ythickness, double Zthickness, int sensor, int device);

	void addScreen(int topLeft, int topRight, int bottomLeft, int bottomRight,
			int sensor0, int sensor1, int device, double monitorDepth, int x, int y, int width, int height);

	void UpdateInfo();

	__fastcall TForm11(TComponent* Owner);

};

extern PACKAGE TForm11 *Form11;
#endif

