#include <cstdlib>
#include <cstdio>
#include <utility>
#include <iostream>
#include <string>
#include <cmath>
using namespace std;
#define RESET "\033[0m"
#define WHITE "\033[37m" 
#define PURPLE "\033[35m"
#define RED "\033[31m" 
#define WALL "\033[41m" 
#define GROUND "\033[47m"
#define STARTEND "\033[45m"

/*
** Program to find the path between 2 points in a grid using
** A* Pathfinding algorithm.
*/

// Pre-built grid, 1's represent walls, 0's represent passable space
int gridbase[10][10]={
    {1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1}
};

// Forward function declarations
void buildWorld();
void printGrid();

// Node used in A* directional graph
struct node{
    node* parent;
    node* next;
    node* prev;
    float getG();
    float getH();
    float getF();
    void setG(float);
    void setH(float);
    void setF(float);
    int x,y;
    float f,g,h;
    bool solid;
    bool standing;
};
node Start,Goal;

// Gets the G-value of the node,
// that is, the cost of the path from
// the start to the current node
float node::getG(){
    float Gxy=parent->g+1;//not sure if this should be parent's f or g cost
    return Gxy;
}

// Gets the H-value of the node,
// that is, the estimate of the pat from
// the current node to the goal
float node::getH(){
    float Hx,Hy;
    Hx=(x-Goal.x)*(x-Goal.x);
    Hy=(y-Goal.y)*(y-Goal.y);
    return sqrt(Hx+Hy)*10;
}

// Gets the F-value of the node,
// that is, the sum of the G and G values
float node::getF(){
    return g+h;
}

// Setters for G, H, and F values
void node::setG(float G){ g=G; }
void node::setH(float H){ h=H; }
void node::setF(float F){ f=F; }

// The directional graph of paths from the Start
static node GRID[10][10];

// A priority queue of nodes, with pointers to the first and last elements
class listType{
public:
    listType();
    node* first;
    node* last;
    bool isEmpty();
    void pushNode(node*);
    bool has(node*);
    node* getFirst();
    void popNode(node*);
};

// Constructs an empty listType object
listType::listType(){
    first=NULL;last=NULL;
}

// Returns if listType is empty or not
bool listType::isEmpty(){
    if(first==NULL)
        return true;
}

// Pushes a new node to listType
void listType::pushNode(node *nptr){
    node* current;
    // If list is empty, first and last element is the new node
    if(first==NULL){
        first=nptr;
        last=nptr;
        
    }else{ // If list is not empty
        // Traverse the list until new node has smalle F-value than an old node
        for(current=first; current!=NULL; current=current->next){
            if(nptr->f < current->f)
                break;
        }
        // If no such node exists
        // the new node is the last node in the queue
        if(current==NULL){
            current=last;
            current->next=nptr;
            nptr->prev=current;
            last=nptr;
        }
        // Else if the new node is the first node in the queue   
        else if(current==first){
            nptr->next=current;
            current->prev=nptr;
            first=nptr;
        }
        // Otherewise the new node is somewhere in the middle of the queue
        else{
            nptr->next=current;
            nptr->prev=current->prev;
            current->prev->next=nptr;
            current->prev=nptr;
        }
    }
}

// Returns and removes pointer to first node in queue
void listType::popNode(node* thisnode){
    if(first!=thisnode){
        if(last!=thisnode){
            thisnode->prev->next=thisnode->next;
            thisnode->next->prev=thisnode->prev;
        }
    }else if((last==thisnode)&&(first==thisnode)){
            first=NULL;
            last=NULL;
    }else if((last==thisnode)&&(first!=thisnode)){
        last=thisnode->prev;
        thisnode->prev->next=NULL;
    }else if((last!=thisnode)&&(first==thisnode)){
        first=thisnode->next;
        thisnode->next->prev=NULL;
    }
    thisnode->next=NULL;
    thisnode->prev=NULL;
}

// Get firs element in queue
node* listType::getFirst(){
    return first;
}

// Returns true if queue has a particular node
bool listType::has(node* thisnode){ 
    // Traverses queue until node is found or queue is exhausted
    for(node*current=first;current!=NULL;current=current->next){
        if((current->x==thisnode->x)&&(current->y==thisnode->y))
            return true;
    }
    return false;
}

// Class type for player object
class playerType{
public:
    playerType(node,node);
    listType closedlist;
    listType openlist;
    void walk();
    void findGoal();
    float getDistance();
    void printPath();
    void getAdjacents();
    node *standon;
    node *adj[4];
};

// Constructs playerType
playerType::playerType(node start,node goal){
    openlist.pushNode(&Start);
}

float getDistance(){}

// Gets adjacent nodes
void playerType::getAdjacents(){
    int xx=standon->x;
    int yy=standon->y;
    cout<<"standon: "<<xx<<yy<<endl;
    node* S=&GRID[xx+1][yy];
    adj[0]=S;
    node* W=&GRID[xx][yy-1];
    adj[1]=W;
    node* N=&GRID[xx-1][yy];
    adj[2]=N;
    node* E=&GRID[xx][yy+1];
    adj[3]=E; 
}

// Moves player to smallest F-value node
void playerType::walk(){
    standon=openlist.getFirst();
    standon->standing=true;
}

// Moves player from Start to Goal
void playerType::findGoal(){
    float ff,gg,hh;
    cout<<"!!!!!Player Dropped into World!!!!!\n";
    printGrid();
    // While there is a node to visit do
    while(openlist.first!=NULL){
        walk();

        // Print openlist
        for(node * current=openlist.first; current!=NULL; current=current->next)
            cout<<"openglist: "<<current->x<<':'<<current->y<<'='<<current->f<<';';

        // Print standon list
        for(node * current=standon;current!=NULL;current=current->parent)
            cout<<"path: "<<current->x<<':'<<current->y<<'='<<current->f<<';';

        cout<<endl;

        // Set standon node as 'on path'
        gridbase[standon->x][standon->y]=8;
        cout<<".....walking\n";printGrid();
        if((standon->x==Goal.x)&&(standon->y==Goal.y)){
            cout<<"!!!!!Reached the Goal, Mofo!!!!!\n";
            printGrid();
            printPath();
            break;
        }
        // Generate list of pointers to surrounding nodes
        getAdjacents();
        cout<<".....Looking Around\n";
        for(int k=0;k<4;++k){// Evaluate all nodes in that list
            gg=standon->g+1;
            hh=adj[k]->getH();
            ff=gg+hh;
            if(adj[k]->solid==true){// If node is a wall do nothing
                cout<<"!!No,Thats a Wall!!"<<adj[k]->x<<','<<adj[k]->y<<endl;
            }
            else if(closedlist.has(adj[k])){// If node is in closed list check if current g is lower
                if(adj[k]->g>gg){
                    adj[k]->g=gg;
                    adj[k]->f=ff;
                    adj[k]->parent=standon;
                }
            }else if(openlist.has(adj[k])){// If node is in openlist check if current g is lower
                if(adj[k]->g>gg){
                    adj[k]->g=gg;
                    adj[k]->f=ff;
                    adj[k]->parent=standon;
                    openlist.popNode(adj[k]);
                    closedlist.pushNode(adj[k]);
                }
            }else{
                cout<<"!!Havent been here yet!!"<<adj[k]->x<<','<<adj[k]->y<<endl;
                // Unchecked node set parent to current and get fcost
                adj[k]->parent=standon;
                cout<<"g:"<<gg<<' ';
                cout<<"h:"<<hh<<' ';
                cout<<"f:"<<ff<<endl;

                adj[k]->setG(gg);
                adj[k]->setH(hh);
                adj[k]->setF(ff);
                openlist.pushNode(adj[k]);
            }
        }// End of for statement
        if((standon->x==Goal.x)&&(standon->y==Goal.y))
            break;

        openlist.popNode(standon);
        closedlist.pushNode(standon);
        standon->standing=false;
    }
}

void playerType::printPath(){}

// Forward declaration
void setstartfinish(int,int,int,int);

// Program start point
int main(){
    printGrid();
    buildWorld();
    setstartfinish(4,4,2,8);
    playerType P(Start,Goal);
    cout<<"Creating Player.....completed\n";
    cout<<"Start Path Search.....\n";
    printGrid();
    P.findGoal();
    printGrid();
    return 0;
}

// Sets the start and finish locations
void setstartfinish(int sx,int sy,int ex,int ey){
    Start=GRID[sx][sy];
    cout<<"Calculating Start.....completed\n"; 
    Goal=GRID[ex][ey];
    gridbase[sx][sy]=8;
    gridbase[ex][ey]=9;
    cout<<"Calculating Goal.....completed\n";
}

// Initializes GRID
void buildWorld(){
    cout<<"Building World.....";
    for(int d=0;d<10;++d){
        for(int p=0;p<10;++p){
            GRID[d][p].solid=false;
            if(gridbase[d][p]==1){
                GRID[d][p].solid=true;    
            }
            GRID[d][p].x=d;
            GRID[d][p].y=p;
        }
    }
    cout<<"completed\n";
}

// Prints gridbase
void printGrid(){
    int space;
    cout<<" ------------------------------\n";
    for(int x=0;x<10;++x){
        cout<<'|';
        for(int y=0;y<10;++y){
            space=gridbase[x][y];
            switch(space){
                case 0:
                    cout<<GROUND<<WHITE;
                    break;
                case 1:
                    cout<<WALL<<RED;
                    break;
                case 8:
                case 9: 
                    cout<<RED<<STARTEND;                    
                    break;
                case 3:
                    cout<<PURPLE;
                    break;
                defaulet:
                    break;
            }
            cout<<'.'<<space<<'.'<<RESET;
        }
        cout<<"|\n";
    }
}
