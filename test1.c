#include	<reg52.h>
#include	<intrins.h>

sbit motor_in1 = P2^0;
sbit motor_in2 = P2^1;
sbit display_data = P2^2;
sbit display_clk = P2^3;
enum SPEED{ low = 0, mid = 1, high = 2} speed = low;
enum STATE{ stop = 0, run =1} state = stop;
unsigned char number [] = {0xFC};
void setInit();
void setTimer();
void setSerial();
void runStart();
void runOnce(enum SPEED speed);
void updateDisplay();
void main(){
	int start_flag = 0;
  setInit();
	setSerial();
	//setTimer();
	updateDisplay();
	while(1){
		if (state == run){
			if (start_flag == 0){
				runStart();
		    start_flag++;
			}
			else{
				runOnce(speed);
			}
		}
		else{
		  start_flag = 0;
			motor_in1 = 0;
			motor_in2 = 0;
		}
	}
}
void controlSpeed() interrupt 0{
	unsigned char transferedData;
	if (state == run){
		speed = (speed+1)%3;
		switch(speed){
			case low: transferedData = 0x02;break;
			case mid: transferedData = 0x03;break;
			case high: transferedData = 0x04;break;
		}
		SBUF = transferedData;
		while(!TI);
		TI = 0;
	}
	
}
void controlState() interrupt 2{
	unsigned char transferedData;
	state = 1 - state;
	switch(state){
		case run: transferedData = 0x01;break;
		case stop: transferedData = 0x00;break;
	}
	SBUF = transferedData;
	while(!TI);
	TI = 0;
}
void timer0() interrupt 1{
	
}
void setInit(){
	IT0 = 1;  //外部中断0下降沿触发
	IT1 = 1;
	EX0 = 1;  //外部中断0允许
	EX1 = 1; 
	EA = 1;   //总中断允许
}
void setTimer(){
	TMOD = 0x01; //定时器控制方式
	TH0 = 156; //定时器初值
	TL0 = 156; 
	ET0 = 1; //定时器0中断允许
	TR0 = 1; //定时器开启
}
void runOnce(enum SPEED speed){
	int num1 = 0;
	int num2 = 10000;
	switch(speed){
		case low:
				num1 = 3000;break;
		case mid:
				num1 = 6000;break;
		case high:
				num1 = 9000;break;
	}
	num2 = 10000 - num1;
	motor_in1 = 0;
	motor_in2 = 1;
	while(num1--);
	motor_in1 = 0;
	motor_in2 = 0;
	while(num2--);
}
void runStart(){
	int num = 10000;
	motor_in1 = 0;
	motor_in2 = 1;
	while(num--);
}
void setSerial(){
	SCON = 0x50;
	TMOD = 0x20;
	PCON = 0x80;
	TH1 = 0xFA;
	TL1 = 0xFA;
	ES = 1;
	EA = 1;
	TR1 = 1;
}
void serialInit() interrupt 4{
	unsigned char receivedData;
	receivedData = SBUF;
	RI = 0;
	switch (receivedData){
		case 0x00: state = stop; break;
		case 0x01: state = run; break;
		case 0x02: speed = low; break;
		case 0x03: speed = mid; break;
		case 0x04: speed = high; break;
	}
	SBUF = receivedData;
	while(!TI);
	TI = 0;
}
void updateDisplay(){
	int i,j = 0;
	unsigned char display_number = number[0];
	display_clk = 0;
	display_data = 0;
	
	for(i = 0; i < 6; i++){
		switch(i){
			case 0: display_number = number[state];break;
			case 1: display_number = number[speed];break;
			default: display_number = number[0];
		}
		for(j = 0; j < 8; j++)
		{
			display_clk = 0;
			display_data = number[0]&1<<j;
			display_clk = 1;
		}
	}
}
	

