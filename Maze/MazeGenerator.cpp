#include "MazeGenerator.h"
#include <iostream>
#include <algorithm>
#include <vector>

MazeGenerator::MazeGenerator(MazeGenerator::Config config) : config(config)
{
	this->config = config;
	maze = new int* [config.size.x];
	for (int x = 0; x < config.size.x; x++)
	{
		maze[x] = new int[config.size.y];
		for (int y = 0; y < config.size.y; y++)
		{
			maze[x][y] = 1;
		}
	}
}

MazeGenerator::~MazeGenerator()
{
	for (int i = 0; i < config.size.x; i++)
	{
		delete[] maze[i];
	}
	delete[] maze;
}

void MazeGenerator::generate()
{
	engine.seed(config.seed);

	generate_galleries();
	generate_squares();
	dig_branches();
	generate_enter_and_exit();
}

MazeGenerator::Config MazeGenerator::get_config()
{
	return config;
}

int** MazeGenerator::get_maze()
{
	return maze;
}


void MazeGenerator::dig_branches()
{
	dig_branch(Vector2::DOWN, Vector2::RIGHT, Vector2(1, 1));

	Vector2 directions[] = { Vector2::UP, Vector2::RIGHT, Vector2::DOWN, Vector2::LEFT };

	for (int i = 0; i < 3; i++)
	{
		for (int y = 1; y < config.size.y - 1; y++)
		{
			for (int x = 1; x < config.size.x - 1; x++)
			{
				Vector2 curr_coord = { x, y };
				if (!get_cell(curr_coord))
				{
					std::shuffle(std::begin(directions), std::end(directions), engine);
					for (int i = 0; i < 4; i++)
					{
						auto move_dir = directions[i];
						auto coord = curr_coord + directions[i];
						if (get_cell(coord) == WALL && (
							(move_dir.x != 0 && get_cell(coord + Vector2::UP) == WALL && get_cell(coord + Vector2::DOWN) == WALL) ||
							(move_dir.y != 0 && get_cell(coord + Vector2::LEFT) == WALL && get_cell(coord + Vector2::RIGHT) == WALL)
							)) {
							dig_branch(directions[i], directions[i].x != 0 ? get_random_vertical_direction() : get_random_horizontal_direction(), coord);
						}
					}
				}
			}
		}
	}
}

void MazeGenerator::dig_branch(Vector2 init_move_dir, Vector2 init_dig_dir, Vector2 init_coord)
{
	Vector2 move_dir = init_move_dir;
	Vector2 dig_dir = init_dig_dir;
	Vector2 coord = init_coord;

	Vector2 stopped_coord;
	Vector2 prev_move_dir;
	Vector2 prev_dig_dir;
	int prev_width;

	int length = get_random_pass_length();
	int width = get_random_width();

	if (!dig_pass(move_dir, dig_dir, coord, length, width))
	{
		return;
	}

	while (true)
	{
		stopped_coord = coord;
		prev_move_dir = move_dir;
		prev_dig_dir = dig_dir;
		prev_width = width;

		width = get_random_width();
		if (width > length)
		{
			width = length;
		}
		length = get_random_pass_length();
		dig_dir = move_dir * -1;

		if (move_dir.x != 0)
		{
			move_dir = get_random_vertical_direction();
			coord = stopped_coord + (move_dir == prev_dig_dir ? move_dir * prev_width : move_dir);
			if (dig_pass(move_dir, dig_dir, coord, length, width))
			{
				continue;
			}

			move_dir.reverse();
			coord = stopped_coord + (move_dir == prev_dig_dir ? move_dir * prev_width : move_dir);
			if (dig_pass(move_dir, dig_dir, coord, length, width))
			{
				continue;
			}
		}
		else
		{
			move_dir = get_random_horizontal_direction();
			coord = stopped_coord + (move_dir == prev_dig_dir ? move_dir * prev_width : move_dir);
			if (dig_pass(move_dir, dig_dir, coord, length, width))
			{
				continue;
			}

			move_dir.reverse();
			coord = stopped_coord + (move_dir == prev_dig_dir ? move_dir * prev_width : move_dir);
			if (dig_pass(move_dir, dig_dir, coord, length, width))
			{
				continue;
			}
		}
		move_dir = prev_move_dir;
		width = prev_width;
		length = 1;
		coord = stopped_coord + move_dir;
		dig_dir = prev_dig_dir;
		if (dig_pass(move_dir, dig_dir, coord, length, width))
		{
			continue;
		}
		return;
	}
}

bool MazeGenerator::dig_pass(Vector2 move_dir, Vector2 dig_dir, Vector2& coord, int& length, int& width)
{
	int final_width = width;
	int final_length = 0;
	if (length <= 0)
	{
		std::cerr << "Length is 0" << std::endl;
		return false;
	}

	while (final_width >= config.min_width && !ability_to_dig(move_dir, dig_dir, final_width, coord)) {
		final_width--;
	}

	if (final_width < config.min_width)
	{
		return false;
	}

	width = final_width;
	do
	{
		dig(dig_dir, width, coord);
		coord += move_dir;
		final_length++;
	} while ((length - final_length) > 0 && ability_to_dig(move_dir, dig_dir, width, coord));

	length = final_length;
	coord -= move_dir;

	return true;
}

void MazeGenerator::generate_squares()
{
	for (int y = 10; y < config.size.y - 10; y++)
	{
		for (int x = 10; x < config.size.x - 10; x++)
		{
			if (is_generate_square())
			{
				generate_square(Vector2(x, y));
			}
		}
	}
}

void MazeGenerator::generate_square(Vector2 coord)
{
	for (int y = coord.y; y < coord.y + config.max_width; y++)
	{
		for (int x = coord.x; x < coord.x + config.max_width; x++)
		{
			if (get_cell({ x, y }) != WALL)
			{
				return;
			}
		}
	}

	for (int y = coord.y; y < coord.y + config.max_width; y++)
	{
		for (int x = coord.x; x < coord.x + config.max_width; x++)
		{
			get_cell({ x, y }) = NONE;
		}
	}

	int x1 = coord.x - 1;
	int x2 = coord.x + config.max_width;
	for (int y = coord.y; y < coord.y + config.max_width; y++)
	{
		auto& cell1 = get_cell({ x1, y });
		if (cell1 == WALL)
		{
			cell1 = SQUARE_ENTER;
		}

		auto& cell2 = get_cell({ x2, y });
		if (cell2 == WALL)
		{
			cell2 = SQUARE_ENTER;
		}
	}

	int y1 = coord.y - 1;
	int y2 = coord.y + config.max_width;
	for (int x = coord.x; x < coord.x + config.max_width; x++)
	{
		auto& cell1 = get_cell({ x, y1 });
		if (cell1 == WALL)
		{
			cell1 = SQUARE_ENTER;
		}

		auto& cell2 = get_cell({ x, y2 });
		if (cell2 == WALL)
		{
			cell2 = SQUARE_ENTER;
		}
	}
}

void MazeGenerator::generate_galleries()
{
	for (int y = 10; y < config.size.y - 10; y++)
	{
		for (int x = 10; x < config.size.x - 10; x++)
		{
			if (is_generate_gallery())
			{
				generate_gallery(Vector2(x, y));
			}
		}
	}
}

bool MazeGenerator::generate_gallery(Vector2 coord)
{
	std::uniform_int_distribution<int> rotation_dis(0, 3);
	Vector2 directions[] = { Vector2::UP, Vector2::RIGHT, Vector2::DOWN, Vector2::LEFT };
	std::rotate(std::begin(directions), std::begin(directions) + rotation_dis(engine), std::end(directions));

	bool generate = false;

	//Определяем куда будем строить галерею и на сколько можем построить
	Vector2 move_dir;
	Vector2 dig_dir;
	int available_length = 0;

	for (int i = 0; i < 4; i++)
	{
		available_length = 0;
		Vector2 c = coord + directions[i];
		while (ability_to_dig(directions[i], directions[(i + 1) % 4], config.max_width, c))
		{
			available_length++;
			c += directions[i];
		}

		if (available_length > config.gallery_min_length)
		{
			move_dir = directions[i];
			dig_dir = directions[(i + 1) % 4];
			generate = true;
			break;
		}

		available_length = 0;
		c = coord + directions[i];
		while (ability_to_dig(directions[i], directions[(i - 1 + 4) % 4], config.max_width, c))
		{
			available_length++;
			c += directions[i];
		}

		if (available_length > config.gallery_min_length)
		{
			move_dir = directions[i];
			dig_dir = directions[(i - 1 + 4) % 4];
			generate = true;
			break;
		}
	}

	//Если нет места
	if (!generate) {
		return false;
	}

	auto start_coord = coord;

	coord += move_dir;
	int length = get_gallery_length(available_length);
	//Копаем галерею и по бокам создаём нерушимую стенку
	while (length > 0 && ability_to_dig(move_dir, dig_dir, config.max_width, coord)) {
		dig(dig_dir, config.max_width, coord);
		get_cell(coord - dig_dir) = INDESTRUCTIBLE_WALL;
		get_cell(coord + (dig_dir * config.max_width)) = INDESTRUCTIBLE_WALL;
		coord += move_dir;
		length--;
	}

	//Вход
	for (int i = 0; i < config.max_width; i++)
	{
		get_cell(coord + (dig_dir * i)) = GALLERY_ENTER;
		get_cell(start_coord + (dig_dir * i)) = GALLERY_ENTER;
	}
	return true;
}

void MazeGenerator::generate_enter_and_exit()
{
	auto dir = Vector2::DOWN;
	auto point = Vector2(1, 0);
	get_cell(point) = NONE;
	point += dir;
	while (get_cell(point) != NONE)
	{
		get_cell(point) = NONE;
		point += dir;
	}

	dir = Vector2::UP;
	point = config.size - Vector2(2, 1);
	get_cell(point) = NONE;
	point += dir;
	while (get_cell(point) != NONE)
	{
		get_cell(point) = NONE;
		point += dir;
	}
}

bool MazeGenerator::is_out_of_bounds(Vector2 coord)
{
	return coord.x < 0 || coord.y < 0 || coord.x >= config.size.x || coord.y >= config.size.y;
}

bool MazeGenerator::ability_to_dig(Vector2 move_dir, Vector2 dig_dir, int width, Vector2 coord)
{
	for (int i = 0; i < 2; i++) {
		for (int j = -1; j <= width; j++)
		{
			auto c = coord + (dig_dir * j);
			if (is_out_of_bounds(c) || get_cell(c) == NONE)
			{
				return false;
			}
		}
		coord += move_dir;
	}
	return true;
}

void MazeGenerator::dig(Vector2 dig_dir, int width, Vector2 coord)
{
	Vector2 directions[] = { Vector2::UP, Vector2::RIGHT, Vector2::DOWN, Vector2::LEFT };

	std::vector<Vector2> gallery_enters;

	for (int i = 0; i < width; i++) {
		Vector2 c = coord + (dig_dir * i);
		if (get_cell(c) == WALL)
		{
			get_cell(c) = NONE;
			for (int j = 0; j < 4; j++)
			{
				Vector2 mb_entry = c + directions[j];
				if (get_cell(mb_entry) == GALLERY_ENTER)
				{
					if (gallery_enters.empty())
					{
						gallery_enters = get_all_gallery_enters(mb_entry);
					}
					get_cell(c + directions[j]) = NONE;
				}
				else if (get_cell(mb_entry) == SQUARE_ENTER)
				{
					get_cell(c + directions[j]) = NONE;
				}
			}
		}
	}

	for (Vector2 v : gallery_enters)
	{
		if (get_cell(v) == GALLERY_ENTER)
			get_cell(v) = WALL;
	}
}

int& MazeGenerator::get_cell(Vector2 coord)
{
	return maze[coord.x][coord.y];
}

int MazeGenerator::get_random_width()
{
	float v = width_dis(engine);
	float w = (std::pow(2, v) - 1) / (2.0f - 1);

	int num = config.max_width + 1 - config.min_width;
	float unit = 1.0f / num;
	for (int i = 0; i < num; i++)
	{
		if (w > i * unit)
		{
			w -= unit;
		}
		else
		{
			return config.min_width + i;
		}
	}
}

int MazeGenerator::get_random_pass_length()
{
	return pass_length_dis(engine);
}

Vector2 MazeGenerator::get_random_vertical_direction()
{
	return direction_dis(engine) ? Vector2::UP : Vector2::DOWN;
}

Vector2 MazeGenerator::get_random_horizontal_direction()
{
	return direction_dis(engine) ? Vector2::RIGHT : Vector2::LEFT;
}

bool MazeGenerator::is_generate_square()
{
	return config.availability_of_squares && chances_dis(engine) <= config.square_chance;
}

bool MazeGenerator::is_generate_gallery()
{
	return config.availability_of_galleries && chances_dis(engine) <= config.gallery_chance;
}

int MazeGenerator::get_gallery_length(int available)
{
	std::uniform_int_distribution<int> dis(config.gallery_min_length, available > config.gallery_max_length ? config.gallery_max_length : available);
	return dis(engine);
}

std::vector<Vector2> MazeGenerator::get_all_gallery_enters(Vector2 coord)
{
	Vector2 directions[] = { Vector2::UP, Vector2::RIGHT, Vector2::DOWN, Vector2::LEFT };
	std::vector<Vector2> enters;

	if (get_cell(coord) != GALLERY_ENTER)
	{
		return enters;
	}

	enters.push_back(coord);

	for (int k = 0; k < 4; k++)
	{
		int d = 0;
		while (get_cell(coord + (directions[k] * d)) == GALLERY_ENTER)
		{
			enters.push_back(coord + (directions[k] * d));
			d++;
		}
	}
	return enters;
}


