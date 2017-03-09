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

void buildWorld();
void printGrid();

struct node{
    node*parent;
    node*next;
    node*prev;
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

float node::getG(){
    float Gxy=parent->g+1;//not sure if this should be parent's f or g cost
    return Gxy;
}

float node::getH(){
    float Hx,Hy;
    Hx=(x-Goal.x)*(x-Goal.x);
    Hy=(y-Goal.y)*(y-Goal.y);
    return sqrt(Hx+Hy)*10;
}

float node::getF(){
    return g+h;
}

void node::setG(float G){
    g=G;
}

void node::setH(float H){
    h=H;
}

void node::setF(float F){
    f=F;
}

static node GRID[10][10];

class listType{
public:
    listType();
    node *first;
    node*last;
    bool isEmpty();
    void pushNode(node*);
    bool has(node*);
    node* getFirst();
    void popNode(node*);
};

listType::listType(){
    first=NULL;last=NULL;
}

bool listType::isEmpty(){
    if(first==NULL){
        return true;
    }
}

void listType::pushNode(node *nptr){
    node*current;
    if(first==NULL){
        first=nptr;
        last=nptr;
        
    }else{
        for(current=first;current!=NULL;current=current->next){
            if(nptr->f<current->f){
                break;}
        }
        if(current==NULL){
            current=last;
            current->next=nptr;
            nptr->prev=current;
            last=nptr;
        }       
        else if(current==first){
            nptr->next=current;
            current->prev=nptr;
            first=nptr;
        }
        else{
            nptr->next=current;
            nptr->prev=current->prev;
            current->prev->next=nptr;
            current->prev=nptr;
        }
    }
}

void listType::popNode(node* thisnode){//removes first node from list and returns its ptr
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

node* listType::getFirst(){
    return first;
}//return ptr to first node

bool listType::has(node* thisnode){ 
    for(node*current=first;current!=NULL;current=current->next){
        if((current->x==thisnode->x)&&(current->y==thisnode->y)){
            return true;
        }
    }
    return false;
}

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

playerType::playerType(node start,node goal){
    openlist.pushNode(&Start);
}

float getDistance(){}

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

void playerType::walk(){
    standon=openlist.getFirst();
    standon->standing=true;
}

void playerType::findGoal(){
    float ff,gg,hh;
    cout<<"!!!!!Player Dropped into World!!!!!\n";
    printGrid();
    while(openlist.first!=NULL){
        walk();
        for(node * current=openlist.first;current!=NULL;current=current->next){
            cout<<"openglist: "<<current->x<<':'<<current->y<<'='<<current->f<<';';
        }
        for(node * current=standon;current!=NULL;current=current->parent){
            cout<<"path: "<<current->x<<':'<<current->y<<'='<<current->f<<';';
        }
        cout<<endl;
        gridbase[standon->x][standon->y]=8;
        cout<<".....walking\n";printGrid();
        if((standon->x==Goal.x)&&(standon->y==Goal.y)){
            cout<<"!!!!!Reached the Goal, Mofo!!!!!\n";
            printGrid();
            printPath();
            break;
        }
        //generate list of pointers to surrounding nodes
        getAdjacents();
        cout<<".....Looking Around\n";
        for(int k=0;k<4;++k){//evaluate all nodes in that list
            gg=standon->g+1;
            hh=adj[k]->getH();
            ff=gg+hh;
            if(adj[k]->solid==true){//if node is a wall do nothing
                cout<<"!!No,Thats a Wall!!"<<adj[k]->x<<','<<adj[k]->y<<endl;
            }
            else if(closedlist.has(adj[k])){//if node is in closed list check if current g is lower
                if(adj[k]->g>gg){
                    adj[k]->g=gg;
                    adj[k]->f=ff;
                    adj[k]->parent=standon;
                }
            }else if(openlist.has(adj[k])){//if node is in openlist check if current g is lower
                if(adj[k]->g>gg){
                    adj[k]->g=gg;
                    adj[k]->f=ff;
                    adj[k]->parent=standon;
                    openlist.popNode(adj[k]);
                    closedlist.pushNode(adj[k]);
                }
            }else{
                cout<<"!!Havent been here yet!!"<<adj[k]->x<<','<<adj[k]->y<<endl;
                //unchecked node set parent to current and get fcost
                adj[k]->parent=standon;
                //cout<<"check";
                cout<<"g:"<<gg<<' ';
                cout<<"h:"<<hh<<' ';
                cout<<"f:"<<ff<<endl;
                adj[k]->setG(gg);
                adj[k]->setH(hh);
                adj[k]->setF(ff);
                openlist.pushNode(adj[k]);
            }
        }//end of for statement
        if((standon->x==Goal.x)&&(standon->y==Goal.y)){
            break;
        }
        openlist.popNode(standon);
        closedlist.pushNode(standon);
        standon->standing=false;
    }
}

void playerType::printPath(){}

void setstartfinish(int,int,int,int);

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

void setstartfinish(int sx,int sy,int ex,int ey){
    Start=GRID[sx][sy];
    cout<<"Calculating Start.....completed\n"; 
    Goal=GRID[ex][ey];
    gridbase[sx][sy]=8;
    gridbase[ex][ey]=9;
    cout<<"Calculating Goal.....completed\n";
}

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
