#include	<reg52.h>
#include	<intrins.h>

sbit motor_in1 = P2^0;
sbit motor_in2 = P2^1;
sbit display_data = P2^2;
sbit display_clk = P2^3;
sbit state_button = P3^3;
sbit speed_button = P3^2;
sbit timer_button = P3^7;
int mins = 0;
int secs = 5;
int times = 0;
bit secs_will_set = 0;
bit mins_will_set = 0;
bit start_timer = 0;
enum SPEED{ low = 0, mid = 1, high = 2} speed = low;
enum STATE{ stop = 0, run =1} state = stop;
unsigned char number [] = {0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xF6,0x00};
void setInit();
void setTimer();
void setSerial();
void runStart();
void runOnce(enum SPEED speed);
void updateDisplay();
void main(){
	int start_flag = 0;
	int update_flag = 0;
	updateDisplay();
  setInit();
	setSerial();
	setTimer();
	times = secs*20 + mins*1200;
	while(1){
		update_flag = (update_flag+1)%3000;
		if (update_flag%3000 == 0){
			updateDisplay();
		}
		if (start_timer == 1)
		{
				TR0 = 1; //定时器开启
			  start_timer = 0;
		}
		if (state == run){
			if (timer_button == 0){
				int i = 1000;
				while(i--);
				if (timer_button == 0)
				{
					start_timer = 1;
				}
			}
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
	int i = 1000;
	unsigned char transferedData;
	while(i--);
	if(speed_button != 0){return;}
	updateDisplay();
	if (state == run){
		speed = (speed+1)%3;
		updateDisplay();
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
	int i = 1000;
	unsigned char transferedData;
	while(i--);
	if(state_button != 0){return;}
	state = 1 - state;
	updateDisplay();
	switch(state){
		case run: transferedData = 0x01;break;
		case stop: transferedData = 0x00;break;
	}
	SBUF = transferedData;
	while(!TI);
	TI = 0;
}
void timer0() interrupt 1{
	times--;
	if(times % 20 == 0){
		mins = times / 1200;
		secs = (times - mins*1200)/20;
		updateDisplay();
	}
	if(times == 0){
		state = stop;
		updateDisplay();
		start_timer = 0;
		TR0 = 0;
		return;
	}
	TH0 = 0x4B; //定时器初值
	TL0 = 0x28; 
	TR0 = 1;
}
void setInit(){
	IT0 = 1;  //外部中断0下降沿触发
	IT1 = 1;
	EX0 = 1;  //外部中断0允许
	EX1 = 1; 
	EA = 1;   //总中断允许
}
void setTimer(){
	TMOD = 0x01|(TMOD&0xF0);
	TH0 = 0x4B; //定时器初值
	TL0 = 0x28; 
	ET0 = 1; //定时器0中断允许
	TR0 = 0; //定时器关闭
}
void runOnce(enum SPEED speed){
	int num1 = 0;
	int num2 = 1000;
	switch(speed){
		case low:
				num1 = 300;break;
		case mid:
				num1 = 600;break;
		case high:
				num1 = 900;break;
	}
	num2 = 1000 - num1;
	motor_in1 = 0;
	motor_in2 = 1;
	while(num1--);
	motor_in1 = 0;
	motor_in2 = 0;
	while(num2--);
}
void runStart(){
	int num = 1000;
	motor_in1 = 0;
	motor_in2 = 1;
	while(num--);
}
void setSerial(){
	SCON = 0x50;
	TMOD = 0x20|(TMOD&0x0F);
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
	if(mins_will_set == 1){
		mins = receivedData;
		mins_will_set == 0;
	}
	else if(secs_will_set == 1){
		secs = receivedData;
		secs_will_set == 0;
	}
	else{
		switch (receivedData){
			case 0x00: state = stop; break;
			case 0x01: state = run; break;
			case 0x02: speed = low; break;
			case 0x03: speed = mid; break;
			case 0x04: speed = high; break;
			case 0x05: mins_will_set = 1;
			case 0x06: secs_will_set = 1;
		}
  }
	updateDisplay();
	SBUF = receivedData;
	while(!TI);
	TI = 0;
}
void updateDisplay(){
	int i,j = 0;
	int num = 10;
	unsigned char display_number = number[0];
	display_clk = 0;
	display_data = 0;
	
	for(i = 0; i < 6; i++){
		switch(i){
			case 0: display_number = number[state];break;
			case 1: display_number = number[speed];break;
			case 2: display_number = number[secs%10];break;
			case 3: display_number = number[secs/10];break;
			case 4: display_number = number[mins%10];break;
			case 5: display_number = number[mins/10];break;
			default: display_number = number[10];
		}
		for(j = 0; j < 8; j++)
		{
			display_clk = 0;
			display_data = display_number&1<<j;
			num = 10;
			while(num--);
			display_clk = 1;
		}
	}
}
	

