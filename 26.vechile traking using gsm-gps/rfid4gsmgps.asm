							 			    /*===========================================*/
  /*------radio freq id programming------------*/
 /*------PH:89C51RD+,08/07/08-----------------*/
/*===========================================*/

	LCD_DPort EQU P0

	LCD_Rs	  EQU  P1.5
	LCD_Rw    EQU  P1.6
	LCD_En	  EQU  P1.7

	DELAY1	  DATA 30h
	DELAY2    DATA 31h					 
	DELAY3	  DATA 32h


	FDispRFID Bit 01h

/*-------------------------------------*/
	org  00h	
	ljmp Power_on

	org  23h
	ljmp serial_ISR
/*-------------------------------------*/	
	org  100h
Power_on:
	mov  TMOD,#20h
	mov  TH1,#0FDh
	mov  TL1,#0FDh
	mov  SCON,#50h
	mov  IE,#90h
	setb TR1



	mov   R0,#70h
	mov   R2,#10
	mov   R3,#00h
	mov   P2,#0ffh
	setb  EA	
/*-------------------------------------*/
MainLoop:
	jb FDispRFID,RFIDLoop
	jmp MainLoop
/*-------------------------------------*/
RFIDLoop:		Ljmp RFIDDATA
/*-------------------------------------*/
RFIDDATA: 
	clr FDispRFID

	mov R0,#70h
	mov R2,#10

	mov dptr,#CARDNUM0
	call Compare
	cjne R3,#01h,Check_CARD1
	mov  P2,#0ffh
	call delay1sec
 	mov  P2,#00h

	mov R0,#70h
	mov R2,#10
	mov R3,#00h
	jmp MainLoop

Check_CARD1:
	mov dptr,#CARDNUM1
	call Compare
	cjne R3,#01h,Check_Invalid
	mov  P2,#0ffh
	call delay1sec
	mov  P2,#00h
	mov R0,#70h
	mov R2,#10
	mov R3,#00h
	jmp MainLoop

Check_Invalid:
	mov R0,#70h
	mov R2,#10
	mov R3,#00h
	jmp MainLoop

	
CARDNUM0:    DB "8400816353"
CARDNUM1:    DB "8400857EE7"
/*-------------------------------------*/
Compare:
Card1_ChK:
	clr a
	movc a,@a+dptr
	mov b,a
	mov a,@R0
	cjne a,b,Next_ChK
	inc R0
	inc dptr
	djnz R2,Card1_ChK
	inc R3
	ret
Next_ChK:
	mov R0,#70h
	mov R2,#10
	ret
/*-------------------------------------*/
serial_ISR:
	push Acc
	push DPH
	push DPL
	jb   RI,RX_Service
	jb   TI,TX_Service
	pop  DPL	
	pop  DPH
	pop  Acc
	reti

RX_Service:
	clr  RI	
	mov  a,sbuf
	mov @R0,a
	inc  R0
	djnz R2,End_isr
	setb FDispRFID
	ljmp End_isr
TX_Service:
	clr  TI
	ljmp End_isr

End_isr:
	pop  DPL	
	pop  DPH
	pop  Acc
	reti
/******************************************/		
/*-------Delay routine-------------------*/
/****************************************/
delay1sec:
	mov  DELAY1,#10		
wait2:	mov  DELAY2,#200 
wait1:	mov  DELAY3,#250
wait:	djnz DELAY3,wait
	djnz DELAY2,wait1
	djnz DELAY1,wait2
	ret


end