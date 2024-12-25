#include <iostream>
#include "Grid.h"
using namespace std;

void get_text_and_rect(SDL_Renderer *, int, int, string,
        TTF_Font *, SDL_Texture **, SDL_Rect *, int);

Node::Node() : Node(0,0)
{}

Node::Node(int rCoord,int cCoord){
    isWall = false;
    G_cost = 0;
    H_cost = 0;
    coords.first = rCoord; // Row
    coords.second = cCoord; //Column
    symbol = ' ';
    parent = nullptr;

    for(int i = 0; i<8 ; i++){
        neighbours[i] = nullptr;
    }
}

Node::~Node(){
    for(int i = 0; i<8 ; i++){
        neighbours[i] = nullptr;
    }
    parent = nullptr;
}

Grid::Grid() : Grid(3,3,defaultGrid)
{}

Grid::Grid(int ROWS, int COLS, string stringMap){
    colCount = COLS;
    rowCount = ROWS;
    grid2D = new Node **[ROWS];
    int arrIndex1D = 0;

    for(int i = 0; i < ROWS; i++){
        grid2D[i] = new Node *[COLS];
    }

    for(int i = 0; i < ROWS; i++){
        for (int j = 0; j < COLS; j++){
            grid2D[i][j] = new Node(i,j);
            if(stringMap[arrIndex1D] == '#'){
                grid2D[i][j]->isWall = true;
                grid2D[i][j]->symbol = '#';
            }
            arrIndex1D++;
        }
    }
}

float Grid::getDist(pair<int,int> coord1,pair<int,int> coord2){
    int r1 = coord1.first, r2 = coord2.first;
    int c1 = coord1.second, c2 = coord2.second;
    float distance;

    distance = sqrt(pow(c2-c1,2) + pow(r2-r1,2));
    return distance;
}

void Grid::drawGrid(SDL_Texture **textures, SDL_Rect *rects, TTF_Font *font, SDL_Renderer *renderer) const{
    for(int i = 0; i < rowCount; i++){
        string temp = "";
        for (int j = 0; j < colCount; j++){
           temp += grid2D[i][j]->symbol; 
        }

        if(i == 0){
            get_text_and_rect(renderer, 0, 0, temp, font, &textures[i], &rects[i], i);
        }
        else{
            get_text_and_rect(renderer, 0, rects[i-1].y + rects[i-1].h, temp, font, &textures[i], &rects[i], i);
        }
    }
}

bool Grid::validIndex(int row, int col) const{
    if(row >= 0 && row < rowCount && col >= 0 && col < colCount)
        return true;
    
  //  cout << "Invalid Index";
    return false;

}

Node* Grid::getNode(int row, int col) {
        if(validIndex(row,col) == true)
            return grid2D[row][col];
        cout << "Invalid Grid Co-ordinates";
        return nullptr;
}

void Grid::addNeighbours(Node* parentNode){
    int i = 0, row = -1;
    for(row = -1; row < 2; row++){
        for(int col = -1; col < 2; col++){
          //  cout << row << "" << col << endl;
            if(validIndex(parentNode->getCoords().first - row, parentNode->getCoords().second - col)
               && ( row !=0 || col != 0)){
               // cout << "itr " << i << endl;
                parentNode->neighbours[i] = grid2D[parentNode->getCoords().first - row][parentNode->getCoords().second - col];
               // parentNode->neighbours[i]->symbol = 'X';
                i++;
            }
        }
    }
}

Grid::~Grid(){
    for(int i = 0; i < rowCount; i++){
        for(int j = 0; j < colCount; j++){
            delete grid2D[i][j];
        }
        delete [] grid2D[i];
    }
    delete [] grid2D;
    grid2D = nullptr;
}
