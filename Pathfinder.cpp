#include <iostream>
// #include <Windows.h> -- replace with sdl
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#include "Grid.h"
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;

void readFromFile(const char *);
bool inList(const vector<Node*> &, pair<int,int>);
int findSmallestF(const vector<Node*> &);
vector<Node*> FindShortestPath(pair<int,int>, pair<int,int>, Grid&);
void get_text_and_rect(SDL_Renderer *, int, int, string,
        TTF_Font *, SDL_Texture **, SDL_Rect *, int);
void createWindow();
bool isPollingEvent();
void clearMemory();
void initTTF();

int rows = 0, columns = 0;
pair<int,int> startPos(0,0), endPos(0,0); // coordinates of form (row,column)
string gridString;

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Event windowEvent;
SDL_Texture **textures;
SDL_Rect *rects;
TTF_Font *font;


int main(){

    readFromFile("SourceFile.txt");
    columns =  gridString.size()/rows;

   /* cout << temp[temp.size()-1] << endl;
    cout << gridString.size() << endl;
    cout << "Columns: " << gridString.size()/rows << endl;
    cout << "Rows: " << rows << endl << endl;
   cout << startPos.first << "," <<  startPos.second << endl;
   cout << endPos.first << "," <<  endPos.second << endl;*/
    Grid matrix(rows,columns,gridString);

    textures = new SDL_Texture*[rows];
    rects = new SDL_Rect[rows];

    // initialising texture array
    for(int i=0; i<rows; i++){
        textures[i] = nullptr;
    }


    // Testing rerendering text
    char const *words[] = {"Hello", "beautiful \n majestic", "world", "How", "are", "you", "today?"};
    
    createWindow();
    initTTF();

    FindShortestPath(startPos, endPos, matrix);

    // populate texture and rect arrays
    // for(int i=0; i<rows; i++){
    //     if(i == 0){
    //         get_text_and_rect(renderer, 0, 0, "hello", font, &textures[i], &rects[i]);
    //     }
    //     else{
    //         get_text_and_rect(renderer, 0, rects[i-1].y + rects[i-1].h, "hello", font, &textures[i], &rects[i]);
    //     }
    // }


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
        // ???
        if((list[i]->getF() < smallestNode->getF()) || (list[i]->getF() < smallestNode->getF() && list[i]->getH() < smallestNode->getH())){
            smallestNode = list[i];
            Smallestindex = i;
        }
    }

    return Smallestindex;
}

vector<Node*> FindShortestPath(pair<int,int> startNode, pair<int,int> targetNode, Grid& grid){
    vector<Node*> openList, closedList;
    Node* current, *temp;
    bool isFound = false;

    temp = grid.getNode(startNode.first, startNode.second);
    openList.push_back(temp);

    while (isPollingEvent()) {

        if(!openList.empty() && !isFound){
            int minIndex;

            // find smallest F cost in open list
            minIndex = findSmallestF(openList);
            // cout << minIndex;

            current = openList[minIndex];
            openList.erase(openList.begin() + minIndex);
            closedList.push_back(current);
            current->setSymbol('.');

            if((current->getCoords().first == targetNode.first) && (current->getCoords().second == targetNode.second)){ //target is found
                isFound = true;
                continue;
            }

            //Process neighbours of current
            grid.addNeighbours(current);

            int i = 0;
            while(current->getNeighbour(i) != nullptr && i < 8)
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
        
        if(!isFound){
            cout << "No path found" << endl;}
        else{
            //traceback
            Node* temp = grid.getNode(targetNode.first,targetNode.second);
            while(temp->getCoords().first != startNode.first || temp->getCoords().second != startNode.second){
                temp->setSymbol('P');
                temp = temp->getParent();
            }
            temp->setSymbol('S');
        } 
        grid.drawGrid(textures,rects,font,renderer);
        // Sleep(5000);
        // return openList;

        ////// Rendering //////

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        for(int i=0; i<rows; i++){
            SDL_RenderCopy(renderer, textures[i], NULL, &rects[i]);
        }

        SDL_RenderPresent(renderer);
    }


}

void createWindow(){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_Window* win;

    SDL_CreateWindowAndRenderer(*(new int(1000)), *(new int(1000)), 0, &window, &renderer);
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

    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void initTTF(){
    const char *font_path;
    font_path = "CONSOLA.TTF";

    TTF_Init();
    font = TTF_OpenFont(font_path, 20);

    if (font == NULL) {
        cout << "error: font not found\n" << endl;
        exit(EXIT_FAILURE);
    }
}

void get_text_and_rect(SDL_Renderer *renderer, int x, int y, string text,
        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect, int rowIndex){
    int text_width;
    int text_height;
    static string * previousText = new string [rows];
    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255, 0};

    // if(rows > 0 && !(*texture)){
    //     for(int i=0; i<rows; i++){
    //         previousText[i] = "";
    //     }
    // }

    // cout << "a: " << previousText[rowIndex] << "b: " << text << " ";
    bool textDiffers = previousText[rowIndex] != text;
    if(textDiffers){
        cout << "HIT " << textDiffers << endl;

        surface = TTF_RenderText_Solid(font, text.c_str(), textColor);

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

