#pragma once
#include "Vector2.h"
#include "Bitmap.h"
#include <random>

class MazeGenerator {

public:
	struct Config {
		Vector2 size = Vector2(1000, 1000);
		int min_width = 1;
		int max_width = 6;
		bool availability_of_squares = false;
		float square_chance = 0.01;
		bool availability_of_galleries = false;
		float gallery_chance = 0.01;
		int gallery_min_length = 20;
		int gallery_max_length = 100;
		unsigned long seed = time(nullptr);
	};

private:
	Config config;

	int** maze;

	std::default_random_engine engine;

	std::uniform_real_distribution<float> width_dis = std::uniform_real_distribution<float>(0.0, 1.0);
	std::uniform_int_distribution<int> pass_length_dis = std::uniform_int_distribution<int>(2, 6);
	std::uniform_int_distribution<int> direction_dis = std::uniform_int_distribution<int>(0, 1);
	std::uniform_real_distribution<float> chances_dis = std::uniform_real_distribution<float>(0, 100);

public:
	static const int NONE = 0;
	static const int WALL = 1;
	static const int INDESTRUCTIBLE_WALL = 2;
	static const int GALLERY_ENTER = 3;
	static const int SQUARE_ENTER = 4;

public:

	MazeGenerator(Config config);

	~MazeGenerator();

	void generate();

	Config get_config();

	int** get_maze();

private:

	void dig_branches();

	void dig_branch(Vector2 move_dir, Vector2 dig_dir, Vector2 coord);

	bool dig_pass(Vector2 move_dir, Vector2 dig_dir, Vector2& coord, int& length, int& width);

	void generate_squares();

	void generate_square(Vector2 coord);

	void generate_galleries();

	bool generate_gallery(Vector2 coord);

	void generate_enter_and_exit();

	bool is_out_of_bounds(Vector2 coord);

	bool ability_to_dig(Vector2 move_dir, Vector2 dig_dir, int width, Vector2 coord);

	void dig(Vector2 dig_dir, int width, Vector2 coord);

	int& get_cell(Vector2 coord);

	int get_random_width();

	int get_random_pass_length();

	Vector2 get_random_vertical_direction();

	Vector2 get_random_horizontal_direction();

	bool is_generate_square();

	bool is_generate_gallery();

	int get_gallery_length(int available);

	std::vector<Vector2> get_all_gallery_enters(Vector2 coord);
};