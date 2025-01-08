#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <utility>
#include <cmath>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

class Node{
    private:
        float G_cost, H_cost;
        pair<int,int> coords;
        bool isWall;
        string symbol;
        Node *parent;
        Node *neighbours[8];

    public:
        Node();
        Node(int,int);
        void setG(float gCost) {G_cost = gCost;}
        void setH(float hCost) {H_cost = hCost;}
        void setSymbol(char character) {symbol = character;}
        void setParent(Node* Parent){parent = Parent;}
        Node* getParent() const {return parent;}
        float getG() const {return G_cost;}
        float getH() const {return H_cost;}
        string getSymbol() const {return symbol;}
        pair<int, int> getCoords() {return make_pair(coords.first,coords.second);}
        bool blocked() const {return isWall;}
        float getF() const {return G_cost + H_cost;}
        Node* getNeighbour(int index){return neighbours[index];}
        ~Node();
        friend class Grid;
};


class Grid{

    private:
    string defaultGrid = "OOO"
                         "OOO"
                         "OOO";

    Node ***grid2D;
    int rowCount, colCount;

    public:
    Grid();
    Grid(int, int, string);
    float getDist(pair<int,int>, pair<int,int>);
    bool validIndex(int, int) const;
    Node* getNode(int, int);
    void drawGrid(SDL_Texture**, SDL_Rect*, TTF_Font*, SDL_Renderer*) const;
    void addNeighbours(Node*);
    ~Grid();

};

#endif