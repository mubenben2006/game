#pragma once

#include<graphics.h>
#include<vector>
using namespace std;

struct Point {
	int row;
	int col;
};

class Block
{
public:
	Block();
	Block& operator=(const Block* block);
	
	void draw(int left_lim, int top_lim);
	void move(int dx);  //  dx=1 :-> dx=-1 :<-
	void drop();
	void rotate();
	static IMAGE** getImg() { return imgs; }
	bool is_block_inmap(const vector<vector<int>>& map);
	void solidify(vector<vector<int>>& map);
	int getBlockType() { return block_type; }
private:
	int block_type;
	Point smallBlocks[4];
	IMAGE* img;

	static IMAGE* imgs[7];
	static int size;
};