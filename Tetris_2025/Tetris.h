#pragma once

#include<graphics.h>
#include<vector>
#include "Block.h"
using namespace std;

class Tetris
{
public:
	Tetris(int rows, int cols, int left, int top, int block_size);
	
	void init();  //��ʼ��
	void play();  //��Ϸ��ʼ
private:
	void keyEvent();  //�����¼�
	void drop();  //��������
	void updateWindow();  //���´���
	int getDelay();  //��ȡ�ӳ�ʱ��(ms)
	void clearLine();  //����
	void move(int dx);  //  dx=1 :-> dx=-1 :<-
	void rotate();  //��ת
	void draw_score();  //���Ʒ���
	void check() { game_over = !block_cur->is_block_inmap(map); };  //�����Ϸ�Ƿ����
	void score_save(); //������߷�
	void display_over();  //������Ϸ��������
private:
	int delay = 0;  //�ӳ�ʱ�����
	bool ifupdate = true;  //�Ƿ���´���
	vector<vector<int>> map; //��Ϸ��ͼ
	int rows;  //����
	int cols;  //����
	int left;  //��߽�
	int top;  //�ϱ߽�
	int block_size;  //�����С
	int score;  //����
	int score_highest;  //��߷�	
	int level;  //�ȼ�
	int line_counts;  //������
	bool game_over;  //��Ϸ�Ƿ����
	
	IMAGE img_bg;  //����ͼƬ
	IMAGE img_over;  //��Ϸ��������
	IMAGE img_win; //ʤ������

	Block* block_cur;  //��ǰ����
	Block* block_next;  //��һ������
	Block block_hold;  //�ݴ淽��
};