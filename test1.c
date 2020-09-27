#include	<reg52.h>
#include	<intrins.h>

sbit motor_in1 = P2^0;
sbit motor_in2 = P2^1;
sbit led = P2^4;
enum SPEED{ low = 0, mid = 1, high = 2} speed = low;
enum STATE{ stop = 0, run =1} state = stop;

void setInit();
void setTimer();
void setSerial();
void runStart();
void runOnce(enum SPEED speed);

void main(){
	int start_flag = 0;
  setInit();
	setSerial();
	//setTimer();
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
	if (state == run){
		speed = (speed+1)%3;
	}	
}
void controlState() interrupt 2{
	state = 1 - state;
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
	

