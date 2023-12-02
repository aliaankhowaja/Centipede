#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include<unistd.h>

using namespace std;

// Initializing Dimensions.
// resolutionX and resolutionY determine the rendering resolution.
// Don't edit unless required. Use functions on lines 43, 44, 45 for resizing the game window.
const int resolutionX = 960;
const int resolutionY = 960;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX; // Total rows on grid
const int gameColumns = resolutionY / boxPixelsY; // Total columns on grid

// Initializing GameGrid.
int gameGrid[gameRows][gameColumns] = {};

// The following exist purely for readability.
const int x = 0; // X value of the sprite
const int y = 1; // Y value of the sprite
const int r = 0; // row number of the sprite.
const int c = 1; // column number of the sprite.
const int h = 1; // head of the centepede.
const int additionalData = 0; // additional data about the array
const int numSeg = 0; // number of the segments in a centepede
const int dir = 3; // direction of the segment
const int lastDir = 4; // revious direction of the centepede
const int exists = 2; // the sprite exist (only for bullet until now)
const int anim = 2; // animation for segment and head 
const int left = 0;
const int down = 3;
const int right = 2;
const int up = 1;

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// Write your functions declarations here. Some have been written for you. //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

void movePlayer(float player[], sf::Sprite& playerSprite, int direction); // moves the player in a specific direction
void drawPlayer(sf::RenderWindow& window, float player[], sf::Sprite& playerSprite); // draws the player in the game window
void moveBullet(float bullet[], sf::Clock& bulletClock); // moves the bullet
void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite); // draws the bullet
void generateMushrooms(); // generates mushrooms in the game grid
void drawMushrooms(sf::RenderWindow& window, sf::Sprite& mushroomSprite); // loops through the game grid and if mushroom found then draw it in the game window
void moveCentepedes(int*** centepedes); // moves the centepede
void drawCentepedes(int*** centepedes, sf::RenderWindow& window, sf::Sprite& headSprite, sf::Sprite& segmentSprite); // draws the centepede in the game window
int main() {
	int*** centepedes = new int** [2]; // 3d dynamic array. with 2 int arrays
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
	bgMusic.setVolume(0); // 50

	//  Bullet Sound effect.
	sf::SoundBuffer buffer;
	buffer.loadFromFile("Sound Effects/fire1.wav");
	sf::Sound bulletSound;
	bulletSound.setBuffer(buffer);

	// Initializing Background.
	sf::Texture backgroundTexture;
	sf::Sprite backgroundSprite;
	backgroundTexture.loadFromFile("Textures/background.png");
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setColor(sf::Color(255, 255, 255, 255 * 0.20)); // Reduces Opacity to 25%

	// Initializing the mushrooms
	generateMushrooms();

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
	int emptySpaces = 0;
	int startingX = 25 * boxPixelsX, startingY = 0;

	sf::Texture mushroomTexture;
	sf::Sprite mushroomSprite;
	mushroomTexture.loadFromFile("Textures/mushroom.png");
	mushroomSprite.setTexture(mushroomTexture);
	// mushroomSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

	sf::Texture headTexture;
	sf::Sprite headSprite;
	headTexture.loadFromFile("Textures/c_head_left_walk.png");
	headSprite.setTexture(headTexture);

	sf::Texture segmentTexture;
	sf::Sprite segmentSprite;
	segmentTexture.loadFromFile("Textures/c_body_left_walk.png");
	segmentSprite.setTexture(segmentTexture);
	segmentSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

	centepedes[additionalData] = new int* [1];
	centepedes[additionalData][0] = new int[1];
	centepedes[additionalData][0][0] = 1;

	centepedes[1] = new int* [13];
	centepedes[1][additionalData] = new int[1];
	centepedes[1][1] = new int[5];
	centepedes[1][additionalData][numSeg] = 12;
	centepedes[1][1][x] = startingX;
	centepedes[1][1][y] = startingY;
	centepedes[1][1][anim] = 0;
	centepedes[1][1][dir] = 0;
	centepedes[1][1][lastDir] = 0;


	for (int i = 2; i < 13; i++) {
		centepedes[1][i] = new int[5];
		centepedes[1][i][x] = centepedes[1][i - 1][0] + boxPixelsX;
		centepedes[1][i][y] = startingY;
		if (centepedes[1][i - 1][anim] == 7) {
			centepedes[1][i][anim] = 0;
		} else {
			centepedes[1][i][anim] = centepedes[1][i - 1][anim] + 1;
		}
		centepedes[1][i][dir] = 0;
		centepedes[1][i][lastDir] = 0;
	}

	while (window.isOpen()) {
		int framesPassed = 0;
		// if ((!framesPassed % 32)) {
		window.draw(backgroundSprite);
		drawMushrooms(window, mushroomSprite);
		// }
		moveCentepedes(centepedes);
		drawCentepedes(centepedes, window, headSprite, segmentSprite);
		if (bullet[exists] == true) {
			moveBullet(bullet, bulletClock);
			drawBullet(window, bullet, bulletSprite);
		}

		sf::Event e;
		while (window.pollEvent(e)) {

			switch (e.type) {

			case sf::Event::Closed:
				return 0;
				break;
			case sf::Event::KeyPressed:
				if (e.key.code == 6)
					generateMushrooms();
				if (e.key.code == 57) {
					if (bullet[exists]) {
						drawBullet(window, bullet, bulletSprite);
						moveBullet(bullet, bulletClock);
					} else {
						bulletSound.play();
						bullet[x] = player[x];
						bullet[y] = player[y] - boxPixelsY;
						bullet[exists] = true;
					}
				} else {
					movePlayer(player, playerSprite, e.key.code);
				}
				break;
			default:
				break;
			}
		}

		drawPlayer(window, player, playerSprite);
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

	if (gameGrid[bulletRow][bulletCol] == 2) {
		gameGrid[bulletRow][bulletCol] = 1;
		bullet[exists] = false;
	} else if (gameGrid[bulletRow][bulletCol] == 1) {
		gameGrid[bulletRow][bulletCol] = 0;
		bullet[exists] = false;
	}
}
void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite) {
	bulletSprite.setPosition(bullet[x], bullet[y]);
	window.draw(bulletSprite);
}
void movePlayer(float player[], sf::Sprite& playerSprite, int direction) {
	int row = (960 - player[y]) / boxPixelsY;
	int column = (player[x]) / boxPixelsX;

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
		if (row < 6)
			player[y] -= boxPixelsY;
		break;
	case 74:
		if (row > 2)
			player[y] += boxPixelsY;
		break;
	}
}
void generateMushrooms() {
	int mushrooms = 0;
	static int totalMushrooms = 20;

	for (int i = 0;i < 23;i++) {
		for (int j = 0;j < gameColumns;j++) {
			if (!(rand() % 28) && mushrooms <= totalMushrooms && gameGrid[i][j] != 1 && totalMushrooms && gameGrid[i][j] != 2) {
				gameGrid[i][j] = 2;
				mushrooms++;
			}
		}
	}
	// totalMushrooms += totalMushrooms * 0.2;
}
void drawMushrooms(sf::RenderWindow& window, sf::Sprite& mushroomSprite) {
	for (int i = 0; i < gameRows; i++) {
		for (int j = 0; j < gameColumns; j++) {
			if (gameGrid[i][j] == 2) {
				mushroomSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
				mushroomSprite.setPosition(j * boxPixelsX, i * boxPixelsY);
				window.draw(mushroomSprite);
			} else if (gameGrid[i][j] == 1) {
				mushroomSprite.setTextureRect(sf::IntRect(2 * boxPixelsY, 0, boxPixelsX, boxPixelsY));
				mushroomSprite.setPosition(j * boxPixelsX, i * boxPixelsY);

				window.draw(mushroomSprite);
			}
		}
	}
}
void drawCentepedes(int*** centepedes, sf::RenderWindow& window, sf::Sprite& headSprite, sf::Sprite& segmentSprite) {

	int numberOfCentepedes = centepedes[0][0][0];
	for (int centepede = 1; centepede <= numberOfCentepedes; centepede++) {
		int numberOfSegments = centepedes[centepede][additionalData][0];
		int headX = centepedes[centepede][h][x];
		int headY = centepedes[centepede][h][y];
		int animation = centepedes[centepede][h][anim];
		int direction = centepedes[centepede][h][dir];
		headSprite.setTextureRect(sf::IntRect(animation * boxPixelsX, 0, boxPixelsX, boxPixelsY));
		headSprite.setPosition(headX, headY);
		headSprite.setRotation(direction * 90);
		window.draw(headSprite);
		for (int segment = 2; segment <= numberOfSegments; segment++) {
			int segmentX = centepedes[centepede][segment][x];
			int segmentY = centepedes[centepede][segment][y];
			animation = centepedes[centepede][segment][anim];
			direction = centepedes[centepede][segment][dir];
			segmentSprite.setTextureRect(sf::IntRect(animation * boxPixelsX, 0, boxPixelsX, boxPixelsY));
			segmentSprite.setPosition(segmentX, segmentY);
			segmentSprite.setRotation(direction * 90);
			window.draw(segmentSprite);
		}
	}
}
void moveCentepedes(int*** centepedes) {
	static int numOfTimesCalled = 0;
	int numberOfCentepedes = centepedes[0][0][0];
	for (int centepede = 1; centepede <= numberOfCentepedes; centepede++) {
		int lastDirection = centepedes[centepede][h][lastDir];
		if (!(numOfTimesCalled % 10)) {
			if (centepedes[centepede][h][anim] == 7) {
				centepedes[centepede][h][anim] = 0;
			} else {
				centepedes[centepede][h][anim] = centepedes[1][1][2] + 1;
			}
		}
		// if (!(numOfTimesCalled % 32))cout << centepedes[centepede][h][x] << " ";
		if (!(numOfTimesCalled % 32) && centepedes[centepede][h][dir] == down) {
			centepedes[centepede][h][dir] = abs(lastDirection - 2);
			centepedes[centepede][h][lastDir] = abs(lastDirection - 2);
			if (centepedes[centepede][h][dir] == 2)
				centepedes[centepede][h][x] = centepedes[centepede][h][x] + 32;
			else
				centepedes[centepede][h][y] = centepedes[centepede][h][y] - 32;
		} else if (centepedes[centepede][h][x] == 0 && centepedes[centepede][h][dir] == 0) {
			centepedes[centepede][h][dir] = down;
			centepedes[centepede][h][y] = centepedes[centepede][h][y] + 32;
		} else if (centepedes[centepede][h][x] == 960 && centepedes[centepede][h][dir] == 2) {
			centepedes[centepede][h][dir] = down;
			centepedes[centepede][h][x] = centepedes[centepede][h][x] - 32;
		}
		if (centepedes[centepede][h][dir] % 2) {
			centepedes[centepede][h][y] = centepedes[centepede][h][y] + centepedes[centepede][h][dir] - 2;
		} else {
			centepedes[centepede][h][x] = centepedes[centepede][h][x] + centepedes[centepede][h][dir] - 1;
		}
		for (int segment = 2; segment < 13; segment++) {
			lastDirection = centepedes[centepede][segment][lastDir];
			if (!(numOfTimesCalled % 32) && centepedes[centepede][segment][dir] == down) {
				centepedes[centepede][segment][dir] = abs(lastDirection - 2);
				centepedes[centepede][segment][lastDir] = abs(lastDirection - 2);
				if (centepedes[centepede][segment][dir] == 2)
					centepedes[centepede][segment][x] = centepedes[centepede][segment][x] + 32;
				else
					centepedes[centepede][segment][y] = centepedes[centepede][segment][y] - 32;
			} else if (centepedes[centepede][segment][x] == 0 && centepedes[centepede][segment][dir] == 0) {
				centepedes[centepede][segment][dir] = down;
				centepedes[centepede][segment][y] = centepedes[centepede][segment][y] + 32;
			} else if (centepedes[centepede][segment][x] == 960 && centepedes[centepede][segment][dir] == 2) {
				centepedes[centepede][segment][dir] = down;
				centepedes[centepede][segment][x] = centepedes[centepede][segment][x] - 32;
			}
			if (centepedes[centepede][segment][dir] % 2) {
				centepedes[centepede][segment][y] = centepedes[centepede][segment][y] + centepedes[centepede][segment][dir] - 2;
			} else {
				centepedes[centepede][segment][x] = centepedes[centepede][segment][x] + centepedes[centepede][segment][dir] - 1;
			}
			if (centepedes[centepede][segment - 1][anim] == 7) {
				centepedes[centepede][segment][anim] = 0;
			} else {
				centepedes[centepede][segment][anim] = centepedes[centepede][segment - 1][anim] + 1;
			}
		}
	}
	numOfTimesCalled++;
}
//TODO: fps in player movement and bullet