#ifndef __VARIANTS_H__
#define __VARIANTS_H__

extern volatile __byte_type  	flag0_time;//时间标志
#define flag0_time_Byte			flag0_time.u8
#define fg_time_2ms				flag0_time.bits.b0
#define fg_time_100ms			flag0_time.bits.b1
#define fg_time_1s				flag0_time.bits.b2
#define fg_time_3s				flag0_time.bits.b3
#define fg_time_10s				flag0_time.bits.b4
#define fg_time_test			flag0_time.bits.b5//for test.
//#define fg_time_test2			flag0_time.bits.b6//
//#define fg_time_test3			flag0_time.bits.b7//


extern volatile __byte_type  	flag1;// 标志
#define flag1_Byte				flag1.u8
#define fg_led_timing			flag1.bits.b0//LED显示闪烁定时,=1 正在进行闪烁定时; =0 不进行闪烁定时.
#define fg_led_delay			flag1.bits.b1//延时标志:=0进入延时; =1延时完成.
#define fg_led_flash			flag1.bits.b2//控制LED闪烁，=1 LED全灭; =0 中断正常扫描LED.
#define fg_led_change			flag1.bits.b3//控制LED闪烁时显示体重和体脂率轮流显示.
//#define 						flag1.bits.b4
//#define     					flag1.bits.b5
//#define 						flag1.bits.b6
//#define 						flag1.bits.b7

extern volatile __byte_type  	flag2;// 标志
#define flag2_Byte				flag2.u8
#define fg_loadok				flag2.bits.b0//控制完成一次称重后是否执行闪烁.
#define fg_uart_rec_start		flag2.bits.b1//=1串口可以开始接收数据; =0 进行判断是否接收数据.
#define fg_uart_rec_end			flag2.bits.b2//=1串口接收完成
#define fg_bodyfatrate_rec_ok   flag2.bits.b3//=1串口接收到体脂率; =0 没接收到体脂率.
//#define 						flag2.bits.b4
//#define 						flag2.bits.b5
//#define 						flag2.bits.b6
//#define 						flag2.bits.b7


/*flag3_remenber 睡眠时不能清除,需要记忆.*/
extern volatile __byte_type  	flag3_remenber;// 标志
#define flag3_remenber_Byte		flag3_remenber.u8
#define fg_remember_200g		flag3_remenber.bits.b0////30s中内两次称重相差200g,显示上一次的值.
//#define 			flag3_remenber.bits.b1
//#define 			flag3_remenber.bits.b2
//#define       	flag3_remenber.bits.b3
//#define 			flag3_remenber.bits.b4
//#define 			flag3_remenber.bits.b5
//#define 			flag3_remenber.bits.b6
//#define 			flag3_remenber.bits.b7


extern u8 gu8v_time_100ms;
extern u8 R_UartData;
extern u8 gu8v_time_30s;

extern const unsigned char lu8v_LED_HEX[C_LED_CHAR_NUM];
//由于硬件2COM的LED连接方式与另外三个COM不一样，所以需要再建个不同的显示数字表�?
extern const unsigned char lu8v_2COM_HEX[16];

#endif //__VARIANTS_H__
