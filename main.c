/*==============================================================
*	作者：老猫@xmprocat
*	版权所有：老猫@xmprocat
*	介于本人水平有限 部分地方可能存在纰漏 欢迎在issues中提出
*	若要用作商业用途请联系本人进行授权
*	发布地址：https://github.com/xmprocat/Calculating_machine_BY_AT89C52
*==============================================================*/
//包含头文件
#include "reg52.h"	 //此文件中定义了单片机的一些特殊功能寄存器
#include "stdlib.h"

//向全局进行函数声明
void delay(unsigned int n);
void display(long int number);
void control();

//全局定义变量
unsigned char segs[13] = { 0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6,0x02,0x00,0xde, }; //位选	0,1,2,3,4,5,6,7,8,9,kong,-,e
unsigned char tongd[7] = { 0xff,0xfe,0xfd,0xfb,0xf7,0xef,0xdf, };								//段选
long int m, i, z, k = 0, amount = 0;
long int temp = 0;
long int g, u = 0;


//主函数
void main()
{
	m = 999;//这个99可以任意数，只要不是0~9之间的数即可
	g = 0;//这是给计算器赋初值，即开计算器就在数码管上显示0
	while (1)
	{
		control();//检测键盘的输入情况
		if (m != 999)//如果键盘上有数字输入，那么m就必然是0~9之间的一个数,执行函数体
		{
			g = 10 * g + m;
			m = 999;		//一定要给回这个m值，因为键盘上的键值已经赋给了g，如果不给回m值，就会造成这个if语句每次都会被执行，而不管你有没有按下键盘
		}
		if (k == 31 || k == 32 || k == 33 || k == 34)//运算符的标志位
		{
			u = k;			//把k值赋给另一个变量，然后把k清零，如果不清零，这个if语句也会每次都被执行的，而不管你有没有按下键盘
			temp = g;		//按下运算符后，是把刚才的这个数存起来

			g = 0;			//这个g也要清零的，否则记不了你要参加运算的第2个数的 
			k = 0;			//如上所说，必须清零
		}
		if (amount == 30)//这里就是等于啦。
		{
			switch (u)
			{
			case 31:
				g = temp + g;
				break;//求和
			case 32:
				g = temp - g;
				break;//求差
			case 33:
				g = temp * g;
				break;//求积
			case 34:
				g = temp / g;
				break;//求商
			/*case 35:						//按键不够 砍了
				g=pow(temp,g);
				break;//求n次方
			case 36:
				g=log10(temp)/log10(g);
				break;//求开n次方
				case 37:g=temp*g;break;
				case 38:g=temp/g;break;*/
			}
			u = 0;
			amount = 0;//运算结束，都清零
		}
		display(g);//显示结果啦
		if (z == 40) {//算完了，用这个键把计算器复位
			/*m = 999;
			g = 0;*/
			break;
		}
	}
}


//键盘扫描读取函数,我键盘接在P3上
void control() {
	const unsigned char tablelie[4] = { 0x7f,0xbf,0xdf,0xef };//4*4矩阵键盘，接P3口 高4位轮流给低电平，根据低4位的电平变化确定按下的是哪个键
	//0x7f	->	0111 1111B
	//0xbf	->	1011 1111B
	//0xdf	->	1101 1111B
	//0xef	->	1110 1111B
	//同时按两个只会记录第一个按下的按键
	for (i = 0; i < 4; i++) //这个循环是给P3口上的键盘的高4位轮流给低电平
	{
		P0 = tablelie[i];
		if (P0 != tablelie[i]) //这里是检测是否有键盘被按下 跟输出的不一致就是有按键按下
		{

			delay(10);
			if (P0 != tablelie[i]) //这里经过10的延时后，看这个键是否处于被按下的状态	实现软件消抖 
			{
				switch (P0)//获取按下的字符
				{
				case 0xb7://这是按下时的电平情况,可根据Proteus模拟得出
					m = 1;
					break;
				case 0xbb:
					m = 2;
					break;
				case 0xbd:
					m = 3;
					break;
				case 0x7e:
					k = 31;
					break;//这是"加"的标志位 
				case 0xd7:
					m = 4;
					break;
				case 0xdb:
					m = 5;
					break;
				case 0xdd:
					m = 6;
					break;
				case 0xbe:
					k = 32;
					break;//这是"减"的标志位 
				case 0xe7:
					m = 7;
					break;
				case 0xeb:
					m = 8;
					break;
				case 0xed:
					m = 9;
					break;
				case 0xde:
					k = 33;
					break;//这是"乘"的标志位;
				case 0x77:
					z = 40;
					break;//这是"计算器清零"的标志位 
				case 0x7b:
					m = 0;
					break;
				case 0x7d:
					amount = 30;
					break;//这是"等于"的标志位 
				case 0xee:
					k = 34;
					break;//这是"除"的标志位 
				}
			}
			while (P0 != tablelie[i]);//松手检测	用于等待按键松开 如果按键没松开则进程会进行等待
		}
	}
}






//数码管显示函数，负责显示数字在数码管上面
void display(long int number) {
	int fu = 0;
	int cdu, se;
	long int temp;
	if (number > 999999 || number < -99999) { //传入数字的合理性判断
		for (se = 6;se > 0;se--) { //错误处理 超范围就报错
			P2 = tongd[se];
			P1 = segs[10];
			delay(1);
			//P1 = segs[11];
			//P2 = tongd[0];
			delay(1);
		}

	}
	else if (number <= 999999 && number >= -99999) { //获取数字的长度，暂时想不出更好的算法

		temp = labs(number);

		if (number <= 9 && number >= 0) { cdu = 1; }//这样写是防止数字中间有0
		else if (number <= 99 && number >= 10) { cdu = 2; }
		else if (number <= 999 && number >= 100) { cdu = 3; }
		else if (number <= 9999 && number >= 1000) { cdu = 4; }
		else if (number <= 99999 && number >= 10000) { cdu = 5; }
		else if (number <= 999999 && number >= 100000) { cdu = 6; }

		else if (number < 0 && number > -10) { cdu = 1;P2 = 0xfe;P1 = 0x02; }
		else if (number <= -10 && number >= -99) { cdu = 2;P2 = 0xfe;P1 = 0x02; }
		else if (number <= -100 && number >= -999) { cdu = 3;P2 = 0xfe;P1 = 0x02; }
		else if (number <= -1000 && number >= -9999) { cdu = 4;P2 = 0xfe;P1 = 0x02; }
		else if (number <= -10000 && number >= -99999) { cdu = 5;P2 = 0xfe;P1 = 0x02; }

		for (se = 6;se > 6 - cdu;se--) {//刷新显示数码管se为当前刷新
			if (number < 0) {
				P2 = 0xfe;P1 = 0x02;
				delay(1);//延时
				//P1 = segs[11];//清空位选
				//P2 = tongd[0];//清空段选防止重影

			}
			delay(1);//延时

			P1 = segs[11];//清空
			P2 = tongd[0];//清空防止重影
			delay(1);
			P2 = tongd[se];//位选
			P1 = segs[temp % 10];//段选
			temp /= 10;//每次段选取出最后一位，然后除10砍掉最后一位




		}
	}

}
//一个简单的延时函数
void delay(unsigned int n) {
	unsigned int i, j;
	for (j = 0;i < n;i++) {
		for (j = 0;j < 123;j++) {}
	}
}