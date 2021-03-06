asm(" message' **************************************************** ' ");
asm(" message' *       2018 BY BEST HEALTH ELECTRONIC INC         * ' ");
asm(" message' *__________________________________________________* ' ");
asm(" message' *        SDK  NAME  : 	SDK_Impedance.c	          * ' ");
asm(" message' *   Compiler   Ver. :      V3.52                   * ' ");
asm(" message' *   IDE3000    Ver. :      V7.96                   * ' ");
asm(" message' *   RELEASE    Ver. :      1.0.2                   * ' ");
asm(" message' *   RELEASE   DATA  :     2018/06/22               * ' ");
asm(" message' *__________________________________________________* ' ");
asm (" message' *    MCU / CFG Ver. :   HT45F75 / 1.6             * ' ");
asm (" message' *                       HT45F77 / 2.2             * ' ");
asm(" message' **************************************************** ' ");
#include "SDK_Interface.h"
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// ====================================================================================@
//                                  功能說明                                            @
// ====================================================================================@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*
1. 8電極阻抗量測都可以測量
3. 頻率依據外部加載的頻率測量阻抗
2. 阻抗量測兩次,依據兩次差值判斷結果
3. 可選雙手/雙腳並聯判斷是否有人
4. 不同錯誤類型輸出不同的提示
5. 阻抗測量範圍0~1200ohm
*/
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// ====================================================================================@
//                                  封庫設置                                            @
// ====================================================================================@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#include "HT45F75.h"
#define ImpedanceADCFilterUseBit 16 // 20 or 16 重量 ADC 數據使用的Bit數
// 玄波發送器開關
#define SET_BODYFAT_CIRCUIT_ON()        { _opaen =1; _sgen = 1; _bren = 1; _ftren = 1; _hysen = 1;}
#define SET_BODYFAT_CIRCUIT_OFF()       { _opaen=0;  _sgc  = 0; _ftrc = 0;}
// OPA 放大倍數設定
#define	SET_BODYFAT_OPAGAIN_1()			{ _opac = 0x00; _daco = 0x3f;}
#define	SET_BODYFAT_OPAGAIN_1_14()		{ _opac = 0x01; _daco = 0x3f;}
#define	SET_BODYFAT_OPAGAIN_1_31()		{ _opac = 0x02; _daco = 0x3f;}
#define	SET_BODYFAT_OPAGAIN_1_5()		{ _opac = 0x03; _daco = 0x3f;}
#define	SET_BODYFAT_OPAGAIN_1_73()		{ _opac = 0x04; _daco = 0x3f;}
#define	SET_BODYFAT_OPAGAIN_2()			{ _opac = 0x05; _daco = 0x3f;}
#define	SET_BODYFAT_OPAGAIN_2_33()		{ _opac = 0x06; _daco = 0x3f;}
#define	SET_BODYFAT_OPAGAIN_2_75()		{ _opac = 0x07; _daco = 0x3f;}
#define	SET_BODYFAT_OPAGAIN_3_285()		{ _opac = 0x08; _daco = 0x3f;}
#define	SET_BODYFAT_OPAGAIN_4()			{ _opac = 0x09; _daco = 0x3f;}
#define	SET_BODYFAT_OPAGAIN_5() 		{ _opac = 0x0A; _daco = 0x3f;}
// 體阻抗開關列表
#define SET_BODYFAT_SWC_RF200()     { _swc = 0B00001110;}  // 200
#define SET_BODYFAT_SWC_RF1000()    { _swc = 0B00110010;}	// 1000
#define SET_BODYFAT_SWC_TWOLEGS1K() { _swc = 0B11110011;}  // 1000 & TwoLegs
#define SET_BODYFAT_SWC_TWOLEGS()   { _swc = 0B11000001;}  // TwoLegs
// ADC 倍率設置
#define SET_ADCGAIN_IMPENDACNE()    { _pgac0 = 0x20;}   // 阻抗ADC放大設置,VGS=0.5,ADGN =1,PGA=1
#define SET_DCSET_IMPENDACNE()      { _pgac1 = 0x00;}   // 阻抗DCSET設置,DCSET = 0V
#define SET_ADCCHAN_IMPENDACNE()    { _pgacs = 0x61;}   // 阻抗ADC 通道VCM&AN2

//#define SET_ADCCHAN_IMPENDACNE()    { _pgacs = 0x68;}   // 阻抗ADC 通道VCM&RFC
#define SET_ADCVREF_IMPEDANCE()	    { _vrefs = 1;_enldo=1;_envcm=1;_vcms=1; }     // 外部參考電壓
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// ====================================================================================@
//                                  依賴參數                                            @
// ====================================================================================@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void fun_ADCStart();
void fun_ADCStop();
void fun_Filtering();
void fun_FilterInit();
void fun_LoadImpedanceSetting();
unsigned int fun_unsigned32BitABS(unsigned int a, unsigned int b); // 取32位無符號差值
extern unsigned char BHSDKState;									  // RW   工作狀態讀取與切換,參考 BodyfatSDKState 枚舉
extern ADCFilter_t SDKADCFilterData;								  // 濾波ADC數據,詳細參考SDK_typedef.h ADCFilter_t

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// ====================================================================================@
//                                  對外參數                                            @
// ====================================================================================@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*
typedef struct
{
    struct
    {
        unsigned long Cal0;	    // R    ADC值 - 參考電阻1
        unsigned long Cal1;	    // R    ADC值 - 參考電阻2
        unsigned long CalRx0;	// R    ADC值 - 參考電阻1並聯Rx
        unsigned long CalRx;	// R    ADC值 - 待測電阻Rx
    }CalADC;
    unsigned char Channel;	    // RW   阻抗 - 待測通道 設置,參考impedance_channel枚舉
    unsigned int Data;          // R    阻抗 - 阻抗值,單位ohm. 若為(0xffff-無待測阻抗錯誤)/(0xFFF1-接觸異常錯誤)/(0xFFF2-阻抗超出範圍)
}Impedance_t;
*/
extern Impedance_t SDKImpedance;
void fun_Enter_Impedance();
void fun_Impedance();
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// ====================================================================================@
//                                  源代碼                                             @
// ====================================================================================@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Impedance_t SDKImpedance;
volatile unsigned int gu16v_LastImpedance_ohm; // 上一筆阻抗值,單位為ohm. 若為0xffff,表示阻抗量測失敗
volatile bit gbv_GetImpedanceOnce;			   // 得到第一次阻抗值
volatile unsigned int *ImpedancePointer; // 指針使用特別注意範圍,防止越界更改值！！！

/**************************************
Function: 阻抗測量硬件配置
INPUT	:
OUTPUT	:
NOTE	:
***************************************/
void fun_ImpedanceHardwareSetting()
{
	// ADC
	SET_ADCCHAN_IMPENDACNE();
	SET_ADCGAIN_IMPENDACNE();
	SET_DCSET_IMPENDACNE();
	SET_ADCVREF_IMPEDANCE();
	// BodyFat
	SET_BODYFAT_OPAGAIN_2();
	SET_BODYFAT_CIRCUIT_ON();
}
/********************************************************************
Function: 得到阻抗值
INPUT	:
OUTPUT	:
NOTE	:
********************************************************************/
unsigned int fun_UseAdcGetImpedance()
{
	unsigned int ImpedanceOhm;
	if (SDKImpedance.CalADC.CalRx > SDKImpedance.CalADC.Cal1)
	{
		ImpedanceOhm = 200 + 800 * (unsigned long)(SDKImpedance.CalADC.CalRx - SDKImpedance.CalADC.Cal1) / (SDKImpedance.CalADC.Cal0 - SDKImpedance.CalADC.Cal1);
	}
	else
	{
		ImpedanceOhm = 200 - 800 * (unsigned long)(SDKImpedance.CalADC.Cal1 - SDKImpedance.CalADC.CalRx) / (SDKImpedance.CalADC.Cal0 - SDKImpedance.CalADC.Cal1);
	}
	return ImpedanceOhm;
}
/********************************************************************
Function: 阻抗量測模式
INPUT	:
OUTPUT	:
NOTE	: // 考慮實際測試是否因為時間比較長
********************************************************************/
void fun_Impedance()
{
	fun_Filtering();
	if (SDKADCFilterData.flag.b.IsStable)
	{
		*ImpedancePointer = SDKADCFilterData.Current;
		ImpedancePointer++;
		fun_FilterInit();
		switch (BHSDKState)
		{
		case STATE_IMPEDANCE_REFERENCE1:
			BHSDKState++;
			SET_BODYFAT_SWC_RF200();
			break;
		case STATE_IMPEDANCE_REFERENCE2:
			BHSDKState++;
			SET_BODYFAT_SWC_TWOLEGS1K();
			break;
		case STATE_IMPEDANCE_CHECKBODY:
			BHSDKState++;
			SET_BODYFAT_SWC_TWOLEGS();
#if ImpedanceADCFilterUseBit == 16
			if (fun_unsigned32BitABS(SDKImpedance.CalADC.CalRx0, SDKImpedance.CalADC.Cal0) < 300)
#endif
#if ImpedanceADCFilterUseBit == 20
			if (fun_unsigned32BitABS(SDKImpedance.CalADC.CalRx0, SDKImpedance.CalADC.Cal0) < 2000)
#endif
			{
				fun_ADCStop();
				SET_BODYFAT_CIRCUIT_OFF();
				SDKImpedance.Data = 0xFFFF;
				BHSDKState = STATE_IMPEDANCE_FINISH;
			}
			//ImpedancePointer--;
			break;
		case STATE_IMPEDANCE_RX:
			if (!gbv_GetImpedanceOnce)
			{
				gbv_GetImpedanceOnce = 1;
				gu16v_LastImpedance_ohm = fun_UseAdcGetImpedance(); // 當前阻抗值保存
				// 切換倍率,兩次測量排除異常值
				SET_BODYFAT_OPAGAIN_1_14();
				SET_BODYFAT_SWC_RF1000();
				SET_BODYFAT_CIRCUIT_ON(); // 使能SIN波輸出
				ImpedancePointer = &SDKImpedance.CalADC.Cal0;
				BHSDKState = STATE_IMPEDANCE_REFERENCE1;
			}
			else
			{
				if (fun_unsigned32BitABS(gu16v_LastImpedance_ohm, fun_UseAdcGetImpedance()) > 15)
				{
					SDKImpedance.Data = 0xFFF1;
				}
				else
				{
					SDKImpedance.Data = (gu16v_LastImpedance_ohm + fun_UseAdcGetImpedance()) / 2;
					if (SDKImpedance.Data > 1200) // 阻抗測試錯誤時
					{
						SDKImpedance.Data = 0xfff2;
					}
				}
				fun_ADCStop();
				SET_BODYFAT_CIRCUIT_OFF();
				BHSDKState = STATE_IMPEDANCE_FINISH;
			}
			break;
		default:
			break;
		}
	}
}
void fun_Enter_Impedance()
{
	BHSDKState = STATE_IMPEDANCE_REFERENCE1;
	ImpedancePointer = &SDKImpedance.CalADC.Cal0;
	gbv_GetImpedanceOnce = 0;
	SDKImpedance.Data = 0;
	fun_LoadImpedanceSetting();
	fun_ImpedanceHardwareSetting(); // 玄波設置
	SET_BODYFAT_SWC_RF1000();
	fun_ADCStart();
	fun_FilterInit();
}
void fun_ImpedancePowerDown()
{
	SET_BODYFAT_CIRCUIT_OFF();
}