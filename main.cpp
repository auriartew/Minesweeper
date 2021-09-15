#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <vector>
#include <random>
#include <functional>

using namespace std;



struct Tile
{
    int row, col;
    int bombsNearby;
    bool hasBomb, shown, hasFlag;
    vector<Tile*> adjacentTiles;
    Tile()
    {
        row = 0;
        col = 0;
        hasBomb = false;
        shown = false;
        hasFlag = false;
    }
    void setPosition(int r, int c)
    {
        row = r;
        col = c;
    }

    void setBomb(bool bomb)
    {
        hasBomb = true;
    }

    void setBombsNearby(int numBombs)
    {
        bombsNearby = numBombs;
    }

    void showFlag(sf::Sprite& flagSprite, sf::Texture& flagText)
    {
        flagSprite.setTexture(flagText);
    }

    void removeFlag(sf::Sprite& flagSprite)
    {
        hasFlag = false;
        flagSprite.setTextureRect(sf::IntRect(1, 1, 0, 0));
        cout << "flag removed" << endl;
    }

    void addAdjacentTile(Tile* nearTile)
    {
        adjacentTiles.push_back(nearTile);
    }
    void RevealTile(sf::Sprite& tileSprite, sf::Texture& newTexture)
    {
        tileSprite.setTexture(newTexture);
        shown = true;
    }
    void RevealAdjTile(sf::Sprite tileSprite[16][25], sf::Texture* numTextures)
    {
        for (int i = 0; i < adjacentTiles.size(); ++i)
        {
            if (adjacentTiles.at(i)->bombsNearby == 0)
            {
                if (!adjacentTiles.at(i)->shown)
                {
                    adjacentTiles.at(i)->RevealTile(tileSprite[adjacentTiles.at(i)->row][adjacentTiles.at(i)->col], numTextures[8]);
                    adjacentTiles.at(i)->RevealAdjTile(tileSprite, numTextures);
                }
            }
            else {
                adjacentTiles.at(i)->RevealTile(tileSprite[adjacentTiles.at(i)->row][adjacentTiles.at(i)->col], numTextures[adjacentTiles.at(i)->bombsNearby - 1]);
            }
        }
    }
};

void changeSpriteTexture(sf::Sprite& spriteRef, sf::Texture& text)
{
    spriteRef.setTexture(text);
}

struct Board
{
    mt19937 random_mt;
    static const int row = 16, col = 25, mineCount = 50;
    bool gameIsOver = false;
    std::vector<int> mines;
    Tile boardTiles[row][col];
    Board() : mines(400)
    {
    }
    void createMines()
    {
        fill (mines.begin(), mines.begin()+mineCount,1);
        fill(mines.begin()+mineCount, mines.end(), 0);
        shuffle(mines.begin(), mines.end(), std::mt19937(std::random_device()()));
    }
    int Random(int min, int max)
    {
        uniform_int_distribution<int> dist(min, max);
        return dist(random_mt);
    }
    void createBoard(sf::Sprite tileSprite[row][col], sf::Texture& topTexture, sf::Sprite bombSprites[row][col], sf::Texture& bombs) {
        random_mt.seed(time(nullptr));
        createMines();
        vector<int>::iterator it=mines.begin();
        for (int c = 0; c < col; ++c) {
            for (int r = 0; r < row; ++r) {
                boardTiles[r][c].setPosition(r, c);
                tileSprite[r][c].setTexture(topTexture);
                tileSprite[r][c].setPosition(c*32, r*32);
                if (*it == 1) {
                    boardTiles[r][c].setBomb(true);
                    bombSprites[r][c].setPosition(c*32, r*32);
                    bombSprites[r][c].setTexture(bombs);
                }
                it++;
            }
        }
    }

    void gameOver(sf::Sprite bombSprite[row][col], sf::Texture& bombs, sf::Sprite topTiles[row][col])
    {
        //boardTiles[r][c].RevealTile(tileSprites[r][c], bombs);
        for (int c = 0; c < row; c++) {
            for (int r = 0; r < col; r++) {
                if (boardTiles[r][c].hasBomb) {
                    //topTiles[r][c].setColor(sf::Color(0, 0, 0, 0));
                }
            }
        }
    }

    void createNearbyTiles()
    {
        for (int c = 0; c < col; c++)
            for (int r = 0; r < row; r++)
            {
                int n = 0;
                if (c != 0)
                {
                    boardTiles[r][c].addAdjacentTile(&boardTiles[r][c-1]);
                    n += boardTiles[r][c-1].hasBomb;
                    if (r != 0)
                    {
                        n += boardTiles[r-1][c-1].hasBomb;
                        boardTiles[r][c].addAdjacentTile(&boardTiles[r-1][c-1]);
                    }
                    if ( r != 15)
                    {
                        boardTiles[r][c].addAdjacentTile(&boardTiles[r+1][c-1]);
                        n += boardTiles[r+1][c-1].hasBomb;
                    }
                }
                if (r != 0)
                {
                    boardTiles[r][c].addAdjacentTile(&boardTiles[r-1][c]);
                    n += boardTiles[r-1][c].hasBomb;

                    if (c != 24)
                    {
                        boardTiles[r][c].addAdjacentTile(&boardTiles[r-1][c+1]);
                        n += boardTiles[r-1][c+1].hasBomb;
                    }
                }
                if (r != 15)
                {
                    boardTiles[r][c].addAdjacentTile(&boardTiles[r+1][c]);
                    n += boardTiles[r+1][c].hasBomb;
                    if ( c != 24)
                    {
                        boardTiles[r][c].addAdjacentTile(&boardTiles[r+1][c+1]);
                        n += boardTiles[r+1][c+1].hasBomb;
                    }
                }
                if (c != 24)
                {
                    boardTiles[r][c].addAdjacentTile(&boardTiles[r][c+1]);
                    n += boardTiles[r][c+1].hasBomb;
                }
                boardTiles[r][c].setBombsNearby(n);
            }
    }

};


int main() {
    int pixels = 32;

    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");

    sf::Texture hiddenTile;
    hiddenTile.loadFromFile("images/tile_hidden.png");

    sf::Texture bombs;
    bombs.loadFromFile("images/mine.png");

    sf::Texture flag;
    flag.loadFromFile("images/flag.png");

    sf::Texture background;
    background.loadFromFile("images/tile_revealed.png");

    sf::Sprite backgroundS;
    backgroundS.setTexture(background);

    sf::Texture one, two, three, four, five, six, seven, eight;
    one.loadFromFile("images/number_1.png");
    two.loadFromFile("images/number_2.png");
    three.loadFromFile("images/number_3.png");
    four.loadFromFile("images/number_4.png");
    five.loadFromFile("images/number_5.png");
    six.loadFromFile("images/number_6.png");
    seven.loadFromFile("images/number_7.png");
    eight.loadFromFile("images/number_8.png");
    sf::Texture* numbers = new sf::Texture[9];
    numbers[0] = one;
    numbers[1] = two;
    numbers[2] = three;
    numbers[3] = four;
    numbers[4] = five;
    numbers[5] = six;
    numbers[6] = seven;
    numbers[7] = eight;
    numbers[8] = background;


    sf::Sprite tileSprites[16][25];
    sf::Sprite flagSprites[16][25];
    sf::Sprite bombSprites[16][25];
    Board ms;
    ms.createBoard(tileSprites, hiddenTile, bombSprites, bombs);
    ms.createNearbyTiles();



    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }


        for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 16; ++j) {
            backgroundS.setPosition(i*pixels, j*pixels);
            window.draw(backgroundS);
            flagSprites[j][i].setPosition(i*pixels, j*pixels);
            window.draw(bombSprites[j][i]);
            window.draw(flagSprites[j][i]);
            window.draw(tileSprites[j][i]);
            }
        }


        if (event.type == sf::Event::MouseButtonPressed)
        {
            int c = event.mouseButton.x / pixels;
            int r = event.mouseButton.y / pixels;
            if (event.mouseButton.button == sf::Mouse::Right) {
                switch (!ms.boardTiles[r][c].hasFlag) {
                    case true :
                        flagSprites[r][c].setTexture(flag);
                        ms.boardTiles[r][c].hasFlag = true;
                        ms.boardTiles[r][c].showFlag(flagSprites[r][c], flag);
                        cout << "add flag" << endl;
                        break;
                    case false :
                        flagSprites[r][c].setTexture(flag);
                        ms.boardTiles[r][c].hasFlag = false;
                        cout << "remove flag" << endl;
                        break;
                }
            }
            else if (event.mouseButton.button == sf::Mouse::Left)
            {
                if (ms.boardTiles[r][c].hasBomb)
                {
                    for (int col = 0; col < 25; col++) {
                        for (int row = 0; row < 16; row++) {
                            if (ms.boardTiles[row][col].hasBomb) {
                                window.draw(bombSprites[row][col]);
                                tileSprites[row][col].setColor(sf::Color(0, 0, 0, 0));
                            }
                        }
                    }
                    //ms.gameOver(bombSprites, bombs, tileSprites);
                }
                else if (!ms.boardTiles[r][c].hasBomb)
                {
                    int bombsNear = ms.boardTiles[r][c].bombsNearby;
                    cout << "left click" << endl;
                    if (bombsNear > 0)
                    {
                        ms.boardTiles[r][c].RevealTile(tileSprites[r][c], numbers[bombsNear - 1]);
                    } else {
                        ms.boardTiles[r][c].RevealTile(tileSprites[r][c], numbers[8]);
                        ms.boardTiles[r][c].RevealAdjTile(tileSprites, numbers);
                    }
                }
            }
        }
        window.display();
        window.clear();
    }

    return 0;
}
