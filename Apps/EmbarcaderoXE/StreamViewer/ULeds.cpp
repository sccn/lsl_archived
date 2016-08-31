//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ULeds.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormLEDs *FormLEDs;
//---------------------------------------------------------------------------
__fastcall TFormLEDs::TFormLEDs(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormLEDs::FormCreate(TObject *Sender)
{
	CheckBoxLED[0]=CheckBox1;
	CheckBoxLED[1]=CheckBox2;
	CheckBoxLED[2]=CheckBox3;
	CheckBoxLED[3]=CheckBox4;
	CheckBoxLED[4]=CheckBox5;
	CheckBoxLED[5]=CheckBox6;
	CheckBoxLED[6]=CheckBox7;
	CheckBoxLED[7]=CheckBox8;
	CheckBoxLED[8]=CheckBox9;
	CheckBoxLED[9]=CheckBox10;
	CheckBoxLED[10]=CheckBox11;
	CheckBoxLED[11]=CheckBox12;
	CheckBoxLED[12]=CheckBox13;
	CheckBoxLED[13]=CheckBox14;
	CheckBoxLED[14]=CheckBox15;
	CheckBoxLED[15]=CheckBox16;
	CheckBoxLED[16]=CheckBox17;
	CheckBoxLED[17]=CheckBox18;
	CheckBoxLED[18]=CheckBox19;
	CheckBoxLED[19]=CheckBox20;
	CheckBoxLED[20]=CheckBox21;
	CheckBoxLED[21]=CheckBox22;
	CheckBoxLED[22]=CheckBox23;
	CheckBoxLED[23]=CheckBox24;
	CheckBoxLED[24]=CheckBox25;
	CheckBoxLED[25]=CheckBox26;
	CheckBoxLED[26]=CheckBox27;
	CheckBoxLED[27]=CheckBox28;
	CheckBoxLED[28]=CheckBox29;
	CheckBoxLED[29]=CheckBox30;
	CheckBoxLED[30]=CheckBox31;
	CheckBoxLED[31]=CheckBox32;
	CheckBoxLED[32]=CheckBox33;
	CheckBoxLED[33]=CheckBox34;
	CheckBoxLED[34]=CheckBox35;
	CheckBoxLED[35]=CheckBox36;
	CheckBoxLED[36]=CheckBox37;
	CheckBoxLED[37]=CheckBox38;
	CheckBoxLED[38]=CheckBox39;
	CheckBoxLED[39]=CheckBox40;
	CheckBoxLED[40]=CheckBox41;
	CheckBoxLED[41]=CheckBox42;
	CheckBoxLED[42]=CheckBox43;
	CheckBoxLED[43]=CheckBox44;
	CheckBoxLED[44]=CheckBox45;
	CheckBoxLED[45]=CheckBox46;
	CheckBoxLED[46]=CheckBox47;
	CheckBoxLED[47]=CheckBox48;
	CheckBoxLED[48]=CheckBox49;
	CheckBoxLED[49]=CheckBox50;
	CheckBoxLED[50]=CheckBox51;
	CheckBoxLED[51]=CheckBox52;
	CheckBoxLED[52]=CheckBox53;
	CheckBoxLED[53]=CheckBox54;
	CheckBoxLED[54]=CheckBox55;
	CheckBoxLED[55]=CheckBox56;
	CheckBoxLED[56]=CheckBox57;
	CheckBoxLED[57]=CheckBox58;
	CheckBoxLED[58]=CheckBox59;
	CheckBoxLED[59]=CheckBox60;
	CheckBoxLED[60]=CheckBox61;
	CheckBoxLED[61]=CheckBox62;
	CheckBoxLED[62]=CheckBox63;
	CheckBoxLED[63]=CheckBox64;
	CheckBoxLED[64]=CheckBox65;
	CheckBoxLED[65]=CheckBox66;
	CheckBoxLED[66]=CheckBox67;
	CheckBoxLED[67]=CheckBox68;
	CheckBoxLED[68]=CheckBox69;
	CheckBoxLED[69]=CheckBox70;
	CheckBoxLED[70]=CheckBox71;
	CheckBoxLED[71]=CheckBox72;
	CheckBoxLED[72]=CheckBox73;
	CheckBoxLED[73]=CheckBox74;
	CheckBoxLED[74]=CheckBox75;
	CheckBoxLED[75]=CheckBox76;
	CheckBoxLED[76]=CheckBox77;
	CheckBoxLED[77]=CheckBox78;
	CheckBoxLED[78]=CheckBox79;
	CheckBoxLED[79]=CheckBox80;
	CheckBoxLED[80]=CheckBox81;
	CheckBoxLED[81]=CheckBox82;
	CheckBoxLED[82]=CheckBox83;
	CheckBoxLED[83]=CheckBox84;
	CheckBoxLED[84]=CheckBox85;
	CheckBoxLED[85]=CheckBox86;
	CheckBoxLED[86]=CheckBox87;
	CheckBoxLED[87]=CheckBox88;
	CheckBoxLED[88]=CheckBox89;
	CheckBoxLED[89]=CheckBox90;
	CheckBoxLED[90]=CheckBox91;
	CheckBoxLED[91]=CheckBox92;
	CheckBoxLED[92]=CheckBox93;
	CheckBoxLED[93]=CheckBox94;
	CheckBoxLED[94]=CheckBox95;
	CheckBoxLED[95]=CheckBox96;
	CheckBoxLED[96]=CheckBox97;
	CheckBoxLED[97]=CheckBox98;
	CheckBoxLED[98]=CheckBox99;
	CheckBoxLED[99]=CheckBox100;
	CheckBoxLED[100]=CheckBox101;
	CheckBoxLED[101]=CheckBox102;
	CheckBoxLED[102]=CheckBox103;
	CheckBoxLED[103]=CheckBox104;
	CheckBoxLED[104]=CheckBox105;
	CheckBoxLED[105]=CheckBox106;
	CheckBoxLED[106]=CheckBox107;
	CheckBoxLED[107]=CheckBox108;
	CheckBoxLED[108]=CheckBox109;
	CheckBoxLED[109]=CheckBox110;
	CheckBoxLED[110]=CheckBox111;
	CheckBoxLED[111]=CheckBox112;
	CheckBoxLED[112]=CheckBox113;
	CheckBoxLED[113]=CheckBox114;
	CheckBoxLED[114]=CheckBox115;
	CheckBoxLED[115]=CheckBox116;
	CheckBoxLED[116]=CheckBox117;
	CheckBoxLED[117]=CheckBox118;
	CheckBoxLED[118]=CheckBox119;
	CheckBoxLED[119]=CheckBox120;
	AnsiString str;
	for (int i=0;i<120;i++)
	{
		str.sprintf("LED%d",i);
		CheckBoxLED[i]->Caption=str;
		CheckBoxLED[i]->Width = 70;
		CheckBoxLED[i]->Checked=true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormLEDs::FormDeactivate(TObject *Sender)
{
  	Close();
}
//---------------------------------------------------------------------------
