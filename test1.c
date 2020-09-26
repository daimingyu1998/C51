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
	IT0 = 1;  //�ⲿ�ж�0�½��ش���
	IT1 = 1;
	EX0 = 1;  //�ⲿ�ж�0����
	EX1 = 1; 
	EA = 1;   //���ж�����
}
void setTimer(){
	TMOD = 0x01; //��ʱ�����Ʒ�ʽ
	TH0 = 156; //��ʱ����ֵ
	TL0 = 156; 
	ET0 = 1; //��ʱ��0�ж�����
	TR0 = 1; //��ʱ������
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
	

