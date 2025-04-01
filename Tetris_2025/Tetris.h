#pragma once

#include<graphics.h>
#include<vector>
#include "Block.h"
using namespace std;

class Tetris
{
public:
	Tetris(int rows, int cols, int left, int top, int block_size);
	
	void init();  //初始化
	void play();  //游戏开始
private:
	void keyEvent();  //键盘事件
	void drop();  //方块下落
	void updateWindow();  //更新窗口
	int getDelay();  //获取延迟时间(ms)
	void clearLine();  //消行
	void move(int dx);  //  dx=1 :-> dx=-1 :<-
	void rotate();  //旋转
	void draw_score();  //绘制分数
	void check() { game_over = !block_cur->is_block_inmap(map); };  //检查游戏是否结束
	void score_save(); //保存最高分
	void display_over();  //更新游戏结束画面
private:
	int delay = 0;  //延迟时间参数
	bool ifupdate = true;  //是否更新窗口
	vector<vector<int>> map; //游戏地图
	int rows;  //行数
	int cols;  //列数
	int left;  //左边界
	int top;  //上边界
	int block_size;  //方块大小
	int score;  //分数
	int score_highest;  //最高分	
	int level;  //等级
	int line_counts;  //消行数
	bool game_over;  //游戏是否结束
	
	IMAGE img_bg;  //背景图片
	IMAGE img_over;  //游戏结束背景
	IMAGE img_win; //胜利背景

	Block* block_cur;  //当前方块
	Block* block_next;  //下一个方块
	Block block_hold;  //暂存方块
};