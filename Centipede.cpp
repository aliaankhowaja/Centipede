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
// const int x = 0; // X value of the sprite
// const int y = 1; // Y value of the sprite
// const int r = 0; // row number of the sprite.
// const int c = 1; // column number of the sprite.
// const int h = 1; // head of the centepede.
// const int additionalData = 0; // additional data about the array
// const int numSeg = 0; // number of the segments in a centepede
// // const int centepedeIndex = 1; // index of centepede
// const int dir = 3; // direction of the segment
// const int lastDir = 4; // revious direction of the centepede
// // const int segmentIndex = 5;
// const int exists = 2; // the sprite exist (only for bullet until now)
// const int animationLocation = 2; // animation for segment and head 

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

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// Write your functions declarations here. Some have been written for you. //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

void movePlayer(float player[], sf::Sprite& playerSprite, int direction); // moves the player in a specific direction
void drawPlayer(sf::RenderWindow& window, float player[], sf::Sprite& playerSprite); // draws the player in the game window
void moveBullet(float bullet[], sf::Clock& bulletClock); // moves the bullet
void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite); // draws the bullet
int* generateMushrooms(); // generates mushrooms in the game grid
void drawMushrooms(sf::RenderWindow& window, sf::Sprite& mushroomSprite); // loops through the game grid and if mushroom found then draw it in the game window
void moveCentepedes(int centepedes[12][7]); // moves the centepede
void drawCentepedes(int centepedes[12][7], sf::RenderWindow& window, sf::Sprite& headSprite, sf::Sprite& segmentSprite); // draws the centepede in the game window
bool mushroomExist(int x, int y);
int checkMushroomCollision(int row, int col);
void checkBulletHit(int centepedes[12][7], float bullet[]);
void fragmentCentepede(int centepedes[12][7], int hitSegment);
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
	bgMusic.setVolume(50); // 50

	//  Bullet Sound effect.
	sf::SoundBuffer buffer;
	buffer.loadFromFile("Sound Effects/fire1.wav");
	sf::Sound bulletSound;
	bulletSound.setBuffer(buffer);

	// Initializing Background.
	sf::Texture backgroundTexture;
	sf::Sprite backgroundSprite;
	backgroundTexture.loadFromFile("Textures/background.jpg");
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setColor(sf::Color(255, 255, 255, 255 * 0.5)); // Reduces Opacity to 50%


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
	// segmentSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
	// segmentSprite.setColor(sf::Color(255, 128, 255, 255));// code from chatgpt

	// I tried using triple pointer array for the following task but had lots of memory errors
	int centepedes[12][7]; // centepedes data array. holds the data of each centepede and its segment.
	// centepedes[additionalData] = new int* [1];
	// centepedes[additionalData][0] = new int[1];
	// centepedes[additionalData][0][0] = 1;

	// centepedes[1] = new int* [13];
	// centepedes[1][additionalData] = new int[1];
	// centepedes[1][1] = new int[5];
	// centepedes[1][additionalData][numSeg] = 12;
	// centepedes[1][1][x] = startingX;
	// centepedes[1][1][y] = startingY;
	// centepedes[1][1][animationLocation] = 0;
	// centepedes[1][1][dir] = 0;
	// centepedes[1][1][lastDir] = 0;
	// centepedes[1][1][segmentIndex] = 1;
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
		// 	centepedes[1][i] = new int[5];
		// centepedes[1][i][x] = centepedes[1][i - 1][0] + boxPixelsX;
		// centepedes[1][i][y] = startingY;
		// if (centepedes[1][i - 1][animationLocation] == 7) {
		// 	centepedes[1][i][animationLocation] = 0;
		// } else {
		// 	centepedes[1][i][animationLocation] = centepedes[1][i - 1][animationLocation] + 1;
		// }
	// centepedes[1][i][segmentIndex] = i;
	}
	int* hitDetails;
	while (window.isOpen()) {
		int framesPassed = 0;
		// if ((!framesPassed % 32)) {
		window.draw(backgroundSprite);
		drawMushrooms(window, mushroomSprite);
		// }
		moveCentepedes(centepedes);
		drawCentepedes(centepedes, window, headSprite, segmentSprite);
		// if (!(framesPassed % 16)) {
		if (bullet[exists] == true) {
			moveBullet(bullet, bulletClock);
			drawBullet(window, bullet, bulletSprite);
			int bulletRow = bullet[y] / boxPixelsY;
			int bulletCol = bullet[x] / boxPixelsX;
			bulletRow--;
			checkBulletHit(centepedes, bullet);
		}
		// }

		sf::Event e;
		while (window.pollEvent(e)) {

			switch (e.type) {

			case sf::Event::Closed:
				return 0;
				break;
			case sf::Event::KeyPressed:
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

	if (mushroomExist(bulletRow, bulletCol)) {
		gameGrid[bulletRow][bulletCol] = checkMushroomCollision(bulletRow, bulletCol);
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
		if (row < 5)
			player[y] -= boxPixelsY;
		break;
	case 74:
		if (row > 1)
			player[y] += boxPixelsY;
		break;
	}
}
int* generateMushrooms() {
	int* startingLoc = new int[2];
	startingLoc[0] = rand() % (gameRows - 18);
	startingLoc[1] = rand() % (gameColumns - 13);
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

			// if (gameGrid[i][j] == 2) {
			mushroomSprite.setTextureRect(sf::IntRect(x * boxPixelsX, y * boxPixelsY, boxPixelsX, boxPixelsY));
			mushroomSprite.setPosition(j * boxPixelsX, i * boxPixelsY);
			window.draw(mushroomSprite);
			// } else if (gameGrid[i][j] == 1) {
			// 	mushroomSprite.setTextureRect(sf::IntRect(2 * boxPixelsY, 0, boxPixelsX, boxPixelsY));
			// 	mushroomSprite.setPosition(j * boxPixelsX, i * boxPixelsY);
			// 	window.draw(mushroomSprite);
			// }
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
void moveCentepedes(int centepedes[12][7]) {
	static int numOfTimesCalled = 0;
	// int numberOfCentepedes = centepedes[0][0][0];
	int row, col, lastDirection;
	// for (int segment = 1; centepede <= numberOfCentepedes; centepede++) {
		// int lastDirection = centepedes[centepede][h][lastDir];
		// int row = (centepedes[centepede][h][y]) / boxPixelsY;
		// int col = centepedes[centepede][h][x] / boxPixelsX;
		// if (centepedes[centepede][h][dir]) row--;
		// if (!centepedes[centepede][h][dir])col--;
		// if (!(numOfTimesCalled % 10)) {
		// 	if (centepedes[centepede][h][animationLocation] == 7) {
		// 		centepedes[centepede][h][animationLocation] = 0;
		// 	} else {
		// 		centepedes[centepede][h][animationLocation] = centepedes[1][1][2] + 1;
		// 	}
		// }
		// if (!(numOfTimesCalled % 16) && centepedes[centepede][h][dir] == down) {
		// 	centepedes[centepede][h][dir] = abs(lastDirection - 2);
		// 	centepedes[centepede][h][lastDir] = abs(lastDirection - 2);
		// 	if (centepedes[centepede][h][dir] == 2)
		// 		centepedes[centepede][h][x] = centepedes[centepede][h][x] + boxPixelsX;
		// 	else
		// 		centepedes[centepede][h][y] = centepedes[centepede][h][y] - boxPixelsY;
		// } else if (!(numOfTimesCalled % 16) && (centepedes[centepede][h][x] == 0 || mushroomExist(row, col)) && centepedes[centepede][h][dir] == 0) {
		// 	centepedes[centepede][h][dir] = down;
		// 	centepedes[centepede][h][y] = centepedes[centepede][h][y] + boxPixelsY;
		// } else if (!(numOfTimesCalled % 16) && (centepedes[centepede][h][x] == 960 || mushroomExist(row, col)) && centepedes[centepede][h][dir] == 2) {
		// 	centepedes[centepede][h][dir] = down;
		// 	centepedes[centepede][h][x] = centepedes[centepede][h][x] - boxPixelsX;
		// }
		// if (centepedes[centepede][h][dir] % 2) {
		// 	centepedes[centepede][h][y] = centepedes[centepede][h][y] + (centepedes[centepede][h][dir] - 2) * 2;
		// } else {
		// 	centepedes[centepede][h][x] = centepedes[centepede][h][x] + (centepedes[centepede][h][dir] - 1) * 2;
		// }
	for (int segment = 0; segment < 12; segment++) {
		lastDirection = centepedes[segment][lastDir];
		row = (centepedes[segment][y]) / boxPixelsY;
		col = centepedes[segment][x] / boxPixelsX;
		if (centepedes[segment][dir]) row--;
		if (!centepedes[segment][dir])col--;
		if (!(numOfTimesCalled % 16) && centepedes[segment][dir] == down) {
			centepedes[segment][dir] = abs(lastDirection - 2);
			centepedes[segment][lastDir] = abs(lastDirection - 2);
			if (centepedes[segment][dir] == 2)
				centepedes[segment][x] = centepedes[segment][x] + boxPixelsX;
			else
				centepedes[segment][y] = centepedes[segment][y] - boxPixelsY;
		} else if (!(numOfTimesCalled % 16) && (centepedes[segment][x] == 0 || mushroomExist(row, col)) && centepedes[segment][dir] == 0) {
			centepedes[segment][dir] = down;
			centepedes[segment][y] = centepedes[segment][y] + boxPixelsY;
			// if ((segment == 11 || !centepedes[segment + 1][exists]) && gameGrid[row][col])
			// 	gameGrid[row][col] = 0;
		} else if (!(numOfTimesCalled % 16) && (centepedes[segment][x] == 960 || mushroomExist(row, col)) && centepedes[segment][dir] == 2) {
			centepedes[segment][dir] = down;
			centepedes[segment][x] = centepedes[segment][x] - boxPixelsX;
			// if ((segment == 11 || !centepedes[segment + 1][exists]) && gameGrid[row][col])
			// 	gameGrid[row][col] = 0;
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
		// }
	}
	// if (!(numOfTimesCalled % 16)) checkBulletHit(centepedes);
	// if (!(numOfTimesCalled % 16))printGameGrid();
	// printCentepede(centepedes);
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
void checkBulletHit(int centepedes[12][7], float bullet[]) {
	int bulletRow = bullet[y] / boxPixelsY;
	int bulletCol = bullet[x] / boxPixelsX;
	int row, col;
	// bulletRow--;
	bool hit = false;
	// int* hitSegment = new int[2];
	int hitSegment;
	// hitDetails[0] = 0;
	// int numberOfCentepedes = centepedes[0][0][0];
	// for (int centepede = 1; centepede <= numberOfCentepedes; centepede++) {
	// int lastDirection = centepedes[h][lastDir];
	// int row = (centepedes[centepede][h][y]) / boxPixelsY;
	// int col = centepedes[centepede][h][x] / boxPixelsX;
	// if (centepedes[centepede][h][dir]) row--;
	// if (!centepedes[centepede][h][dir])col--;
	// if (col == bulletCol && row == bulletRow) {
		// cout << "headshot";
	// }
	for (int segment = 0; segment < 12; segment++) {
		if (centepedes[segment][exists]) {
			row = (centepedes[segment][y]) / boxPixelsY;
			col = centepedes[segment][x] / boxPixelsX;
			// if (centepedes[centepede][h][dir]) row--;
			// if (!centepedes[centepede][h][dir])col--;
			if (row == bulletRow && col == bulletCol) {
				hit = true;
				// *hitDetails = centepede;
				// *(hitDetails + 1) = segment;
				hitSegment = segment;

				bullet[exists] = false;
				// cout << "centepede: " << centepede << ", segment: " << segment << endl;
				break;
			}
		}
	}

	// }
	if (hit) {
		if (centepedes[hitSegment][dir] == 2) {
			row--;
		}
		if (centepedes[hitSegment][y] < 960 && centepedes[hitSegment][y] > 800)
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


	// for (int segment = hitSegment + 1;segment < 12;segment++) {
	// 	centepedes[segment][dir] = abs(centepedes[segment][dir] - 2);
	// }
	// int numOfCentepedes = centepedes[0][0][0];
	// int*** temp = new int** [++numOfCentepedes + 1];
	// temp[additionalData] = new int* [1];
	// temp[additionalData][0] = new int[1];
	// temp[additionalData][0][0] = numOfCentepedes;
	// cout << temp[additionalData][0][0] << endl;
	// for (int centepede = 1;centepede < numOfCentepedes;centepede++) {
	// 	cout << centepede << endl;
	// 	if (centepede == hitDetails[0]) {
	// 		cout << "yeah\n";
	// 	}
	// }
	// for (int centepede = 1; centepede < numOfCentepedes; centepede++) {
	// 	int segmentIndex = hitDetails[1];
	// 	if (centepede == hitDetails[0]) {
	// 		int numOfSegments = centepedes[centepede][additionalData][numSeg] - segmentIndex - 1;
	// 		temp[numOfCentepedes + 1] = new int* [numOfSegments + 1];
	// 		temp[numOfCentepedes + 1][additionalData] = new int[1];
	// 		temp[numOfCentepedes + 1][additionalData][0] = numOfSegments;
	// 		temp[numOfCentepedes + 1][h] = new int[5];
	// 		temp[numOfCentepedes + 1][h][x] = centepedes[centepede][segmentIndex + 1][x];
	// 		temp[numOfCentepedes + 1][h][y] = centepedes[centepede][segmentIndex + 1][y];
	// 		temp[numOfCentepedes + 1][h][animationLocation] = centepedes[centepede][segmentIndex + 1][animationLocation];
	// 		temp[numOfCentepedes + 1][h][dir] = centepedes[centepede][segmentIndex + 1][dir];
	// 		centepedes[centepede][segmentIndex + 1][dir];
	// 		temp[numOfCentepedes + 1][h][lastDir] = centepedes[centepede][segmentIndex + 1][lastDir];
	// 		for (int segment = 2; segment < numOfSegments; segment++) {
	// 			temp[numOfCentepedes + 1][segment] = new int[5];
	// 			temp[numOfCentepedes + 1][segment][x] = centepedes[centepede][segmentIndex + segment][x];
	// 			temp[numOfCentepedes + 1][segment][y] = centepedes[centepede][segmentIndex + segment][y];
	// 			temp[numOfCentepedes + 1][segment][animationLocation] = centepedes[centepede][segmentIndex + segment][animationLocation];
	// 			temp[numOfCentepedes + 1][segment][dir] = centepedes[centepede][segmentIndex + segment][dir];
	// 			temp[numOfCentepedes + 1][segment][lastDir] = centepedes[centepede][segmentIndex + segment][lastDir];
	// 		}
	// 		numOfSegments = centepedes[centepede][additionalData][numSeg] - numOfSegments - 2;
	// 		temp[centepede] = new int* [numOfSegments + 1];
	// 		temp[centepede][additionalData] = new int[1];
	// 		temp[centepede][additionalData][0] = numOfSegments;
	// 		cout << numOfSegments << temp[centepede][additionalData][0];
	// 		temp[centepede][h] = new int[5];
	// 		temp[centepede][h][x] = centepedes[centepede][1][x];
	// 		temp[centepede][h][y] = centepedes[centepede][1][y];
	// 		temp[centepede][h][animationLocation] = centepedes[centepede][1][animationLocation];
	// 		temp[centepede][h][dir] = centepedes[centepede][1][dir];
	// 		temp[centepede][h][lastDir] = centepedes[centepede][1][lastDir];
	// 		for (int segment = 2; segment <= numOfSegments; segment++) {
	// 			temp[centepede][segment] = new int[5];
	// 			temp[centepede][segment][x] = centepedes[centepede][segment][x];
	// 			temp[centepede][segment][y] = centepedes[centepede][segment][y];
	// 			temp[centepede][segment][animationLocation] = centepedes[centepede][segment][animationLocation];
	// 			temp[centepede][segment][dir] = centepedes[centepede][segment][dir];
	// 			// centepedes[centepede][segment][dir];
	// 			temp[centepede][segment][lastDir] = centepedes[centepede][segment][lastDir];
	// 		}
	// 	} else {
	// 		int numOfSegments = centepedes[centepede][additionalData][numSeg];
	// 		temp[centepede] = new int* [numOfSegments + 1];
	// 		temp[centepede][additionalData];
	// 		temp[centepede][h] = new int[5];
	// 		temp[centepede][h][x] = centepedes[centepede][h][x];
	// 		temp[centepede][h][y] = centepedes[centepede][h][y];
	// 		temp[centepede][h][animationLocation] = centepedes[centepede][h][animationLocation];
	// 		temp[centepede][h][dir] = centepedes[centepede][h][dir];
	// 		temp[centepede][h][lastDir] = centepedes[centepede][h][lastDir];
	// 		// temp[centepede][h][segmentIndex] = centepedes[centepede][h][segmentIndex];
	// 		for (int i = 2; i < 13; i++) {
	// 			temp[centepede][i] = new int[5];
	// 			temp[centepede][i][x] = centepedes[centepede][i][x];
	// 			temp[centepede][i][y] = centepedes[centepede][i][y];
	// 			temp[1][i][animationLocation] = centepedes[centepede][i][animationLocation];
	// 			temp[1][i][dir] = centepedes[centepede][i][dir];
	// 			temp[1][i][lastDir] = centepedes[centepede][i][lastDir];
	// 		}
	// 	}
	// }
	// delete centepedes;
	// centepedes = new int** [numOfCentepedes + 2];
	// for (int i = 0; i < numOfCentepedes + 2;i++) {
	// 	*(centepedes + i) = *(temp + i);
	// }
	// cout << centepedes[0][0][0] << endl;
	// centepedes = temp;
	// printCentepede(temp);
}