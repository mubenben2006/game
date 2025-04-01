#pragma comment(lib, "winmm.lib")

#include "Tetris.h"
#include<ctime>
#include<cstdlib>
#include<iostream>
#include<fstream>
#include "Block.h"
#include <mmsystem.h>

#define MAX_LEVEL 5
#define RECORDED_FILE "recorder.txt"

const int SPEED_NORMAL[MAX_LEVEL] = { 500,350,200,150,100 }; //ms 不同级别速度
const int SPEED_FAST = 50; //ms 向下加速下落速度
const int TIME = 120; //ms 键盘操作缓存时间

Tetris::Tetris(int rows, int cols, int left, int top, int block_size)
{
	this->rows = rows;
	this->cols = cols;
	this->left = left;
	this->top = top;
	this->block_size = block_size;
	map.resize(rows);
	for (int i = 0; i < rows; i++)
	{
		map[i].resize(cols);
		for (int j = 0; j < cols; j++)
			map[i][j] = 0;
	}
}

void Tetris::init()
{
	mciSendString(L"play res/mp3/bg.mp3 repeat", NULL, 0, NULL);  //播放background音乐

	score = 0;
	line_counts = 0;
	level = 1;
	game_over = false;
    delay = SPEED_NORMAL[0];
	
	srand(unsigned(time(NULL)));

	//创建游戏窗口
	initgraph(938,896);

	//加载背景图片
	loadimage(&img_over, L"res/png/over.png");
	loadimage(&img_win, L"res/png/win.png");
	loadimage(&img_bg, L"res/png/bg2.png");

	//初始化游戏区中的数组
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			map[i][j] = 0;

	//获取历史最高分
	ifstream file(RECORDED_FILE);
	if (!file.is_open()) {
		cout << RECORDED_FILE << "打开失败" << endl;
		score_highest = 0;
	}
	else {
		file >> score_highest;
		file.close();  
	}
}

void Tetris::play()
{
	init();  //初始化

	block_next = new Block;
	block_cur = new Block;

	int timer = 0;
	while (1)  //游戏主循环
	{  
		keyEvent();  //接受用户输入
		timer += getDelay();  //计时器  
		if (timer > delay) 
		{
			timer = 0;
			drop();  //方块下落
			ifupdate = true;  //渲染游戏画面
		}

		if (ifupdate)
		{
			ifupdate = false;

			//更新游戏画面
			updateWindow();  
			//更新游戏数据
			clearLine();
		}

		if (game_over)
		{
			score_save();
			display_over();  //更新游戏结束画面
			system("pause");
			init();  //重新开始
		}
	}
}

void Tetris::keyEvent()
{
	int dx = 0;
	
	//获取异步按键状态(旋转)
	if (GetAsyncKeyState(VK_UP))
	{
		rotate();
		Sleep(TIME);
	}
	
	//获取异步按键状态(加速下落)
	if (GetAsyncKeyState(VK_DOWN))
	{
		delay = SPEED_FAST;
		Sleep(TIME);
	}
	
	//获取异步按键状态(左移)
	if (GetAsyncKeyState(VK_LEFT))
	{
		dx = -1;
		Sleep(TIME);
	}
	
	//获取异步按键状态(右移)
	if (GetAsyncKeyState(VK_RIGHT))
	{
		dx = 1;
		Sleep(TIME);
	}
	
	if (dx != 0)
	move(dx);
	
	ifupdate = true;
}

void Tetris::drop()
{
	block_hold = block_cur;
	block_cur->drop();

	if (!block_cur->is_block_inmap(map))
	{
		block_hold.solidify(map);
		delete block_cur;
		block_cur = block_next;
		block_next = new Block;
	}

	if (!block_cur->is_block_inmap(map))
		game_over = true;

	delay = SPEED_NORMAL[level - 1];
}

void Tetris::updateWindow()
{
	putimage(0, 0, &img_bg);  //绘制背景
	
	BeginBatchDraw();
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
		{
			if (map[i][j] == 0) continue;
			
			int x = left + j * block_size;
			int y = top + i * block_size;
			putimage(x, y, Block::getImg()[map[i][j] - 1]);		
		}

	//绘制方块
	block_cur->draw(left, top);
	block_next->draw(750, 100);

	//绘制界面信息
	draw_score();
    EndBatchDraw();
}

int Tetris::getDelay()
{
	static unsigned long long last_time = 0;
	unsigned long long current_time = GetTickCount();
	
	if (last_time == 0)
	{
		last_time = current_time;
		return 0;
	}
	else {
		int ret = current_time - last_time;
		last_time = current_time;
		return ret;
	}
}

void Tetris::clearLine()
{
	int lines = 0;
	int k = rows - 1;
	
	for (int i = rows - 1; i >= 0; i--)
	{
		int count = 0;
		for (int j = 0; j < cols; j++)
		{
			if (map[i][j]) count++;
			map[k][j] = map[i][j];
		}
		if (count < cols)  k--;
		else lines++;
	}

	if (lines > 0)
	{
		//加分
		int add_score[4] = { 10,30,60,100 };
		score += add_score[lines - 1];

		mciSendString(L"play res/mp3/xiaochu1.mp3", NULL, 0, NULL);  //播放音乐
		ifupdate = true;

		level = (score + 99) / 100;  //更新等级
		if (level > MAX_LEVEL) game_over = true;

		line_counts += lines;  //更新消行数
	}
}

void Tetris::move(int dx)
{
	block_hold = block_cur;
	block_cur->move(dx);

	if (!block_cur->is_block_inmap(map))
	{
		*block_cur = block_hold;
	}
}

void Tetris::rotate()
{
	block_hold = block_cur;
	block_cur->rotate();

	if (!block_cur->is_block_inmap(map))
	{
		*block_cur = block_hold;
	}
}

void Tetris::draw_score()
{
	BeginBatchDraw();
	settextcolor(WHITE);
	setbkmode(TRANSPARENT);
	settextstyle(40, 0, L"黑体");
	wchar_t str[4][10];
	swprintf_s(str[0], L"%d", score);
	swprintf_s(str[1], L"%d", level);
	swprintf_s(str[2], L"%d", line_counts);
	swprintf_s(str[3], L"%d", score_highest);
	outtextxy(710, 735, str[0]);
	outtextxy(155, 745, str[1]);
	outtextxy(175, 825, str[2]);
	outtextxy(750, 825, str[3]);
	EndBatchDraw();
}

void Tetris::score_save()
{
	if (score > score_highest)
	{
		score_highest = score;
		
		ofstream file(RECORDED_FILE);
		if (!file.is_open()) {
			cout << RECORDED_FILE << "打开失败" << endl;
		}
		else {
			file << score_highest;
			file.close();
		}
	}
}

void Tetris::display_over()
{
	mciSendString(L"close res/mp3/bg.mp3", NULL, 0, NULL);  //关闭背景音乐

	//胜利还是失败
	if (level < MAX_LEVEL)
	{
		putimage(262, 361, &img_over);  //绘制失败背景
		mciSendString(L"play res/mp3/over.mp3", NULL, 0, NULL);  //播放失败音乐
	}
	else
	{
		putimage(0, 0, &img_bg);  //绘制背景
		mciSendString(L"play res/mp3/win.mp3", NULL, 0, NULL);  //播放胜利音乐
	}
}

