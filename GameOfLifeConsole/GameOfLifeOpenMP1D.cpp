#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace std;
int n;
time_t t;


const int grid_h=30,grid_w=30;
int grid_1[grid_h][grid_w],grid_2[grid_h][grid_w];
bool first=true;
int mov_x[8]={-1,-1,0,1,1,1,0,-1},mov_y[8]={0,-1,-1,-1,0,1,1,1};
const int init_live_no=15;
int init_live[2][init_live_no]={{0,1,2,1,5 ,10,11,12,11,15 ,20,21,22,21,25},{1,0,1,2,1 ,11,10,11,12,11 ,21,20,21,22,21}};

int get_random(){
   return rand()%2;
}


void init(){
    int i,j;
    for(i=0;i<grid_w;i++)
        for(j=0;j<grid_h;j++){
            if(first)
                grid_1[i][j]=0;
            else grid_2[i][j]=0;}
    for(i=0;i<init_live_no;i++){
        if(first)
            grid_1[init_live[0][i]][init_live[1][i]]=1;
        else
            grid_2[init_live[0][i]][init_live[1][i]]=1;}
}

void die(int i,int j){
    if(first){
        grid_2[i][j]=0;}
    else{
        grid_1[i][j]=0;}
}

void update_matrix(){
    int i,j;
    cout<<first<<endl;
    for(i=0;i<grid_w;i++){
        for(j=0;j<grid_h;j++){
            int q,neig=0;
            for(q=0;q<8;q++){
                int new_x=i+mov_x[q],new_y=j+mov_y[q];
                if(new_x>=0&&new_x<grid_w&&new_y>=0&&new_y<grid_h){
                    if(first&&grid_1[new_x][new_y])
                        neig++;
                    else if(!first&&grid_2[new_x][new_y])
                        neig++;
                }
            }
            cout<<first<<" "<<i<<" "<<j<<" "<<neig<<endl;
            if((first&&grid_1[i][j]==1)||(!first&&grid_2[i][j]==1)){
                    if(neig<2 || neig>=3){
                            die(i,j);
                    }
            }
            else {
                if(neig==3)
                        if(first){
                            grid_2[i][j]=1;
                        }
                        else {grid_1[i][j]=1;}
                else{
                        if(first){
                            grid_2[i][j]=0;
                        }
                        else{
                            grid_1[i][j]=0;
                        }

                }
            }

            /*
            if(neig<2){
                if(first&&grid_1[i][j]==1){
                    grid_2[i][j]=0;}
                else if(!first&&grid_2[i][j]==1){
                    grid_1[i][j]=0;}
                }
            else {
                    if(neig>=2&&neig<=3){
                        if(first&&grid_1[i][j]==1){
                            grid_2[i][j]=0;}
                    else if(!first&&grid_2[i][j]==1){
                        grid_1[i][j]=0;}
                    }
                    else {
                            if(neig>=3){
                                if(first&&grid_1[i][j]==1){
                                    grid_2[i][j]=0;}
                                else if(!first&&grid_2[i][j]==1){
                                    grid_1[i][j]=0;}
                            }
                            else {
                                    if(neig==3){
                                        if(first&&grid_1[i][j]==0){
                                            grid_2[i][j]=0;}
                                        else if(!first&&grid_2[i][j]==0){
                                            grid_1[i][j]=0;}
                                    }
                                    else{
                                        if(first){
                                            grid_2[i][j]=0;}
                                        else{
                                            grid_1[i][j]=0;
                                            }
                                    }
                            }
                    }
            }*/
        }
    }
    first=!first;
}


void refresh(){
    COORD cur = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cur);
}


void display_grid(bool clean){
    if(clean){
        refresh();}
    int i=0,j=0;
    for(i=0;i<grid_h;i++){
        for(j=0;j<grid_w;j++)
            if((first&&grid_1[i][j])||(!first&&grid_2[i][j]))
                cout<<"*";
            else cout<<"O";
        cout<<endl;}

}

void randomize_grid(int **grid){
    int i=0,j=0;
    for(i=0;i<grid_h;i++)
        for(j=0;j<grid_w;j++)
            (*(*(grid+i)+j))=get_random();
}

int main()

{  int i;
    srand((unsigned) time(NULL));
  /*for(q=0;q<100;q++)
    cout<<get_random();
  cout<<endl;*/
  int steps=2;
    init();
    display_grid(false);
   while(i++<steps){
        update_matrix();
        display_grid(false);
}
    //cout << "Hello world!" << endl;
    return 0;
}
