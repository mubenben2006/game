#include "Block.h"
#include<cstdlib>

IMAGE* Block::imgs[7] = { NULL };
int Block::size = 36;

Block::Block()
{
	if (imgs[0] == NULL)
	{
		IMAGE imgTmp;
		loadimage(&imgTmp, L"res/png/tiles.png");

		SetWorkingImage(&imgTmp);
		for (int i = 0; i < 7; i++)
		{
			imgs[i] = new IMAGE;
			getimage(imgs[i], size * i, 0, size, size);
		}
		SetWorkingImage();  //恢复工作区 
	}
	int blocks[7][4] = {
	1,3,5,7, // I
	2,4,5,7, // Z
	3,5,4,6, // S
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // O
	};

	//随机生成方块
	block_type = rand() % 7 + 1;

	for (int i = 0; i < 4; i++)
	{
		int value = blocks[block_type - 1][i];
		smallBlocks[i].row = value / 2;
		smallBlocks[i].col = value % 2;
	}

	img = imgs[block_type - 1];
}

Block& Block::operator=(const Block* block)
{
	if (this != block)
	{
		block_type = block->block_type;
		for (int i = 0; i < 4; i++)
		{
			smallBlocks[i].row = block->smallBlocks[i].row;
			smallBlocks[i].col = block->smallBlocks[i].col;
		}
		img = block->img;
	}
	return *this;
}

void Block::draw(int left_lim, int top_lim)
{
	for (int i = 0; i < 4; i++)
	{
		int x = left_lim + smallBlocks[i].col * size;
		int y = top_lim + smallBlocks[i].row * size;
		putimage(x, y, img);
	}
}

void Block::move(int dx)
{
	for(int i=0;i<4;i++)
		smallBlocks[i].col += dx;
}

void Block::drop()
{
	for (int i = 0; i < 4; i++)
		smallBlocks[i].row++;
}

void Block::rotate()
{
	if (getBlockType() == 7)  //方块类型为O时不旋转
		return;
	
	Point center = smallBlocks[1];
	for (int i = 0; i < 4; i++)
	{
		int row = smallBlocks[i].row;
		int col = smallBlocks[i].col;
		int new_row = center.row + center.col - col;
		int new_col = center.col - center.row + row;
		smallBlocks[i].row = new_row;
		smallBlocks[i].col = new_col;
	}
}

bool Block::is_block_inmap(const vector<vector<int>>& map)
{
	for (int i = 0; i < 4; i++)
	{
		int row = smallBlocks[i].row;
		int col = smallBlocks[i].col;
		if (row < 0 || row >= map.size() || col < 0 || col >= map[0].size())
			return false;
		if (map[row][col] != 0)
			return false;
	}
	return true;
}

void Block::solidify(vector<vector<int>>& map)
{
	for (int i = 0; i < 4; i++)
	{
		int row = smallBlocks[i].row;
		int col = smallBlocks[i].col;
		map[row][col] = block_type;
	}	
}

