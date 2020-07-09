#undef UNICODE
#undef _UNICODE
#include <graphics.h>
#include<stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <windows.h>
#include<algorithm>
#include <cstring>
#include <easyx.h>
#include <time.h>
#include<queue>
#define GRIDSIZE 8
#define INF 100
using namespace std;

int gridInfo[8][8]; //真实棋盘,2为黑棋,3为白棋
int gridInfo_back_up[8][8]; //棋盘备份

struct pos { int x; int y; };
int ai_side, pl_side; //ai与玩家的color
pos ai[4], pl[4]; //双方Queen位置
pos pl_cursor; pos pl_start, pl_end, pl_obstacle; //玩家使用

int dx[8] = { 1,0,-1,0,1,1,-1,-1 };
int dy[8] = { 0,1,0,-1,1,-1,1,-1 };
int legality[8][8]; //合法的走棋位置
//qmove,kmove用于评估函数
int queenmove[8][8][4]; //黑为2,白为3
int kingmove[8][8][4]; //同上
int turnID = 0; //循环数

//Mouse
int msx, msy; int msx_grid, msy_grid; //鼠标位置与所在格子

//Images
IMAGE menu_bk, board_bk; //背景图片
COLORREF SKY = RGB(30, 144, 235);

//Files
char address[50] = "D:/Amazons/Saves/save0.txt";
int flag_load = 0;

//Graphic Sizes
const int graph_width = 960, graph_height = 540; //窗口尺寸
const int text_height = 20;
const int but_height = 40, but_width = 200; //按键大小
const int board_point1 = 54, board_point2 = 486; //棋盘顶点坐标
const int center = 270;
const int gridsiz = 54;

//Discriptions
char menu[4][10] = { "新游戏", "载入存档", "游戏规则", "退出" };
char pause[20] = { "点击以继续游戏." };
char quit[2][50] = { "确认要退出吗?您的进度将不会被保存.", "确定" };
char board[3][15] = { "保存游戏", "暂停", "退出至主菜单" };
char result[2][15] = { "你赢了!", "很遗憾,你输了." };
char back[5] = { "返回" };
char savesuc[20] = { "保存成功!" };

//Character Drawings
const int amazons[7][41] = 
{
{0,1,1,1,0,0,0,1,0,1,0,0,0,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,0,1,1,0,0,1,0,0,1,1,1,1},
{1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0},
{1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,0,1,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0},
{1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,0,1,1,1,0},
{1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,1},
{1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,1},
{1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,1,1,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,1,1,1,0}
};

int startX, startY, resultX, resultY, obstacleX, obstacleY;
double now;
const double priorAdvantage = 0.2;//调参值

//----- FUNCTIONS -----//
void init_set() 
{
	initgraph(graph_width, graph_height);
	settextstyle(text_height, 0, 0);
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	loadimage(&menu_bk, _T("D:/Amazons/menu_bk.jpg"), graph_width, graph_height, FALSE);
	loadimage(&board_bk, _T("D:/Amazons/board_bk.jpg"), graph_width, graph_height, FALSE);
}

//---UI inittration---//
void init_amazons() 
{
	for (int q = 0; q < 7; ++q) 
	{
		Sleep(100);
		cleardevice();
		for (int i = 0; i < 7; ++i) 
			for (int j = 0; j < 41; ++j) 
				if (amazons[i][j] && (j < 24 || i <= q)) 
					fillcircle(80 + 20 * j, 210 + 20 * i, 8);
	}
	Sleep(1000);//图形化初始界面
	COLORREF TEMP = RGB(0, 0, 0);
	int light = 0;
	while (light < 255) 
	{
		cleardevice();
		light += 15;
		TEMP = RGB(light, light, light);
		setlinecolor(TEMP);
		setbkcolor(TEMP);
		for (int i = 0; i < 7; ++i) 
			for (int j = 0; j < 41; ++j) 
				if (amazons[i][j]) 
					fillcircle(80 + 20 * j, 210 + 20 * i, 8); 
		Sleep(60);
	}//设置退出的渐变效果
	setfillcolor(WHITE);
	bar(0, 0, graph_width, graph_height);
	Sleep(600);
	setlinecolor(WHITE);
}
void init_main() {
	cleardevice();
	putimage(0, 0, &menu_bk);
	setfillcolor(WHITE);
	int rec[4] = { -250,-350,-450,-550 };
	while (1) 
	{ 
		cleardevice();
		putimage(0, 0, &menu_bk);
		bar(rec[0], 85, rec[0] + 250, 125);
		bar(rec[1], 195, rec[1] + 250, 235);
		bar(rec[2], 305, rec[2] + 250, 345);
		bar(rec[3], 415, rec[3] + 250, 455);
		if (rec[3] == 100)
			break; 
		for (int i = 0; i < 4; ++i) 
			if (rec[i] < 100)
				rec[i] += 50;
		Sleep(30);
	}
	settextstyle(20, 0, _T("宋体"));
	settextcolor(BLACK);
	outtextxy(120, 95, menu[0]);
	outtextxy(120, 205, menu[1]);
	outtextxy(120, 315, menu[2]);
	outtextxy(120, 425, menu[3]);
}
void init_settings() {
	cleardevice();
	putimage(0, 0, &board_bk);
	settextstyle(20, 0, _T("宋体"));
	settextcolor(WHITE);
	outtextxy(20, 10, "1、在10*10的棋盘上，每方有四个棋子（四个Amazons）；");
	outtextxy(20, 60, "2、每个棋子都相当于国际象棋中的皇后，它们的行棋方法与皇后相同，可以 ");
	outtextxy(20, 110, "  在八个方向上任意行走，但不能穿过阻碍；");
	outtextxy(20, 160, "3、当轮到一方行棋时，此方只能而且必须移动四个Amazons中的一个，并在");
	outtextxy(20, 210, "  移动完成后，由当前移动的棋子释放一个障碍，障碍的释放方法与棋子的");
	outtextxy(20, 260, "  移动方法相同（皇后的走法，不能穿过障碍），同样障碍的放置也是必须的；");
	outtextxy(20, 310, "4、当某方完成某次移动后，对方四个棋子均不能再移动时，对方将输掉比赛； ");
	outtextxy(20, 360, "5、每次开局位于棋盘下方的玩家先手； ");
	outtextxy(20, 410, "6、整个比赛中双方均不能吃掉对方或己方的棋子或障碍。 ");
	setfillcolor(WHITE);
	bar(380, 480, 580, 530);
	setbkmode(WHITE);
	settextstyle(40, 0, _T("宋体"));
	settextcolor(BLACK);
	outtextxy(400, 490, "关闭规则");
}
void init_saveload() 
{
	cleardevice();
	putimage(0, 0, &board_bk);
	setfillcolor(WHITE);
	char save[6] = "0";
	for (int i = 1; i < 6; ++i) 
	{
		bar(180, 90 * i - 20, 380, 90 * i + 20);
		save[0] = i + '0';
		outtextxy(200, 90 * i - 10, save);
	}
	for (int i = 6; i < 10; ++i) 
	{
		bar(580, 90 * i - 470, 780, 90 * i - 430);
		save[0] = i + '0';
		outtextxy(600, 90 * i - 460, save);
	}
	bar(580, 430, 780, 470);
	outtextxy(600, 440, back);
}
void init_colorselect() 
{
	cleardevice();
	putimage(0, 0, &menu_bk);
	setfillcolor(WHITE);
	bar(380, 100, 580, 200);
	setfillcolor(WHITE);
	bar(380, 340, 580, 440);
	setbkmode(WHITE);
	settextstyle(60, 0, _T("宋体"));
	settextcolor(BLACK);
	outtextxy(420, 120, "黑棋");
	setbkmode(WHITE);
	settextstyle(60, 0, _T("宋体"));
	settextcolor(BLACK);
	outtextxy(420, 360, "白棋");
}
void init_pause() 
{
	putimage(0, 0, &board_bk);
	setfillcolor(WHITE);
	bar(380, 260, 580, 300);
	settextstyle(20, 0, _T("宋体"));
	settextcolor(BLACK);
	outtextxy(400, 270, pause);
}
void init_quit() {
	cleardevice();
	putimage(0, 0, &board_bk);
	setfillcolor(WHITE);
	bar(325, 230, 700, 260);
	outtextxy(345, 240, quit[0]);
	setfillcolor(WHITE);
	bar(440, 280, 520, 320);
	outtextxy(463, 290, quit[1]);
}
void init_chessboard(int k = 0) 
{
	cleardevice();
	putimage(0, 0, &board_bk);
	int sleeptime = 30;
	for (int i = 0; i < 9; ++i) 
	{
		line(board_point1, board_point1 + gridsiz * i, board_point2, board_point1 + gridsiz * i);
		if (k) 
			Sleep(sleeptime); 
	}
	for (int i = 0; i < 9; ++i) 
	{
		line(board_point1 + gridsiz * i, board_point1, board_point1 + gridsiz * i, board_point2);
		if (k) 
			Sleep(sleeptime); 
	}
	for (int i = 0; i < 8; ++i) 
		for (int j = 0; j < 8; ++j)
		{
			if (gridInfo[i][j] == 1)
			{
				setfillcolor(SKY);
				fillcircle(board_point1 + gridsiz * 0.5 + gridsiz * i, board_point1 + gridsiz * 0.5 + gridsiz * j, 23);
				if (k)
					Sleep(sleeptime);
			}
			if (gridInfo[i][j] == 2)
			{
				setfillcolor(BLACK);
				fillcircle(board_point1 + gridsiz * 0.5 + gridsiz * i, board_point1 + gridsiz * 0.5 + gridsiz * j, 23);
				if (k) 
					Sleep(sleeptime); 
			}
			if (gridInfo[i][j] == 3) 
			{
				setfillcolor(WHITE);
				fillcircle(board_point1 + gridsiz * 0.5 + gridsiz * i, board_point1 + gridsiz * 0.5 + gridsiz * j, 23);
				if (k) 
					Sleep(sleeptime); 
			}
		}
	setfillcolor(WHITE);
	int temp1 = (board_point2 - board_point1 - 3 * but_height) / 2;
	int temp2 = 630;
	bar(temp2, board_point1, temp2 + but_width, board_point1 + but_height);
	settextstyle(20, 0, _T("宋体"));
	settextcolor(BLACK);
	outtextxy(temp2 + 20, board_point1 + 10, board[0]);
	bar(temp2, board_point1 + but_height + temp1, temp2 + but_width, board_point2 - but_height - temp1);
	settextstyle(20, 0, _T("宋体"));
	settextcolor(BLACK);
	outtextxy(temp2 + 20, board_point1 + but_height + temp1 + 10, board[1]);
	bar(630, board_point2 - but_height, temp2 + but_width, board_point2);
	settextstyle(20, 0, _T("宋体"));
	settextcolor(BLACK);
	outtextxy(temp2 + 20, board_point2 - but_height + 10, board[2]);
}
void init_result(int temp) 
{
	setfillcolor(WHITE);
	bar(center - 100, center - 20, center + 100, center + 20);
	if (temp) 
		outtextxy(center - 80, center - 10, result[0]); 
	else 
		outtextxy(center - 80, center - 10, result[1]); 
}
void init_legality() {
	init_chessboard();
	setfillcolor(YELLOW);
	for (int i = 0; i < 8; ++i)
		for (int j = 0; j < 8; ++j)
			if (legality[i][j] == 1)
				bar(board_point1 + gridsiz * i, board_point1 + gridsiz * j, board_point1 + gridsiz * i + gridsiz, board_point1 + gridsiz * j + gridsiz);
}

//---Mouse Control---//
void get_mouse() 
{
	while (1) 
	{
		MOUSEMSG msg = GetMouseMsg();
		if (msg.mkLButton) {
			msx = msg.x; msy = msg.y;
			FlushMouseMsgBuffer();
			return;
		}
	}
}
void get_mouse_grid() 
{
	msx_grid = (msx - board_point1) / gridsiz;
	msy_grid = (msy - board_point1) / gridsiz;
}

//---File Service---//
int save_id() 
{
	init_saveload();
	get_mouse();
	if (msx > 180 && msx < 380 && msy > 70 && msy < 110) { return 1; }
	if (msx > 180 && msx < 380 && msy > 160 && msy < 200) { return 2; }
	if (msx > 180 && msx < 380 && msy > 250 && msy < 290) { return 3; }
	if (msx > 180 && msx < 380 && msy > 340 && msy < 380) { return 4; }
	if (msx > 180 && msx < 380 && msy > 430 && msy < 470) { return 5; }
	if (msx > 580 && msx < 780 && msy > 70 && msy < 110) { return 6; }
	if (msx > 580 && msx < 780 && msy > 160 && msy < 200) { return 7; }
	if (msx > 580 && msx < 780 && msy > 250 && msy < 290) { return 8; }
	if (msx > 580 && msx < 780 && msy > 340 && msy < 380) { return 9; }
	if (msx > 580 && msx < 780 && msy > 430 && msy < 470) { return 0; }
	return save_id();
}
void get_save() 
{
	int id = save_id();
	if (id) 
	{
		address[21] = id + '0';
		ofstream fff(address);
		fff << pl_side << endl; 
		ai_side = 5 - pl_side;
		fff << turnID - 1 << endl;
		for (int i = 0; i < 4; ++i) 
			fff << pl[i].x << ' ' << pl[i].y << endl;
		for (int i = 0; i < 4; ++i) 
			fff << ai[i].x << ' ' << ai[i].y << endl;
		for (int i = 0; i < 8; ++i) 
			for (int j = 0; j < 8; ++j) 
				fff << gridInfo[i][j] << ' ';
		fff.close();
		setfillcolor(WHITE);
		bar(430, 260, 550, 300);
		settextstyle(20, 0, _T("宋体"));
		settextcolor(BLACK);
		outtextxy(450, 270, savesuc);
		Sleep(1000);
	}
}
int get_load() 
{
	int id = save_id();
	if (id) 
	{
		address[21] = id + '0';
		ifstream fff(address);
		if (!fff) 
			return 0; 
		fff >> pl_side; 
		ai_side = 5 - pl_side;
		fff >> turnID;
		for (int i = 0; i < 4; ++i) {
			fff >> pl[i].x; fff >> pl[i].y;
		}
		for (int i = 0; i < 4; ++i) {
			fff >> ai[i].x; fff >> ai[i].y;
		}
		for (int i = 0; i < 8; ++i) 
			for (int j = 0; j < 8; ++j) 
				fff >> gridInfo[i][j];
		fff.close();
		return 1;
	}
	return 0;
}

//---Other Functions---//
void init_gridInfo() 
{
	memset(gridInfo, 0, sizeof(gridInfo));
	gridInfo[0][2] = 2; gridInfo[2][0] = 2;
	gridInfo[5][0] = 2; gridInfo[7][2] = 2;
	gridInfo[0][5] = 3; gridInfo[2][7] = 3;
	gridInfo[5][7] = 3; gridInfo[7][5] = 3;
	if (ai_side == 2) //ai方是黑棋
	{
		ai[0].x = 0; ai[0].y = 2; ai[1].x = 2; ai[1].y = 0;
		ai[2].x = 5; ai[2].y = 0; ai[3].x = 7; ai[3].y = 2;
		pl[0].x = 0; pl[0].y = 5; pl[1].x = 2; pl[1].y = 7;
		pl[2].x = 5; pl[2].y = 7; pl[3].x = 7; pl[3].y = 5;
	}
	else //ai方是白棋
	{
		pl[0].x = 0; pl[0].y = 2; pl[1].x = 2; pl[1].y = 0;
		pl[2].x = 5; pl[2].y = 0; pl[3].x = 7; pl[3].y = 2;
		ai[0].x = 0; ai[0].y = 5; ai[1].x = 2; ai[1].y = 7;
		ai[2].x = 5; ai[2].y = 7; ai[3].x = 7; ai[3].y = 5;
	}
	turnID = 0;//游戏回合
}

void get_back(int flag)
{
	if (flag == 0)
	{
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				gridInfo_back_up[i][j] = gridInfo[i][j];
			}
		}
	}
	else if (flag == 1)
	{
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				gridInfo[i][j] = gridInfo_back_up[i][j];
			}
		}
	}
}

bool judge(int side) 
{
	if (side == pl_side) 
	{
		for (int q = 0; q < 4; ++q) 
			for (int i = 0; i < 8; ++i) 
			{
				int cx = pl[q].x + dx[i], cy = pl[q].y + dy[i];
				if (gridInfo[cx][cy] == 0 && cx >= 0 && cy >= 0 && cx < 8 && cy < 8) 
					return false; 
			}
	}
	else if (side == ai_side) 
	{
		for (int q = 0; q < 4; ++q) 
			for (int i = 0; i < 8; ++i) 
			{
				int cx = ai[q].x + dx[i], cy = ai[q].y + dy[i];
				if (gridInfo[cx][cy] == 0 && cx >= 0 && cy >= 0 && cx < 8 && cy < 8) 
					return false; 
			}
	}
	return true;
}

//---Basic Functions for AI---//
void refresh(int flag)
{
	if (flag == 0)
	{
		int cnt = 0;
		for (int i = 0; i < 8; ++i)
			for (int j = 0; j < 8; ++j)
				if (gridInfo[i][j] == pl_side)
				{
					pl[cnt].x = i; pl[cnt].y = j;
					cnt++;
				}
	}
	else if (flag == 1)
	{
		int cnt = 0;
		for (int i = 0; i < 8; ++i) 
			for (int j = 0; j < 8; ++j) 
				if (gridInfo[i][j] == ai_side) 
				{
					ai[cnt].x = i; ai[cnt].y = j;
					cnt++;
				}
	}
}

inline bool inMap(int x, int y)
{
	if (x < 0 || x >= 8 || y < 0 || y >= 8)
		return false;
	return true;
}

void refresh_legality(int x, int y) 
{
	memset(legality, 0, sizeof(legality));
	int cx = x; int cy = y;
	for (int q = 0; q < 8; ++q) {
		cx = x; cy = y;
		while ((gridInfo[cx][cy] == 0 || (cx == x && cy == y)) && cx >= 0 && cy >= 0 && cx < 8 && cy < 8) 
		{
			legality[cx][cy] = 1; 
			cx += dx[q], cy += dy[q];
		}
	}
	legality[x][y] = 0;
}

bool ProcStep(int x0, int y0, int x1, int y1, int x2, int y2, int color, bool check_only)
{
	if ((!inMap(x0, y0)) || (!inMap(x1, y1)) || (!inMap(x2, y2)))
		return false;
	if (gridInfo[x0][y0] != color || gridInfo[x1][y1] != 0)
		return false;
	if ((gridInfo[x2][y2] != 0) && !(x2 == x0 && y2 == y0))
		return false;
	if (!check_only)
	{
		gridInfo[x0][y0] = 0;
		gridInfo[x1][y1] = color;
		gridInfo[x2][y2] = 1;
	}
	return true;
}

struct loc
{
	int x, y, d;
	loc(int xx, int yy, int dd)
	{
		x = xx;
		y = yy;
		d = dd;
	}
	loc(void)
	{
		x = 0;
		y = 0;
		d = 0;
	}
};
queue<loc> q_queen;
queue<loc> q_king;
int stepbq[GRIDSIZE][GRIDSIZE];
int stepwq[GRIDSIZE][GRIDSIZE];
int stepbk[GRIDSIZE][GRIDSIZE];
int stepwk[GRIDSIZE][GRIDSIZE];

void init_gridInfo(int color)
{
	if (color == ai_side)
	{
		for (int i = 0; i < GRIDSIZE; i++)
			for (int j = 0; j < GRIDSIZE; j++)
			{
				if (gridInfo[i][j] == color)
				{
					stepbq[i][j] = 0;
					stepbk[i][j] = 0;
					loc b(i, j, 0);
					q_queen.push(b);
					q_king.push(b);
				}
				else
				{
					stepbq[i][j] = INF;
					stepbk[i][j] = INF;
				}
			}
	}
	else {
		for (int i = 0; i < GRIDSIZE; i++)
			for (int j = 0; j < GRIDSIZE; j++)
			{
				if (gridInfo[i][j] == color)
				{
					stepwq[i][j] = 0;
					stepwk[i][j] = 0;
					loc b(i, j, 0);
					q_queen.push(b);
					q_king.push(b);
				}
				else
				{
					stepwq[i][j] = INF;
					stepwk[i][j] = INF;
				}
			}
	}
}

void queen_bfs(int flag) 
{
	if (flag == 0)
	{
		while (!q_queen.empty())
		{
			loc a = q_queen.front(); q_queen.pop();
			for (int k = 0; k < GRIDSIZE; ++k)
			{
				for (int delta1 = 1; delta1 < GRIDSIZE; delta1++)
				{
					int xx = a.x + dx[k] * delta1;
					int yy = a.y + dy[k] * delta1;
					if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))


						break;
					if (stepbq[xx][yy] >= INF)
					{
						loc b(xx, yy, a.d + 1);
						q_queen.push(b);
						stepbq[xx][yy] = a.d + 1;
					}
				}
			}
		}

	}
	else {
		while (!q_queen.empty())
		{
			loc a = q_queen.front(); q_queen.pop();
			for (int k = 0; k < GRIDSIZE; ++k)
			{
				for (int delta1 = 1; delta1 < GRIDSIZE; delta1++)
				{
					int xx = a.x + dx[k] * delta1;
					int yy = a.y + dy[k] * delta1;
					if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
						break;
					if (stepwq[xx][yy] >= INF)
					{
						loc b(xx, yy, a.d + 1);// t.x = xx, t.y = yy, t.d = r.d + 1;
						q_queen.push(b);
						stepwq[xx][yy] = a.d + 1;
					}
				}
			}
		}
	}
}

void king_bfs(int flag)
{
	if (flag == 0)
	{
		while (!q_king.empty())
		{
			loc a = q_king.front(); q_king.pop();
			for (int k = 0; k < GRIDSIZE; ++k)
			{
				int xx = a.x + dx[k];
				int yy = a.y + dy[k];
				if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
					continue;
				if (stepbk[xx][yy] >= INF)
				{
					loc b(xx, yy, a.d + 1);
					q_king.push(b);
					stepbk[xx][yy] = a.d + 1;
				}
			}
		}
	}
	else {
		while (!q_king.empty())
		{
			loc a = q_king.front(); q_king.pop();
			for (int k = 0; k < GRIDSIZE; ++k)
			{
				int xx = a.x + dx[k];
				int yy = a.y + dy[k];
				if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
					continue;
				if (stepwk[xx][yy] >= INF)
				{
					loc b(xx, yy, a.d + 1);
					q_king.push(b);
					stepwk[xx][yy] = a.d + 1;
				}
			}
		}
	}
}

double func()
{
	loc a;
	init_gridInfo(ai_side);
	queen_bfs(0);// 0 for black
	king_bfs(0);

	init_gridInfo(pl_side);
	queen_bfs(1);
	king_bfs(1);// 1 for white

	double sum1 = 0, sum2 = 0;
	for (int i = 0; i < GRIDSIZE; i++)
		for (int j = 0; j < GRIDSIZE; j++)
		{
			sum1 += -(stepbq[i][j] > stepwq[i][j]) + (stepbq[i][j] < stepwq[i][j])
				+ 0.2 * (stepbq[i][j] == stepwq[i][j]) - 0.2 * (stepbq[i][j] == INF && stepwq[i][j] == INF);
			sum2 += -(stepbk[i][j] > stepwk[i][j]) + (stepbk[i][j] < stepwk[i][j])
				+ 0.2 * (stepbk[i][j] == stepwk[i][j]) - 0.2 * (stepbk[i][j] == INF && stepwk[i][j] == INF);
		}
	if (turnID <= 20)
		return sum1 * 0.14 + sum2 * 0.37;
	else if (turnID < 50)
		return sum1 * 0.30 + sum2 * 0.25;
	else
		return sum1 * 0.80 + sum2 * 0.10;
}
double dfsw(double zz, int size)
{
	double now_time = clock();
	if ((now_time - now) / CLOCKS_PER_SEC > 0.95) return -INF;
	double min = 200;
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			if (gridInfo[i][j] == pl_side)
				for (int k = 0; k < GRIDSIZE; ++k)
				{
					for (int delta1 = 1; delta1 < GRIDSIZE; delta1++)
					{
						int xx = i + dx[k] * delta1;
						int yy = j + dy[k] * delta1;
						if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
							break;
						for (int l = 0; l < size; ++l)
						{
							for (int delta2 = 1; delta2 < size; delta2++)
							{
								int xxx = xx + dx[l] * delta2;
								int yyy = yy + dy[l] * delta2;
								if (!inMap(xxx, yyy))
									break;
								if (gridInfo[xxx][yyy] != 0 && !(i == xxx && j == yyy))
									break;
								gridInfo[xxx][yyy] = 1;
								gridInfo[xx][yy] = pl_side;
								gridInfo[i][j] = 0;


								double territory;
								territory = func();
								if (territory < min)
									min = territory;
								if (min < zz)
								{
									gridInfo[xxx][yyy] = 0;
									gridInfo[xx][yy] = 0;
									gridInfo[i][j] = pl_side;
									return min;
								}
								gridInfo[xxx][yyy] = 0;
								gridInfo[xx][yy] = 0;
								gridInfo[i][j] = pl_side;
							}
						}
					}
				}
		}
	}
	return min;
}
void dfsb()
{
	double max = -200;
	for (int i = 0; i < GRIDSIZE; ++i)
	{
		for (int j = 0; j < GRIDSIZE; ++j)
		{
			if (gridInfo[i][j] == ai_side)
				for (int k = 0; k < GRIDSIZE; ++k)
				{
					for (int delta1 = 1; delta1 < GRIDSIZE; delta1++)
					{
						int xx = i + dx[k] * delta1;
						int yy = j + dy[k] * delta1;
						if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
							break;
						for (int l = 0; l < GRIDSIZE; ++l)
						{
							for (int delta2 = 1; delta2 < GRIDSIZE; delta2++)
							{
								int xxx = xx + dx[l] * delta2;
								int yyy = yy + dy[l] * delta2;
								if (!inMap(xxx, yyy))
									break;
								if (gridInfo[xxx][yyy] != 0 && !(i == xxx && j == yyy))
									break;
								gridInfo[xxx][yyy] = 1;

								gridInfo[xx][yy] = ai_side;
								gridInfo[i][j] = 0;

								double territory;
								territory = dfsw(max, GRIDSIZE);
								if (territory > max)
								{
									max = territory;
									startX = i;
									startY = j;
									resultX = xx;
									resultY = yy;
									obstacleX = xxx;
									obstacleY = yyy;
								}
								gridInfo[xxx][yyy] = 0;
								gridInfo[xx][yy] = 0;
								gridInfo[i][j] = ai_side;
							}
						}
					}
				}
		}
	}
	return;
}


void first_black_ai() 
{ //黑棋第一步的快速落子方案
	int seed = rand();
	if (seed < 16300) 
		{ gridInfo[2][0] = 0; gridInfo[2][6] = 2; gridInfo[5][3] = 1; }
	else 
		{ gridInfo[5][0] = 0; gridInfo[5][6] = 2; gridInfo[2][3] = 1; }
	refresh(1);
}

//----- MAIN PROGRAM -----//
int main() {
	init_set();
	init_amazons();
	srand(time(NULL));
start: init_main();
	//main menu options
start1: get_mouse();
	if (msx > 100 && msx < 350 && msy > 85 && msy < 125) { //New game
		init_colorselect();
		while (1)
		{
			get_mouse();
			if (msx > 380 && msx < 580 && msy > 100 && msy < 200)
			{
				ai_side = 3;
				break;
			}
			else if (msx > 380 && msx < 580 && msy > 340 && msy < 440)
			{
				ai_side = 2;
				break;
			}
		}
		pl_side = 5 - ai_side;
		init_gridInfo();
		init_chessboard(1);
		if (ai_side == 2)
		{ 
			Sleep(1000); 
			first_black_ai();
			refresh(1);
			init_chessboard();
		}
	}
	else if (msx > 100 && msx < 350 && msy > 195 && msy < 235) 
	{ //Load game
		int load_suc = get_load();
		if (load_suc == 0) 
			goto start; 
		else 
			init_chessboard(1); flag_load = 1; 
	}
	else if (msx > 100 && msx < 350 && msy > 305 && msy < 345) 
	{ //Rules
		init_settings();
		get_mouse();
		while (1)
		{
			get_mouse();
			if (msx > 380 && msx < 580 && msy > 480 && msy < 530)
				break;
		}
		goto start;
	}
	else if (msx > 100 && msx < 350 && msy > 415 && msy < 455)
	{ //Quit
		return 0;
	}
	else 
		goto start1; 
	while (1) 
	{
		if (flag_load) 
			flag_load = 0; 
		turnID++;
	layer1: init_chessboard();
		get_back(0);
		get_mouse(); //select entity
		if (msx > board_point1&& msx < board_point2 && msy > board_point1&& msy < board_point2) 
		{ //chessboard
			get_mouse_grid();
			if (gridInfo[msx_grid][msy_grid] == pl_side) 
			{
				pl_cursor.x = msx_grid; pl_cursor.y = msy_grid;
				pl_start.x = msx_grid; pl_start.y = msy_grid;
				refresh_legality(msx_grid, msy_grid);
				init_legality();
			layer2: get_mouse(); //select destination
				if (msx > board_point1&& msx < board_point2 && msy > board_point1&& msy < board_point2) 
				{
					get_mouse_grid();
					if (legality[msx_grid][msy_grid]) 
					{
						pl_cursor.x = msx_grid; pl_cursor.y = msy_grid;
						pl_end.x = msx_grid; pl_end.y = msy_grid;
						gridInfo[pl_start.x][pl_start.y] = 0; gridInfo[pl_end.x][pl_end.y] = pl_side;
						refresh_legality(msx_grid, msy_grid);
						init_legality();
					layer3: get_mouse(); //select 1
						if (msx > board_point1&& msx < board_point2 && msy > board_point1&& msy < board_point2) 
						{
							get_mouse_grid();
							if (legality[msx_grid][msy_grid]) 
							{
								pl_obstacle.x = msx_grid; pl_obstacle.y = msy_grid;
								gridInfo[pl_obstacle.x][pl_obstacle.y] = 1;
								refresh(0);
								init_chessboard();
							}
							else if (msx_grid == pl_end.x && msy_grid == pl_end.y) 
								{get_back(1);  goto layer1; } //cancel input
							else 
								goto layer3;  //illegal input
						}
						else 
							goto layer3; //out of chessboard
					}
					else if (msx_grid == pl_start.x && msy_grid == pl_start.y) 
						{ get_back(1);  goto layer1; } //cancel input
					else 
						goto layer2;  //illegal input
				}
				else 
					goto layer2;  //out of chessboard
			}
			else { goto layer1; } //not selecting player's entity
		}
		else if (msx > 630 && msx < 630 + but_width && msy > board_point1 && msy < board_point1 + but_height)
		{ //save
			get_save();
			goto layer1;
		}
		else if (msx > 630 && msx < 630 + but_width && msy > center - 0.5 * but_height && msy < center + 0.5 * but_height) 
		{ //pause
			init_pause();
			while (1)
			{
				get_mouse();
				if (msx > 380 && msx < 580 && msy > 260 && msy < 300)
					break;
			}
			goto layer1;
		}
		else if (msx > 630 && msx < 630 + but_width && msy > board_point2 - but_height && msy < board_point2) 
		{ //quit
			init_quit();
			int flag = 0;
			while (1)
			{
				get_mouse();
				if (msx > 325 && msx < 700 && msy > 230 && msy < 260)
				{
					flag = 1;
					break;
				}
				else if (msx > 440 && msx < 520 && msy > 280 && msy < 320)
				{
					flag = 2;
					break;
				}
			}
			if (flag == 1)
				goto layer1;
			else if (flag == 2)
				goto start;
		}
		else 
			goto layer1;  //out of chessboard and buttons
		if (judge(ai_side)) 
		{ //you win!
			init_result(1);
			get_mouse();
			goto layer1;
		}
	aidecision: refresh(0);refresh(1);
		now = clock();
		dfsb();
		gridInfo[startX][startY] = 0, gridInfo[resultX][resultY] = ai_side, gridInfo[obstacleX][obstacleY] = 1;
		refresh(1);
		init_chessboard();
		if (judge(pl_side)) 
		{ //you lose!
			init_result(0);
			get_mouse(); goto layer1;
		}
	}
}