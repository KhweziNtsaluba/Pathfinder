#include <iostream>
// #include <Windows.h> -- replace with sdl
#include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#include "Grid.h"
#include <vector>
#include <fstream>
#include <algorithm>

#define MAX_WINDOW_WIDTH 1000
#define MAX_WINDOW_HEIGHT 1000
#define PT_FONT_SIZE 20

using namespace std;

void readFromFile(const char *);
bool inList(const vector<Node*> &, pair<int,int>);
int findSmallestF(const vector<Node*> &);
void FindShortestPath(pair<int,int>, pair<int,int>, Grid&);
void get_text_and_rect(SDL_Renderer *, int, int, string,
        TTF_Font *, SDL_Texture **, SDL_Rect *, int);
void createWindow(int, int);
bool isPollingEvent();
void clearMemory();
void initTTF();

int rows = 0, columns = 0;
pair<int,int> startPos(0,0), endPos(0,0); // coordinates of form (row,column)
string gridString;
string * previousText;

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Event windowEvent;
SDL_Texture **textures;
SDL_Rect *rects;
TTF_Font *font;


int main(){

    readFromFile("SourceFile.txt");
    columns =  gridString.size()/rows;
    Grid matrix(rows,columns,gridString);

    previousText = new string [rows];
    textures = new SDL_Texture*[rows];
    rects = new SDL_Rect[rows];

    // initialising texture array
    for(int i=0; i<rows; i++){
        textures[i] = nullptr;
    }

    const float pt_to_px = 1.3;
    int windowWidth = (columns * PT_FONT_SIZE * pt_to_px)/2.35; // arbitrary offset
    int windowHeight = rows * PT_FONT_SIZE * pt_to_px; 

    cout << "WINDOW WIDTH: " << windowWidth << endl;
    cout << "WINDOW HEIGHT " << windowHeight << endl;
    createWindow(windowWidth, windowHeight);
    initTTF();

    
    FindShortestPath(startPos, endPos, matrix);


    clearMemory();
 
    return 0;
}

void readFromFile(const char * filename)
{
    string temp = "";
    bool startFound = false, endFound = false;

    ifstream inFile(filename);

    while (!inFile.eof()){
        getline(inFile, temp);
        gridString.append(temp);

        if(!startFound || !endFound){
            for(int i = 0; i < temp.size(); i++){
                if(temp[i] == 'S'){ // only one starting position
                    startPos.first = rows;
                    startPos.second = i;
                    startFound = true;
                }
                else if (temp[i] == 'E'){ // only one ending position
                    endPos.first = rows;
                    endPos.second = i;
                    endFound = true;
                }
            }
        }
        rows++;
    }

    // remove carriage return
    char ch = '\r';
    gridString.erase(remove(gridString.begin(), gridString.end(), ch), gridString.end());
}

bool inList(const vector<Node *> &list, pair<int, int> co_ordinates)
{
    for (Node* n : list){
        if((n->getCoords().first == co_ordinates.first) && (n->getCoords().second == co_ordinates.second)){
            return true;
        }
    }
    return false;
}

int findSmallestF(const vector<Node*> &list){
    int Smallestindex = 0;
    Node* smallestNode = list[0];

    for (int i = 0; i < list.size(); i++){
        if((list[i]->getF() < smallestNode->getF()) || (list[i]->getF() == smallestNode->getF() && list[i]->getH() < smallestNode->getH())){
            smallestNode = list[i];
            Smallestindex = i;
        }
    }

    return Smallestindex;
}

void FindShortestPath(pair<int,int> startNode, pair<int,int> targetNode, Grid& grid){
    vector<Node*> openList, closedList;
    Node* current, *temp;
    bool pathFound = false;
    bool done = false;

    temp = grid.getNode(startNode.first, startNode.second);
    openList.push_back(temp);

    while (isPollingEvent()) {
        if(!done){
            if(!openList.empty() && !pathFound){
                int minIndex;

                // find smallest F cost in open list
                minIndex = findSmallestF(openList);

                current = openList[minIndex];
                openList.erase(openList.begin() + minIndex);
                closedList.push_back(current);
                current->setSymbol('.');

                if((current->getCoords().first == targetNode.first) && (current->getCoords().second == targetNode.second)){ //target is found
                    pathFound = true;
                    continue;
                }

                //Process neighbours of current
                grid.addNeighbours(current);

                int i = 0;
                while(i < 8 && current->getNeighbour(i) != nullptr)
                {
                    Node* neighbourNode = current->getNeighbour(i++);
                    if (neighbourNode->blocked() || inList(closedList, neighbourNode->getCoords()))
                    {
                        continue;
                    }
                    float newGcost = current->getG() + grid.getDist(current->getCoords(),neighbourNode->getCoords());

                    if(newGcost < neighbourNode->getG() || !inList(openList, neighbourNode->getCoords())){
                        neighbourNode->setG(newGcost);
                        neighbourNode->setParent(current);
                        
                        if(!inList(openList, neighbourNode->getCoords())){
                            openList.push_back(neighbourNode);
                        }

                    }
                    
                }
            }
            else{
                done = true;
            }
            
            
            if(pathFound){
                //traceback
                Node* temp = grid.getNode(targetNode.first,targetNode.second);
                while(temp->getCoords().first != startNode.first || temp->getCoords().second != startNode.second){
                    temp->setSymbol('o');
                    temp = temp->getParent();
                }
                temp->setSymbol('S');
                done = true;
            } 
            grid.drawGrid(textures,rects,font,renderer);

            ////// Rendering //////

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            for(int i=0; i<rows; i++){
                SDL_RenderCopy(renderer, textures[i], NULL, &rects[i]);
            }

            SDL_RenderPresent(renderer);
        }
        else{
            break;
        }
    }

    if(!pathFound){
        SDL_InitSubSystem(SDL_INIT_VIDEO);
    
        if(SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "error", "no path was found!", window))
            cout << "ERROR: no path was found! " << "(" << SDL_GetError() << ")" << endl;
    }

    SDL_Delay(5000);
}

void createWindow(int width, int height){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    width = min(width,MAX_WINDOW_WIDTH);
    height = min(height,MAX_WINDOW_HEIGHT);

    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
}

bool isPollingEvent(){
    while (SDL_PollEvent(&windowEvent)) {
        switch (windowEvent.type) {
            case SDL_QUIT: return false;
        }
    }
    return true;
}

void clearMemory(){

    /* Deinit TTF. */
    for(int i=0; i<rows; i++){
        SDL_DestroyTexture(textures[i]);
    }

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    delete [] previousText;
    delete [] textures;
    delete [] rects;
}

void initTTF(){
    const char *font_path;
    font_path = "CONSOLA.TTF";

    TTF_Init();
    font = TTF_OpenFont(font_path, PT_FONT_SIZE);

    if (font == NULL) {
        cout << "error: font not found\n" << endl;
        exit(EXIT_FAILURE);
    }
}

void get_text_and_rect(SDL_Renderer *renderer, int x, int y, string text,
        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect, int rowIndex){
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255, 0};

    bool textDiffers = previousText[rowIndex] != text;
    if(textDiffers){
        surface = TTF_RenderUTF8_Solid(font, text.c_str(), textColor);

        if (*texture) {
            SDL_DestroyTexture(*texture);
        }

        *texture = SDL_CreateTextureFromSurface(renderer, surface);
        text_width = surface->w;
        text_height = surface->h;

        SDL_FreeSurface(surface);
        rect->x = x;
        rect->y = y;
        rect->w = text_width;
        rect->h = text_height;


        previousText[rowIndex] = text;
    }
}

