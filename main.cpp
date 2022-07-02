#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>
#include <queue>
#include <vector>

const int WIDTH = 500;
const int HEIGHT = 500;

class Tile{
    private:
        sf::Vector2f pos;
        int width = 9;
        int height = 9;
        sf::Vector2i parentPos;//hold the previous tile that found this tile as its neighbor (in my head this works perfectly)
        
    public:
        sf::Color color = sf::Color::White;
        Tile() : pos(sf::Vector2f(0,0)){ }

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

        void setParent(int tx,int ty){
            this->parentPos.x = tx;
            this->parentPos.y = ty;
            
        }

        sf::Vector2i getParent(){
            return this->parentPos;
        }

        void setPos(int x, int y){
            pos = sf::Vector2f(x,y);
        }

        void setColor(sf::Color c){
            color = c;
        }
};

Tile grid[50][50];

std::queue<Tile> openSet;
std::queue<Tile>  closedSet;
std::queue<Tile> path;

bool placeStart = false;
bool placeEnd = false;

bool hasStart = false;
bool hasEnd = false;

bool canStart = false;
bool active = false;
bool found = false;
sf::Vector2i startPos;
sf::Vector2i endPos;

int steps = 0;

//prgram crashes at certain point of drawing too many walls


sf::Color open = sf::Color(145, 247, 131);
sf::Color closed = sf::Color(66, 126, 255);


//@return returns true if target tile is found
bool findSurroundingTile(int tileX,int tileY){

   std::vector<sf::Vector2i> sur;//all possible positions around the current pos
    sur.push_back(sf::Vector2i(tileX-1,tileY));
    sur.push_back(sf::Vector2i(tileX,tileY-1));
    sur.push_back(sf::Vector2i(tileX+1,tileY));
    sur.push_back(sf::Vector2i(tileX,tileY+1));

    for(int i = 0; i < sur.size(); i++){
        if(sur.at(i) == endPos){
            grid[endPos.x][endPos.y].setParent(tileX,tileY);
            return true;
        }
    }

    //check sides of each for green to not check there

    if((grid[tileX-1][tileY].color == sf::Color::White) && tileX - 1 >= 0){
        grid[tileX-1][tileY].color = open;
        grid[tileX-1][tileY].setParent(tileX,tileY);

        openSet.push(grid[tileX-1][tileY]);
    }
     if((grid[tileX+1][tileY].color == sf::Color::White) && tileX + 1 <= 49){
        grid[tileX+1][tileY].color = open;
        grid[tileX+1][tileY].setParent(tileX,tileY);
        openSet.push(grid[tileX+1][tileY]);
    }
    if((grid[tileX][tileY+1].color == sf::Color::White) && tileY + 1 <= 49){
        grid[tileX][tileY+1].color = open;
        grid[tileX][tileY+1].setParent(tileX,tileY);
        openSet.push(grid[tileX][tileY+1]);
    }
     if((grid[tileX][tileY-1].color == sf::Color::White) && tileY - 1 >= 0){
        grid[tileX][tileY-1].color = open;
        grid[tileX][tileY-1].setParent(tileX,tileY);
        openSet.push(grid[tileX][tileY-1]);
    }

    grid[tileX][tileY].color = closed;
    closedSet.push(grid[tileX][tileY]);

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
    window.setFramerateLimit(45);
    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        std::cout << "Couldn't load font" << std::endl;
    }

    auto reset = [](){
    placeStart = false;
     placeEnd = false;

     hasStart = false;
     hasEnd = false;

     canStart = false;
     active = false;
     found = false;

    steps = 0;

     startPos = sf::Vector2i(0,0);
     endPos = sf::Vector2i(0,0);

     while(!openSet.empty()){
        openSet.pop();
     }

     while(!closedSet.empty()){
        closedSet.pop();
     }

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

        if(hasStart && hasEnd){
            canStart = true;
        }

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
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
            else if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                //place a black tile there
                if(!active){
                sf::Vector2i mPos = sf::Mouse::getPosition(window);
                int x = mPos.x/10;
                    int y = mPos.y/10;
                //invert pos calculation to find tile pos
                if(placeStart && !hasStart){//place start
                    grid[x][y].setColor(sf::Color::Green);
                    startPos = sf::Vector2i(x,y);
                    placeStart = false;
                    hasStart = true;//prevent multiple start tiles
                    openSet.push(grid[x][y]);
                }else if(placeEnd && !hasEnd){//place end
                    grid[x][y].setColor(sf::Color::Red);
                    endPos = sf::Vector2i(x,y);
                    placeEnd = false;
                    hasEnd = true;
                }else{
                    if(hasStart && x == startPos.x && y == startPos.y) break;//prevent start from being covered
                    if(hasEnd && x == endPos.x && y == endPos.y) break;//prevent end from being covered by walls
                    
                    grid[x][y].setColor(sf::Color::Black);
                }
                }
                
            }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
                //next tile placed will be the starting tile green
                if(!hasStart && !placeStart){
                    placeStart = true;
                    placeEnd = false;
                }
            }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::T)){
                //next tile placed will be the target tile Red
                if(!hasEnd && !placeEnd){
                placeEnd = true;
                placeStart = false;
                }
            }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
                reset();
            }
            
            if(hasStart && hasEnd){//establish when the program is allowed to be started
                canStart = true;
            }
        }

        window.clear(sf::Color::Black);

      //draw the tiles every frame
        for (size_t i = 0; i < sizeof(grid)/sizeof(grid[0]); i++){
            for (size_t j = 0; j < sizeof(grid)/sizeof(grid[0]); j++) {
                window.draw(grid[i][j].Render());
            }
            
        }
        
        if(active && !found){
            steps++;//track the steps it takes to find the end so it can be reversed
            int curSize = openSet.size();
            for(int i = 0; i < openSet.size(); i++){
                if(findSurroundingTile(openSet.front().getX(),openSet.front().getY())){
                    active = false;
                    found = true;
                }
                openSet.pop();
            }
        }

        if(found){
            Tile* endTile = &grid[endPos.x][endPos.y];
            for(int i = 0; i < steps; i++){
                endTile->color = sf::Color::Red;
                endTile = &grid[endTile->getParent().x][endTile->getParent().y];
                if(endTile->getX() == startPos.x && endTile->getY() == startPos.y) break;
            }
            
        }
        window.display();
    }

    return 0;
}