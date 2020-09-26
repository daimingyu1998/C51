#include	<reg52.h>
#include	<intrins.h>

sbit motor_in1 = P1^0;
sbit motor_in2 = P1^1;
enum SPEED{ low = 0, mid = 1, high = 2} speed = low;
enum STATE{ stop = 0, run =1} state = stop;
void setInit();
void setTimer();
void runOnce(enum SPEED speed);
void main(){
  setInit();
	//setTimer();
	while(1){
		if (state == run){
			runOnce(speed);
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
void runOnce(enum SPEED speed)
{
	int num1 = 0;
	int num2 = 100;
	switch(speed){
		case low:
				num1 = 10;break;
		case mid:
				num1 = 50;break;
		case high:
				num1 = 90;break;
	}
	num2 = 100 - num1;
	motor_in1 = 1;
	motor_in2 = 0;
	while(num1--);
	motor_in1 = 0;
	motor_in2 = 0;
	while(num2--);
}
	

