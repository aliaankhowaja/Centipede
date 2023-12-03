#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include<unistd.h>

using namespace std;

// Initializing Dimensions.
const int resolutionX = 960;
const int resolutionY = 960;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX; // Total rows on grid
const int gameColumns = resolutionY / boxPixelsY; // Total columns on grid

// Initializing GameGrid.
int gameGrid[gameRows][gameColumns] = {};

const int x = 0; // x value of the sprite
const int y = 1; // y value of the sprite
const int exists = 2; // does the object exists
const int isHead = 3; // is the object a head
const int animationLocation = 4; // animation location of the head or segment
const int dir = 5; // direction of the segment or head
const int lastDir = 6; // last horizontal direction of the sprite 

const int left = 0;
const int down = 3;
const int right = 2;
const int up = 1;

void movePlayer(float player[], int direction, bool* gameOver); // moves the player
void drawPlayer(sf::RenderWindow& window, float player[], sf::Sprite& playerSprite); // draws the player in the game window
void moveBullet(float bullet[], sf::Clock& bulletClock); // moves the bullet
void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite); // draws the bullet
int* generateMushrooms(); // generates mushrooms in the game grid
void drawMushrooms(sf::RenderWindow& window, sf::Sprite& mushroomSprite); // loops through the game grid and if mushroom found then draw it in the game window
void moveCentepedes(float player[], int centepedes[12][7], bool* gameOver); // moves the centepede
void drawCentepedes(int centepedes[12][7], sf::RenderWindow& window, sf::Sprite& headSprite, sf::Sprite& segmentSprite); // draws the centepede in the game window
bool mushroomExist(int x, int y);
int checkMushroomCollision(int row, int col);
void checkBulletHit(int centepedes[12][7], float bullet[], sf::Sound& killSound);
void fragmentCentepede(int centepedes[12][7], int hitSegment);
void die(sf::RenderWindow& window, sf::Sound& deathSound, sf::Sprite& playerSprite, int row, int col, bool* animationDone);

int main() {
	// int*** centepedes = new int** [2]; // 3d dynamic array. with 2 int arrays
	srand(time(0));// adding the seeding value in the rand() function

	// Declaring RenderWindow.
	sf::RenderWindow window(sf::VideoMode(resolutionX, resolutionY), "Centipede", sf::Style::Close | sf::Style::Titlebar);

	// Used to resize your window if it's too big or too small. Use according to your needs.
	window.setSize(sf::Vector2u(640, 640)); // Recommended for 1366x768 (768p) displays.
	// window.setSize(sf::Vector2u(1280, 1280)); // Recommended for 2560x1440 (1440p) displays.
	// window.setSize(sf::Vector2u(1920, 1920)); // Recommended for 3840x2160 (4k) displays.

	// Used to position your window on every launch. Use according to your needs.
	window.setPosition(sf::Vector2i(100, 0));

	// Initializing Background Music.
	sf::Music bgMusic;
	bgMusic.openFromFile("Music/field_of_hopes.ogg");
	bgMusic.play();
	bgMusic.setVolume(30); // 50

	//  Bullet Sound effect.
	sf::SoundBuffer bulletBuffer;
	bulletBuffer.loadFromFile("Sound Effects/fire1.wav");
	sf::Sound bulletSound;
	bulletSound.setBuffer(bulletBuffer);

	// Kill Sound Effect
	sf::SoundBuffer killBuffer;
	killBuffer.loadFromFile("Sound Effects/kill.wav");
	sf::Sound killSound;
	killSound.setBuffer(killBuffer);

	// death Sound Effect
	sf::SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("Sound Effects/death.wav");
	sf::Sound deathSound;
	deathSound.setBuffer(deathBuffer);

	// Initializing Background.
	sf::Texture backgroundTexture;
	sf::Sprite backgroundSprite;
	backgroundTexture.loadFromFile("Textures/background.jpg");
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setColor(sf::Color(255, 255, 255, 255 * 0.4)); // Reduces Opacity to 50%

	// Initializing Player and Player Sprites.
	float player[2] = {};
	player[x] = (gameColumns / 2) * boxPixelsX;
	player[y] = (27) * boxPixelsY;
	// player[y] = (gameColumns * 3 / 4) * boxPixelsY;
	sf::Texture playerTexture;
	sf::Sprite playerSprite;
	playerTexture.loadFromFile("Textures/player.png");
	playerSprite.setTexture(playerTexture);
	playerSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

	// Initializing Bullet and Bullet Sprites.
	float bullet[3] = {};
	bullet[x] = player[x];
	bullet[y] = player[y] - boxPixelsY;
	bullet[exists] = false;
	sf::Clock bulletClock;
	sf::Texture bulletTexture;
	sf::Sprite bulletSprite;
	bulletTexture.loadFromFile("Textures/bullet.png");
	bulletSprite.setTexture(bulletTexture);
	bulletSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

	// Initializing the mushrooms
	int* startingLocation = generateMushrooms();
	int startingX = startingLocation[1] * boxPixelsX, startingY = startingLocation[0] * boxPixelsY;

	sf::Texture mushroomTexture;
	sf::Sprite mushroomSprite;
	mushroomTexture.loadFromFile("Textures/mushroom.png");
	mushroomSprite.setTexture(mushroomTexture);
	// mushroomSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

	sf::Texture headTexture;
	sf::Sprite headSprite;
	headTexture.loadFromFile("Textures/c_head_left_walk.png");
	headSprite.setTexture(headTexture);
	// headSprite.setColor(sf::Color(255, 128, 255, 255)); // Code from ChatGPT

	sf::Texture segmentTexture;
	sf::Sprite segmentSprite;
	segmentTexture.loadFromFile("Textures/c_body_left_walk.png");
	segmentSprite.setTexture(segmentTexture);
	// segmentSprite.setColor(sf::Color(255, 128, 255, 255));// code from chatgpt

	// I tried using triple pointer array for the following task but had lots of memory errors
	int centepedes[12][7]; // centepedes data array. holds the data of each centepede and its segment.
	for (int i = 0; i < 12; i++) {
		if (i == 0) {
			centepedes[i][x] = startingX;
			centepedes[i][isHead] = true;
			centepedes[i][animationLocation] = 0;
		} else {
			centepedes[i][x] = centepedes[i - 1][x] + boxPixelsX;
			centepedes[i][isHead] = false;
			if (centepedes[i - 1][animationLocation] == 7) {
				centepedes[i][animationLocation] = 0;
			} else {
				centepedes[i][animationLocation] = centepedes[i - 1][animationLocation] + 1;
			}
		}
		centepedes[i][y] = startingY;
		centepedes[i][exists] = true;
		centepedes[i][dir] = 0;
		centepedes[i][lastDir] = 0;
	}
	int* hitDetails;
	bool gameOver = false, animationDone = false;
	while (window.isOpen()) {
		int framesPassed = 0;
		sf::Event e;
		window.draw(backgroundSprite);
		drawMushrooms(window, mushroomSprite);
		drawCentepedes(centepedes, window, headSprite, segmentSprite);
		if (gameOver) {
			int row = (player[y]) / boxPixelsY;
			int col = (player[x]) / boxPixelsX;
			if (!animationDone)
				die(window, deathSound, playerSprite, row, col, &animationDone);
		} else {
			moveCentepedes(player, centepedes, &gameOver);
			if (bullet[exists] == true) {
				moveBullet(bullet, bulletClock);
				drawBullet(window, bullet, bulletSprite);
				int bulletRow = bullet[y] / boxPixelsY;
				int bulletCol = bullet[x] / boxPixelsX;
				bulletRow--;
				checkBulletHit(centepedes, bullet, killSound);
			}
			while (window.pollEvent(e)) {
				if (e.type == sf::Event::Closed) return 0;
				if (e.type == sf::Event::KeyPressed) {
					if (e.key.code == 57) {
						if (!(framesPassed % 16)) {
							if (bullet[exists]) {
								// drawBullet(window, bullet, bulletSprite);
								// moveBullet(bullet, bulletClock);
							} else {
								bulletSound.play();
								bullet[x] = player[x];
								bullet[y] = player[y];
								bullet[exists] = true;
							}
						}
					} else {
						movePlayer(player, e.key.code, &gameOver);
					}
				}
			}
			drawPlayer(window, player, playerSprite);
		}
		while (window.pollEvent(e)) {
			if (e.type == sf::Event::Closed) return 0;
		}

		window.display();
		window.clear();
		framesPassed++;
		usleep(1000);
	}
}
void drawPlayer(sf::RenderWindow& window, float player[], sf::Sprite& playerSprite) {
	playerSprite.setPosition(player[x], player[y]);
	window.draw(playerSprite);
}
void moveBullet(float bullet[], sf::Clock& bulletClock) {
	if (bulletClock.getElapsedTime().asMilliseconds() < 20)
		return;
	bulletClock.restart();
	bullet[y] -= 32;
	int bulletCol = (int)bullet[x] / boxPixelsX;
	int bulletRow = (int)bullet[y] / boxPixelsY;
	bulletRow--;
	if (bullet[y] < -32)
		bullet[exists] = false;
	if (mushroomExist(bulletRow, bulletCol)) {
		gameGrid[bulletRow][bulletCol] = checkMushroomCollision(bulletRow, bulletCol);
		bullet[exists] = false;
	}
}
void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite) {
	bulletSprite.setPosition(bullet[x], bullet[y]);
	window.draw(bulletSprite);
}
void movePlayer(float player[], int direction, bool* gameOver) {
	switch (direction) {
	case 71:
		if (player[x] > 0)
			player[x] -= boxPixelsX;
		break;
	case 72:
		if (player[x] < 928)
			player[x] += boxPixelsX;
		break;
	case 73:
		if (player[y] > 800)
			player[y] -= boxPixelsY;
		break;
	case 74:
		if (player[y] < 928)
			player[y] += boxPixelsY;
		break;
	}
	int row = (player[y]) / boxPixelsY;
	int col = (player[x]) / boxPixelsX;
	if (gameGrid[row][col])*gameOver = true;
}
int* generateMushrooms() {
	int* startingLoc = new int[2];
	startingLoc[0] = rand() % (gameRows - 18);
	startingLoc[1] = rand() % (gameColumns - 13);
	startingLoc[0] = 24;

	int mushrooms = 0;
	static int totalMushrooms = rand() % 10 + 20;

	for (int i = 1;i < 23;i++) {
		for (int j = 2;j < gameColumns - 2;j++) {

			if (!(rand() % 30) && mushrooms <= totalMushrooms && gameGrid[i][j] != 1 && gameGrid[i][j] != 2 && startingLoc[0] != i) {
				gameGrid[i][j] = 2;
				mushrooms++;
			}
		}
	}
	return startingLoc;
}
void drawMushrooms(sf::RenderWindow& window, sf::Sprite& mushroomSprite) {
	int y, x;
	for (int i = 0; i < gameRows; i++) {
		for (int j = 0; j < gameColumns; j++) {
			if (gameGrid[i][j] == 0)continue;
			y = (gameGrid[i][j] - 1) / 2;
			x = gameGrid[i][j] % 2 * 2;
			mushroomSprite.setTextureRect(sf::IntRect(x * boxPixelsX, y * boxPixelsY, boxPixelsX, boxPixelsY));
			mushroomSprite.setPosition(j * boxPixelsX, i * boxPixelsY);
			window.draw(mushroomSprite);
		}
	}
}
void drawCentepedes(int centepedes[12][7], sf::RenderWindow& window, sf::Sprite& headSprite, sf::Sprite& segmentSprite) {
	int segmentX, segmentY, segmentDirection, segmentAnimation;
	for (int segment = 0; segment < 12; segment++) {
		segmentX = centepedes[segment][x];
		segmentY = centepedes[segment][y];
		segmentAnimation = centepedes[segment][animationLocation];
		segmentDirection = centepedes[segment][dir];
		if (centepedes[segment][exists]) {
			if (centepedes[segment][isHead]) {
				headSprite.setTextureRect(sf::IntRect(segmentAnimation * boxPixelsX, 0, boxPixelsX, boxPixelsY));
				headSprite.setPosition(segmentX, segmentY);
				headSprite.setRotation(segmentDirection * 90);
				window.draw(headSprite);
			} else {
				segmentSprite.setTextureRect(sf::IntRect(segmentAnimation * boxPixelsX, 0, boxPixelsX, boxPixelsY));
				segmentSprite.setPosition(segmentX, segmentY);
				segmentSprite.setRotation(segmentDirection * 90);
				window.draw(segmentSprite);
			}
		}
	}
}
void moveCentepedes(float player[], int centepedes[12][7], bool* gameOver) {
	static int numOfTimesCalled = 0;
	static int playerRegionDir = 3;
	int row, col, lastDirection;
	for (int segment = 0; segment < 12; segment++) {
		lastDirection = centepedes[segment][lastDir];
		row = (centepedes[segment][y]) / boxPixelsY;
		col = centepedes[segment][x] / boxPixelsX;
		if (centepedes[segment][dir]) row--;
		if (!centepedes[segment][dir]) col--;
		int l = (!centepedes[segment][dir]) ? 1 : 0;
		if (player[x] / boxPixelsX - l == col && player[y] / boxPixelsY == row) {
			*gameOver = true;
			break;
		}
		if (!(numOfTimesCalled % 16) && centepedes[segment][dir] == playerRegionDir) {
			centepedes[segment][dir] = abs(lastDirection - 2);
			centepedes[segment][lastDir] = abs(lastDirection - 2);
			if (centepedes[segment][dir] == 2)
				centepedes[segment][x] = centepedes[segment][x] + boxPixelsX;
			else
				centepedes[segment][y] = centepedes[segment][y] - boxPixelsY;
		} else if (!(numOfTimesCalled % 16) && (centepedes[segment][x] == 0 || mushroomExist(row, col)) && centepedes[segment][dir] == 0) {
			centepedes[segment][dir] = down;
			centepedes[segment][y] = centepedes[segment][y] + boxPixelsY;
		} else if (!(numOfTimesCalled % 16) && (centepedes[segment][x] == 960 || mushroomExist(row, col)) && centepedes[segment][dir] == 2) {
			centepedes[segment][dir] = down;
			centepedes[segment][x] = centepedes[segment][x] - boxPixelsX;
		}
		if (centepedes[segment][dir] % 2) {
			centepedes[segment][y] = centepedes[segment][y] + (centepedes[segment][dir] - 2) * 2;
		} else {
			centepedes[segment][x] = centepedes[segment][x] + (centepedes[segment][dir] - 1) * 2;
		}

		if (!(numOfTimesCalled % 5))
			if (segment != 0) {
				if (centepedes[segment - 1][animationLocation] == 7) {
					centepedes[segment][animationLocation] = 0;
				} else {
					centepedes[segment][animationLocation] = centepedes[segment - 1][animationLocation] + 1;
				}
			} else {
				if (centepedes[segment][animationLocation] == 7) {
					centepedes[segment][animationLocation] = 0;
				} else {
					centepedes[segment][animationLocation] = centepedes[0][animationLocation] + 1;
				}
			}
	}
	numOfTimesCalled++;
}
bool mushroomExist(int row, int col) {
	if (gameGrid[row][col] >= 1 && gameGrid[row][col] <= 4) {
		return true;
	}
	return false;
}
int checkMushroomCollision(int row, int col) {
	if (gameGrid[row][col] == 4) {
		return 3;
	} else if (gameGrid[row][col] == 3) {
		return 0;
	} else if (gameGrid[row][col] == 2) {
		return 1;
	} else {
		return 0;
	}
}
void checkBulletHit(int centepedes[12][7], float bullet[], sf::Sound& killSound) {
	int bulletRow = bullet[y] / boxPixelsY;
	int bulletCol = bullet[x] / boxPixelsX;
	int row, col;
	bool hit = false;
	int hitSegment;
	for (int segment = 0; segment < 12; segment++) {
		if (centepedes[segment][exists]) {
			row = (centepedes[segment][y]) / boxPixelsY;
			col = centepedes[segment][x] / boxPixelsX;
			if (row == bulletRow && col == bulletCol) {
				hit = true;
				hitSegment = segment;
				bullet[exists] = false;
				break;
			}
		}
	}
	if (hit) {
		killSound.play();
		if (centepedes[hitSegment][dir] == 2) {
			row--;
		}
		if (centepedes[hitSegment][y] < 960 && (centepedes[hitSegment][y] > 800 || (centepedes[hitSegment][y] >= 800 && centepedes[hitSegment][dir] == 0)))
			gameGrid[row][col] = 4;
		else
			gameGrid[row][col] = 2;
		fragmentCentepede(centepedes, hitSegment);
	}
}
void fragmentCentepede(int centepedes[12][7], int hitSegment) {
	centepedes[hitSegment][exists] = false;
	if (hitSegment != 11)
		if (centepedes[hitSegment + 1][exists]) centepedes[hitSegment + 1][isHead] = true;
}
void die(sf::RenderWindow& window, sf::Sound& deathSound, sf::Sprite& playerSprite, int row, int col, bool* animationDone) {
	static int numOfTimesCalled = 0;
	sf::Texture deathTexture;
	deathTexture.loadFromFile("Textures/death.png");
	playerSprite.setTexture(deathTexture);
	if (!numOfTimesCalled) deathSound.play();
	gameGrid[row][col] = 0;
	int i = numOfTimesCalled / 8;
	int j = numOfTimesCalled % 8;
	cout << i << " " << j << endl;
	playerSprite.setTextureRect(sf::IntRect(j * 32, i * boxPixelsY, boxPixelsX, boxPixelsY));
	playerSprite.setPosition(col * boxPixelsX, row * boxPixelsY);
	window.draw(playerSprite);
	numOfTimesCalled++;
	if (numOfTimesCalled == 16)*animationDone = true;
	usleep(40000);
}