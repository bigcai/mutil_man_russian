////////////////////////////////////////////
// �������ƣ�����˹����
// ���뻷����Visual C++ 6.0 / 2010��EasyX_v20131006(beta)
// �����д��liangdu <596630076@qq.com>
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
 char receiveBuf[100]={0};//����
SOCKET clientSocket;
 SOCKADDR_IN clientsock_in;
/////////////////////////////////////////////
// ���峣����ö�������ṹ�塢ȫ�ֱ���
/////////////////////////////////////////////

#define	WIDTH	10		// ��Ϸ������
#define	HEIGHT	22		// ��Ϸ���߶�
#define	UNIT	20		// ÿ����Ϸ����λ��ʵ������

// �����������
enum CMD
{
	CMD_ROTATE,						// ������ת
	CMD_LEFT, CMD_RIGHT, CMD_DOWN,	// �������ҡ����ƶ�
	CMD_SINK,						// �������
	CMD_QUIT						// �˳���Ϸ
};

// ������Ʒ���ķ���
enum DRAW
{
	SHOW,	// ��ʾ����
	CLEAR,	// ��������
	FIX		// �̶�����
};

// �������ֶ���˹����
struct BLOCK
{
	WORD dir[4];	// ������ĸ���ת״̬
	COLORREF color;	// �������ɫ
}	g_Blocks[7] = {	{0x0F00, 0x4444, 0x0F00, 0x4444, RED},		// I
					{0x0660, 0x0660, 0x0660, 0x0660, BLUE},		// ��
					{0x4460, 0x02E0, 0x0622, 0x0740, MAGENTA},	// L
					{0x2260, 0x0E20, 0x0644, 0x0470, YELLOW},	// ��L
					{0x0C60, 0x2640, 0x0C60, 0x2640, CYAN},		// Z
					{0x0360, 0x4620, 0x0360, 0x4620, GREEN},	// ��Z
					{0x4E00, 0x4C40, 0x0E40, 0x4640, BROWN}};	// T

// ���嵱ǰ���顢��һ���������Ϣ
struct BLOCKINFO
{
	byte id;	// ���� ID
	char x, y;	// ��������Ϸ���е�����
	byte dir:2;	// ����
}	g_CurBlock, g_NextBlock,xx;

// ������Ϸ��
BYTE g_World[WIDTH][HEIGHT] = {0};
//�з�����Ϸ��
BYTE op_g_World[WIDTH][HEIGHT] = {0};
//�ҵĵ÷�,���˵ĵ÷�
int mydefen,opdefen;
//����ʣ��ʱ��
int mytime=60;
/////////////////////////////////////////////
// ��������
/////////////////////////////////////////////

void Init();											// ��ʼ����Ϸ
void Quit();											// �˳���Ϸ
void NewGame();											// ��ʼ����Ϸ
void GameOver();										// ������Ϸ
CMD  GetCmd();											// ��ȡ��������
void DispatchCmd(CMD _cmd);								// �ַ���������
void NewBlock();										// �����µķ���
bool CheckBlock(BLOCKINFO _block);						// ���ָ�������Ƿ���Է���
void DrawUnit(int x, int y, COLORREF c, DRAW _draw);	// ����Ԫ����
void DrawBlock(BLOCKINFO _block, DRAW _draw = SHOW);	// ������
void OnRotate();										// ��ת����
void OnLeft();											// ���Ʒ���
void OnRight();											// ���Ʒ���
void OnDown();											// ���Ʒ���
void OnSink();											// ���׷���



void doo(int x, int y,IMAGE img);						// ��굥���¼�
void up(int x,int y,IMAGE img);							// ����ɿ��¼�
void backspace();										// ������һ��
void usertable();										// �û�ѡ�����
int SorC();												// ѡ�񷿼�Ȩ��
char *login(char s[],int n);											// ��¼ģ��
char s[20];
IMAGE img1,img2,img3,img4,img5,img6,img7;
MOUSEMSG  msg;



														
void buildConne();										//S��������
void sendNum(int number);								//S��������
void receiveNum(int *number);										//S��������
void buildConneC();										//C��������
void sendNumC(int number);								//C��������
void receiveNumC(int *number);										//C��������
void drawoppo(int x,int y);									
void drawothers(BYTE g_World[10][22],int x,int y);

void rungame();

//˫�������̺߳���
DWORD WINAPI sendmyMAP();             //�����ҷ���ͼ
DWORD WINAPI sendmyfenshu();          //�����ҵķ���
DWORD WINAPI receiveMAP(LPVOID xx);			  //���յз���ͼ
DWORD WINAPI receivefenshu(LPVOID xx);		  //���յз�����
DWORD WINAPI newtime(LPVOID xx);

//�������̺߳���
DWORD WINAPI sendtime(LPVOID xx);			 //���ͱ���ʣ��ʱ��
//�ͻ����̺߳���
DWORD WINAPI receivetime(LPVOID xx);			//���ձ���ʣ��ʱ��

//5���̣߳��ֱ����ڷ���MAP�����ͷ���������MAP,���շ�����ʣ��ʱ�����
HANDLE sendMapThread,sendFenshuThread,recMapThread,recFenshuThread,timeThread,newtimeThread;

DWORD id;

/////////////////////////////////////////////
// ��������
/////////////////////////////////////////////

// ������
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
		closesocket(serConn); //�ر�
		WSACleanup(); //�ͷ���Դ�Ĳ���
	}else if(choice==0){
		closesocket(clientSocket);
		WSACleanup();
	}
	else{
		return ;
	}

}
void usertable(){
	 initgraph(640, 480);   // ����� TC ��������
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
	while (MouseHit())			// ���������Ϣ��ʱ��ִ��
	{
		msg = GetMouseMsg();	// ��ȡ�����Ϣ

		switch(msg.uMsg)		// ���ݲ�ͬ�������Ϣ��ִ�в�ͬ�Ĵ���
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

	

	Sleep(20);					// ��ʱ������ CPU ռ����
}

    
    closegraph();          // �ر�ͼ�ν���
}

// ��ʼ����Ϸ
void Init()
{

	
		initgraph(640, 480);
	
	 putimage(0, 0, &imgbk);   
	
    setbkmode(TRANSPARENT);	// ����ͼ�����ı���ɫΪ͸��
 
	// ��ʾ����˵��
	srand((unsigned)time(NULL));
	settextstyle(14, 0, _T("����"));
	outtextxy(20, 330, _T("����˵��"));
	outtextxy(20, 350, _T("�ϣ���ת"));
	outtextxy(20, 370, _T("������"));
	outtextxy(20, 390, _T("�ң�����"));
	outtextxy(20, 410, _T("�£�����"));
	outtextxy(20, 430, _T("�ո񣺳���"));
	outtextxy(20, 450, _T("ESC���˳�"));
    
	
	//�з�����Ϸ����
	rectangle(100, 70, WIDTH *( UNIT-5)+100, HEIGHT * (UNIT-5)+70);
	setfillcolor(BLACK);
	
	solidrectangle(100, 70, WIDTH *( UNIT-5)+100, HEIGHT * (UNIT-5)+70);
	ZeroMemory(g_World, WIDTH * HEIGHT);
	//��ʾ�÷�
	setorigin(0, 0);
	
	setfillcolor(0x000000);
	fillrectangle(20,20,150,60);
	sprintf(text,"���ĵ÷֣�%d",mydefen);
	outtextxy(20, 20, text);
	sprintf(text,"���ֵĵ÷֣�%d",opdefen);
    outtextxy(20, 45, text);

	setorigin(300, 20);

   
	// ������Ϸ���߽�

		
		//�ҷ�����Ϸ����
	rectangle(-1, -1, WIDTH * UNIT, HEIGHT * UNIT);
	rectangle((WIDTH + 1) * UNIT - 1, -1, (WIDTH + 5) * UNIT, 4 * UNIT);

    
	// ��ʼ����Ϸ
	NewGame();
}


// �˳���Ϸ
void Quit()
{
	closegraph();
	exit(0);
}


// ��ʼ����Ϸ
void NewGame()
{
	setorigin(0, 0);
	setfillcolor(BLACK);
	solidrectangle(100, 70, WIDTH *( UNIT-5)+100, HEIGHT * (UNIT-5)+70);
	setorigin(300, 20);
	// �����Ϸ��
	setfillcolor(BLACK);
	
	solidrectangle(0, 0, WIDTH * UNIT - 1, HEIGHT * UNIT - 1);
	ZeroMemory(g_World, WIDTH * HEIGHT);

	// ������һ������
	g_NextBlock.id = rand() % 7;
	g_NextBlock.dir = rand() % 4;
	g_NextBlock.x = WIDTH + 1;
	g_NextBlock.y = HEIGHT - 1;

	// ��ȡ�·���
	NewBlock();
}


// ������Ϸ
void GameOver()
{
	HWND wnd = GetHWnd();
	if (MessageBox(wnd, _T("��Ϸ������\n����������һ����"), _T("��Ϸ����"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		NewGame();
	else
		Quit();
}


// ��ȡ��������
DWORD m_oldtime;
CMD GetCmd()
{
	// ��ȡ����ֵ
	while(true)
	{
		// �����ʱ���Զ�����һ��
		DWORD newtime = GetTickCount();
		if (newtime - m_oldtime >= 500)
		{
			m_oldtime = newtime;
			return CMD_DOWN;
		}

		// ����а��������ذ�����Ӧ�Ĺ���
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

		// ��ʱ (���� CPU ռ����)
		Sleep(20);
	}
}


// �ַ���������
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


// �����µķ���
void NewBlock()
{   
	setorigin(0, 0);
	
	setfillcolor(0x000000);
	fillrectangle(20,20,170,60);
	sprintf(text,"���ĵ÷֣�%d ʱ�䣺%d",mydefen,mytime);
	outtextxy(20, 20, text);
	sprintf(text,"���ֵĵ÷֣�%d",opdefen);
    outtextxy(20, 45, text);
	
	setorigin(300, 20);
	g_CurBlock.id = g_NextBlock.id,		g_NextBlock.id = rand() % 7;
	g_CurBlock.dir = g_NextBlock.dir,	g_NextBlock.dir = rand() % 4;
	g_CurBlock.x = (WIDTH - 4) / 2;
	g_CurBlock.y = HEIGHT + 2;

	// �����·���ֱ���оֲ���ʾ
	WORD c = g_Blocks[g_CurBlock.id].dir[g_CurBlock.dir];
	while((c & 0xF) == 0)
	{
		g_CurBlock.y--;
		c >>= 4;
	}

	// �����·���
	DrawBlock(g_CurBlock);

	// ������һ������
	setfillcolor(BLACK);
	solidrectangle((WIDTH + 1) * UNIT, 0, (WIDTH + 5) * UNIT - 1, 4 * UNIT - 1);
	DrawBlock(g_NextBlock);

	// ���ü�ʱ���������ж��Զ�����
	m_oldtime = GetTickCount();
}


// ����Ԫ����
void DrawUnit(int x, int y, COLORREF c, DRAW _draw)
{
	// ���㵥Ԫ�����Ӧ����Ļ����
	int left = x * UNIT;
	int top = (HEIGHT - y - 1) * UNIT;
	int right = (x + 1) * UNIT - 1;
	int bottom = (HEIGHT - y) * UNIT - 1;

	// ����Ԫ����
	switch(_draw)
	{
		case SHOW:
			// ����ͨ����
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
			// ���̶��ķ���
			setfillcolor(RGB(GetRValue(c) * 2 / 3, GetGValue(c) * 2 / 3, GetBValue(c) * 2 / 3));
			setlinecolor(DARKGRAY);
			fillrectangle(left + 1, top + 1, right - 1, bottom - 1);
			break;

		case CLEAR:
			// ��������
			setfillcolor(BLACK);
			solidrectangle(x * UNIT, (HEIGHT - y - 1) * UNIT, (x + 1) * UNIT - 1, (HEIGHT - y) * UNIT - 1);
			break;
	}
}


// ������
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


// ���ָ�������Ƿ���Է���
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


// ��ת����
void OnRotate()
{
	// ��ȡ������ת�� x ƫ����
	int dx;
	BLOCKINFO tmp = g_CurBlock;
	tmp.dir++;					if (CheckBlock(tmp))	{	dx = 0;		goto rotate;	}
	tmp.x = g_CurBlock.x - 1;	if (CheckBlock(tmp))	{	dx = -1;	goto rotate;	}
	tmp.x = g_CurBlock.x + 1;	if (CheckBlock(tmp))	{	dx = 1;		goto rotate;	}
	tmp.x = g_CurBlock.x - 2;	if (CheckBlock(tmp))	{	dx = -2;	goto rotate;	}
	tmp.x = g_CurBlock.x + 2;	if (CheckBlock(tmp))	{	dx = 2;		goto rotate;	}
	return;

rotate:
	// ��ת
	DrawBlock(g_CurBlock, CLEAR);
	g_CurBlock.dir++;
	g_CurBlock.x += dx;
	DrawBlock(g_CurBlock);
}


// ���Ʒ���
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


// ���Ʒ���
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


// ���Ʒ���
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
		OnSink();	// ��������ʱ��ִ�С����׷��顱����
}


// ���׷���
void OnSink()
{  
	int i, x, y;

	// �������Ʒ���
	DrawBlock(g_CurBlock, CLEAR);
	BLOCKINFO tmp = g_CurBlock;
	tmp.y--;
	while (CheckBlock(tmp))
	{
		g_CurBlock.y--;
		tmp.y--;
	}
	DrawBlock(g_CurBlock, FIX);

	// �̶���������Ϸ��
	WORD b = g_Blocks[g_CurBlock.id].dir[g_CurBlock.dir];
	for(i = 0; i < 16; i++, b <<= 1)
		if (b & 0x8000)
		{
			if (g_CurBlock.y - i / 4 >= HEIGHT)
			{	// �������Ĺ̶�λ�ó����߶ȣ�������Ϸ
				GameOver();
				return;
			}
			else
				g_World[g_CurBlock.x + i % 4][g_CurBlock.y - i / 4] = 1;
		}

	// ����Ƿ���Ҫ�����У������
	BYTE remove = 0;	// �� 4 λ������Ƿ����漰�� 4 ���Ƿ���������Ϊ
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

	if (remove)	// ���������������
	{   
			 mydefen++;
		     if(choice!=-1)
	         sendmyfenshu();	
		   


	setorigin(0, 0);
	
	setfillcolor(0x000000);
	fillrectangle(20,20,150,60);
	sprintf(text,"���ĵ÷֣�%d",mydefen);
	outtextxy(20, 20, text);
	sprintf(text,"���ֵĵ÷֣�%d",opdefen);
    outtextxy(20, 45, text);

	setorigin(300, 20);



		// ��ʱ 300 ����
		Sleep(300);

		// �����ղű�ǵ���
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
	// �����·���
	 
	NewBlock();
}




void doo(int x, int y,IMAGE img){  //���°�ť����ʾ�°���ťЧ��
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
void up(int x,int y,IMAGE img){     //�ɿ���꣬������Ӧ�¼�
		putimage(x, y, &img);
		if(x>=280 && x<(img.getwidth()+280)  &&  y>=280 && y<=(280+img.getheight())){
			if(SorC()){     //�ж��Ƿ�ѡ�񵱷���������ǣ���
				//���������� 
                      choice=1;
					buildConne();    //�������ӣ��ȴ�������������
					rungame();

			}
			else{     //���ѡ�񵱷���ˡ���
				
				choice=0;
				login(s,20);          //���뷿����IP��ַ��S�û���ŷ�����IP��ַ
				buildConneC();        //���������������
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
	if (MessageBox(wnd, _T("��ȷ��Ҫ����ѡ�������"), _T("���ضԻ���"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		usertable();
}

//ѡ��Ȩ��
int SorC(){
	HWND wnd = GetHWnd();
	if (MessageBox(wnd, _T("���Ƿ�ѡ���Ϊ������"), _T("ѡ��Ȩ��"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		return 1;
	else
		return 0;
	
}

//��¼ģ��
char *login(char s[],int n){
	
	InputBox(s, 20, "����������IP","��¼����",NULL,0,0,false);
	return s;
}






void buildConne(){
	myVersionRequest=MAKEWORD(1,1);
	
	err=WSAStartup(myVersionRequest,&wsaData);
 if (!err)
 {
  printf("�Ѵ��׽���\n");

 } 
 else
 {
  //��һ�����׽���
  printf("Ƕ����δ��!");
  return;
 }
 serSocket=socket(AF_INET,SOCK_STREAM,0);//�����˿�ʶ���׽���
 //��Ҫ�󶨵Ĳ���

 addr.sin_family=AF_INET;
 addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//ip��ַ
 addr.sin_port=htons(6885);//�󶨶˿�
 
 bind(serSocket,(SOCKADDR*)&addr,sizeof(SOCKADDR));//�����
 listen(serSocket,5);//���еڶ������������ܹ����յ�����������
 
 //////////////////////////////////////////////////////////////////////////
 //��ʼ���м���
 //////////////////////////////////////////////////////////////////////////
 
 int len=sizeof(SOCKADDR);
 //while (1)
 //{
	serConn=accept(serSocket,(SOCKADDR*)&clientsocket,&len);//������ﲻ��accept����conection�Ļ������ͻ᲻�ϵļ���
  
  
  
	

}


void sendNum(int number){
	sprintf(sendBuf,"%d",number);//�ҶԶ�Ӧ��IP���ҽ������ִ�ӡ������
	
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
	err=WSAStartup(versionRequired,&wsaData);//Э���İ汾��Ϣ

 if (!err)
 {
  printf("�ͻ���Ƕ�����Ѿ���!\n");
 }
 else
 {
  printf("�ͻ��˵�Ƕ���ִ�ʧ��!\n");
  return;//����
 }
 clientSocket=socket(AF_INET,SOCK_STREAM,0);
 //clientsock_in.sin_addr.S_un.S_addr=inet_addr(s);// ��������ַ
 clientsock_in.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");// ��������ַ
 clientsock_in.sin_family=AF_INET;
 clientsock_in.sin_port=htons(6885);                       //���ӷ������˿�
 //bind(clientSocket,(SOCKADDR*)&clientsock_in,strlen(SOCKADDR));//ע�����������
 //listen(clientSocket,5);

  connect(clientSocket,(SOCKADDR*)&clientsock_in,sizeof(SOCKADDR));//��ʼ����
	
 
	
}	




									//C��������
void receiveNumC(int *number){
	k=0;
	while(!k)
    k=recv(clientSocket,receiveBuf,101,0);

	
	sscanf(receiveBuf,"%d",number);
	
}								//C��������
void sendNumC(int number){
	 	sprintf(sendBuf,"%d",number);//�ҶԶ�Ӧ��IP���ҽ������ִ�ӡ������
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
	sprintf(text,"���ĵ÷֣�%d",mydefen);
	outtextxy(20, 20, text);
	sprintf(text,"���ֵĵ÷֣�%d",opdefen);
    outtextxy(20, 45, text);

	setorigin(300, 20);
   
		CMD c; 
	while(true)
	{  	
		c = GetCmd();
		DispatchCmd(c);

		// ���˳�ʱ����ʾ�Ի�����ѯ�û��Ƿ��˳�
		if (c == CMD_QUIT)
		{
			HWND wnd = GetHWnd();
			if (MessageBox(wnd, _T("��Ҫ�˳���Ϸ��"), _T("����"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
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


//˫�������̺߳���
DWORD WINAPI sendmyMAP(){     //�����ҷ���ͼ
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
DWORD WINAPI sendmyfenshu(){         //�����ҵķ���

			if(choice==1)
				sendNum(mydefen);
			else if(choice==0)
				sendNumC(mydefen);
    return 1;
}
DWORD WINAPI receiveMAP(LPVOID xx){		  //���յз���ͼ
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
DWORD WINAPI receivefenshu(LPVOID xx){	  //���յз�����
	while(mytime>160){	
			if(choice==1)
				receiveNum(&opdefen);
			else if(choice==0)
				receiveNumC(&opdefen);
				
	}
    return 1;
		
}

//�������̺߳���
DWORD WINAPI sendtime(LPVOID xx){			 //���ͱ���ʣ��ʱ��
	while (mytime>0){
		sendNum(555);
		sendNum(mytime);
		Sleep(1000);
		mytime--;
	}
	return 1;
}	
//�ͻ����̺߳���
DWORD WINAPI receivetime(LPVOID xx){		//���ձ���ʣ��ʱ��
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

DWORD WINAPI newtime(LPVOID xx){		//���ձ���ʣ��ʱ��
	while(mytime>0){
		
	setorigin(0, 0);
	
	setfillcolor(0x000000);
	fillrectangle(20,20,170,60);
	sprintf(text,"���ĵ÷֣�%d ʱ�䣺%d",mydefen,mytime);
	outtextxy(20, 20, text);
	sprintf(text,"���ֵĵ÷֣�%d",opdefen);
    outtextxy(20, 45, text);
	
	setorigin(300, 20);
Sleep(5000);
	}
	return 1;
}