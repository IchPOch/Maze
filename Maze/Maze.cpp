#include <iostream>
#include <fstream>
#include <queue>
#include "MazeGenerator.h"
#include "Bitmap.h"

const int PATH = 100;

void save_image(int width, int height, int** values, char* filename);
void lee(int** maze, Vector2 size);
void save_config(MazeGenerator::Config config, char* filename);
void read_config(MazeGenerator::Config& config, char* filename);

int main()
{
	MazeGenerator::Config config;
	config.size = Vector2(500, 500);
	config.max_width = 5;
	config.availability_of_galleries = false;
	config.gallery_chance = 0.01f;
	config.gallery_min_length = 50;
	config.gallery_max_length = 150;
	config.availability_of_squares = true;
	config.square_chance = 0.01f;
	config.seed = 5938;

	MazeGenerator generator = MazeGenerator(config);
	generator.generate();

	lee(generator.get_maze(), config.size);

	save_image(config.size.x, config.size.y, generator.get_maze(), (char*)"maze.bmp");

	save_config(config, (char*)"config.txt");

	MazeGenerator::Config config_from_file;
	read_config(config_from_file, (char*)"config.txt");
	MazeGenerator generator_from_file(config_from_file);
	generator_from_file.generate();
	save_image(config.size.x, config.size.y, generator_from_file.get_maze(), (char*)"maze_from_file.bmp");

}

int color(int value)
{
	switch (value)
	{
	case PATH: return 0xFF0000;
	case MazeGenerator::NONE: return 0xFFFFFF;
	case MazeGenerator::WALL: return 0x000000;
	case MazeGenerator::INDESTRUCTIBLE_WALL: return 0x000000;
	case MazeGenerator::GALLERY_ENTER: return 0x000000;
	case MazeGenerator::SQUARE_ENTER: return 0x000000;
	}

}

void save_image(int width, int height, int** values, char* filename)
{
	unsigned char* image = new unsigned char[width * height * 3];
	int c = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int clr = color(values[x][y]);
			image[c] = (clr & 0x0000FF);
			image[c + 1] = (clr & 0x00FF00) >> 8;
			image[c + 2] = (clr & 0xFF0000) >> 16;
			c += 3;
		}
	}

	generateBitmapImage((unsigned char*)image, height, width, filename);
	delete[] image;
}

void lee(int** maze, Vector2 size)
{
	int** map = new int* [size.x];
	for (int i = 0; i < size.x; i++)
	{
		map[i] = new int[size.y];
		for (int j = 0; j < size.y; j++)
		{
			map[i][j] = 0;
		}
	}

	Vector2 end = size - Vector2(2, 1);
	std::queue<Vector2> q;
	Vector2 v = { 1, 0 };
	q.push(v);
	while (!q.empty()) {
		auto p = q.front();
		q.pop();
		int x = p.x;
		int y = p.y;
		if (p == end) break;
		for (int dx = -1; dx <= 1; dx++)
		{
			for (int dy = -1; dy <= 1; dy++)
			{
				if (dx != 0 || dy != 0)
				{
					if (x + dx >= 0 && x + dx < size.x && y + dy >= 0 && y + dy < size.y)
					{
						if (maze[x + dx][y + dy] == MazeGenerator::NONE && map[x + dx][y + dy] == MazeGenerator::NONE) {
							map[x + dx][y + dy] = map[x][y] + 1;
							Vector2 v = { x + dx, y + dy };
							q.push(v);
						}
					}
				}
			}
		}
	}

	if (q.empty()) {
		std::cout << "UNREAL";
		return;
	}

	Vector2 p = end;
	maze[p.x][p.y] = PATH;
	int d = map[p.x][p.y];
	while (d > 0)
	{
		for (int dx = -1; dx <= 1; dx++)
		{
			for (int dy = -1; dy <= 1; dy++)
			{
				if (dx != 0 || dy != 0)
				{
					if (map[p.x + dx][p.y + dy] == d - 1)
					{
						p = { p.x + dx, p.y + dy };
						maze[p.x][p.y] = PATH;
						d = map[p.x][p.y];
					}
				}
			}
		}
	}
}

void save_config(const MazeGenerator::Config config, char* filename)
{
	std::ofstream file(filename);
	file << config.size.x << ' ';
	file << config.size.y << ' ';
	file << config.min_width << ' ';
	file << config.max_width << ' ';
	file << config.availability_of_squares << ' ';
	file << config.square_chance << ' ';
	file << config.availability_of_galleries << ' ';
	file << config.gallery_chance << ' ';
	file << config.gallery_min_length << ' ';
	file << config.gallery_max_length << ' ';
	file << config.seed << ' ';
	file.close();
}

void read_config(MazeGenerator::Config& config, char* filename) {
	std::ifstream file(filename);
	file >> config.size.x;
	file >> config.size.y;
	file >> config.min_width;
	file >> config.max_width;
	file >> config.availability_of_squares;
	file >> config.square_chance;
	file >> config.availability_of_galleries;
	file >> config.gallery_chance;
	file >> config.gallery_min_length;
	file >> config.gallery_max_length;
	file >> config.seed;
	file.close();
}