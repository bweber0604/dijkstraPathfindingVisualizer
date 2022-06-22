#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>
#include <vector>

const int WIDTH = 500;
const int HEIGHT = 500;

class Tile{
    private:
        sf::Vector2f pos;
        int width = 9;
        int height = 9;

    public:
        sf::Color color = sf::Color::White;
        Tile() : pos(sf::Vector2f(0,0)){}

        Tile(int x, int y) : pos(sf::Vector2f(x,y)){}

        sf::RectangleShape Render(){
            sf::RectangleShape shape(sf::Vector2f(width,height));
            shape.setFillColor(color);
            shape.setPosition(sf::Vector2f((getX())*10,(getY())*10));

            return shape;
        }

        int getX(){
            return pos.x;
        }

        int getY(){
            return pos.y;
        }

        void setX(int x){
            pos.x = x;
        }
        void setY(int x){
            pos.y = x;
        }

        

        void setPos(int x, int y){
            pos = sf::Vector2f(x,y);
        }

        void setColor(sf::Color c){
            color = c;
        }
};

Tile grid[50][50];

std::vector<Tile> openSet;
std::vector<Tile>  closedSet;

bool placeStart = false;
bool placeEnd = false;

bool hasStart = false;
bool hasEnd = false;

bool canStart = true;
bool active = false;

sf::Vector2i startPos;
sf::Vector2i endPos;

//gfg link explanation: https://www.geeksforgeeks.org/a-search-algorithm/

//@return returns true if target tile is found
bool findSurroundingTile(int tileX,int tileY){
    /*
    surrounding tile positions y - 1, x - 1, x + 1, y + 1
    check if they are target
    check if target in call function
    dont return if:
        is wall
        is blue (already searched)
    */

   std::vector<sf::Vector2i> sur;//all possible positions around the current pos
    sur.push_back(sf::Vector2i(tileX-1,tileY));
    sur.push_back(sf::Vector2i(tileX,tileY-1));
    sur.push_back(sf::Vector2i(tileX+1,tileY));
    sur.push_back(sf::Vector2i(tileX,tileY+1));

   //if(tileX != startPos.x && tileY != startPos.y) grid[tileX][tileY].color = sf::Color::Blue;

    grid[tileX-1][tileY].color = sf::Color::Green;
    grid[tileX+1][tileY].color = sf::Color::Green;
    grid[tileX][tileY-1].color = sf::Color::Green;
    grid[tileX][tileY+1].color = sf::Color::Green;

    openSet.pop_back();

    openSet.push_back(grid[tileX-1][tileY]);
    openSet.push_back(grid[tileX+1][tileY]);
    openSet.push_back(grid[tileX][tileY-1]);
    openSet.push_back(grid[tileX][tileY+1]);

    // for(int i = 0; i < sur.size(); i++){
    //     grid[sur.at(i).x][sur.at(i).y].color = sf::Color::Blue;
    // }

//    if((tileX == endPos.x && tileY-1 == endPos.y) || (tileX-1 == endPos.x && tileY == endPos.y) || (tileX+1 == endPos.x && tileY == endPos.y) || (tileX == endPos.x && tileY+1 == endPos.y)){
//     return true;
//    }else if((grid[tileX-1][tileY].color != sf::Color::Blue) || (grid[tileX-1][tileY].color != sf::Color::Black)){
//     //add to open set
//     openSet.push_back(grid[tileX-1][tileY]);
//    }else if((grid[tileX][tileY-1].color != sf::Color::Blue) || (grid[tileX][tileY-1].color != sf::Color::Black)){
//     openSet.push_back(grid[tileX][tileY-1]);
//    }else if((grid[tileX+1][tileY].color != sf::Color::Blue) || (grid[tileX+1][tileY].color != sf::Color::Black)){
//     openSet.push_back(grid[tileX+1][tileY]);
//    }else if((grid[tileX][tileY+1].color != sf::Color::Blue) || (grid[tileX][tileY+1].color != sf::Color::Black)){
//     openSet.push_back(grid[tileX][tileY+1]);
//    }

     //remove(openSet.begin(),openSet.end(), grid[tileX][tileY]);
     
     //closedSet.push_back(grid[tileX][tileY]);
   return false;//default return value
}

int main()
{


    sf::Image icon;
    if (!icon.loadFromFile("icon.png"))
    {
        std::cout << "Error loading Icon" << std::endl;
    }

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "A* Pathfinding Visualization");
    //window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        std::cout << "Couldn't load font" << std::endl;
    }

    auto reset = [](){
     bool placeStart = false;
    bool placeEnd = false;

    bool hasStart = false;
    bool hasEnd = false;

    bool canStart = true;
    bool active = false;

     startPos = sf::Vector2i(0,0);
     endPos = sf::Vector2i(0,0);

     openSet.clear();
     closedSet.clear();

        for (size_t i = 0; i < sizeof(grid)/sizeof(grid[0]); i++)
        {
            for (size_t j = 0; j < sizeof(grid)/sizeof(grid[0]); j++)
            {
                grid[i][j].setPos(i,j);//set default grid position
                grid[i][j].setColor(sf::Color::White);
            }
        }
    };
    reset();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            { 
                if(hasStart && hasEnd && canStart){//only start if there can be a closed loop
                //start pathfinding alg here
                canStart = false;
                active = true;
                std::cout << "starting pathfinding" << std::endl;
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
            else if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                //place a black tile there
                sf::Vector2i mPos = sf::Mouse::getPosition(window);
                int x = mPos.x/10;
                    int y = mPos.y/10;
                //invert pos calculation to find tile pos
                if(placeStart && !hasStart){//place start
                    grid[x][y].setColor(sf::Color::Green);
                    endPos = sf::Vector2i(x,y);
                    placeStart = false;
                    hasStart = true;//prevent multiple start tiles
                    openSet.push_back(grid[x][y]);
                }else if(placeEnd && !hasEnd){//place end
                    grid[x][y].setColor(sf::Color::Red);
                    endPos = sf::Vector2i(x,y);
                    placeEnd = false;
                    hasEnd = true;
                }else{
                    grid[mPos.x/10][mPos.y/10].setColor(sf::Color::Black);
                }
            }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
                //next tile placed will be the starting tile green
                placeStart = true;
                placeEnd = false;
            }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::T)){
                //next tile placed will be the target tile Red
                placeEnd = true;
                placeStart = false;
            }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
                reset();
            }
            
           
        }

        window.clear(sf::Color::Black);

       //draw here
        for (size_t i = 0; i < sizeof(grid)/sizeof(grid[0]); i++)
        {
            for (size_t j = 0; j < sizeof(grid)/sizeof(grid[0]); j++)
            {
                window.draw(grid[i][j].Render());
            }
            
        }

    // if(active){//testing
    // std::cout << openSet.at(0).getX() << " " << openSet.at(0).getY() << std::endl;
    //     findSurroundingTile(openSet.at(0).getX(),openSet.at(0).getY());
    //     active = false;
    // }

        if(active){
            for(int i = 0; i < openSet.size(); i++){
                if(findSurroundingTile(openSet.at(i).getX(),openSet.at(i).getY())){
                    active = false;
                 
                }
        }
        }
        window.display();
    }

    return 0;
}