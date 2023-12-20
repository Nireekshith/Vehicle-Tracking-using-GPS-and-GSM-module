#include<reg52.h>
#include<string.h>
#include<intrins.h>
#define lcd P0

sbit rs=P1^5; 
sbit rw=P1^6;
sbit en=P1^7;
sbit lcd_bsy = P0^7;

void lcd_init(void);    
void lcdcmd(unsigned char value);
void lcddata(unsigned char value);
void LCD_Busy_Chk(void);
void msgdisplay(unsigned char b[]);
void delay(unsigned int value);
                           
void gsm_init(void);
void modem_send(unsigned char *);
void enter(void);
void Readmessage(void);
void sendbyte(unsigned char *);
void sendslot(unsigned char);
unsigned char dat[10];
unsigned char number[15];		
unsigned char interruptno[13];
unsigned char LT[13];
unsigned char LG[13];
unsigned char LT1[13];
unsigned char LG1[13];
unsigned char SlotNo;
void gps(void);
void gps1(void);
sbit rl=P2^0;
sbit rfid=P2^7;
bit MsgRead;
void comp(void);

void send_sms(unsigned char *cmd1,unsigned char *cmd2,unsigned char *ch);
void send_sms1(unsigned char *cmd1,unsigned char *cmd2,unsigned char *ch1,unsigned char *ch2);
bit flag;

////////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{

SCON = 0x50;
TMOD = 0x20;
TH1 = -3;///////////////GSM 9600
TR1 = 1;

	
rl=0;rfid=0; 
lcd_init();  lcd_init();  lcd_init();
lcdcmd(0x01);
lcdcmd(0x80);
msgdisplay("VEHICLE TRACKING"); 
delay(100);
lcdcmd(0xC0);
msgdisplay("USING GSM & GPS"); 
delay(200);
delay(200);delay(200);delay(200);delay(200);
rl=0;
gsm_init();
EA=1;
ES=1;
MsgRead=0;
while(1) 	
{
TH1=-3;
rl=0;
if(MsgRead)
{
EA=0;
Readmessage();
MsgRead=0;
EA=1;
ES=1;
}
if(rfid==1)
{
lcdcmd(0x01);
lcdcmd(0x80);
msgdisplay("CARD DETECTED"); 
delay(500);
///////////////
rl=1;
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("connecting to GPS");
gps1();
////////////////
rl=0;
delay(500);
lcdcmd(0x01);
lcdcmd(0x80);
msgdisplay("SENDING MSG..."); 
delay(500);
sendbyte("AT+CMGS=");
sendslot(0x22);
sendbyte("9966066600");
sendslot(0x22);
enter();
sendbyte("VEHICLE POSITION AT");
enter();
sendbyte("CARD NUMBER:0123");
enter();
sendbyte("LT:");
sendbyte(LT1);
delay(200);
enter();
sendbyte("LG:");
sendbyte(LG1);
delay(200);
enter();		   
sendslot(0x1A);
sendslot(0x1A);
delay(500);
lcdcmd(0x01);
lcdcmd(0x80);
msgdisplay("MSG SENT......"); 
delay(200);delay(200);delay(200);
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("WAITING FOR SMS");
delay(200);delay(200);delay(200);
}
}
}
//////////////////////////////////////////////////////////////////
void serialintr(void) interrupt 4
{
unsigned char i;
if(SBUF == '+')
{
EA=0;
ES=0;
RI=0;
for(i=0;i<11;i++)
{
while(!RI);
interruptno[i]=SBUF;
RI=0;
}
while(!RI);
SlotNo=SBUF;
RI=0;
MsgRead=1;
}
RI=0;
}
////////////////////////////////////////////////////////////////
void gsm_init(void)
{
rl=0;
modem_send("AT");
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("AT");
modem_send("ATE0");
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("ATE0");
modem_send("AT+CSMS=0");
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("AT+CSMS=0");
modem_send("AT+IPR=9600");
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("AT+IPR=9600");
modem_send("AT+CMGF=1");
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("AT+CMGF=1");		
modem_send("AT&W");
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("AT&W");
modem_send("AT+CNMI=2,1,0,0,0");
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("AT+CNMI=2,1,0,0,0");
lcdcmd(0x01);lcdcmd(0x80);
modem_send("AT+CMGD=1");modem_send("AT+CMGD=2");modem_send("AT+CMGD=3");modem_send("AT+CMGD=4");
delay(200);delay(200);
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("GSM Initialized");
delay(200);delay(200);
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("WAITING FOR SMS");
delay(200);delay(200);
}
//////////////////////////////////////////////////////////////////////////
void modem_send(unsigned char *ch)				
{
unsigned char cha;
while(*ch)
{
SBUF = *ch;
while(TI == 0);
TI = 0;
ch++;
}	
enter();
TI = 0;
RI = 0;
while(1)
{
while(RI == 0);
RI = 0;
cha = SBUF;
if(cha == 'O');
while(RI == 0);
RI = 0;
cha = SBUF;
if(cha == 'K')		
break;
SBUF = 'A';
while(TI == 0);
TI = 0;
SBUF = '/';
while(TI == 0);
TI = 0;
}
TI = 0;
RI = 0;
}
//////////////////////////////////////////////////////////////////////////
void enter(void)
{
SBUF = 0x0a;
while(TI == 0);
TI = 0;
SBUF = 0x0d;
while(TI == 0);
TI = 0;
}
////////////////////////////////////////////////////////////////////////
void sendbyte(unsigned char *str)
{
EA=0;
while(*str!='\0')
{
TI=0;
SBUF=(*str++);
while(!TI);
TI=0;
}
}
//////////////////////////////////////////////////////////////////
void sendslot(unsigned char slot)
{
EA=0;
TI=0;
SBUF=slot;
while(!TI);
TI=0;
}
//////////////////////////////////////////////////////////////
void Readmessage(void) 
{
unsigned char *no;
unsigned char i,cha='X';
*no=SlotNo;
lcdcmd(0x01);lcdcmd(0x80);
delay(200);delay(200);
msgdisplay("Message Received");
delay(200);delay(200);delay(200);delay(200);
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("Slot No:");
msgdisplay(no);
delay(200);delay(200);delay(200);delay(200);delay(200);delay(200);
lcdcmd(0x01);
lcdcmd(0x80);
msgdisplay("Message Reading");
delay(200);delay(200);delay(200);delay(200);delay(200);delay(200);
sendbyte("AT+CMGR=");
sendslot(SlotNo);
enter();
while(SBUF!=':')
{
while(!RI);
RI=0;
}
while(SBUF!=',')
{
while(!RI);
RI=0;
}	
while(SBUF!=',')
{
while(!RI);
RI=0;
}
for(i=0;i<4;i++)
{
while(!RI);
RI=0;
}
for(i=0;i<10;i++)
{
while(!RI);
number[i] = SBUF;
RI=0;
}
number[i]='\0';
while(SBUF!='+')
{
while(!RI);
RI=0;
}
for(i=0;i<5;i++)
{
while(!RI);
RI=0;
}
for(i=0;i<5;i++)
{
while(!RI);
dat[i]=SBUF;
RI=0;
}
dat[i] = '\0';
while(1)
{
while(RI == 0);
cha = SBUF;
RI = 0;
if(cha == 'O');
while(RI == 0);
cha = SBUF;
RI = 0;
if(cha=='K')
break;
}	
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("Message Showing");
delay(200);delay(200);delay(200);delay(200);delay(200);delay(200);
lcdcmd(0x01);
lcdcmd(0x80);
msgdisplay(number);
delay(200);delay(200);delay(200);delay(200);
lcdcmd(0x01);
lcdcmd(0x80);
msgdisplay(dat);
delay(200);delay(200);delay(200);delay(200);delay(200);delay(200);
comp();
}
//////////////////////////////////////////////////////////////////////////////////////////////
void send_sms1(unsigned char *cmd1,unsigned char *cmd2,unsigned char *ch1,unsigned char *ch2)				
{
unsigned char cha='X',P;
while(*cmd1)
{
RI=0;			
TI=0;
SBUF=*cmd1;
while(TI==0);
TI=0;
RI=0;
cmd1++;
}
for(P=0;P<10;P++)
{
RI=0;
TI=0;
SBUF=number[P];
while(TI==0);
TI=0;
RI=0;
}
sendbyte(cmd2);
enter();
while(cha!='>')
{
while(RI==0);
cha=SBUF;
RI=0;
}
RI=0;
TI=0;
enter();

SBUF='V';while(TI==0);TI=0;
SBUF='E';while(TI==0);TI=0;
SBUF='H';while(TI==0);TI=0;
SBUF='I';while(TI==0);TI=0;
SBUF='C';while(TI==0);TI=0;
SBUF='L';while(TI==0);TI=0;
SBUF='E';while(TI==0);TI=0;

SBUF=' ';while(TI==0);TI=0;
SBUF=' ';while(TI==0);TI=0;
SBUF='P';while(TI==0);TI=0;
SBUF='O';while(TI==0);TI=0;
SBUF='S';while(TI==0);TI=0;
SBUF='I';while(TI==0);TI=0;
SBUF='T';while(TI==0);TI=0;
SBUF='I';while(TI==0);TI=0;
SBUF='O';while(TI==0);TI=0;
SBUF='N';while(TI==0);TI=0;
SBUF=' ';while(TI==0);TI=0;
SBUF=' ';while(TI==0);TI=0;
SBUF='A';while(TI==0);TI=0;
SBUF='T';while(TI==0);TI=0;
enter();
////////////////////////////////////////////////////////////////////////////
SBUF='#';
while(TI==0);
TI=0;
SBUF='L';
while(TI==0);
TI=0;
SBUF='A';
while(TI==0);
TI=0;
SBUF='T';
while(TI==0);
TI=0;
enter();
///////////////////////
for(P=0;P<11;P++)
{
RI=0;
TI=0;
SBUF=*ch1;
while(TI==0);
TI=0;
RI=0;
ch1++;
}
RI=0;
TI=0;
enter();
////////////////////////////////////////////////////////////
SBUF='#';
while(TI==0);
TI=0;
SBUF='L';
while(TI==0);
TI=0;
SBUF='N';
while(TI==0);
TI=0;
SBUF='G';
while(TI==0);
TI=0;
enter();
////////////////////////
for(P=0;P<12;P++)
{
RI=0;
TI=0;
SBUF=*ch2;
while(TI==0);
TI=0;
RI=0;
ch2++;
}
RI=0;
TI=0;
SBUF=0x1a;
while(TI==0);
TI=0;
RI=0;
}
/////////////////////////////////////////////////////////////
void comp(void)
{
if(!strcmp(dat,"TRACK"))
{
rl=1;
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("connecting to GPS");
gps();
}
else
{
lcdcmd(0x01); lcdcmd(0x80);
msgdisplay("Invalid command");
delay(200);delay(200);delay(200);delay(200);
delay(200);delay(200);
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("WAITING FOR SMS");

}
}
/////////////////////////////////////////////////////////////////
void gps(void)
{
unsigned char gp='\0',t;	
rl=1;
TH1=-3;	//////////////GPS  4800
while(gp!='$')
{
while(RI==0);
gp=SBUF;
RI=0;
}
gp='\0';
while(gp!='G')
{
while(RI==0);
gp=SBUF;
RI=0;
}
gp='\0';
while(gp!='P')
{
while(RI==0);
gp=SBUF;
RI=0;
}
gp='\0';
while(gp!='R')
{
while(RI==0);
gp=SBUF;
RI=0;
}
gp='\0';
while(gp!='M')
{
while(RI==0);
gp=SBUF;
RI=0;
}
gp='\0';
while(gp!='C')
{
while(RI==0);
gp=SBUF;
RI=0;
} 
while(RI==0);
RI=0;				
for(t=0;t<13;t++)
{
while(RI==0);
RI=0;
}
for(t=0;t<11;t++)
{
while(RI==0);
LT[t]=SBUF;
RI=0;				
}
LT[t]='\0';
while(RI==0);
RI=0;
for(t=0;t<12;t++)
{
while(RI==0);
LG[t]=SBUF;
RI=0;				
}
LG[t]='\0';
for(t=0;t<11;t++)
{
while(RI==0);
RI=0;
}
for(t=0;t<6;t++)
{
while(RI==0);
RI=0;				
}
lcdcmd(0x01);lcdcmd(0x80);	   
msgdisplay("LT:");
lcdcmd(0x83);
msgdisplay(LT);
lcdcmd(0xC0);	   
msgdisplay("LG:");
lcdcmd(0xC3);	
msgdisplay(LG);
delay(200);delay(200);delay(200);
delay(200);delay(200);delay(200);
rl=0;TH1=-3;TI=0;RI=0;
send_sms1("AT+CMGS=\"","\"",LT,LG);
delay(200);delay(200);delay(200);
modem_send("AT+CMGD=1");
TI=0;RI=0;
delay(200);delay(200);delay(200);
delay(200);delay(200);delay(200);
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("SMS SENT");
delay(200);delay(200);delay(200);
lcdcmd(0x01);lcdcmd(0x80);
msgdisplay("WAITING FOR SMS");
delay(200);delay(200);delay(200);
}
///////////////////////////////////////////////////////////////////////////
void gps1(void)
{
unsigned char gp='\0',z;	
rl=1;
TH1=-3;	//////////////GPS  9600
while(gp!='$')
{
while(RI==0);
gp=SBUF;
RI=0;
}
gp='\0';
while(gp!='G')
{
while(RI==0);
gp=SBUF;
RI=0;
}
gp='\0';
while(gp!='P')
{
while(RI==0);
gp=SBUF;
RI=0;
}
gp='\0';
while(gp!='R')
{
while(RI==0);
gp=SBUF;
RI=0;
}
gp='\0';
while(gp!='M')
{
while(RI==0);
gp=SBUF;
RI=0;
}
gp='\0';
while(gp!='C')
{
while(RI==0);
gp=SBUF;
RI=0;
} 
while(RI==0);
RI=0;				
for(z=0;z<13;z++)
{
while(RI==0);
RI=0;
}
for(z=0;z<11;z++)
{
while(RI==0);
LT1[z]=SBUF;
RI=0;				
}
LT1[z]='\0';
while(RI==0);
RI=0;
for(z=0;z<12;z++)
{
while(RI==0);
LG1[z]=SBUF;
RI=0;				
}
LG1[z]='\0';
for(z=0;z<11;z++)
{
while(RI==0);
RI=0;
}
for(z=0;z<6;z++)
{
while(RI==0);
RI=0;				
}
lcdcmd(0x01);lcdcmd(0x80);	   
msgdisplay("LT:");
lcdcmd(0x83);
msgdisplay(LT1);
lcdcmd(0xC0);	   
msgdisplay("LG:");
lcdcmd(0xC3);	
msgdisplay(LG1);
delay(200);delay(200);delay(200);
delay(200);delay(200);delay(200);
}
///////////////////////////////////////////////////////////////////////////
void lcd_init(void)
{

lcdcmd(0x38);
lcdcmd(0x38);
lcdcmd(0x0c);
lcdcmd(0x06);
lcdcmd(0x01);
lcdcmd(0X80);
delay(5);
}
//////////////////////////////////////////////////////////////////////////
void lcdcmd(unsigned char value)		   // LCD COMMAND
{
 LCD_Busy_Chk();

	lcd=value;
	rs=0;
	rw=0;
	en=1;
	_nop_();
	en=0;
}  
/////////////////////////////////////////////////////////////////////////	
void lcddata(unsigned char value)
{
	LCD_Busy_Chk();
	
	lcd=value;
	rs=1;
	rw=0;
	en=1;
	_nop_();
	en=0;
}  
/////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////
void msgdisplay(unsigned char b[])
{
unsigned char s,count=0;
for(s=0;b[s]!='\0';s++)
{	 
if(s==16)
lcdcmd(0xc0);
if(s==32)
lcdcmd(0x01);
lcddata(b[s]);
}
}	
///////////////////////////////////////////////////////////////
void delay(unsigned int value)
{
unsigned int x,y;
for(x=0;x<100;x++)
for(y=0;y<value;y++);
} 
///////////////////////////////////////////////////////////////////////