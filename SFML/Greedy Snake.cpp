#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <vector>
using namespace sf;

#define WINDOW_WIDTH 80
#define WINDOW_HEIGHT 25
#define STAGE_WIDTH 20
#define STAGE_HEIGHT 20
#define INFO_WIDTH 400
#define GRID_SIZE 25
#define SCALE 0.5
#define STEP 0.2

int gameMode;
bool gameOver;

const int width = STAGE_WIDTH;
const int height = STAGE_HEIGHT;

int x, y, fruitX, fruitY, score;
std::vector<int>tailX, tailY;
int nTail;
float stepX, stepY;

enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir, dir_ing;
bool updateFlag;
int headLocation;


int delay = 0;

sf::RenderWindow window(sf::VideoMode(width * GRID_SIZE + INFO_WIDTH, height * GRID_SIZE + GRID_SIZE), L"Greedy Snake by 万Queen");

Texture tBackground, tSnake, tSnakeHead, tFruit;
Sprite spBackground, spSnake, spSnakeHead, spFruit;
Font font;
Text text;

void setScale()
{
	spBackground.setScale(SCALE, SCALE);
	spSnakeHead.setScale(SCALE, SCALE);
	spSnake.setScale(SCALE, SCALE);
	spFruit.setScale(SCALE, SCALE);
}

void init()
{
	window.setFramerateLimit(10);
	
	if (!font.loadFromFile("./data/fonts/SourceHanSansCN-Normal.otf"))
	{
		std::cout << "没字体你load啥" << std::endl;
	}
	text.setFont(font);

	if (!tBackground.loadFromFile("./data/images/BK.png"))
	{
		std::cout << "没有BK.png！能不能靠谱点！" << std::endl;
	}

	if (!tSnakeHead.loadFromFile("./data/images/sh01.png"))
	{
		std::cout << "没有sh01.png！能不能靠谱点！" << std::endl;
	}

	if (!tSnake.loadFromFile("./data/images/sb0102.png"))
	{
		std::cout << "没有sb0102.png！能不能靠谱点！" << std::endl;
	}

	if (!tFruit.loadFromFile("./data/images/sb0202.png"))
	{
		std::cout << "没有sb0202.png！能不能靠谱点！" << std::endl;
	}

	setScale();

	spBackground.setTexture(tBackground);
	spSnake.setTexture(tSnake);
	spSnakeHead.setTexture(tSnakeHead);
	spFruit.setTexture(tFruit);

	spBackground.setOrigin(GRID_SIZE / SCALE / 2, GRID_SIZE / SCALE / 2);
	spSnakeHead.setOrigin(GRID_SIZE / SCALE / 2, GRID_SIZE / SCALE / 2);
	spSnake.setOrigin(GRID_SIZE / SCALE / 2, GRID_SIZE / SCALE / 2);
	spFruit.setOrigin(GRID_SIZE / SCALE / 2, GRID_SIZE / SCALE / 2);

	gameOver = false;
	gameMode = 1;
	dir = STOP;
	dir_ing = STOP;
	updateFlag = false;
	x = width / 2;
	y = height / 2;

	spSnakeHead.setPosition(x, y);

	srand(time(0));
	fruitX = rand() % (width - 1) + 1;
	fruitY = rand() % (height - 1) + 1;

	score = 0;

	nTail = 1;

	headLocation = 0;

	tailX.push_back(x);
	tailY.push_back(y);

	stepX = 0.0;
	stepY = 0.0;
}

void promptInfo(int _x, int _y)
{
	int initX = 20, initY = 0;
	int CharacterSize = 24;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 255, 255, 255));
	text.setStyle(Text::Bold);

	text.setPosition(_x + initX, _y + initY);
	text.setString(L"◇ 游戏说明：");
	window.draw(text);

	initY += CharacterSize;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"	○ 蛇身自撞，游戏结束");
	window.draw(text);

	initY += CharacterSize;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"	○ 蛇可穿墙");
	window.draw(text);

	initY += CharacterSize * 2;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"◇ 操作说明：");
	window.draw(text);

	initY += CharacterSize;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"	○ 向左移动：A / ←");
	window.draw(text);

	initY += CharacterSize;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"	○ 向右移动：D / →");
	window.draw(text);

	initY += CharacterSize;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"	○ 向上移动：W / ↑");
	window.draw(text);

	initY += CharacterSize;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"	○ 向下移动：S / ↓");
	window.draw(text);

	initY += CharacterSize;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"	○ 开始游戏：任意方向键");
	window.draw(text);

	initY += CharacterSize;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"	○ 退出游戏：esc");
	window.draw(text);

	initY += CharacterSize;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"	○ 动画模式切换：space");
	window.draw(text);

	initY += CharacterSize * 2;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"◇ 万Queen / 制作");
	window.draw(text);
}

void logic()
{
	switch (dir)
	{
	case LEFT:
		x--;
		headLocation = -90;
		break;
	case RIGHT:
		x++;
		headLocation = 90;
		break;
	case UP:
		y--;
		headLocation = 0;
		break;
	case DOWN:
		y++;
		headLocation = 180;
		break;
	default:
		break;
	}

	for (int i = 0; i < nTail; i++)
	{
		if (i == 0)
			continue;
		if (tailX[i] == x && tailY[i] == y)
			gameOver = true;
	}

	int prevX = tailX[0], prevY = tailY[0];
	int prev2X, prev2Y;
	tailX[0] = x;
	tailY[0] = y;

	if (x == fruitX && y == fruitY)
	{
		score += 10;
		srand(time(0));
		fruitX = rand() % (width - 1) + 1;
		fruitY = rand() % (height - 1) + 1;
		nTail++;
	}

	if (nTail == tailX.size() && nTail == tailY.size())
	{
		for (int i = 1; i < nTail; i++)
		{
			prev2X = tailX[i];
			prev2Y = tailY[i];
			tailX[i] = prevX;
			tailY[i] = prevY;
			prevX = prev2X;
			prevY = prev2Y;
		}
	}
	else
	{
		for (int i = 1; i < nTail - 1; i++)
		{
			prev2X = tailX[i];
			prev2Y = tailY[i];
			tailX[i] = prevX;
			tailY[i] = prevY;
			prevX = prev2X;
			prevY = prev2Y;
		}

		tailX.push_back(prevX);
		tailY.push_back(prevY);
	}
	

	if (x >= width - 1 && dir == RIGHT)
		x = -1;
	else if (x <= 0 && dir == LEFT)
		x = width;

	if (y >= height - 1 && dir == DOWN)
		y = -1;
	else if (y <= 0 && dir == UP)
		y = height;
}

void logicStep()
{
	int prevX = tailX[0], prevY = tailY[0];
	int prev2X, prev2Y;
	updateFlag = false;

	switch (dir_ing)
	{
		case LEFT:
			stepX -= STEP;
			if (stepX < -0.5 || stepX >= 0.5)
			{
				x--;
				stepX = 0;
				stepY = 0;
				dir_ing = dir;
				headLocation = -90;
				updateFlag = true;
			}
			break;
		case RIGHT:
			stepX += STEP;
			if (stepX < -0.5 || stepX >= 0.5)
			{
				x++;
				stepX = 0;
				stepY = 0;
				dir_ing = dir;
				headLocation = 90;
				updateFlag = true;
			}
			break;
		case UP:
			stepY -= STEP;
			if (stepY < -0.5 || stepY >= 0.5)
			{
				y--;
				stepX = 0;
				stepY = 0;
				dir_ing = dir;
				headLocation = 0;
				updateFlag = true;
			}
			break;
		case DOWN:
			stepY += STEP;
			if (stepY < -0.5 || stepY >= 0.5)
			{
				y++;
				stepX = 0;
				stepY = 0;
				dir_ing = dir;
				headLocation = 180;
				updateFlag = true;
			}
			break;
		default:
			dir_ing = dir;
			break;
	}

	if (x >= width)
		x = 0;
	else if (x < 0)
		x = width - 1;

	if (y >= height)
		y = 0;
	else if (y < 0)
		y = height - 1;

	tailX[0] = x;
	tailY[0] = y;

	if (updateFlag == true)
	{
		if (x == fruitX && y == fruitY)
		{
			score += 10;
			srand(time(0));
			fruitX = rand() % (width - 1) + 1;
			fruitY = rand() % (height - 1) + 1;
			nTail++;
		}

		if (nTail == tailX.size() && nTail == tailY.size())
		{
			for (int i = 1; i < nTail; i++)
			{
				prev2X = tailX[i];
				prev2Y = tailY[i];
				tailX[i] = prevX;
				tailY[i] = prevY;
				prevX = prev2X;
				prevY = prev2Y;
			}
		}
		else
		{
			for (int i = 1; i < nTail - 1; i++)
			{
				prev2X = tailX[i];
				prev2Y = tailY[i];
				tailX[i] = prevX;
				tailY[i] = prevY;
				prevX = prev2X;
				prevY = prev2Y;
			}

			tailX.push_back(prevX);
			tailY.push_back(prevY);
		}

		for (int i = 0; i < nTail; i++)
		{
			if (i == 0)
				continue;
			if (tailX[i] == x && tailY[i] == y)
				gameOver = true;
		}
	}
}

void gameOverInfo(int _x, int _y)
{
	int initX = 10, initY = 0;
	int CharacterSize = 24;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 0, 255, 255));
	text.setStyle(Text::Bold);

	text.setPosition(_x + initX, _y + initY);
	text.setString(L"蛤蛤你死了");
	window.draw(text);

	initY += CharacterSize;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"Y / 重开 N / 退出");
	window.draw(text);
}

void draw()
{
	window.clear();
	promptInfo(width * GRID_SIZE + GRID_SIZE, GRID_SIZE);

	int detaX = GRID_SIZE / SCALE / 2;
	int detaY = GRID_SIZE / SCALE / 2;

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{
			spBackground.setPosition(i * GRID_SIZE + detaX, j * GRID_SIZE + detaY);
			window.draw(spBackground);
		}

	spSnakeHead.setRotation(headLocation);
	spSnakeHead.setPosition(tailX[0] * GRID_SIZE + detaX, tailY[0] * GRID_SIZE + detaY);
	window.draw(spSnakeHead);

	for (int i = 1; i < nTail; i++)
	{
		spSnake.setPosition(tailX[i] * GRID_SIZE + detaX, tailY[i] * GRID_SIZE + detaY);
		window.draw(spSnake);
	}

	spFruit.setPosition(fruitX * GRID_SIZE + detaX, fruitY * GRID_SIZE + detaY);
	window.draw(spFruit);

	window.display();
}

void drawStep()
{
	window.clear();
	promptInfo(width * GRID_SIZE + GRID_SIZE, GRID_SIZE);

	int detaX = GRID_SIZE / SCALE / 2;
	int detaY = GRID_SIZE / SCALE / 2;

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{
			spBackground.setPosition(i * GRID_SIZE + detaX, j * GRID_SIZE + detaY);
			window.draw(spBackground);
		}

	float stepLenth;
	stepLenth = stepX + stepY;

	if (stepLenth < 0)
		stepLenth = -stepLenth;

	spSnakeHead.setPosition((tailX[0] + stepX) * GRID_SIZE + detaX, (tailY[0] + stepY) * GRID_SIZE + detaY);
	spSnakeHead.setRotation(headLocation);
	window.draw(spSnakeHead);

	for (int i = 1; i < nTail; i++)
	{
		if (tailY[i] == tailY[i - 1] && tailX[i] != tailX[i - 1])
		{
			spSnake.setPosition((tailX[i] + (tailX[i - 1] - tailX[i]) * stepLenth) * GRID_SIZE + detaX, tailY[i] * GRID_SIZE + detaY);
		}

		if (tailY[i] != tailY[i - 1] && tailX[i] == tailX[i - 1])
		{
			spSnake.setPosition(tailX[i] * GRID_SIZE + detaX, (tailY[i] + (tailY[i - 1] - tailY[i]) * stepLenth) * GRID_SIZE + detaY);
		}

		if (tailY[i] != tailY[i - 1] && tailX[i] != tailX[i - 1])
		{
			spSnake.setPosition((tailX[i] + (tailX[i - 1] - tailX[i]) * stepLenth) * GRID_SIZE + detaX, (tailY[i] + (tailY[i - 1] - tailY[i]) * stepLenth) * GRID_SIZE + detaY);
		}

		window.draw(spSnake);
	}

	spFruit.setPosition(fruitX * GRID_SIZE + detaX, fruitY * GRID_SIZE + detaY);
	window.draw(spFruit);

	if (gameOver)
		gameOverInfo(width / 8 * GRID_SIZE, height / 4 * GRID_SIZE);
	window.display();
}

void input()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			window.close();

		if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Space)
		{
			if (gameMode == 1)
				gameMode = 2;
			else
				gameMode = 1;
		}
	}

	if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
		if (dir != RIGHT)
			dir = LEFT;

	if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
		if (dir != LEFT)
			dir = RIGHT;

	if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
		if (dir != DOWN)
			dir = UP;

	if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))
		if (dir != UP)
			dir = DOWN;
	
}

int main()
{
	init();

	while (window.isOpen())
	{
		while (gameOver == false)
		{
			input();
			switch (gameMode)
			{
				case 1:
					delay++;
					if (delay % 10 == 0)
					{
						delay = 0;
						logic();
					}
					draw();
					break;
				case 2:
					logicStep();
					drawStep();
					break;
			}
		}
	}

	return 0;
}