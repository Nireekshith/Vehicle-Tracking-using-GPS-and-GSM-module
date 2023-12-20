   #include <reg52.h>
#include<string.h>
#include<stdio.h>
#include<intrins.h>
#define BUFF_LEN 50
#define lcd P0

sbit rs=P1^5; 
sbit rw=P1^6;
sbit en=P1^7;
sbit lcd_bsy = P0^7;
////////////////////////////////////////////////////////////////////////////

sbit SW1  = P1^0;

/////////////////////////////////////////////////////////////////////////////  
void init_lcd(void);
void cmd_lcd(unsigned char);
void lcd_data(unsigned char);
void display_lcd(unsigned char *);
void LCD_Busy_Chk(void);


void delay_ms(unsigned int);


/////////////////////////////////////////////////////////////////////////////////////////
void GSM_INIT(void);
void SEND_SMS_GPS(unsigned char *);
void RECEIVE_GPS(void);
void RECEIVE_GPGLL(unsigned long);
unsigned char RECEIVE_CHR(unsigned long);
void SEND_CHR(unsigned char);
void SEND_STR(unsigned char *);
unsigned int i=0,j=0;
unsigned char REC_BUFF[BUFF_LEN];
void serial_send(unsigned char);
void SEND_STR(unsigned char *);
////////////////////////////////////////////////////////////////////////////
void main (void)
{

	TMOD = 0x20;
	SCON = 0x50;
	TH1  = 0xFD;
	TR1  = 1;
	IE   = 0x90;


	init_lcd();
	delay_ms(100);
	cmd_lcd(0x80);
	display_lcd("GPS REEIVER");
	cmd_lcd(0xc0);				   
	display_lcd("INFORMATION ");
	delay_ms(500);

	GSM_INIT();	
	delay_ms(500);
	cmd_lcd(0x01);	

while(1)
	{
	if(SW1==0)
		{
		
		cmd_lcd(0x01);
		cmd_lcd(0x80);
		display_lcd("DATA RECVING");
		RECEIVE_GPGLL(1000);
		cmd_lcd(0x01);
		RECEIVE_GPS();
		delay_ms(500);
		cmd_lcd(0x01);
		display_lcd("SENDING DATA...");
		i=1;
		while((REC_BUFF[i++]!=',')||(j++<1));
		j=0;
		SEND_STR("LT:");
		while((REC_BUFF[i]!=',')||(j++<1))
		SEND_CHR(REC_BUFF[i++]);
		SEND_STR(" LN:");
		i++;j=0;
		while((REC_BUFF[i]!=',')||(j++<1))
		SEND_CHR(REC_BUFF[i++]);
	
		delay_ms(1000);
		
		}
	
	}
}
//////////////////////////////////////////////////////////////////////////////
void GSM_INIT(void)
{
	cmd_lcd(0xC0);
	display_lcd("GSM INITIALIZING");
}
/////////////////////////////////////////////////////////////////////////////
void RECEIVE_GPS(void)
{
	unsigned int i=0,j=0;
	i=1;
	
	while((REC_BUFF[i++]!=',')||(j++<1));
	j=0;
	cmd_lcd(0X80);
	display_lcd("LT:");
	
	while((REC_BUFF[i]!=',')||(j++<1))
	lcd_data(REC_BUFF[i++]);
	//input=REC_BUFF[i++];
	cmd_lcd(0XC0);
	display_lcd("LN:");
	
	i++;j=0;
	while((REC_BUFF[i]!=',')||(j++<1))
	lcd_data(REC_BUFF[i++]);
	//nput=REC_BUFF[i++];
}
///////////////////////////////////////////////////////////////////////////////////
void RECEIVE_GPGLL(unsigned long timeout)
{      
	unsigned int i=0;
	char loop = 1;
	while(loop)
	{
	while(RECEIVE_CHR(timeout) != '$');
	if(RECEIVE_CHR(timeout) == 'G')
	{
	if(RECEIVE_CHR(timeout) == 'P')
	{
	if(RECEIVE_CHR(timeout) == 'R')
	{
	if(RECEIVE_CHR(timeout) == 'M')
	{
	if(RECEIVE_CHR(timeout) == 'C')
	{
	i = 0;
	do
	{
		REC_BUFF[i] =
		RECEIVE_CHR(timeout);
		if(i > BUFF_LEN-1) break;
	}
	while((REC_BUFF[i++]!=0x0D)&&(i < (BUFF_LEN-2)));
		REC_BUFF[i-1] = 0x00; loop = 0;
	}
		else loop = 1;
	}
		else loop = 1;
	}
		else loop = 1;
	}
		else loop = 1;
	}
		else loop = 1;
	}
}
////////////////////////////////////////////////////////////////////////////////////
unsigned char RECEIVE_CHR(unsigned long timeout)
{
	unsigned char temp=0x00;
	while((!RI)&&(--timeout > 0));
	if(timeout)
	temp = SBUF;
	RI = 0;
	return temp;
}
//////////////////////////////////////////////////////////////////////////
void SEND_STR(unsigned char *s)
{
	while(*s)
	SEND_CHR(*s++);
}
void SEND_CHR(unsigned char c)
{
	SBUF = c;
	while(TI == 0);
	TI = 0;
}

//////////////////////////////////////////////////////////////////////////////////
void init_lcd(void)
{
	cmd_lcd(0x30);
	cmd_lcd(0x30);
	cmd_lcd(0x38);
	cmd_lcd(0x0c);
	cmd_lcd(0x06);
	cmd_lcd(0x01);
	delay_ms(10);
}
void cmd_lcd(unsigned char c)
{
 LCD_Busy_Chk();

	lcd=c;
	rs=0;
	rw=0;
	en=1;
	_nop_();
	en=0;
}
void lcd_data(unsigned char c)
{
	LCD_Busy_Chk();
	
	lcd=c;
	rs=1;
	rw=0;
	en=1;
	_nop_();
	en=0;	
}
void LCD_Busy_Chk(void)
{
		lcd_bsy=1;
		rs=0;
		rw=1;
	while(lcd_bsy==1)
		{
			en=0;
			_nop_();
		    _nop_();
			_nop_();
		    _nop_();
			en=1;
		}
}
void display_lcd(unsigned char *s)
{
	while(*s)
	lcd_data(*s++);
}
/////////////////////////////////////////////////////////////////////
void delay_ms(unsigned int i)
{
unsigned int j;
	while(i-->0)
	{
		for(j=0;j<500;j++)
		{
			;
		}
	}
} 