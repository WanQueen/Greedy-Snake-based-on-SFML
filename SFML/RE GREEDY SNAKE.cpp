#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <ctime>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <vector>
#include <windows.h>
using namespace sf;

#define STAGE_WIDTH 20
#define STAGE_HEIGHT 20
#define GRID_SIZE 25
#define INFO_WIDTH 400
#define SCALE 0.5
#define FRAME_LIMIT 60
#define STEP 0.1
#define STEP_CHANGE 0.9999

#define WORD_RIGHT_INITX 20
#define WORD_RIGHT_INITY 0
#define WORD_SAVE_DEVIATION_X 37
#define WORD_SAVE_DEVIATION_Y 63
#define WORD_SAVE_DETAIL_DEVIATION_Y 112
#define WORD_NAME_INPUT_X 200
#define WORD_NAME_INPUT_Y 315
#define GAME_OVER_INFO_POSITION_X 172
#define GAME_OVER_INFO_POSITION_Y 172

#define RANK_BOARD_POSITION_INITX 15
#define RANK_BOARD_POSITION_INITY 2

#define START_BUTTON_POSITION_X 47.25
#define LOAD_BUTTON_POSITION_X 331.5
#define END_BUTTON_POSITION_X 615.75
#define BUTTON_POSITION_Y 400
#define BUTTON_WIDTH 237
#define BUTTON_HEIGHT 89

#define SL_BUTTON_WIDTH 242
#define SL_BUTTON_HEIGHT 400
#define SL_BUTTON1_X 43.5
#define SL_BUTTON2_X 329
#define SL_BUTTON3_X 614.5
#define SL_BUTTON_Y 100

#define TRIANGLE_ORIGIN_X 19.5
#define TRIANGLE_ORIGIN_Y 16.5

#define CHARACTER_SIZE_PROMPT_INFO 20
#define CHARACTER_SIZE_SCORE 40
#define CHARACTER_SIZE_GAME_OVER_INFO 48
#define CHARACTER_SIZE_SL_TITLE 58
#define CHARACTER_SIZE_SL_DETAIL 22

#define CHARACTER_SL_DETAIL_DEVIATION 20

#define RANK_PLAYER_NUM 8

int gameState = 0;
int gameMode;
bool gameOver;
bool gameQuit = 0;
bool gamePaused;

const int width = STAGE_WIDTH;
const int height = STAGE_HEIGHT;

int x, y, fruitX, fruitY, score;
std::vector<int> tailX, tailY;
int nTail;

enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir, dir_ing;
bool turned;
int headLocation;
float stepX, stepY;
bool updateFlag;

int delay = 0;

SoundBuffer sbEat, sbDie;
Sound soundEat, soundDie;
Music bkMusic;
int soundVolume;
bool MusicOn;

SYSTEMTIME sysTime = { 0 };

sf::RenderWindow window(sf::VideoMode(width* GRID_SIZE + INFO_WIDTH, height * GRID_SIZE + GRID_SIZE), L"Greedy Snake by 万Queen");

Texture tBackground, tSnakeHead, tSnakeBody, tFruit;
Sprite spBackground, spSnakeHead, spSnakeBody, spFruit;
Texture tStartBackground, tStartButton, tLoadButton, tEndButton;
Sprite spStartBackground, spStartButton, spLoadButton, spEndButton;
sf::IntRect ButtonRectStart, ButtonRectLoad, ButtonRectEnd;
Texture tSaveButton, tTriangle;
Sprite spSaveButton, spTriangle;
sf::IntRect ButtonRectSave1, ButtonRectSave2, ButtonRectSave3;
Texture tGameOverInfo, tGamePausedInfo;
Sprite spGameOverInfo, spGamePausedInfo;
Texture tRankBoardBackground;
Sprite spRankBoardBackground;
Font font;
Text text;



typedef struct data {
	bool isDataExist;
	int score;
	SYSTEMTIME systime;
}Data;

Data d[3];

typedef struct rank {
	sf::String name;
	int score;
}rankBoard;

std::vector<rankBoard> rankPlayer;
bool isRankBoardOn;

std::stringstream name;

void drawSaveAndLoad(int trianglePos);
void draw();
void write_name();
void rank_calc();

eDirection enum_change_to_int(eDirection e, int i)
{
	switch (i)
	{
	case 0:
		e = STOP;
		break;
	case 1:
		e = LEFT;
		break;
	case 2:
		e = RIGHT;
		break;
	case 3:
		e = UP;
		break;
	case 4:
		e = DOWN;
		break;
	}

	return e;
}

void insertionSort()
{
	for (int i = 0; i < rankPlayer.size(); i++)
	{
		rankBoard key = rankPlayer[i];
		int j = i - 1;
		while (j >= 0 && (key.score < rankPlayer[j].score))
		{
			rankPlayer[j + 1] = rankPlayer[j];
			j--;
		}
		rankPlayer[j + 1] = key;
	}
}

void setScale()
{
	spBackground.setScale(SCALE, SCALE);
	spSnakeHead.setScale(SCALE, SCALE);
	spSnakeBody.setScale(SCALE, SCALE);
	spFruit.setScale(SCALE, SCALE);
}

void save()
{
	std::ofstream saveFile;
	sf::Event event;
	bool wait4Input = true;
	int trianglePos = 1;
	
	while (wait4Input)
	{
		drawSaveAndLoad(trianglePos);

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Escape)
			{
				window.close();
				gameQuit = true;
			}

			if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A)
			{
				trianglePos--;
				if (trianglePos < 1)
					trianglePos = 3;
			}
			else if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D)
			{
				trianglePos++;
				if (trianglePos > 3)
					trianglePos = 1;
			}

			if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::BackSpace)
			{
				draw();
				window.display();
				return;
			}
			
			if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Enter)
			{
				switch (trianglePos)
				{
				case 1:
					saveFile.open("save 1.txt");
					wait4Input = false;
					break;
				case 2:
					saveFile.open("save 2.txt");
					wait4Input = false;
					break;
				case 3:
					saveFile.open("save 3.txt");
					wait4Input = false;
					break;
				default:
					break;
				}
			}
			else if (event.type == sf::Event::EventType::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
			{
				switch (trianglePos)
				{
				case 1:
					saveFile.open("save 1.txt");
					wait4Input = false;
					break;
				case 2:
					saveFile.open("save 2.txt");
					wait4Input = false;
					break;
				case 3:
					saveFile.open("save 3.txt");
					wait4Input = false;
					break;
				default:
					break;
				}
			}
		}
	}

	GetLocalTime(&sysTime);
	saveFile << sysTime.wYear << " " << sysTime.wMonth << " " << sysTime.wDay << std::endl;
	saveFile << sysTime.wHour << " " << sysTime.wMinute << " " << sysTime.wSecond << std::endl;
	saveFile << score << std::endl;
	saveFile << gameMode << std::endl;
	saveFile << x << " " << y << std::endl;
	saveFile << fruitX << " " << fruitY << std::endl;
	saveFile << nTail << std::endl;
	for (int i = 0; i < tailX.size(); i++)
	{
		saveFile << tailX[i] << " " << tailY[i] << std::endl;
	}
	saveFile << dir << " " << dir_ing << " " << headLocation << std::endl;
	saveFile << stepX << " " << stepY << std::endl;
	saveFile << updateFlag << std::endl;
	saveFile << turned << " " << delay << std::endl;
	
	saveFile.close();
	//std::cout << "Saved." << std::endl;
	draw();
	window.display();

	std::ifstream loadFile;
	for (int i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0:
			loadFile.open("save 1.txt");
			break;
		case 1:
			loadFile.open("save 2.txt");
			break;
		case 2:
			loadFile.open("save 3.txt");
			break;
		}

		if (loadFile.is_open())
			d[i].isDataExist = true;
		else
		{
			d[i].isDataExist = false;
			loadFile.close();
		}

		if (d[i].isDataExist)
		{
			loadFile >> d[i].systime.wYear >> d[i].systime.wMonth >> d[i].systime.wDay;
			loadFile >> d[i].systime.wHour >> d[i].systime.wMinute >> d[i].systime.wSecond;
			loadFile >> d[i].score;
		}

		loadFile.close();
	}
}

void load()
{
	std::ifstream loadFile;
	sf::Event event;
	bool wait4Input = true;
	int trianglePos = 1;

	while (wait4Input)
	{
		drawSaveAndLoad(trianglePos);

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Escape)
			{
				window.close();
				gameQuit = true;
			}

			if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Left)
			{
				trianglePos--;
				if (trianglePos < 1)
					trianglePos = 3;
			}
			else if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Right)
			{
				trianglePos++;
				if (trianglePos > 3)
					trianglePos = 1;
			}

			if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::BackSpace)
			{
				draw();
				window.display();
				return;
			}

			if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Enter)
			{
				switch (trianglePos)
				{
				case 1:
					loadFile.open("save 1.txt");
					wait4Input = false;
					break;
				case 2:
					loadFile.open("save 2.txt");
					wait4Input = false;
					break;
				case 3:
					loadFile.open("save 3.txt");
					wait4Input = false;
					break;
				default:
					break;
				}
			}
			else if (event.type == sf::Event::EventType::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
			{
				switch (trianglePos)
				{
				case 1:
					loadFile.open("save 1.txt");
					wait4Input = false;
					break;
				case 2:
					loadFile.open("save 2.txt");
					wait4Input = false;
					break;
				case 3:
					loadFile.open("save 3.txt");
					wait4Input = false;
					break;
				default:
					break;
				}
			}
		}
	}

	loadFile >> sysTime.wYear >> sysTime.wMonth >> sysTime.wDay;
	loadFile >> sysTime.wHour >> sysTime.wMinute >> sysTime.wSecond;
	loadFile >> score;
	loadFile >> gameMode;
	loadFile >> x >> y;
	loadFile >> fruitX >> fruitY;
	loadFile >> nTail;
	tailX.clear();
	tailY.clear();
	int tx, ty;
	for (int i = 0; i < nTail; i++)
	{
		loadFile >> tx >> ty;
		tailX.push_back(tx);
		tailY.push_back(ty);
	}
	int ds, dis;
	loadFile >> ds >> dis >> headLocation;
	dir = enum_change_to_int(dir, ds);
	dir_ing = enum_change_to_int(dir_ing, dis);
	loadFile >> stepX >> stepY;
	loadFile >> updateFlag;
	loadFile >> turned >> delay;

	loadFile.close();
	//std::cout << "Loaded." << std::endl;
	draw();
	window.display();

	for (int i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0:
			loadFile.open("save 1.txt");
			break;
		case 1:
			loadFile.open("save 2.txt");
			break;
		case 2:
			loadFile.open("save 3.txt");
			break;
		}

		if (loadFile.is_open())
			d[i].isDataExist = true;
		else
		{
			d[i].isDataExist = false;
			loadFile.close();
		}

		if (d[i].isDataExist)
		{
			loadFile >> d[i].systime.wYear >> d[i].systime.wMonth >> d[i].systime.wDay;
			loadFile >> d[i].systime.wHour >> d[i].systime.wMinute >> d[i].systime.wSecond;
			loadFile >> d[i].score;
		}

		loadFile.close();
	}
}

void write_name()
{
	sf::Event event;
	sf::String buffer = "_";
	bool isWriteOver = false;
	int nameNum = 0;
	std::string nameString;

	draw();
	text.setPosition(WORD_NAME_INPUT_X, WORD_NAME_INPUT_Y);
	text.setString(buffer);
	window.draw(text);
	window.display();

	while (!isWriteOver)
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Escape)
			{
				window.close();
				gameQuit = true;
			}

			if (event.type == sf::Event::EventType::TextEntered)
			{
				if (event.text.unicode != 8 && nameNum < 5)
				{
					if (event.text.unicode != 32)
					{
						if (buffer.getSize() != 5)
						{
							buffer.insert(buffer.getSize() - 1, event.text.unicode);
						}
						else
						{
							buffer.erase(buffer.getSize() - 1, 1);
							buffer += event.text.unicode;
						}

						//std::cout << (std::string)buffer << std::endl;

						draw();
						text.setPosition(WORD_NAME_INPUT_X, WORD_NAME_INPUT_Y);
						text.setString(buffer);
						window.draw(text);
						window.display();

						nameNum++;
					}
					else
					{
						if (buffer.getSize() != 5)
						{
							buffer.insert(buffer.getSize() - 1, '_');
						}
						else
						{
							buffer.erase(buffer.getSize() - 1, 1);
							buffer += '_';
						}

						draw();
						text.setPosition(WORD_NAME_INPUT_X, WORD_NAME_INPUT_Y);
						text.setString(buffer);
						window.draw(text);
						window.display();

						nameNum++;
					}
				}
			}

			if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Return)
			{
				if (buffer.getSize() > 1 && nameNum != 5)
					buffer.erase(buffer.getSize() - 2, 1);
				name.str((std::string)buffer);
				isWriteOver = true;
			}
			
			if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Key::BackSpace)
			{
				if (buffer.getSize() > 1)
				{
					if (nameNum != 5)
						buffer.erase(buffer.getSize() - 2, 1);
					else
					{
						buffer.erase(buffer.getSize() - 1, 1);
						buffer += "_";
					}
				}
				draw();
				text.setPosition(WORD_NAME_INPUT_X, WORD_NAME_INPUT_Y);
				text.setString(buffer);
				window.draw(text);
				window.display();
				nameNum--;
			}
		}
	}

	if (nameNum < 5)
		nameString = name.str().substr(0, nameNum - 1);
	else
		nameString = name.str().substr(0, nameNum);
	
	std::fstream out;
	out.open("rank board.txt");
	out.seekp(0, std::ios::end);
	out << nameString << " " << score << std::endl;
	//std::cout << nameString << std::endl;
	out.close();
	rank_calc();
}

void rank_calc()
{
	rankPlayer.clear();
	std::ifstream loadRank;
	loadRank.open("rank board.txt");
	if (loadRank.is_open())
	{
		rankBoard player;
		std::string n;
		while (!loadRank.eof())
		{
			loadRank >> n >> player.score;
			player.name = n;
			rankPlayer.push_back(player);
			//std::cout << player.name << " " << player.score << std::endl;
		}
	}
	loadRank.close();

	rankPlayer.pop_back();
	
	insertionSort();
	std::reverse(rankPlayer.begin(), rankPlayer.end());
	while (rankPlayer.size() > RANK_PLAYER_NUM)
	{
		rankPlayer.pop_back();
	}

	std::ofstream reloadRank;
	reloadRank.open("rank board.txt");
	if (reloadRank.is_open())
	{
		std::string n;
		for (int i = 0; i < rankPlayer.size(); i++)
		{
			n = rankPlayer[i].name;
			reloadRank << n << " " << rankPlayer[i].score << std::endl;
			//std::cout << rankPlayer[i].name << " " << rankPlayer[i].score << std::endl;
		}
	}
	reloadRank.close();
}

void initStartPage()
{
	window.setFramerateLimit(FRAME_LIMIT);

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

	if (!tSnakeBody.loadFromFile("./data/images/sb0102.png"))
	{
		std::cout << "没有sb0102.png！能不能靠谱点！" << std::endl;
	}

	if (!tFruit.loadFromFile("./data/images/apple.png"))
	{
		std::cout << "没有apple.png！能不能靠谱点！" << std::endl;
	}

	setScale();

	spBackground.setTexture(tBackground);
	spSnakeBody.setTexture(tSnakeBody);
	spSnakeHead.setTexture(tSnakeHead);
	spFruit.setTexture(tFruit);

	spBackground.setOrigin(GRID_SIZE / SCALE / 2, GRID_SIZE / SCALE / 2);
	spSnakeHead.setOrigin(GRID_SIZE / SCALE / 2, GRID_SIZE / SCALE / 2);
	spSnakeBody.setOrigin(GRID_SIZE / SCALE / 2, GRID_SIZE / SCALE / 2);
	spFruit.setOrigin(30 / SCALE / 2, 24 / SCALE / 2);

	if (!tGameOverInfo.loadFromFile("./data/images/gameOverInfo.png"))
	{
		std::cout << "没有gameOverInfo.png！能不能靠谱点！" << std::endl;
	}

	if (!tGamePausedInfo.loadFromFile("./data/images/PausedInfo.png"))
	{
		std::cout << "没有PausedInfo.png！能不能靠谱点！" << std::endl;
	}

	spGameOverInfo.setTexture(tGameOverInfo);
	spGamePausedInfo.setTexture(tGamePausedInfo);

	if (!font.loadFromFile("./data/fonts/SourceHanSansCN-Normal.otf"))
	{
		std::cout << "没字体你load啥" << std::endl;
	}
	text.setFont(font);

	if (!tStartBackground.loadFromFile("./data/images/startBK.png"))
	{
		std::cout << "没有startBK.png！能不能靠谱点！" << std::endl;
	}

	if (!tStartButton.loadFromFile("./data/images/StartButton.png"))
	{
		std::cout << "没有StartButton.png！能不能靠谱点！" << std::endl;
	}

	if (!tLoadButton.loadFromFile("./data/images/LoadButton.png"))
	{
		std::cout << "没有LoadButton.png！能不能靠谱点！" << std::endl;
	}

	if (!tEndButton.loadFromFile("./data/images/EndButton.png"))
	{
		std::cout << "没有EndButton.png！能不能靠谱点！" << std::endl;
	}

	spStartBackground.setTexture(tStartBackground);
	spStartButton.setTexture(tStartButton);
	spLoadButton.setTexture(tLoadButton);
	spEndButton.setTexture(tEndButton);

	if (!tSaveButton.loadFromFile("./data/images/SL-bk.png"))
	{
		std::cout << "没有SL-bk.png！能不能靠谱点！" << std::endl;
	}

	if (!tTriangle.loadFromFile("./data/images/triangle.png"))
	{
		std::cout << "没有triangle.png！能不能靠谱点！" << std::endl;
	}

	spSaveButton.setTexture(tSaveButton);
	spTriangle.setTexture(tTriangle);

	spTriangle.setOrigin(TRIANGLE_ORIGIN_X, TRIANGLE_ORIGIN_Y);

	if (!tRankBoardBackground.loadFromFile("./data/images/rank board background.png"))
	{
		std::cout << "没有rank board background.png！能不能靠谱点！" << std::endl;
	}

	spRankBoardBackground.setTexture(tRankBoardBackground);

	if (!sbEat.loadFromFile("./data/Audios/Eat01.ogg"))
	{
		std::cout << "没有Eat01.ogg！能不能靠谱点！" << std::endl;
	}

	if (!sbDie.loadFromFile("./data/Audios/Die01.ogg"))
	{
		std::cout << "没有Die01.ogg！能不能靠谱点！" << std::endl;
	}

	if (!bkMusic.openFromFile("./data/Audios/BGM01.ogg"))
	{
		std::cout << "没有找到BGM01.ogg！能不能靠谱点！" << std::endl;
	}

	soundEat.setBuffer(sbEat);
	soundDie.setBuffer(sbDie);

	std::ifstream loadFile;
	for (int i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0:
			loadFile.open("save 1.txt");
			break;
		case 1:
			loadFile.open("save 2.txt");
			break;
		case 2:
			loadFile.open("save 3.txt");
			break;
		}

		if (loadFile.is_open())
			d[i].isDataExist = true;
		else
		{
			d[i].isDataExist = false;
			loadFile.close();
		}
			

		if (d[i].isDataExist)
		{
			loadFile >> d[i].systime.wYear >> d[i].systime.wMonth >> d[i].systime.wDay;
			loadFile >> d[i].systime.wHour >> d[i].systime.wMinute >> d[i].systime.wSecond;
			loadFile >> d[i].score;
		}

		loadFile.close();
	}

	bkMusic.play();
	bkMusic.setLoop(true);

	soundVolume = 50;
	MusicOn = true;

	isRankBoardOn = false;
}

void init()
{
	gameOver = false;
	gamePaused = false;
	gameQuit = false;
	gameMode = 1;

	dir = STOP;
	dir_ing = STOP;
	turned = false;
	updateFlag = false;
	x = width / 2;
	y = height / 2;

	spSnakeHead.setPosition(x, y);

	srand(time(0));
	while (true)
	{
		fruitX = rand() % width;
		fruitY = rand() % height;

		if (x == fruitX && y == fruitY)
			continue;
		else
			break;
	}

	score = 0;

	nTail = 1;

	headLocation = 0;

	stepX = 0.0;
	stepY = 0.0;

	tailX.clear();
	tailY.clear();
	tailX.push_back(x);
	tailY.push_back(y);
}

void input_start_page()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			window.close();

		if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Escape)
		{
			window.close();
			gameQuit = true;
		}

		if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
		{
			if (ButtonRectStart.contains(event.mouseButton.x, event.mouseButton.y))
			{
				gameState = 1;
				init();
			}
				
			if (ButtonRectEnd.contains(event.mouseButton.x, event.mouseButton.y))
			{
				window.close();
				gameQuit = true;
			}

			if (ButtonRectLoad.contains(event.mouseButton.x, event.mouseButton.y))
			{
				load();
				gamePaused = true;
				draw();
				window.display();
				gameState = 1;
			}
		}
	}
}

void input_in_game()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			window.close();

		if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Escape)
		{
			window.close();
			gameQuit = true;
		}

		if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Space)
		{
			if (gameMode == 1)
				gameMode = 2;
			else
				gameMode = 1;
		}

		if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Add)
		{
			soundVolume += 5;
			bkMusic.setVolume(soundVolume);
		}

		if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Subtract)
		{
			soundVolume -= 5;
			bkMusic.setVolume(soundVolume);
		}

		if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Multiply || event.key.code == sf::Keyboard::Enter)
		{
			if (MusicOn == true)
			{
				bkMusic.stop();
				MusicOn = false;
			}
			else
			{
				bkMusic.play();
				MusicOn = true;
			}
		}

		if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::P)
		{
			gamePaused = true;
		}

		if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::R)
		{
			if (isRankBoardOn)
				isRankBoardOn = false;
			else
				isRankBoardOn = true;
		}
	}

	if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A) && y != -1 && y != height && !turned)
	{
		if (dir != RIGHT)
			dir = LEFT;
		turned = true;
	}
	else if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D) && y != -1 && y != height && !turned)
	{
		if (dir != LEFT)
			dir = RIGHT;
		turned = true;
	}
	else if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W) && x != -1 && x != width && !turned)
	{
		if (dir != DOWN)
			dir = UP;
		turned = true;
	}
	else if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S) && x != -1 && x != width && !turned)
	{
		if (dir != UP)
			dir = DOWN;
		turned = true;
	}
}

void input_game_paused()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			window.close();

		if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Escape)
		{
			window.close();
			gameQuit = true;
		}

		if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::S)
			save();

		if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::L)
			load();

		if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::BackSpace)
			gamePaused = false;
	}
}

void logic()
{ 
	if (gameMode == 1)
	{
		turned = false;

		if (x >= width - 1 && dir == RIGHT)
			x = -1;
		else if (x <= 0 && dir == LEFT)
			x = width;

		if (y >= height - 1 && dir == DOWN)
			y = -1;
		else if (y <= 0 && dir == UP)
			y = height;

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
			{
				soundDie.play();
				gameOver = true;
			}

		}

		int prevX = tailX[0], prevY = tailY[0];
		int prev2X, prev2Y;
		tailX[0] = x;
		tailY[0] = y;

		if (x == fruitX && y == fruitY)
		{
			score += 10;
			soundEat.play();
			srand(time(0));
			while (true)
			{
				fruitX = rand() % width;
				fruitY = rand() % height;

				if (x == fruitX && y == fruitY)
					continue;
				else
					break;
			}
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
	}
	else if (gameMode == 2)
	{
		turned = false;

		int prevX = tailX[0], prevY = tailY[0];
		int prev2X, prev2Y;
		updateFlag = false;

		if (x >= width - 1 && dir == RIGHT)
			x = 0;
		else if (x <= 0 && dir == LEFT)
			x = width - 1;

		if (y >= height - 1 && dir == DOWN)
			y = 0;
		else if (y <= 0 && dir == UP)
			y = height - 1;

		switch (dir_ing)
		{
		case LEFT:
			stepX -= STEP;
			if (stepX < -STEP_CHANGE || stepX >= STEP_CHANGE)
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
			if (stepX < -STEP_CHANGE || stepX >= STEP_CHANGE)
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
			if (stepY < -STEP_CHANGE || stepY >= STEP_CHANGE)
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
			if (stepY < -STEP_CHANGE || stepY >= STEP_CHANGE)
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

		tailX[0] = x;
		tailY[0] = y;

		if (updateFlag == true)
		{
			if (x == fruitX && y == fruitY)
			{
				score += 10;
				soundEat.play();
				srand(time(0));
				while (true)
				{
					fruitX = rand() % width;
					fruitY = rand() % height;

					if (x == fruitX && y == fruitY)
						continue;
					else
						break;
				}
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
				{
					soundDie.play();
					gameOver = true;
				}
					
			}
		}
	}
}

void promptInfo(int _x, int _y)
{
	int initX = WORD_RIGHT_INITX, initY = WORD_RIGHT_INITY;
	int CharacterSize = CHARACTER_SIZE_PROMPT_INFO;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(0, 0, 0, 255));
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
	text.setString(L"	○ 背景音开关：Enter / *");
	window.draw(text);

	initY += CharacterSize;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"	○ 背景音音量：+ / -");
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
	text.setString(L"	○ 暂停游戏：P");
	window.draw(text);

	initY += CharacterSize;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"	○ 继续游戏：backspace");
	window.draw(text);

	initY += CharacterSize;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"	○ 动画模式切换：space");
	window.draw(text);

	initY += CharacterSize * 1.5;
	text.setPosition(_x + initX, _y + initY);
	if (gameMode == 1)
	{
		text.setFillColor(Color(255, 0, 0));
		text.setString(L"		步进移动");
		window.draw(text);
	}
	else 
	{
		text.setFillColor(Color(0, 0, 255));
		text.setString(L"		连续移动");
		window.draw(text);
	}

	text.setFillColor(Color(0, 0, 0));
	initY += CharacterSize * 1.5;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"◇ 当前得分：");
	window.draw(text);

	text.setFillColor(sf::Color(255, 0, 0));
	initY += CharacterSize;
	text.setPosition(_x + initX + CharacterSize * 7, _y + initY);
	CharacterSize = CHARACTER_SIZE_SCORE;
	text.setCharacterSize(CharacterSize);
	std::stringstream ss;
	ss << score;
	text.setString(ss.str());
	window.draw(text);

	text.setFillColor(sf::Color(0, 0, 0));
	CharacterSize = CHARACTER_SIZE_PROMPT_INFO;
	text.setCharacterSize(CharacterSize);
	initY += CharacterSize * 2;
	text.setPosition(_x + initX, _y + initY);
	text.setString(L"◇ 万Queen / 制作");
	window.draw(text);
}

void gameOverInfo()
{
	spGameOverInfo.setPosition(GAME_OVER_INFO_POSITION_X, GAME_OVER_INFO_POSITION_Y);
	window.draw(spGameOverInfo);
}

void gamePausedInfo()
{
	spGamePausedInfo.setPosition(GAME_OVER_INFO_POSITION_X, GAME_OVER_INFO_POSITION_Y);
	window.draw(spGamePausedInfo);
}

void show_rank_board(int _x, int _y)
{
	int initX = RANK_BOARD_POSITION_INITX, initY = RANK_BOARD_POSITION_INITY;
	spRankBoardBackground.setPosition(_x + initX, _y + initY);
	window.draw(spRankBoardBackground);

	initX = WORD_RIGHT_INITX + 20;
	initY = WORD_RIGHT_INITY + 20;
	int CharacterSize = CHARACTER_SIZE_PROMPT_INFO;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 0, 0));

	std::stringstream ss;
	ss.str("");
	std::string s = " ";
	std::string n;
	for (int i = 0; i < rankPlayer.size(); i++)
	{
		text.setPosition(_x + initX, _y + initY);
		n = rankPlayer[i].name;
		ss << i + 1 << ". " << n << s << rankPlayer[i].score;
		text.setString(ss.str());
		window.draw(text);
		ss.str("");
		initY += CharacterSize * 1.5;
	}
}

void drawSaveAndLoad(int trianglePos)
{
	window.clear(sf::Color::Color(124, 252, 0, 255));

	spSaveButton.setPosition(SL_BUTTON1_X, SL_BUTTON_Y);
	ButtonRectSave1.left = SL_BUTTON1_X;
	ButtonRectSave1.top = SL_BUTTON_Y;
	ButtonRectSave1.width = SL_BUTTON_WIDTH;
	ButtonRectSave1.height = SL_BUTTON_HEIGHT;
	window.draw(spSaveButton);

	spSaveButton.setPosition(SL_BUTTON2_X, SL_BUTTON_Y);
	ButtonRectSave2.left = SL_BUTTON2_X;
	ButtonRectSave2.top = SL_BUTTON_Y;
	ButtonRectSave2.width = SL_BUTTON_WIDTH;
	ButtonRectSave2.height = SL_BUTTON_HEIGHT;
	window.draw(spSaveButton);

	spSaveButton.setPosition(SL_BUTTON3_X, SL_BUTTON_Y);
	ButtonRectSave3.left = SL_BUTTON3_X;
	ButtonRectSave3.top = SL_BUTTON_Y;
	ButtonRectSave3.width = SL_BUTTON_WIDTH;
	ButtonRectSave3.height = SL_BUTTON_HEIGHT;
	window.draw(spSaveButton);

	switch (trianglePos)
	{
	case 1:
		spTriangle.setPosition(SL_BUTTON1_X + SL_BUTTON_WIDTH / 2, SL_BUTTON_Y - 50);
		break;
	case 2:
		spTriangle.setPosition(SL_BUTTON2_X + SL_BUTTON_WIDTH / 2, SL_BUTTON_Y - 50);
		break;
	case 3:
		spTriangle.setPosition(SL_BUTTON3_X + SL_BUTTON_WIDTH / 2, SL_BUTTON_Y - 50);
		break;
	default:
		std::cout << "显示Triangle error！能不能行啊喂！" << std::endl;
		break;
	}
	window.draw(spTriangle);

	text.setCharacterSize(CHARACTER_SIZE_SL_TITLE);
	text.setPosition(WORD_SAVE_DEVIATION_X + SL_BUTTON1_X, WORD_SAVE_DEVIATION_Y +SL_BUTTON_Y);
	text.setString(L"存档一");
	window.draw(text);

	text.setPosition(WORD_SAVE_DEVIATION_X + SL_BUTTON2_X, WORD_SAVE_DEVIATION_Y + SL_BUTTON_Y);
	text.setString(L"存档二");
	window.draw(text);

	text.setPosition(WORD_SAVE_DEVIATION_X + SL_BUTTON3_X, WORD_SAVE_DEVIATION_Y + SL_BUTTON_Y);
	text.setString(L"存档三");
	window.draw(text);

	text.setCharacterSize(CHARACTER_SIZE_SL_DETAIL);
	std::stringstream ss;
	for (int i = 0; i < 3; i++)
	{
		text.setFillColor(sf::Color(0, 0, 0));
		switch (i)
		{
		case 0:
			text.setPosition(WORD_SAVE_DEVIATION_X + SL_BUTTON1_X, WORD_SAVE_DETAIL_DEVIATION_Y + WORD_SAVE_DEVIATION_Y + SL_BUTTON_Y);
			break;
		case 1:
			text.setPosition(WORD_SAVE_DEVIATION_X + SL_BUTTON2_X, WORD_SAVE_DETAIL_DEVIATION_Y + WORD_SAVE_DEVIATION_Y + SL_BUTTON_Y);
			break;
		case 2:
			text.setPosition(WORD_SAVE_DEVIATION_X + SL_BUTTON3_X, WORD_SAVE_DETAIL_DEVIATION_Y + WORD_SAVE_DEVIATION_Y + SL_BUTTON_Y);
			break;
		}

		if (d[i].isDataExist)
		{
			ss << d[i].systime.wYear << "/" << d[i].systime.wMonth << "/" << d[i].systime.wDay;
			text.setString(ss.str());
			window.draw(text);
			ss.str("");

			switch (i)
			{
			case 0:
				text.setPosition(WORD_SAVE_DEVIATION_X + SL_BUTTON1_X, CHARACTER_SIZE_SL_DETAIL + CHARACTER_SL_DETAIL_DEVIATION + WORD_SAVE_DETAIL_DEVIATION_Y + WORD_SAVE_DEVIATION_Y + SL_BUTTON_Y);
				break;
			case 1:
				text.setPosition(WORD_SAVE_DEVIATION_X + SL_BUTTON2_X, CHARACTER_SIZE_SL_DETAIL + CHARACTER_SL_DETAIL_DEVIATION + WORD_SAVE_DETAIL_DEVIATION_Y + WORD_SAVE_DEVIATION_Y + SL_BUTTON_Y);
				break;
			case 2:
				text.setPosition(WORD_SAVE_DEVIATION_X + SL_BUTTON3_X, CHARACTER_SIZE_SL_DETAIL + CHARACTER_SL_DETAIL_DEVIATION + WORD_SAVE_DETAIL_DEVIATION_Y + WORD_SAVE_DEVIATION_Y + SL_BUTTON_Y);
				break;
			}

			ss << d[i].systime.wHour << ":" << d[i].systime.wMinute << ":" << d[i].systime.wSecond;
			text.setString(ss.str());
			window.draw(text);
			ss.str("");

			switch (i)
			{
			case 0:
				text.setPosition(WORD_SAVE_DEVIATION_X + SL_BUTTON1_X, (CHARACTER_SIZE_SL_DETAIL + CHARACTER_SL_DETAIL_DEVIATION) * 2 + WORD_SAVE_DETAIL_DEVIATION_Y + WORD_SAVE_DEVIATION_Y + SL_BUTTON_Y);
				break;
			case 1:
				text.setPosition(WORD_SAVE_DEVIATION_X + SL_BUTTON2_X, (CHARACTER_SIZE_SL_DETAIL + CHARACTER_SL_DETAIL_DEVIATION) * 2 + WORD_SAVE_DETAIL_DEVIATION_Y + WORD_SAVE_DEVIATION_Y + SL_BUTTON_Y);
				break;
			case 2:
				text.setPosition(WORD_SAVE_DEVIATION_X + SL_BUTTON3_X, (CHARACTER_SIZE_SL_DETAIL + CHARACTER_SL_DETAIL_DEVIATION) * 2 + WORD_SAVE_DETAIL_DEVIATION_Y + WORD_SAVE_DEVIATION_Y + SL_BUTTON_Y);
				break;
			}

			text.setString(L"分数：");
			window.draw(text);

			switch (i)
			{
			case 0:
				text.setPosition(WORD_SAVE_DEVIATION_X + SL_BUTTON1_X, (CHARACTER_SIZE_SL_DETAIL + CHARACTER_SL_DETAIL_DEVIATION) * 3 + WORD_SAVE_DETAIL_DEVIATION_Y + WORD_SAVE_DEVIATION_Y + SL_BUTTON_Y);
				break;
			case 1:
				text.setPosition(WORD_SAVE_DEVIATION_X + SL_BUTTON2_X, (CHARACTER_SIZE_SL_DETAIL + CHARACTER_SL_DETAIL_DEVIATION) * 3 + WORD_SAVE_DETAIL_DEVIATION_Y + WORD_SAVE_DEVIATION_Y + SL_BUTTON_Y);
				break;
			case 2:
				text.setPosition(WORD_SAVE_DEVIATION_X + SL_BUTTON3_X, (CHARACTER_SIZE_SL_DETAIL + CHARACTER_SL_DETAIL_DEVIATION) * 3 + WORD_SAVE_DETAIL_DEVIATION_Y + WORD_SAVE_DEVIATION_Y + SL_BUTTON_Y);
				break;
			}

			ss << d[i].score;
			text.setFillColor(sf::Color(255, 0, 0));
			text.setString(ss.str());
			window.draw(text);
			ss.str("");
		}
		else
		{
			text.setString(L"空存档");
			window.draw(text);
		}
	}

	window.display();
}

void drawStartPage()
{
	spStartBackground.setPosition(0, 0);
	window.draw(spStartBackground);

	spStartButton.setPosition(START_BUTTON_POSITION_X, BUTTON_POSITION_Y);
	ButtonRectStart.left = START_BUTTON_POSITION_X;
	ButtonRectStart.top = BUTTON_POSITION_Y;
	ButtonRectStart.width = BUTTON_WIDTH;
	ButtonRectStart.height = BUTTON_HEIGHT;
	window.draw(spStartButton);

	spLoadButton.setPosition(LOAD_BUTTON_POSITION_X, BUTTON_POSITION_Y);
	ButtonRectLoad.left = LOAD_BUTTON_POSITION_X;
	ButtonRectLoad.top = BUTTON_POSITION_Y;
	ButtonRectLoad.width = BUTTON_WIDTH;
	ButtonRectLoad.height = BUTTON_HEIGHT;
	window.draw(spLoadButton);

	spEndButton.setPosition(END_BUTTON_POSITION_X, BUTTON_POSITION_Y);
	ButtonRectEnd.left = END_BUTTON_POSITION_X;
	ButtonRectEnd.top = BUTTON_POSITION_Y;
	ButtonRectEnd.width = BUTTON_WIDTH;
	ButtonRectEnd.height = BUTTON_HEIGHT;
	window.draw(spEndButton);

	window.display();
}

void draw()
{
	if (gameMode == 1)
	{
		window.clear(sf::Color::Color(124, 252, 0, 255));
		promptInfo(width * GRID_SIZE + GRID_SIZE, GRID_SIZE);

		int detaX = GRID_SIZE / SCALE / 2;
		int detaY = GRID_SIZE / SCALE / 2;

		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
			{
				spBackground.setPosition(i * GRID_SIZE + detaX, j * GRID_SIZE + detaY);
				window.draw(spBackground);
			}

		spFruit.setPosition(fruitX * GRID_SIZE + detaX, fruitY * GRID_SIZE + detaY);
		window.draw(spFruit);

		spSnakeHead.setRotation(headLocation);
		spSnakeHead.setPosition(tailX[0] * GRID_SIZE + detaX, tailY[0] * GRID_SIZE + detaY);
		window.draw(spSnakeHead);

		for (int i = 1; i < nTail; i++)
		{
			spSnakeBody.setPosition(tailX[i] * GRID_SIZE + detaX, tailY[i] * GRID_SIZE + detaY);
			window.draw(spSnakeBody);
		}
	}
	else if (gameMode == 2)
	{
		window.clear(sf::Color::Color(124, 252, 0, 255));
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

		spFruit.setPosition(fruitX * GRID_SIZE + detaX, fruitY * GRID_SIZE + detaY);
		window.draw(spFruit);

		spSnakeHead.setPosition((tailX[0] + stepX) * GRID_SIZE + detaX, (tailY[0] + stepY) * GRID_SIZE + detaY);
		spSnakeHead.setRotation(headLocation);
		window.draw(spSnakeHead);

		for (int i = 1; i < nTail; i++)
		{
			if (tailY[i] == tailY[i - 1] && tailX[i] != tailX[i - 1] && abs(tailX[i] - tailX[i - 1]) <= 1)
			{
				spSnakeBody.setPosition((tailX[i] + (tailX[i - 1] - tailX[i]) * stepLenth) * GRID_SIZE + detaX, tailY[i] * GRID_SIZE + detaY);
			}
			else if (tailY[i] == tailY[i - 1] && tailX[i] != tailX[i - 1] && abs(tailX[i] - tailX[i - 1]) > 1)
			{
				spSnakeBody.setPosition((tailX[i] - (tailX[i - 1] - tailX[i]) / abs(tailX[i - 1] - tailX[i]) * stepLenth) * GRID_SIZE + detaX, tailY[i] * GRID_SIZE + detaY);
			}

			if (tailY[i] != tailY[i - 1] && tailX[i] == tailX[i - 1] && abs(tailY[i] - tailY[i - 1]) <= 1)
			{
				spSnakeBody.setPosition(tailX[i] * GRID_SIZE + detaX, (tailY[i] + (tailY[i - 1] - tailY[i]) * stepLenth) * GRID_SIZE + detaY);
			}
			else if (tailY[i] != tailY[i - 1] && tailX[i] == tailX[i - 1] && abs(tailY[i] - tailY[i - 1]) > 1)
			{
				spSnakeBody.setPosition(tailX[i] * GRID_SIZE + detaX, (tailY[i] - (tailY[i - 1] - tailY[i]) / abs(tailY[i - 1] - tailY[i]) * stepLenth) * GRID_SIZE + detaY);
			}

			if (tailY[i] != tailY[i - 1] && tailX[i] != tailX[i - 1] && abs(tailX[i] - tailX[i - 1]) <= 1 && abs(tailY[i] - tailY[i - 1]) <= 1)
			{
				spSnakeBody.setPosition((tailX[i] + (tailX[i - 1] - tailX[i]) * stepLenth) * GRID_SIZE + detaX, (tailY[i] + (tailY[i - 1] - tailY[i]) * stepLenth) * GRID_SIZE + detaY);
			}
			else if (tailY[i] != tailY[i - 1] && tailX[i] != tailX[i - 1] && abs(tailX[i] - tailX[i - 1]) > 1 && abs(tailY[i] - tailY[i - 1]) > 1)
			{
				spSnakeBody.setPosition((tailX[i] - (tailX[i - 1] - tailX[i]) / abs(tailX[i - 1] - tailX[i]) * stepLenth) * GRID_SIZE + detaX, (tailY[i] - (tailY[i - 1] - tailY[i]) / abs(tailY[i - 1] - tailY[i]) * stepLenth) * GRID_SIZE + detaY);
			}

			window.draw(spSnakeBody);
		}
	}

	if (gameOver)
		gameOverInfo();

	if (gamePaused)
		gamePausedInfo();

	if (isRankBoardOn)
	{
		show_rank_board(width * GRID_SIZE + GRID_SIZE, GRID_SIZE);
	}
}

int main()
{
	rank_calc();
	while (!gameQuit && window.isOpen())
	{
		switch (gameState)
		{
		case 0:
			while (gameState == 0)
			{
				initStartPage();
				drawStartPage();
				input_start_page();
			}

			break;
		case 1:

			while (gameState == 1)
			{
				while (!gameOver && !gamePaused)
				{
					input_in_game();
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
						window.display();
						break;
					case 2:
						logic();
						draw();
						window.display();
						break;
					default:
						break;
					}
				}

				while (gameOver)
				{
					Event event;
					while (window.pollEvent(event))
					{
						if (event.type == Event::Closed)
						{
							window.close();
							gameOver = false;
							gameQuit = true;
						}

						if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Escape)
						{
							window.close();
							gameQuit = true;
						}

						if (event.type == Event::EventType::KeyReleased && event.key.code == Keyboard::Y)
						{
							gameOver = false;
							init();
						}

						if (event.type == Event::EventType::KeyReleased && event.key.code == Keyboard::X)
						{
							gameOver = false;
							gameQuit = true;
							window.close();
						}

						if (event.type == Event::EventType::KeyReleased && event.key.code == Keyboard::L)
						{
							load();
							gameOver = false;
							gamePaused = true;
							draw();
							window.display();
						}

						if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::N)
						{
							write_name();
							gameOver = false;
							gameQuit = true;
							window.close();
						}
							
					}
				}

				while (gamePaused)
				{
					input_game_paused();
				}
			}
			break;
		}
	}
	
	bkMusic.stop();

	return 0;
}