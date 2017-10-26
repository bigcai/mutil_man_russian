////////////////////////////////////////////
// 程序名称：俄罗斯方块
// 编译环境：Visual C++ 6.0 / 2010，EasyX_v20131006(beta)
// 程序编写：liangdu <596630076@qq.com>
//
#include <easyx.h>
#include <conio.h>
#include <time.h>
#include <stdio.h>
#include <WINSOCK2.H>
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")




IMAGE imgbk,imgblock;

int  choice=-1;
int k;
char text[20];
WORD myVersionRequest;
 WORD versionRequired;
 WSADATA wsaData;
 int err;
 SOCKET serSocket;
 SOCKADDR_IN addr;
 SOCKADDR_IN clientsocket;
 SOCKET serConn;
 char sendBuf[100];
 char receiveBuf[100]={0};//接收
SOCKET clientSocket;
 SOCKADDR_IN clientsock_in;
/////////////////////////////////////////////
// 定义常量、枚举量、结构体、全局变量
/////////////////////////////////////////////

#define	WIDTH	10		// 游戏区宽度
#define	HEIGHT	22		// 游戏区高度
#define	UNIT	20		// 每个游戏区单位的实际像素

// 定义操作类型
enum CMD
{
	CMD_ROTATE,						// 方块旋转
	CMD_LEFT, CMD_RIGHT, CMD_DOWN,	// 方块左、右、下移动
	CMD_SINK,						// 方块沉底
	CMD_QUIT						// 退出游戏
};

// 定义绘制方块的方法
enum DRAW
{
	SHOW,	// 显示方块
	CLEAR,	// 擦除方块
	FIX		// 固定方块
};

// 定义七种俄罗斯方块
struct BLOCK
{
	WORD dir[4];	// 方块的四个旋转状态
	COLORREF color;	// 方块的颜色
}	g_Blocks[7] = {	{0x0F00, 0x4444, 0x0F00, 0x4444, RED},		// I
					{0x0660, 0x0660, 0x0660, 0x0660, BLUE},		// 口
					{0x4460, 0x02E0, 0x0622, 0x0740, MAGENTA},	// L
					{0x2260, 0x0E20, 0x0644, 0x0470, YELLOW},	// 反L
					{0x0C60, 0x2640, 0x0C60, 0x2640, CYAN},		// Z
					{0x0360, 0x4620, 0x0360, 0x4620, GREEN},	// 反Z
					{0x4E00, 0x4C40, 0x0E40, 0x4640, BROWN}};	// T

// 定义当前方块、下一个方块的信息
struct BLOCKINFO
{
	byte id;	// 方块 ID
	char x, y;	// 方块在游戏区中的坐标
	byte dir:2;	// 方向
}	g_CurBlock, g_NextBlock,xx;

// 定义游戏区
BYTE g_World[WIDTH][HEIGHT] = {0};
//敌方的游戏区
BYTE op_g_World[WIDTH][HEIGHT] = {0};
//我的得分,敌人的得分
int mydefen,opdefen;
//比赛剩余时间
int mytime=60;
/////////////////////////////////////////////
// 函数声明
/////////////////////////////////////////////

void Init();											// 初始化游戏
void Quit();											// 退出游戏
void NewGame();											// 开始新游戏
void GameOver();										// 结束游戏
CMD  GetCmd();											// 获取控制命令
void DispatchCmd(CMD _cmd);								// 分发控制命令
void NewBlock();										// 生成新的方块
bool CheckBlock(BLOCKINFO _block);						// 检测指定方块是否可以放下
void DrawUnit(int x, int y, COLORREF c, DRAW _draw);	// 画单元方块
void DrawBlock(BLOCKINFO _block, DRAW _draw = SHOW);	// 画方块
void OnRotate();										// 旋转方块
void OnLeft();											// 左移方块
void OnRight();											// 右移方块
void OnDown();											// 下移方块
void OnSink();											// 沉底方块



void doo(int x, int y,IMAGE img);						// 鼠标单击事件
void up(int x,int y,IMAGE img);							// 鼠标松开事件
void backspace();										// 返回上一层
void usertable();										// 用户选择界面
int SorC();												// 选择房间权限
char *login(char s[],int n);											// 登录模块
char s[20];
IMAGE img1,img2,img3,img4,img5,img6,img7;
MOUSEMSG  msg;



														
void buildConne();										//S建立连接
void sendNum(int number);								//S发送数据
void receiveNum(int *number);										//S接受数据
void buildConneC();										//C建立连接
void sendNumC(int number);								//C发送数据
void receiveNumC(int *number);										//C接受数据
void drawoppo(int x,int y);									
void drawothers(BYTE g_World[10][22],int x,int y);

void rungame();

//双方共有线程函数
DWORD WINAPI sendmyMAP();             //发送我方地图
DWORD WINAPI sendmyfenshu();          //发送我的分数
DWORD WINAPI receiveMAP(LPVOID xx);			  //接收敌方地图
DWORD WINAPI receivefenshu(LPVOID xx);		  //接收敌方分数
DWORD WINAPI newtime(LPVOID xx);

//服务器线程函数
DWORD WINAPI sendtime(LPVOID xx);			 //发送比赛剩余时间
//客户端线程函数
DWORD WINAPI receivetime(LPVOID xx);			//接收比赛剩余时间

//5条线程，分别用于发送MAP，发送分数，接收MAP,接收分数，剩余时间计算
HANDLE sendMapThread,sendFenshuThread,recMapThread,recFenshuThread,timeThread,newtimeThread;

DWORD id;

/////////////////////////////////////////////
// 函数定义
/////////////////////////////////////////////

// 主函数
 int player1=0;
 int playee=0;
void main()
{  
    for(int j=21,ii=0;j>=0;j--,ii++){
		for(int i =0,jj =0;i<WIDTH;i++,jj++){
			printf(" %d",op_g_World[i][j]);}
	  printf("\n");
	}
   
	loadimage(&imgbk, _T("D:\\res\\testbk.jpg"));
	loadimage(&imgblock, _T("D:\\res\\testblock.jpg"));
     
	usertable();
    

	
	
   



    

    


	if(choice==1){
		closesocket(serConn); //关闭
		WSACleanup(); //释放资源的操作
	}else if(choice==0){
		closesocket(clientSocket);
		WSACleanup();
	}
	else{
		return ;
	}

}
void usertable(){
	 initgraph(640, 480);   // 这里和 TC 略有区别
     IMAGE img1,img2,img3;
     loadimage(&img1, _T("d:\\res\\test.jpg"));
	 loadimage(&img2, _T("D:\\res\\test2.jpg"));
	 loadimage(&img3, _T("D:\\res\\test3.jpg"));
	 loadimage(&img4, _T("D:\\res\\afterhit.jpg"));
   	 loadimage(&img5, _T("D:\\res\\test4.jpg"));
	 loadimage(&img6, _T("D:\\res\\Quit1.jpg"));
	 loadimage(&img7, _T("D:\\res\\Quit.jpg"));
     putimage(0, 0, &img1);
	 putimage(280, 280, &img2);
	 putimage(280, 330, &img3);
   	 putimage(280, 380, &img6);

    while(true)
	{
	while (MouseHit())			// 当有鼠标消息的时候执行
	{
		msg = GetMouseMsg();	// 获取鼠标消息

		switch(msg.uMsg)		// 根据不同的鼠标消息，执行不同的代码
		{
	
		case WM_LBUTTONDOWN: doo(msg.x,msg.y,img2); break;
		case WM_LBUTTONUP: 
			if(msg.x>=280 && msg.x<(img2.getwidth()+280)  &&  msg.y>=280 && msg.y<=(280+img2.getheight())){
				up(280,280,img2); break;
			}
			if(msg.x>=280 && msg.x<(img3.getwidth()+280)  &&  msg.y>=330 && msg.y<=(330+img3.getheight())){
				up(280,330,img3); break;	
			}
			if(msg.x>=280 && msg.x<(img6.getwidth()+280)  &&  msg.y>=380 && msg.y<=(380+img6.getheight())){
				up(280,380,img6); break;
			}
		}
	}

	

	Sleep(20);					// 延时，降低 CPU 占用率
}

    
    closegraph();          // 关闭图形界面
}

// 初始化游戏
void Init()
{

	
		initgraph(640, 480);
	
	 putimage(0, 0, &imgbk);   
	
    setbkmode(TRANSPARENT);	// 设置图案填充的背景色为透明
 
	// 显示操作说明
	srand((unsigned)time(NULL));
	settextstyle(14, 0, _T("宋体"));
	outtextxy(20, 330, _T("操作说明"));
	outtextxy(20, 350, _T("上：旋转"));
	outtextxy(20, 370, _T("左：左移"));
	outtextxy(20, 390, _T("右：右移"));
	outtextxy(20, 410, _T("下：下移"));
	outtextxy(20, 430, _T("空格：沉底"));
	outtextxy(20, 450, _T("ESC：退出"));
    
	
	//敌方的游戏界面
	rectangle(100, 70, WIDTH *( UNIT-5)+100, HEIGHT * (UNIT-5)+70);
	setfillcolor(BLACK);
	
	solidrectangle(100, 70, WIDTH *( UNIT-5)+100, HEIGHT * (UNIT-5)+70);
	ZeroMemory(g_World, WIDTH * HEIGHT);
	//显示得分
	setorigin(0, 0);
	
	setfillcolor(0x000000);
	fillrectangle(20,20,150,60);
	sprintf(text,"您的得分：%d",mydefen);
	outtextxy(20, 20, text);
	sprintf(text,"对手的得分：%d",opdefen);
    outtextxy(20, 45, text);

	setorigin(300, 20);

   
	// 绘制游戏区边界

		
		//我方的游戏界面
	rectangle(-1, -1, WIDTH * UNIT, HEIGHT * UNIT);
	rectangle((WIDTH + 1) * UNIT - 1, -1, (WIDTH + 5) * UNIT, 4 * UNIT);

    
	// 开始新游戏
	NewGame();
}


// 退出游戏
void Quit()
{
	closegraph();
	exit(0);
}


// 开始新游戏
void NewGame()
{
	setorigin(0, 0);
	setfillcolor(BLACK);
	solidrectangle(100, 70, WIDTH *( UNIT-5)+100, HEIGHT * (UNIT-5)+70);
	setorigin(300, 20);
	// 清空游戏区
	setfillcolor(BLACK);
	
	solidrectangle(0, 0, WIDTH * UNIT - 1, HEIGHT * UNIT - 1);
	ZeroMemory(g_World, WIDTH * HEIGHT);

	// 生成下一个方块
	g_NextBlock.id = rand() % 7;
	g_NextBlock.dir = rand() % 4;
	g_NextBlock.x = WIDTH + 1;
	g_NextBlock.y = HEIGHT - 1;

	// 获取新方块
	NewBlock();
}


// 结束游戏
void GameOver()
{
	HWND wnd = GetHWnd();
	if (MessageBox(wnd, _T("游戏结束。\n您想重新来一局吗？"), _T("游戏结束"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		NewGame();
	else
		Quit();
}


// 获取控制命令
DWORD m_oldtime;
CMD GetCmd()
{
	// 获取控制值
	while(true)
	{
		// 如果超时，自动下落一格
		DWORD newtime = GetTickCount();
		if (newtime - m_oldtime >= 500)
		{
			m_oldtime = newtime;
			return CMD_DOWN;
		}

		// 如果有按键，返回按键对应的功能
		if (kbhit())
		{
			switch(getch())
			{
				case 'w':
				case 'W':	return CMD_ROTATE;
				case 'a':
				case 'A':	return CMD_LEFT;
				case 'd':
				case 'D':	return CMD_RIGHT;
				case 's':
				case 'S':	return CMD_DOWN;
				case 27:	return CMD_QUIT;
				case ' ':	return CMD_SINK;
				case 0:
				case 0xE0:
					switch(getch())
					{
						case 72:	return CMD_ROTATE;
						case 75:	return CMD_LEFT;
						case 77:	return CMD_RIGHT;
						case 80:	return CMD_DOWN;
					}
			}
		}

		// 延时 (降低 CPU 占用率)
		Sleep(20);
	}
}


// 分发控制命令
void DispatchCmd(CMD _cmd)
{   
	switch(_cmd)
	{
		case CMD_ROTATE:	OnRotate();		break;
		case CMD_LEFT:		OnLeft();		break;
		case CMD_RIGHT:		OnRight();		break;
		case CMD_DOWN:		OnDown();		break;
		case CMD_SINK:		OnSink();		break;
		case CMD_QUIT:		break;
	}
}


// 生成新的方块
void NewBlock()
{   
	setorigin(0, 0);
	
	setfillcolor(0x000000);
	fillrectangle(20,20,170,60);
	sprintf(text,"您的得分：%d 时间：%d",mydefen,mytime);
	outtextxy(20, 20, text);
	sprintf(text,"对手的得分：%d",opdefen);
    outtextxy(20, 45, text);
	
	setorigin(300, 20);
	g_CurBlock.id = g_NextBlock.id,		g_NextBlock.id = rand() % 7;
	g_CurBlock.dir = g_NextBlock.dir,	g_NextBlock.dir = rand() % 4;
	g_CurBlock.x = (WIDTH - 4) / 2;
	g_CurBlock.y = HEIGHT + 2;

	// 下移新方块直到有局部显示
	WORD c = g_Blocks[g_CurBlock.id].dir[g_CurBlock.dir];
	while((c & 0xF) == 0)
	{
		g_CurBlock.y--;
		c >>= 4;
	}

	// 绘制新方块
	DrawBlock(g_CurBlock);

	// 绘制下一个方块
	setfillcolor(BLACK);
	solidrectangle((WIDTH + 1) * UNIT, 0, (WIDTH + 5) * UNIT - 1, 4 * UNIT - 1);
	DrawBlock(g_NextBlock);

	// 设置计时器，用于判断自动下落
	m_oldtime = GetTickCount();
}


// 画单元方块
void DrawUnit(int x, int y, COLORREF c, DRAW _draw)
{
	// 计算单元方块对应的屏幕坐标
	int left = x * UNIT;
	int top = (HEIGHT - y - 1) * UNIT;
	int right = (x + 1) * UNIT - 1;
	int bottom = (HEIGHT - y) * UNIT - 1;

	// 画单元方块
	switch(_draw)
	{
		case SHOW:
			// 画普通方块
		    /*
			setlinecolor(0x006060);
			roundrect(left + 1, top + 1, right - 1, bottom - 1, 5, 5);
			setlinecolor(0x003030);
			roundrect(left, top, right, bottom, 8, 8);
			setfillcolor(c);
            
			
			setlinecolor(LIGHTGRAY);
			
			fillrectangle(left + 2, top + 2, right - 2, bottom - 2);
			
			*/
			Resize(&imgblock,right-left,bottom-top);
			putimage(left, top, &imgblock);
			
			break;

		case FIX:
			// 画固定的方块
			setfillcolor(RGB(GetRValue(c) * 2 / 3, GetGValue(c) * 2 / 3, GetBValue(c) * 2 / 3));
			setlinecolor(DARKGRAY);
			fillrectangle(left + 1, top + 1, right - 1, bottom - 1);
			break;

		case CLEAR:
			// 擦除方块
			setfillcolor(BLACK);
			solidrectangle(x * UNIT, (HEIGHT - y - 1) * UNIT, (x + 1) * UNIT - 1, (HEIGHT - y) * UNIT - 1);
			break;
	}
}


// 画方块
void DrawBlock(BLOCKINFO _block, DRAW _draw)
{
	WORD b = g_Blocks[_block.id].dir[_block.dir];
	int x, y;

	for(int i = 0; i < 16; i++, b <<= 1)
		if (b & 0x8000)
		{
			x = _block.x + i % 4;
			y = _block.y - i / 4;
			if (y < HEIGHT)
				DrawUnit(x, y, g_Blocks[_block.id].color, _draw);
		}
}


// 检测指定方块是否可以放下
bool CheckBlock(BLOCKINFO _block)
{
	WORD b = g_Blocks[_block.id].dir[_block.dir];
	int x, y;

	for(int i = 0; i < 16; i++, b <<= 1)
		if (b & 0x8000)
		{
			x = _block.x + i % 4;
			y = _block.y - i / 4;
			if ((x < 0) || (x >= WIDTH) || (y < 0))
				return false;

			if ((y < HEIGHT) && (g_World[x][y]))
				return false;
		}

	return true;
}


// 旋转方块
void OnRotate()
{
	// 获取可以旋转的 x 偏移量
	int dx;
	BLOCKINFO tmp = g_CurBlock;
	tmp.dir++;					if (CheckBlock(tmp))	{	dx = 0;		goto rotate;	}
	tmp.x = g_CurBlock.x - 1;	if (CheckBlock(tmp))	{	dx = -1;	goto rotate;	}
	tmp.x = g_CurBlock.x + 1;	if (CheckBlock(tmp))	{	dx = 1;		goto rotate;	}
	tmp.x = g_CurBlock.x - 2;	if (CheckBlock(tmp))	{	dx = -2;	goto rotate;	}
	tmp.x = g_CurBlock.x + 2;	if (CheckBlock(tmp))	{	dx = 2;		goto rotate;	}
	return;

rotate:
	// 旋转
	DrawBlock(g_CurBlock, CLEAR);
	g_CurBlock.dir++;
	g_CurBlock.x += dx;
	DrawBlock(g_CurBlock);
}


// 左移方块
void OnLeft()
{
	BLOCKINFO tmp = g_CurBlock;
	tmp.x--;
	if (CheckBlock(tmp))
	{
		DrawBlock(g_CurBlock, CLEAR);
		g_CurBlock.x--;
		DrawBlock(g_CurBlock);
	}
}


// 右移方块
void OnRight()
{
	BLOCKINFO tmp = g_CurBlock;
	tmp.x++;
	if (CheckBlock(tmp))
	{
		DrawBlock(g_CurBlock, CLEAR);
		g_CurBlock.x++;
		DrawBlock(g_CurBlock);
	}
}


// 下移方块
void OnDown()
{
	BLOCKINFO tmp = g_CurBlock;
	tmp.y--;
	if (CheckBlock(tmp))
	{
		DrawBlock(g_CurBlock, CLEAR);
		g_CurBlock.y--;
		DrawBlock(g_CurBlock);
	}
	else
		OnSink();	// 不可下移时，执行“沉底方块”操作
}


// 沉底方块
void OnSink()
{  
	int i, x, y;

	// 连续下移方块
	DrawBlock(g_CurBlock, CLEAR);
	BLOCKINFO tmp = g_CurBlock;
	tmp.y--;
	while (CheckBlock(tmp))
	{
		g_CurBlock.y--;
		tmp.y--;
	}
	DrawBlock(g_CurBlock, FIX);

	// 固定方块在游戏区
	WORD b = g_Blocks[g_CurBlock.id].dir[g_CurBlock.dir];
	for(i = 0; i < 16; i++, b <<= 1)
		if (b & 0x8000)
		{
			if (g_CurBlock.y - i / 4 >= HEIGHT)
			{	// 如果方块的固定位置超出高度，结束游戏
				GameOver();
				return;
			}
			else
				g_World[g_CurBlock.x + i % 4][g_CurBlock.y - i / 4] = 1;
		}

	// 检查是否需要消掉行，并标记
	BYTE remove = 0;	// 低 4 位用来标记方块涉及的 4 行是否有消除行为
	for(y = g_CurBlock.y; y >= max(g_CurBlock.y - 3, 0); y--)
	{
		i = 0;
		for(x = 0; x < WIDTH; x++)
			if (g_World[x][y] == 1)
				i++;

		if (i == WIDTH)
		{
			remove |= (1 << (g_CurBlock.y - y));
			setfillcolor(LIGHTGREEN);
			setlinecolor(LIGHTGREEN);
			setfillstyle(BS_HATCHED, HS_DIAGCROSS);
			fillrectangle(0, (HEIGHT - y - 1) * UNIT + UNIT / 2 - 5, WIDTH * UNIT - 1, (HEIGHT - y - 1) * UNIT + UNIT / 2 + 5);
			setfillstyle(BS_SOLID);
		}
	}

	if (remove)	// 如果产生整行消除
	{   
			 mydefen++;
		     if(choice!=-1)
	         sendmyfenshu();	
		   


	setorigin(0, 0);
	
	setfillcolor(0x000000);
	fillrectangle(20,20,150,60);
	sprintf(text,"您的得分：%d",mydefen);
	outtextxy(20, 20, text);
	sprintf(text,"对手的得分：%d",opdefen);
    outtextxy(20, 45, text);

	setorigin(300, 20);



		// 延时 300 毫秒
		Sleep(300);

		// 擦掉刚才标记的行
		IMAGE img;
		for(i = 0; i < 4; i++, remove >>= 1)
		{
			if (remove & 1)
			{
				for(y = g_CurBlock.y - i + 1; y < HEIGHT; y++)
					for(x = 0; x < WIDTH; x++)
					{
						g_World[x][y - 1] = g_World[x][y];
						g_World[x][y] = 0;
					}
				
				getimage(&img, 0, 0, WIDTH * UNIT, (HEIGHT - (g_CurBlock.y - i + 1)) * UNIT);
				putimage(0, UNIT, &img);
			
			    


			}
		}
	}

		 setorigin(0,0);
         drawothers(op_g_World,100,70);
	     setorigin(300, 20);
	if(choice!=-1)
      sendmyMAP();
	// 产生新方块
	 
	NewBlock();
}




void doo(int x, int y,IMAGE img){  //按下按钮，显示下凹按钮效果
	if(x>=280 && x<(img.getwidth()+280)  &&  y>=280 && y<=(280+img.getheight()))
	{
		putimage(280, 280, &img4);
	}
	if(x>=280 && x<(img.getwidth()+280)  &&  y>=330 && y<=(330+img.getheight()))
	{
		putimage(280, 330, &img5);
	}
	if(x>=280 && x<(img.getwidth()+280)  &&  y>=380 && y<=(380+img.getheight()))
	{
		putimage(280, 380, &img7);
		
	}
}
void up(int x,int y,IMAGE img){     //松开鼠标，触发对应事件
		putimage(x, y, &img);
		if(x>=280 && x<(img.getwidth()+280)  &&  y>=280 && y<=(280+img.getheight())){
			if(SorC()){     //判断是否选择当房主，如果是，则
				//创建服务器 
                      choice=1;
					buildConne();    //建立连接，等待监听。。。。
					rungame();

			}
			else{     //如果选择当服务端。则
				
				choice=0;
				login(s,20);          //输入房主的IP地址，S用户存放房主的IP地址
				buildConneC();        //与服务器建立连接
				rungame();
			
			}
		}
		if(x>=280 && x<(img.getwidth()+280)  &&  y>=330 && y<=(330+img.getheight())){
			rungame();
		}
		if(x>=280 && x<(img.getwidth()+280)  &&  y>=380 && y<=(380+img.getheight())){
			Quit();
		}
		
}


void backspace(){
	HWND wnd = GetHWnd();
	if (MessageBox(wnd, _T("您确定要返回选择界面吗？"), _T("返回对话框"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		usertable();
}

//选房权限
int SorC(){
	HWND wnd = GetHWnd();
	if (MessageBox(wnd, _T("您是否选择成为服务器"), _T("选择权限"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		return 1;
	else
		return 0;
	
}

//登录模块
char *login(char s[],int n){
	
	InputBox(s, 20, "请输入主机IP","登录窗口",NULL,0,0,false);
	return s;
}






void buildConne(){
	myVersionRequest=MAKEWORD(1,1);
	
	err=WSAStartup(myVersionRequest,&wsaData);
 if (!err)
 {
  printf("已打开套接字\n");

 } 
 else
 {
  //进一步绑定套接字
  printf("嵌套字未打开!");
  return;
 }
 serSocket=socket(AF_INET,SOCK_STREAM,0);//创建了可识别套接字
 //需要绑定的参数

 addr.sin_family=AF_INET;
 addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//ip地址
 addr.sin_port=htons(6885);//绑定端口
 
 bind(serSocket,(SOCKADDR*)&addr,sizeof(SOCKADDR));//绑定完成
 listen(serSocket,5);//其中第二个参数代表能够接收的最多的连接数
 
 //////////////////////////////////////////////////////////////////////////
 //开始进行监听
 //////////////////////////////////////////////////////////////////////////
 
 int len=sizeof(SOCKADDR);
 //while (1)
 //{
	serConn=accept(serSocket,(SOCKADDR*)&clientsocket,&len);//如果这里不是accept而是conection的话。。就会不断的监听
  
  
  
	

}


void sendNum(int number){
	sprintf(sendBuf,"%d",number);//找对对应的IP并且将这行字打印到那里
	
    send(serConn,sendBuf,strlen(sendBuf)+1,0);
	
	
}
void receiveNum(int *number){
	int kk=0;
	while(!kk)
    kk=recv(serConn,receiveBuf,101,0);
   

	sscanf(receiveBuf,"%d",number);
//	printf("%d\n",*number);
	  
}




void buildConneC(){
	versionRequired=MAKEWORD(1,1);
	err=WSAStartup(versionRequired,&wsaData);//协议库的版本信息

 if (!err)
 {
  printf("客户端嵌套字已经打开!\n");
 }
 else
 {
  printf("客户端的嵌套字打开失败!\n");
  return;//结束
 }
 clientSocket=socket(AF_INET,SOCK_STREAM,0);
 //clientsock_in.sin_addr.S_un.S_addr=inet_addr(s);// 服务器地址
 clientsock_in.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");// 服务器地址
 clientsock_in.sin_family=AF_INET;
 clientsock_in.sin_port=htons(6885);                       //连接服务器端口
 //bind(clientSocket,(SOCKADDR*)&clientsock_in,strlen(SOCKADDR));//注意第三个参数
 //listen(clientSocket,5);

  connect(clientSocket,(SOCKADDR*)&clientsock_in,sizeof(SOCKADDR));//开始连接
	
 
	
}	




									//C建立连接
void receiveNumC(int *number){
	k=0;
	while(!k)
    k=recv(clientSocket,receiveBuf,101,0);

	
	sscanf(receiveBuf,"%d",number);
	
}								//C发送数据
void sendNumC(int number){
	 	sprintf(sendBuf,"%d",number);//找对对应的IP并且将这行字打印到那里
		send(clientSocket,sendBuf,strlen(sendBuf)+1,0);
	    

 }


void rungame(){


if(choice!=-1){
		recFenshuThread=CreateThread(NULL,0,receivefenshu,&xx,0,NULL);
		recMapThread=CreateThread(NULL,0,receiveMAP,&xx,0,NULL);
	//	newtimeThread=CreateThread(NULL,0,newtime,&xx,0,NULL);
		
	}
  
	if(choice==1){
	//	timeThread=CreateThread(NULL,0,sendtime,(LPVOID)&xx,0,NULL);
	

	}
	else if(choice==0){
	//	timeThread=CreateThread(NULL,0,receivetime,(LPVOID)&xx,0,NULL);


		
	}


		Init();
    
	
    	setorigin(0, 0);
	
	setfillcolor(0x000000);
	fillrectangle(20,20,150,60);
	sprintf(text,"您的得分：%d",mydefen);
	outtextxy(20, 20, text);
	sprintf(text,"对手的得分：%d",opdefen);
    outtextxy(20, 45, text);

	setorigin(300, 20);
   
		CMD c; 
	while(true)
	{  	
		c = GetCmd();
		DispatchCmd(c);

		// 按退出时，显示对话框咨询用户是否退出
		if (c == CMD_QUIT)
		{
			HWND wnd = GetHWnd();
			if (MessageBox(wnd, _T("您要退出游戏吗？"), _T("提醒"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
				Quit();
		}
		//playee=receiveNum();
	

	}
}

 
void drawothers(BYTE g_World[10][22],int x,int y){
	for(int j=21,ii=0;j>=0;j--,ii++){
		for(int i =0,jj =0;i<WIDTH;i++,jj++){
			printf(" %d",g_World[i][j]);
			if(g_World[i][j]==1){	
			
				
			drawoppo(x+(jj)*(UNIT-5),y+(ii)*(UNIT-5));}
		}
		printf("\n");
	}
printf("\n");
}

void drawoppo(int x,int y){
	rectangle(
     x,
     y,
     x+UNIT-5,
     y+UNIT-5
);
	fillrectangle(
     x,
     y,
     x+UNIT-5,
     y+UNIT-5
);
}


//双方共有线程函数
DWORD WINAPI sendmyMAP(){     //发送我方地图
  // printf("  %d  ",choice);
	for(int i=0;i<WIDTH;i++)
		for(int j=0;j<HEIGHT;j++){
		
			if(choice==1)
				sendNum(g_World[i][j]?1:0);
			else if(choice==0)
				sendNumC(g_World[i][j]?1:0);
		}


    return 1;
}           
DWORD WINAPI sendmyfenshu(){         //发送我的分数

			if(choice==1)
				sendNum(mydefen);
			else if(choice==0)
				sendNumC(mydefen);
    return 1;
}
DWORD WINAPI receiveMAP(LPVOID xx){		  //接收敌方地图
	int a;
	// printf("  %d  ",choice);
	while(1){		
	for(int i=0;i<WIDTH;i++)
		for(int j=0;j<HEIGHT;j++){
			
			if(choice==1)
			{   
				receiveNum(&a);
				
				//	printf("%d ",a);
				op_g_World[i][j]=a?1:0;
				
			}			
			else if(choice==0){
				
				receiveNumC(&a);
				
				if(a==555)
					Sleep(2000);
				op_g_World[i][j]=a?1:0;
			}
		}
		//Sleep(500);
			
	}		
	return 1;
}
DWORD WINAPI receivefenshu(LPVOID xx){	  //接收敌方分数
	while(mytime>160){	
			if(choice==1)
				receiveNum(&opdefen);
			else if(choice==0)
				receiveNumC(&opdefen);
				
	}
    return 1;
		
}

//服务器线程函数
DWORD WINAPI sendtime(LPVOID xx){			 //发送比赛剩余时间
	while (mytime>0){
		sendNum(555);
		sendNum(mytime);
		Sleep(1000);
		mytime--;
	}
	return 1;
}	
//客户端线程函数
DWORD WINAPI receivetime(LPVOID xx){		//接收比赛剩余时间
	while (1){
		int tab=0;
		receiveNumC(&tab);
		if(tab==555)
		receiveNumC(&mytime);
		
		if(mytime<0)
			break;
	}
	
	return 1;
}

DWORD WINAPI newtime(LPVOID xx){		//接收比赛剩余时间
	while(mytime>0){
		
	setorigin(0, 0);
	
	setfillcolor(0x000000);
	fillrectangle(20,20,170,60);
	sprintf(text,"您的得分：%d 时间：%d",mydefen,mytime);
	outtextxy(20, 20, text);
	sprintf(text,"对手的得分：%d",opdefen);
    outtextxy(20, 45, text);
	
	setorigin(300, 20);
Sleep(5000);
	}
	return 1;
}