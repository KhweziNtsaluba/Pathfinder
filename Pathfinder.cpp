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

using namespace std;

bool inList(const vector<Node*> &, pair<int,int>);
int findSmallestF(const vector<Node*> &);
vector<Node*> FindShortestPath(pair<int,int>, pair<int,int>, Grid&);
void get_text_and_rect(SDL_Renderer *, int, int, const char *,
        TTF_Font *, SDL_Texture **, SDL_Rect *);
void createWindow();
bool isPollingEvent();
void clearMemory();
void initTTF();

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Event windowEvent;
SDL_Texture *texture1;
TTF_Font *font;


int main(){
//     int rows = 0, columns = 0;
//     string temp, gridString;
//     pair<int,int> startPos(0,0), endPos(0,0); // coordinates of form (row,column)
//     bool startFound = false, endFound = false;
//     ifstream inFile("SourceFile.txt");

//     while (!inFile.eof()){
//         getline(inFile, temp);
//         gridString.append(temp);

//         if(!startFound || !endFound){
//             for(int i = 0; i < temp.size(); i++){
//                 if(temp[i] == 'S'){ // only one starting position
//                     startPos.first = rows;
//                     startPos.second = i;
//                     startFound = true;
//                 }
//                 else if (temp[i] == 'E'){ // only one ending position
//                     endPos.first = rows;
//                     endPos.second = i;
//                     endFound = true;
//                 }
//             }
//         }
//         rows++;
//     }
//     columns =  gridString.size()/rows;
//    /* cout << temp[temp.size()-1] << endl;
//     cout << gridString.size() << endl;
//     cout << "Columns: " << gridString.size()/rows << endl;
//     cout << "Rows: " << rows << endl << endl;
//    cout << startPos.first << "," <<  startPos.second << endl;
//    cout << endPos.first << "," <<  endPos.second << endl;*/
//     Grid matrix(rows,columns,gridString);
//     FindShortestPath(startPos, endPos, matrix);

    SDL_Rect rect1, rect2;

    // Testing rerendering text
    char const *words[] = {"Hello", "beautiful \n majestic", "world", "How", "are", "you", "today?"};
    
    createWindow();
    initTTF();

    get_text_and_rect(renderer, 0, 0, "hello", font, &texture1, &rect1);
    // get_text_and_rect(renderer, 0, rect1.y + rect1.h, "world", font, &texture2, &rect2);

    int counter=0;


    while (isPollingEvent()) {

        if(counter < 7)
            get_text_and_rect(renderer, 0, 0, words[counter], font, &texture1, &rect1);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, texture1, NULL, &rect1);
        // SDL_RenderCopy(renderer, texture2, NULL, &rect2);

        SDL_RenderPresent(renderer);

        SDL_Delay(1000);
        counter++;
    }

    clearMemory();
 
    return 0;
}

bool inList(const vector<Node*> &list, pair<int,int> co_ordinates){
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
    int isleep = 0;

    temp = grid.getNode(startNode.first, startNode.second);
    openList.push_back(temp);

    while(!openList.empty() && !isFound){
        int minIndex;
        isleep++;

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
           // cout << i;
            Node* neighbourNode = current->getNeighbour(i++);
           // cout << i;
            if (neighbourNode->blocked() || inList(closedList, neighbourNode->getCoords()))
            {
                continue;
            }
           // cout << i;
            float newGcost = current->getG() + grid.getDist(current->getCoords(),neighbourNode->getCoords());

            if(newGcost < neighbourNode->getG() || !inList(openList, neighbourNode->getCoords())){
                neighbourNode->setG(newGcost);
                neighbourNode->setParent(current);
                
                if(!inList(openList, neighbourNode->getCoords())){
                    openList.push_back(neighbourNode);
                }

            }
            
        }

        // draw grid
        if(isleep % 20 == 0){
            system("CLS");
            grid.drawGrid();
        }
    }
    
    if(!isFound){
        cout << "No path found" << endl;}
    else{
        //traceback
        Node* temp = grid.getNode(targetNode.first,targetNode.second);
        while(temp->getCoords().first != startNode.first || temp->getCoords().second != startNode.second){
            //cout << "again";
            temp->setSymbol('P');
            temp = temp->getParent();
        }
        temp->setSymbol('S');
       cout << temp->getSymbol() << endl;
    } 
    system("clear");
    grid.drawGrid();
    // Sleep(5000);
    return openList;

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
    SDL_DestroyTexture(texture1);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void initTTF(){
    const char *font_path;
    font_path = "FreeSans.ttf";

    TTF_Init();
    font = TTF_OpenFont(font_path, 24);

    if (font == NULL) {
        cout << "error: font not found\n" << endl;
        exit(EXIT_FAILURE);
    }
}

void get_text_and_rect(SDL_Renderer *renderer, int x, int y, const char *text,
        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect){
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255, 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;

    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

