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

const int SPEED_NORMAL[MAX_LEVEL] = { 500,350,200,150,100 }; //ms ��ͬ�����ٶ�
const int SPEED_FAST = 50; //ms ���¼��������ٶ�
const int TIME = 120; //ms ���̲�������ʱ��

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
	mciSendString(L"play res/mp3/bg.mp3 repeat", NULL, 0, NULL);  //����background����

	score = 0;
	line_counts = 0;
	level = 1;
	game_over = false;
    delay = SPEED_NORMAL[0];
	
	srand(unsigned(time(NULL)));

	//������Ϸ����
	initgraph(938,896);

	//���ر���ͼƬ
	loadimage(&img_over, L"res/png/over.png");
	loadimage(&img_win, L"res/png/win.png");
	loadimage(&img_bg, L"res/png/bg2.png");

	//��ʼ����Ϸ���е�����
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			map[i][j] = 0;

	//��ȡ��ʷ��߷�
	ifstream file(RECORDED_FILE);
	if (!file.is_open()) {
		cout << RECORDED_FILE << "��ʧ��" << endl;
		score_highest = 0;
	}
	else {
		file >> score_highest;
		file.close();  
	}
}

void Tetris::play()
{
	init();  //��ʼ��

	block_next = new Block;
	block_cur = new Block;

	int timer = 0;
	while (1)  //��Ϸ��ѭ��
	{  
		keyEvent();  //�����û�����
		timer += getDelay();  //��ʱ��  
		if (timer > delay) 
		{
			timer = 0;
			drop();  //��������
			ifupdate = true;  //��Ⱦ��Ϸ����
		}

		if (ifupdate)
		{
			ifupdate = false;

			//������Ϸ����
			updateWindow();  
			//������Ϸ����
			clearLine();
		}

		if (game_over)
		{
			score_save();
			display_over();  //������Ϸ��������
			system("pause");
			init();  //���¿�ʼ
		}
	}
}

void Tetris::keyEvent()
{
	int dx = 0;
	
	//��ȡ�첽����״̬(��ת)
	if (GetAsyncKeyState(VK_UP))
	{
		rotate();
		Sleep(TIME);
	}
	
	//��ȡ�첽����״̬(��������)
	if (GetAsyncKeyState(VK_DOWN))
	{
		delay = SPEED_FAST;
		Sleep(TIME);
	}
	
	//��ȡ�첽����״̬(����)
	if (GetAsyncKeyState(VK_LEFT))
	{
		dx = -1;
		Sleep(TIME);
	}
	
	//��ȡ�첽����״̬(����)
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
	putimage(0, 0, &img_bg);  //���Ʊ���
	
	BeginBatchDraw();
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
		{
			if (map[i][j] == 0) continue;
			
			int x = left + j * block_size;
			int y = top + i * block_size;
			putimage(x, y, Block::getImg()[map[i][j] - 1]);		
		}

	//���Ʒ���
	block_cur->draw(left, top);
	block_next->draw(750, 100);

	//���ƽ�����Ϣ
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
		//�ӷ�
		int add_score[4] = { 10,30,60,100 };
		score += add_score[lines - 1];

		mciSendString(L"play res/mp3/xiaochu1.mp3", NULL, 0, NULL);  //��������
		ifupdate = true;

		level = (score + 99) / 100;  //���µȼ�
		if (level > MAX_LEVEL) game_over = true;

		line_counts += lines;  //����������
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
	settextstyle(40, 0, L"����");
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
			cout << RECORDED_FILE << "��ʧ��" << endl;
		}
		else {
			file << score_highest;
			file.close();
		}
	}
}

void Tetris::display_over()
{
	mciSendString(L"close res/mp3/bg.mp3", NULL, 0, NULL);  //�رձ�������

	//ʤ������ʧ��
	if (level < MAX_LEVEL)
	{
		putimage(262, 361, &img_over);  //����ʧ�ܱ���
		mciSendString(L"play res/mp3/over.mp3", NULL, 0, NULL);  //����ʧ������
	}
	else
	{
		putimage(0, 0, &img_bg);  //���Ʊ���
		mciSendString(L"play res/mp3/win.mp3", NULL, 0, NULL);  //����ʤ������
	}
}

