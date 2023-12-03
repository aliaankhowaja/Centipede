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

void movePlayer(float player[], int direction, bool* playerDied); // moves the player
void drawPlayer(sf::RenderWindow& window, float player[], sf::Sprite& playerSprite); // draws the player in the game window
void moveBullet(float bullet[], sf::Clock& bulletClock, int* score); // moves the bullet
void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite); // draws the bullet
int* generateMushrooms(); // generates mushrooms in the game grid
void drawMushrooms(sf::RenderWindow& window, sf::Sprite& mushroomSprite); // loops through the game grid and if mushroom found then draw it in the game window
void moveCentepedes(float player[], int centepedes[12][7], bool* playerDied); // moves the centepede
void drawCentepedes(int centepedes[12][7], sf::RenderWindow& window, sf::Sprite& headSprite, sf::Sprite& segmentSprite); // draws the centepede in the game window
bool mushroomExist(int x, int y); // checks if mushroom exists in gamegrid at given location
int checkMushroomCollision(int row, int col); // returns mushroom destroy value
void checkBulletHit(int centepedes[12][7], float bullet[], sf::Sound& killSound, int* score); // sis the bullet hit the centepede
void fragmentCentepede(int centepedes[12][7], int hitSegment); // fragement the centepede
void die(sf::RenderWindow& window, sf::Sound& deathSound, sf::Sprite& playerSprite, int row, int col, bool* animationDone); // player dies

int main() {
	int score = 0;
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

	// Initializing font
	sf::Font roboto;
	roboto.loadFromFile("Roboto-Regular.ttf");

	// "Score" Text
	sf::Text text;
	text.setFont(roboto);
	text.setCharacterSize(40);
	text.setPosition(16, 16);
	text.setFillColor(sf::Color::White);
	text.setString("SCORE: ");
	text.setStyle(sf::Text::Bold);

	// Score text
	sf::Text scoreText;
	scoreText.setFont(roboto);
	scoreText.setCharacterSize(40);
	scoreText.setPosition(170, 16);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setString(to_string(score));
	scoreText.setStyle(sf::Text::Bold);

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

	// Initializing mushroom sprite
	sf::Texture mushroomTexture;
	sf::Sprite mushroomSprite;
	mushroomTexture.loadFromFile("Textures/mushroom.png");
	mushroomSprite.setTexture(mushroomTexture);
	// mushroomSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

	// intializing head sprite
	sf::Texture headTexture;
	sf::Sprite headSprite;
	headTexture.loadFromFile("Textures/c_head_left_walk.png");
	headSprite.setTexture(headTexture);
	// headSprite.setColor(sf::Color(255, 128, 255, 255)); // Code from ChatGPT

	//initializing segment sprite
	sf::Texture segmentTexture;
	sf::Sprite segmentSprite;
	segmentTexture.loadFromFile("Textures/c_body_left_walk.png");
	segmentSprite.setTexture(segmentTexture);
	// segmentSprite.setColor(sf::Color(255, 128, 255, 255));// code from chatgpt

	// I tried using triple pointer array for the following task but had lots of memory errors
	int centepedes[12][7]; // centepedes data array. holds the data of each centepede and its segment.
	for (int i = 0; i < 12; i++) { // loop till all the segments
		if (i == 0) { // if head
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
	bool playerDied = false, animationDone = false;
	while (window.isOpen()) {
		int framesPassed = 0;
		sf::Event e;
		window.draw(backgroundSprite);// draws the background sprite

		drawMushrooms(window, mushroomSprite); // draws mushrooms
		window.draw(text);
		scoreText.setString(to_string(score));
		window.draw(scoreText);
		drawCentepedes(centepedes, window, headSprite, segmentSprite); // draws centepede
		if (playerDied) { // if the player dies
			int row = (player[y]) / boxPixelsY; // row of the player in game grid
			int col = (player[x]) / boxPixelsX; // column of the player in game grid
			if (!animationDone) // player dying animation not done
				die(window, deathSound, playerSprite, row, col, &animationDone); // play the next frame of animation
		} else {
			moveCentepedes(player, centepedes, &playerDied);// move the centepede
			if (bullet[exists] == true) { // if bullet exists
				moveBullet(bullet, bulletClock, &score); // move the bullet
				drawBullet(window, bullet, bulletSprite); // draw the bullet
				int bulletRow = bullet[y] / boxPixelsY; // row of the bullet
				int bulletCol = bullet[x] / boxPixelsX; // column of the bullet
				bulletRow--;
				checkBulletHit(centepedes, bullet, killSound, &score);// checks if the bullet hit the centepede
			}
			while (window.pollEvent(e)) { // event mechainism from chatgpt
				if (e.type == sf::Event::Closed) return 0; // player closes the window
				if (e.type == sf::Event::KeyPressed) { // keyboard event
					if (e.key.code == 57) { // space bar pressed
						if (!(framesPassed % 16)) { //for smoothing purpose
							if (bullet[exists]) {
								// drawBullet(window, bullet, bulletSprite);
								// moveBullet(bullet, bulletClock);
							} else { // create a bullet
								bulletSound.play();
								bullet[x] = player[x];
								bullet[y] = player[y];
								bullet[exists] = true;
							}
						}
					} else {
						movePlayer(player, e.key.code, &playerDied);// move the player
					}
				}
			}
			drawPlayer(window, player, playerSprite); // draw the player
		}
		while (window.pollEvent(e)) {
			if (e.type == sf::Event::Closed) return 0; // player closes wondow
		}

		window.display();
		window.clear();
		framesPassed++;
		usleep(1000); // for smooth functionality
	}
}
void drawPlayer(sf::RenderWindow& window, float player[], sf::Sprite& playerSprite) {// draws the player in wondow
	playerSprite.setPosition(player[x], player[y]);
	window.draw(playerSprite);
}
void moveBullet(float bullet[], sf::Clock& bulletClock, int* score) {// moves the bullet
	if (bulletClock.getElapsedTime().asMilliseconds() < 20)
		return;
	bulletClock.restart();
	bullet[y] -= 32;
	int bulletCol = (int)bullet[x] / boxPixelsX;
	int bulletRow = (int)bullet[y] / boxPixelsY;
	bulletRow--;
	if (bullet[y] < -32)
		bullet[exists] = false;
	if (mushroomExist(bulletRow, bulletCol)) { // check if hit a mushroom
		gameGrid[bulletRow][bulletCol] = checkMushroomCollision(bulletRow, bulletCol);
		if (!gameGrid[bulletRow][bulletCol])*score = *score + 1;
		bullet[exists] = false;
	}
}
void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite) { // draw bullet
	bulletSprite.setPosition(bullet[x], bullet[y]);
	window.draw(bulletSprite);
}
void movePlayer(float player[], int direction, bool* playerDied) { // move player
	switch (direction) {
	case 71: // < key
		if (player[x] > 0)
			player[x] -= boxPixelsX;
		break;
	case 72: // > key
		if (player[x] < 928)
			player[x] += boxPixelsX;
		break;
	case 73: // ^ key
		if (player[y] > 800)
			player[y] -= boxPixelsY;
		break;
	case 74: // v key
		if (player[y] < 928)
			player[y] += boxPixelsY;
		break;
	}
	int row = (player[y]) / boxPixelsY;
	int col = (player[x]) / boxPixelsX;
	if (gameGrid[row][col])*playerDied = true; // if poisonous mushroom in player area
}
int* generateMushrooms() { // generates mushrooms in gamegrid
	int* startingLoc = new int[2]; // starting location pointer
	startingLoc[0] = rand() % (gameRows - 18); // starting row
	startingLoc[1] = rand() % (gameColumns - 13); // starting column
	int mushrooms = 0;
	static int totalMushrooms = rand() % 10 + 20;

	for (int i = 1;i < 23;i++) { // loop through 23 rows
		for (int j = 2;j < gameColumns - 2;j++) { // loop through columns
			if (!(rand() % 30) && mushrooms <= totalMushrooms && gameGrid[i][j] != 1 && gameGrid[i][j] != 2 && startingLoc[0] != i) {
				gameGrid[i][j] = 2; // create mushrooms randomly
				mushrooms++;
			}
		}
	}
	return startingLoc;
}
void drawMushrooms(sf::RenderWindow& window, sf::Sprite& mushroomSprite) { // draws mushroom
	int y, x; // for drawing poisnous and simple mushrooms
	for (int i = 0; i < gameRows; i++) {
		for (int j = 0; j < gameColumns; j++) {
			if (gameGrid[i][j] == 0)continue;
			y = (gameGrid[i][j] - 1) / 2; // differentiates between poisnous and simple mushroom
			x = gameGrid[i][j] % 2 * 2; // differentiated between full and half mushroom
			mushroomSprite.setTextureRect(sf::IntRect(x * boxPixelsX, y * boxPixelsY, boxPixelsX, boxPixelsY));
			mushroomSprite.setPosition(j * boxPixelsX, i * boxPixelsY);
			window.draw(mushroomSprite);
		}
	}
}
void drawCentepedes(int centepedes[12][7], sf::RenderWindow& window, sf::Sprite& headSprite, sf::Sprite& segmentSprite) {
	// draws the centepedes
	int segmentX, segmentY, segmentDirection, segmentAnimation; // vals for each segment
	for (int segment = 0; segment < 12; segment++) {
		segmentX = centepedes[segment][x];
		segmentY = centepedes[segment][y];
		segmentAnimation = centepedes[segment][animationLocation];
		segmentDirection = centepedes[segment][dir];
		if (centepedes[segment][exists]) { // if the segment exists
			if (centepedes[segment][isHead]) { // if the segment is a head
				// draw a headsprite
				headSprite.setTextureRect(sf::IntRect(segmentAnimation * boxPixelsX, 0, boxPixelsX, boxPixelsY));
				headSprite.setPosition(segmentX, segmentY);
				headSprite.setRotation(segmentDirection * 90);
				window.draw(headSprite);
			} else {
				//draw a segment sprite
				segmentSprite.setTextureRect(sf::IntRect(segmentAnimation * boxPixelsX, 0, boxPixelsX, boxPixelsY));
				segmentSprite.setPosition(segmentX, segmentY);
				segmentSprite.setRotation(segmentDirection * 90);
				window.draw(segmentSprite);
			}
		}
	}
}
void moveCentepedes(float player[], int centepedes[12][7], bool* playerDied) {
	static int numOfTimesCalled = 0; // number of times a function is called
	// static int playerRegionDir = 3;
	int row, col, lastDirection;
	for (int segment = 0; segment < 12; segment++) {
		lastDirection = centepedes[segment][lastDir];
		row = (centepedes[segment][y]) / boxPixelsY;
		col = centepedes[segment][x] / boxPixelsX;
		// dealing with problems with sprite rotaitions
		if (centepedes[segment][dir]) row--; // if moving right
		if (!centepedes[segment][dir]) col--; // moving left
		int l = (!centepedes[segment][dir]) ? 1 : 0;

		if (player[x] / boxPixelsX - l == col && player[y] / boxPixelsY == row) { // any segment touch the player
			*playerDied = true;
			break;
		}

		if (!(numOfTimesCalled % 16) && centepedes[segment][dir] == down) { // if full block moved but still in down direction
			centepedes[segment][dir] = abs(lastDirection - 2); // move in opposite direction to the direction it was moving before
			centepedes[segment][lastDir] = abs(lastDirection - 2);
			// following is for rotaition problems
			if (centepedes[segment][dir] == 2)
				centepedes[segment][x] = centepedes[segment][x] + boxPixelsX;
			else
				centepedes[segment][y] = centepedes[segment][y] - boxPixelsY;
		} else if (!(numOfTimesCalled % 16) && (centepedes[segment][x] == 0 || mushroomExist(row, col)) && centepedes[segment][dir] == 0) { // if a dead end i.e mushroom or wall and moving in left direction
			centepedes[segment][dir] = down;
			centepedes[segment][y] = centepedes[segment][y] + boxPixelsY; // for rotaition problems
		} else if (!(numOfTimesCalled % 16) && (centepedes[segment][x] == 960 || mushroomExist(row, col)) && centepedes[segment][dir] == 2) {// if a dead end i.e mushroom or wall and moving in right direction
			centepedes[segment][dir] = down;
			centepedes[segment][x] = centepedes[segment][x] - boxPixelsX; // for rotaition problems
		}
		if (centepedes[segment][dir] % 2) { // if direction == 1 or 3 i.e. up or down
			centepedes[segment][y] = centepedes[segment][y] + (centepedes[segment][dir] - 2) * 2; // add or subtract 2 from y value 
		} else {
			centepedes[segment][x] = centepedes[segment][x] + (centepedes[segment][dir] - 1) * 2; // add or subtract 2 from x value
		}

		if (!(numOfTimesCalled % 5))// for smothness
			//segment animation
			if (segment != 0) {
				if (centepedes[segment - 1][animationLocation] == 7) { // if last segment animation is 7 i.e. end animation
					centepedes[segment][animationLocation] = 0; //set it back to zero
				} else {
					centepedes[segment][animationLocation] = centepedes[segment - 1][animationLocation] + 1; // add 1 to previous segment animation
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
void checkBulletHit(int centepedes[12][7], float bullet[], sf::Sound& killSound, int* score) { // checks if the bullet hit centepede. passes the segment to the fragment centepede function
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
			row--; // rotaition problems
		}if (centepedes[hitSegment][isHead]) {
			*score = *score + 20;
		} else {
			*score = *score + 10;
		}

		if (centepedes[hitSegment][y] < 960 && (centepedes[hitSegment][y] > 800 || (centepedes[hitSegment][y] >= 800 && centepedes[hitSegment][dir] == 0)))
			gameGrid[row][col] = 4; // create poisnous mushroom
		else
			gameGrid[row][col] = 2; // create normal mushroom
		fragmentCentepede(centepedes, hitSegment);
	}
}
void fragmentCentepede(int centepedes[12][7], int hitSegment) {
	centepedes[hitSegment][exists] = false; // segment gets destroyed
	if (hitSegment != 11) // not the last segment
		if (centepedes[hitSegment + 1][exists]) centepedes[hitSegment + 1][isHead] = true; // if the next segment exists, make it a head
}
void die(sf::RenderWindow& window, sf::Sound& deathSound, sf::Sprite& playerSprite, int row, int col, bool* animationDone) { // does the dying animation
	static int numOfTimesCalled = 0;
	sf::Texture deathTexture;
	deathTexture.loadFromFile("Textures/death.png");
	playerSprite.setTexture(deathTexture);
	if (!numOfTimesCalled) deathSound.play(); // plays the sound on first call
	gameGrid[row][col] = 0;
	// animation nums
	int i = numOfTimesCalled / 8; // returns 0, 1
	int j = numOfTimesCalled % 8; // returns val between 0, 7
	playerSprite.setTextureRect(sf::IntRect(j * 32, i * boxPixelsY, boxPixelsX, boxPixelsY));
	playerSprite.setPosition(col * boxPixelsX, row * boxPixelsY);
	window.draw(playerSprite);
	numOfTimesCalled++;
	if (numOfTimesCalled == 16)*animationDone = true;// if 16 frames passed, stop animation
	usleep(40000);
}