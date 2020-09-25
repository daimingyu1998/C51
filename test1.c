#include	<reg52.h>
#include	<intrins.h>

sbit speed_button = P1^0;
sbit state_button = P1^1;
enum SPEED{ low = 0, mid = 1, high = 2} speed = low;
enum STATE{ stop = 0, run =1} state = stop;
void setInit();
void setTimer();
void main(){
  setInit();
	setTimer();
	while(1);
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
}

