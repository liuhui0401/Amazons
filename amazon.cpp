#include "pch.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <queue>
#define GRIDSIZE 8
#define OBSTACLE 2
#define judge_black 0
#define judge_white 1
#define grid_black 1
#define grid_white -1
using namespace std;
int currBotColor;
int turnID;
int gridInfo[GRIDSIZE][GRIDSIZE] = { 0 };
int dx[] = { -1,-1,-1,0,0,1,1,1 };
int dy[] = { -1,0,1,-1,1,-1,0,1 };
int bestx1, besty1, bestx2, besty2, bestx3, besty3;
double tim;
inline bool inMap(int x, int y)
{
	if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
		return false;
	return true;
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
		gridInfo[x2][y2] = OBSTACLE;
	}
	return true;
}
struct ex { int x, y, d; };
queue<ex>qq;
queue<ex>qk;
int disbq[8][8];
int diswq[8][8];
int disbk[8][8];
int diswk[8][8];
double func()
{
	ex r;
	for (int i = 0; i <= 7; i++)
		for (int j = 0; j <= 7; j++)
		{
			if (gridInfo[i][j] == currBotColor)
			{
				disbq[i][j] = 0;
				disbk[i][j] = 0;
				ex t; t.x = i, t.y = j, t.d = 0;
				qq.push(t);
				qk.push(t);
			}
			else
			{
				disbq[i][j] = 100;
				disbk[i][j] = 100;
			}
		}
	while (!qq.empty())
	{
		r = qq.front(); qq.pop();
		for (int k = 0; k < 8; ++k)
		{
			for (int delta1 = 1; delta1 < GRIDSIZE; delta1++)
			{
				int xx = r.x + dx[k] * delta1;
				int yy = r.y + dy[k] * delta1;
				if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
					break;
				if (disbq[xx][yy] == 100)
				{
					ex t; t.x = xx, t.y = yy, t.d = r.d + 1;
					qq.push(t);
					disbq[xx][yy] = r.d + 1;
				}
			}
		}
	}
	while (!qk.empty())
	{
		r = qk.front(); qk.pop();
		for (int k = 0; k < 8; ++k)
		{
			int xx = r.x + dx[k];
			int yy = r.y + dy[k];
			if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
				continue;
			if (disbk[xx][yy] == 100)
			{
				ex t; t.x = xx, t.y = yy, t.d = r.d + 1;
				qk.push(t);
				disbk[xx][yy] = r.d + 1;
			}
		}
	}
	for (int i = 0; i <= 7; i++)
		for (int j = 0; j <= 7; j++)
		{
			if (gridInfo[i][j] == -currBotColor)
			{
				diswq[i][j] = 0;
				diswk[i][j] = 0;
				ex t; t.x = i, t.y = j, t.d = 0;
				qk.push(t);
				qq.push(t);
			}
			else
			{
				diswq[i][j] = 100;
				diswk[i][j] = 100;
			}
		}
	while (!qk.empty())
	{
		r = qk.front(); qk.pop();
		for (int k = 0; k < 8; ++k)
		{
			int xx = r.x + dx[k];
			int yy = r.y + dy[k];
			if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
				continue;
			if (diswk[xx][yy] == 100)
			{
				ex t; t.x = xx, t.y = yy, t.d = r.d + 1;
				qk.push(t);
				diswk[xx][yy] = r.d + 1;
			}
		}
	}
	while (!qq.empty())
	{
		r = qq.front(); qq.pop();
		for (int k = 0; k < 8; ++k)
		{
			for (int delta1 = 1; delta1 < GRIDSIZE; delta1++)
			{
				int xx = r.x + dx[k] * delta1;
				int yy = r.y + dy[k] * delta1;
				if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
					break;
				if (diswq[xx][yy] == 100)
				{
					ex t; t.x = xx, t.y = yy, t.d = r.d + 1;
					qq.push(t);
					diswq[xx][yy] = r.d + 1;
				}
			}
		}
	}
	double sum1 = 0, sum2 = 0;
	for (int i = 0; i <= 7; i++)
		for (int j = 0; j <= 7; j++)
		{
			if (disbq[i][j] == 100 && diswq[i][j] == 100)
			{

			}
			if (disbq[i][j] > diswq[i][j])
				sum1--;
			if (disbq[i][j] < diswq[i][j])
				sum1++;
			if (disbq[i][j] == diswq[i][j])
				sum1 += 0.2;
			if (disbk[i][j] == 100 && diswk[i][j] == 100)
			{

			}
			if (disbk[i][j] > diswk[i][j])
				sum2--;
			if (disbk[i][j] < diswk[i][j])
				sum2++;
			if (disbk[i][j] == diswk[i][j])
				sum2 += 0.2;
		}
	if (turnID <= 20)
		return sum1 * 0.14 + sum2 * 0.37;
	if (turnID > 20 && turnID < 50)
		return sum1 * 0.30 + sum2 * 0.25;
	if (turnID >= 50)
		return sum1 * 0.80 + sum2 * 0.10;
}
double dfsw(double zz)
{
	double u = clock();
	if ((u - tim) / CLOCKS_PER_SEC > 0.95) return -100;
	double min = 100;
	for (int i = 0; i < GRIDSIZE; ++i)
	{
		for (int j = 0; j < GRIDSIZE; ++j)
		{
			if (gridInfo[i][j] == -currBotColor)
				for (int k = 0; k < 8; ++k)
				{
					for (int delta1 = 1; delta1 < GRIDSIZE; delta1++)
					{
						int xx = i + dx[k] * delta1;
						int yy = j + dy[k] * delta1;
						if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
							break;
						for (int l = 0; l < 8; ++l)
						{
							for (int delta2 = 1; delta2 < GRIDSIZE; delta2++)
							{
								int xxx = xx + dx[l] * delta2;
								int yyy = yy + dy[l] * delta2;
								if (!inMap(xxx, yyy))
									break;
								if (gridInfo[xxx][yyy] != 0 && !(i == xxx && j == yyy))
									break;
								gridInfo[i][j] = 0;
								gridInfo[xx][yy] = -currBotColor;
								gridInfo[xxx][yyy] = OBSTACLE;
								double territory;
								territory = func();
								if (territory < min)
									min = territory;
								if (min < zz)
								{
									gridInfo[xxx][yyy] = 0;
									gridInfo[xx][yy] = 0;
									gridInfo[i][j] = -currBotColor;
									return min;
								}
								gridInfo[xxx][yyy] = 0;
								gridInfo[xx][yy] = 0;
								gridInfo[i][j] = -currBotColor;
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
	double max = -100;
	for (int i = 0; i < GRIDSIZE; ++i)
	{
		for (int j = 0; j < GRIDSIZE; ++j)
		{
			if (gridInfo[i][j] == currBotColor)
				for (int k = 0; k < 8; ++k)
				{
					for (int delta1 = 1; delta1 < GRIDSIZE; delta1++)
					{
						int xx = i + dx[k] * delta1;
						int yy = j + dy[k] * delta1;
						if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
							break;
						for (int l = 0; l < 8; ++l)
						{
							for (int delta2 = 1; delta2 < GRIDSIZE; delta2++)
							{
								int xxx = xx + dx[l] * delta2;
								int yyy = yy + dy[l] * delta2;
								if (!inMap(xxx, yyy))
									break;
								if (gridInfo[xxx][yyy] != 0 && !(i == xxx && j == yyy))
									break;
								gridInfo[i][j] = 0;
								gridInfo[xx][yy] = currBotColor;
								gridInfo[xxx][yyy] = OBSTACLE;
								double territory;
								territory = dfsw(max);
								if (territory > max)
								{
									max = territory;
									bestx1 = i;
									besty1 = j;
									bestx2 = xx;
									besty2 = yy;
									bestx3 = xxx;
									besty3 = yyy;
								}
								gridInfo[xxx][yyy] = 0;
								gridInfo[xx][yy] = 0;
								gridInfo[i][j] = currBotColor;
							}
						}
					}
				}
		}
	}
	return;
}
int main()
{
	tim = clock();
	int x0, y0, x1, y1, x2, y2;
	gridInfo[0][(GRIDSIZE - 1) / 3] = gridInfo[(GRIDSIZE - 1) / 3][0]
		= gridInfo[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][0]
		= gridInfo[GRIDSIZE - 1][(GRIDSIZE - 1) / 3] = grid_black;
	gridInfo[0][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = gridInfo[(GRIDSIZE - 1) / 3][GRIDSIZE - 1]
		= gridInfo[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][GRIDSIZE - 1]
		= gridInfo[GRIDSIZE - 1][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = grid_white;
	cin >> turnID;
	currBotColor = grid_white;
	for (int i = 0; i < turnID; i++)
	{
		cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
		if (x0 == -1)
			currBotColor = grid_black;
		else
			ProcStep(x0, y0, x1, y1, x2, y2, -currBotColor, false);
		if (i < turnID - 1)
		{
			cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
			if (x0 >= 0)
				ProcStep(x0, y0, x1, y1, x2, y2, currBotColor, false);
		}
	}
	dfsb();
	cout << bestx1 << " " << besty1 << " " << bestx2 << " " << besty2 << " " << bestx3 << " " << besty3;
	return 0;
}