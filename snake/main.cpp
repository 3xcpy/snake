#include<raylib.h>

#include<vector>
#include<random>
#include<string>
#include<iostream>

#define levelWidth 16
#define levelHeight 16

#define tileSize 50

#define startx 8
#define starty 8

float tickLength = 0.5f;
float tickClock = 0;

int tailLength = 0;

struct
{
	Color tail1 = GREEN;
	Color tail2 = DARKGREEN;
	Color head = GREEN;
	Color bg = BLACK;
	Color pickup = RED;
}colors;

enum Direction
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	NONE
};

enum
{
	MENU,
	GAME,
	GAMEOVER
}state;

struct
{
	int yOffset = -50;

	int titleSize = 100;
	std::string title = "Snake!";
	Color titleColor = WHITE;

	int promptSize = 40;
	std::string prompt = "Press any key to start";
	Color promptColor = RAYWHITE;
	int promptPadding = 20;

	int scoreSize = 100;
	std::string score = "Score: ";
	Color scoreColor = WHITE;

	int gmPromptSize = 40;
	std::string gmPrompt = "Press any key to retry";
	Color gmPromptColor = RAYWHITE;
	int gmPromptPadding = 20;

	Color bg = BLACK;
} menu;

struct tailPiece
{
	int x, y;
};

struct 
{
	int x, y;
} pickup;

struct
{
	int x = startx, y = starty;
	Direction dir = NONE;
	Direction lastDir = NONE;
}head;

std::vector<tailPiece> tail;

void init();
void update();

int main()
{
	init();

	while (!WindowShouldClose())
	{
		update();
	}

	return 0;
}

void setup_level()
{
	tail = {};
	pickup.x = rand() % levelWidth;
	pickup.y = rand() % levelHeight;
	head.x = startx;
	head.y = starty;
	tailLength = 0;
	head.dir = NONE;
	head.lastDir = NONE;
}

void init()
{
	InitWindow(800, 800, "Snake!");
	srand(time(NULL));
	SetTargetFPS(60);
	state = MENU;
}

void poll_events()
{
	if (tail.size() != 0)
	{
		if (IsKeyPressed(KEY_UP))
		{
			for (auto t : tail)
			{
				if (t.x == head.x && t.y == head.y - 1)
					break;
			}
			head.dir = UP;
			head.lastDir = UP;
		}
		if (IsKeyPressed(KEY_DOWN))
		{
			for (auto t : tail)
			{
				if (t.x == head.x && t.y == head.y + 1)
					break;
			}
			head.dir = DOWN;
			head.lastDir = DOWN;
		}
		if (IsKeyPressed(KEY_LEFT))
		{
			for (auto t : tail)
			{
				if (t.x == head.x - 1 && t.y == head.y)
					break;
			}
			head.dir = LEFT;
			head.lastDir = LEFT;
		}
		if (IsKeyPressed(KEY_RIGHT))
		{
			for (auto t : tail)
			{
				if (t.x == head.x + 1 && t.y == head.y)
					break;
			}
			head.dir = RIGHT;
			head.lastDir = RIGHT;
		}
	}
	if (tail.size() == 0)
	{
		if (IsKeyPressed(KEY_UP))
			head.dir = UP;
		if (IsKeyPressed(KEY_DOWN))
			head.dir = DOWN;
		if (IsKeyPressed(KEY_LEFT))
			head.dir = LEFT;
		if (IsKeyPressed(KEY_RIGHT))
			head.dir = RIGHT;
	}
}

void add_to_tail(int c)
{
	if (tail.size() > 0)
	{
		for (int i = 0; i < c; i++)
		{
			tail.push_back(tail.back());
		}
	}
	else
	{
		tail.push_back({ head.x, head.y });
	}
}

void update_player()
{
	tailLength = tail.size();

	bool doChecks = true;

	if (tailLength != 0)
	{
		tail.pop_back();
		tail.insert(tail.begin(), { head.x, head.y });
	}

	switch (head.dir)
	{
	case NONE:
		break;
	case UP:
		head.y -= 1;
		break;
	case DOWN:
		head.y += 1;
		break;
	case LEFT:
		head.x -= 1;
		break;
	case RIGHT:
		head.x += 1;
		break;
	}

	if (head.x == pickup.x && head.y == pickup.y)
	{
		if (tailLength == 0)
			doChecks = false;
		add_to_tail(1);
		pickup.x = rand() % levelWidth;
		pickup.y = rand() % levelHeight;
		for (auto t : tail)
		{
			if (pickup.x == t.x && pickup.y == t.y)
			{
				pickup.x = rand() % levelWidth;
				pickup.y = rand() % levelHeight;
			}
		}
	}

	if (doChecks)
	{
		for (auto t : tail)
		{
			if (t.x == head.x && t.y == head.y && head.dir != NONE)
			{
				state = GAMEOVER;
			}
		}
	}

	if (head.x < 0)
		state = GAMEOVER;
	if (head.x >= levelWidth)
		state = GAMEOVER;
	if (head.y < 0)
		state = GAMEOVER;
	if (head.y >= levelHeight)
		state = GAMEOVER;
}

void draw()
{
	int i = 0;

	BeginDrawing();

	ClearBackground(colors.bg);

	for (auto t : tail)
	{
		i++;
		Color c;
		if (i % 2 == 0)
			c = colors.tail1;
		else if (i % 2 == 1)
			c = colors.tail2;
		DrawRectangle(t.x * tileSize, t.y * tileSize, tileSize, tileSize, c);
	}

	DrawRectangle(pickup.x * tileSize, pickup.y * tileSize, tileSize, tileSize, colors.pickup);

	DrawRectangle(head.x * tileSize, head.y * tileSize, tileSize, tileSize, colors.head);

	std::string scoreText = "Score: " + std::to_string(tailLength);
	DrawText(scoreText.c_str(), 10, 10, 20, WHITE);

	EndDrawing();
}

void poll_menu_events()
{
	for (int key = 0; key < 512; key++)
    {
        if (IsKeyPressed(key))
        {
			setup_level();
			state = GAME;
        }
    }
}

void draw_menu()
{
	BeginDrawing();
	ClearBackground(menu.bg);

	int titleWidth = MeasureText(menu.title.c_str(), menu.titleSize);
	DrawText(menu.title.c_str(), GetScreenWidth() / 2 - titleWidth / 2, GetScreenHeight() / 2 - menu.titleSize / 2 + menu.yOffset, menu.titleSize, menu.titleColor);

	int promptWidth = MeasureText(menu.prompt.c_str(), menu.promptSize);
	DrawText(menu.prompt.c_str(), GetScreenWidth() / 2 - promptWidth / 2, GetScreenHeight() / 2 + menu.titleSize / 2 + menu.promptPadding + menu.yOffset, menu.promptSize, menu.promptColor);
	EndDrawing();
}

void draw_game_over()
{
	menu.score = "Score: " + std::to_string(tailLength);

	BeginDrawing();
	ClearBackground(menu.bg);

	int scoreTextWidth = MeasureText(menu.score.c_str(), menu.scoreSize);
	DrawText(menu.score.c_str(), GetScreenWidth() / 2 - scoreTextWidth / 2, GetScreenHeight() / 2 - menu.scoreSize / 2 + menu.yOffset, menu.scoreSize, menu.scoreColor);

	int gmPromptWidth = MeasureText(menu.gmPrompt.c_str(), menu.gmPromptSize);
	DrawText(menu.gmPrompt.c_str(), GetScreenWidth() / 2 - gmPromptWidth / 2, GetScreenHeight() / 2 + menu.scoreSize / 2 + menu.gmPromptPadding + menu.yOffset, menu.gmPromptSize, menu.gmPromptColor);
	EndDrawing();
}

void update()
{
	if (state == MENU)
	{
		poll_menu_events();
		draw_menu();
	}

	if (state == GAME)
	{
		tickClock += GetFrameTime();

		poll_events();

		if (tickClock >= tickLength)
		{
			tickClock = 0;
			update_player();
		}

		draw();
	}

	if (state == GAMEOVER)
	{
		poll_menu_events();
		draw_game_over();
	}
}
