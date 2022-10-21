#include <SFML/Graphics.hpp>
#include <iostream>
#include <queue>
#include <vector>

const int WIDTH = 500;
const int HEIGHT = 500;

class Tile
{
private:
    sf::Vector2f pos;
    int width = 9;
    int height = 9;
    sf::Vector2i parentPos; // hold the previous tile that found this tile as its neighbor (in my head this works perfectly)

public:
    sf::Color color = sf::Color::White;
    Tile() : pos(sf::Vector2f(0, 0)) {}
    Tile(int x, int y) : pos(sf::Vector2f(x, y)) {}

    sf::RectangleShape Render()
    {
        sf::RectangleShape shape(sf::Vector2f(width, height));
        shape.setFillColor(color);
        shape.setPosition(sf::Vector2f((getX()) * 10, (getY()) * 10));

        return shape;
    }

    int getX() { return pos.x; }

    int getY() { return pos.y; }

    void setX(int x) { pos.x = x; }
    void setY(int x) { pos.y = x; }

    void setParent(int tx, int ty)
    {
        this->parentPos.x = tx;
        this->parentPos.y = ty;
    }

    sf::Vector2i getParent() { return this->parentPos; }

    void setPos(int x, int y) { pos = sf::Vector2f(x, y); }

    void setColor(sf::Color c) { color = c; }
};

Tile grid[50][50];

std::queue<Tile> openSet;
std::queue<Tile> closedSet;

bool placeStart = false;
bool placeEnd = false;

bool hasStart = false;
bool hasEnd = false;

bool canStart = false;
bool active = false;
bool found = false;
bool helpOpen = false;

sf::Vector2i startPos;
sf::Vector2i endPos;

int steps = 0;

bool impossible = false;

sf::Color open = sf::Color(145, 247, 131);
sf::Color closed = sf::Color(66, 126, 255);

//@return returns true if target tile is found
bool findSurroundingTile(int tileX, int tileY)
{

    std::vector<sf::Vector2i> sur; // all possible positions around the current pos
    sur.push_back(sf::Vector2i(tileX - 1, tileY));
    sur.push_back(sf::Vector2i(tileX, tileY - 1));
    sur.push_back(sf::Vector2i(tileX + 1, tileY));
    sur.push_back(sf::Vector2i(tileX, tileY + 1));

    for (int i = 0; i < sur.size(); i++)
    {
        if (sur.at(i) == endPos)
        {
            grid[endPos.x][endPos.y].setParent(tileX, tileY);
            return true;
        }
        sf::Vector2i c = sur.at(i);
        if ((grid[c.x][c.y].color == sf::Color::White) && ((c.x >= 0 && c.x <= 49) && (c.y <= 49 && c.y >= 0)))
        {
            grid[c.x][c.y].color = open;
            grid[c.x][c.y].setParent(tileX, tileY);
            openSet.push(grid[c.x][c.y]);
        }
    }

    grid[tileX][tileY].color = closed;
    closedSet.push(grid[tileX][tileY]);

    return false; // default return value
}

int main()
{
    sf::Image icon;
    if (!icon.loadFromFile("icon.png"))
    {
        std::cout << "Error loading Icon" << std::endl;
    }

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Dijkstra Pathfinding Visualization");
    window.setFramerateLimit(60);
    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        std::cout << "Couldn't load font" << std::endl;
    }

    auto reset = []()
    {
        placeStart = false;
        placeEnd = false;

        hasStart = false;
        hasEnd = false;

        canStart = false;
        active = false;
        found = false;
        impossible = false;
        steps = 0;
        helpOpen = false;
        startPos = sf::Vector2i(0, 0);
        endPos = sf::Vector2i(0, 0);

        while (!openSet.empty())
        {
            openSet.pop();
        }

        while (!closedSet.empty())
        {
            closedSet.pop();
        }

        for (size_t i = 0; i < sizeof(grid) / sizeof(grid[0]); i++)
        {
            for (size_t j = 0; j < sizeof(grid) / sizeof(grid[0]); j++)
            {
                grid[i][j].setPos(i, j); // set default grid position
                grid[i][j].setColor(sf::Color::White);
            }
        }
    };
    reset();

    while (window.isOpen())
    {
        if (hasStart && hasEnd)
        {
            canStart = true;
        }

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                if (hasStart && hasEnd && canStart)
                { // only start if there can be a closed loop
                    // start pathfinding alg here
                    canStart = false;
                    active = true;
                    openSet.push(grid[startPos.x][startPos.y]);
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                // place a black tile there
                if (!active)
                {
                    sf::Vector2i mPos = sf::Mouse::getPosition(window);
                    int x = mPos.x / 10;
                    int y = mPos.y / 10;
                    //prevent attempts to draw off of the screen
                    if(x < 0) x = 0;
                    if(x > 49) x = 49;
                    if(y < 0) y = 0;
                    if(y > 49) y = 49;

                    // invert pos calculation to find tile pos
                    if (placeStart && !hasStart)
                    { // place start
                        grid[x][y].setColor(sf::Color::Green);
                        startPos = sf::Vector2i(x, y);
                        placeStart = false;
                        hasStart = true; // prevent multiple start tiles
                    }
                    else if (placeEnd && !hasEnd)
                    { // place end
                        grid[x][y].setColor(sf::Color::Red);
                        endPos = sf::Vector2i(x, y);
                        placeEnd = false;
                        hasEnd = true;
                    }
                    else
                    {
                        if (hasStart && x == startPos.x && y == startPos.y)
                            break; // prevent start from being covered
                        if (hasEnd && x == endPos.x && y == endPos.y)
                            break; // prevent end from being covered by walls
                            
                        grid[x][y].setColor(sf::Color::Black);
                    }
                }
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
            {
                // erase tile
                if (!active)
                {
                    sf::Vector2i mPos = sf::Mouse::getPosition(window);
                    int x = mPos.x / 10;
                    int y = mPos.y / 10;

                    if (grid[x][y].color == sf::Color::Green)
                        hasStart = false;
                    if (grid[x][y].color == sf::Color::Red)
                        hasEnd = false;

                    grid[x][y].setColor(sf::Color::White);
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                // next tile placed will be the starting tile green
                if (!hasStart && !placeStart)
                {
                    placeStart = true;
                    placeEnd = false;
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            {
                // next tile placed will be the target tile Red
                if (!hasEnd && !placeEnd)
                {
                    placeEnd = true;
                    placeStart = false;
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
            {
                reset();
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::H))
            {
                // show a new menu with the controls to the program
                helpOpen = true;
            }

            if (hasStart && hasEnd)
            { // establish when the program is allowed to be started
                canStart = true;
            }
        }

        window.clear(sf::Color::Black);

        // draw the tiles every frame
        for (size_t i = 0; i < sizeof(grid) / sizeof(grid[0]); i++)
        {
            for (size_t j = 0; j < sizeof(grid) / sizeof(grid[0]); j++)
            {
                window.draw(grid[i][j].Render());
            }
        }
        sf::RectangleShape background(sf::Vector2f(300, 100));
        background.setFillColor(sf::Color(148, 148, 146, 240));
        background.setPosition(WIDTH / 2 - 150, HEIGHT / 2 - 130);

        sf::Text noPathText("No Possible Path", font, 25);
        noPathText.setPosition(WIDTH / 2 - 100, HEIGHT / 2 - 100);
        noPathText.setFillColor(sf::Color::Red);

        sf::RectangleShape helpBackground(sf::Vector2f(250, 25));
        helpBackground.setFillColor(sf::Color(148, 148, 146, 200));
        window.draw(helpBackground);

        // help text
        sf::Text helpText("How to use the Program: H", font, 20);
        helpText.setPosition(0, 0);
        helpText.setFillColor(sf::Color::Black);
        window.draw(helpText);

        sf::RectangleShape helpTextBG(sf::Vector2f(450, 450));
        helpTextBG.setFillColor(sf::Color(148, 148, 146, 250));
        helpTextBG.setPosition(25, 25);

        sf::Text instructions("Controls: \n Left Click to Draw with the mouse \n and create barriers \n \n S: the next tile that is placed will be the start tile  \n E: the next tile that is placed will be the end tile \n R: reset all parts of program \n \n \n Press R to close this menu", font, 20);
        instructions.setPosition(35, 35);
        instructions.setFillColor(sf::Color::Black);

        if (impossible)
        {
            window.draw(background);
            window.draw(noPathText);
        }
        else if (helpOpen)
        {
            window.draw(helpTextBG);
            window.draw(instructions);
        }

        if (active && !found)
        {
            if (openSet.empty())
            {
                active = false;
                impossible = true;
            }

            steps++; // track the steps it takes to find the end so it can be reversed
            int curSize = openSet.size();
            for (int i = 0; i < openSet.size(); i++)
            {

                if (findSurroundingTile(openSet.front().getX(), openSet.front().getY()))
                {
                    active = false;
                    found = true;
                }
                openSet.pop();
            }
        }

        if (found)
        {
            Tile *endTile = &grid[endPos.x][endPos.y];
            for (int i = 0; i < steps + 5; i++)
            {
                endTile->color = sf::Color::Red;
                if (endTile->getX() == startPos.x && endTile->getY() == startPos.y)
                    break;
                endTile = &grid[endTile->getParent().x][endTile->getParent().y];
            }
        }
        window.display();
    }
    return 0;
}