#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <chrono>
using namespace std;
int n;
time_t t;

const int grid_h=30,grid_w=30;
int grid_1[grid_h][grid_w],grid_2[grid_h][grid_w];
bool first=true;
int mov_x[8]={-1,-1,0,1,1,1,0,-1},mov_y[8]={0,-1,-1,-1,0,1,1,1};
const int init_live_no=15;
//int init_live[2][init_live_no]={{0,1,2,1,5 ,10,11,12,11,15 ,20,21,22,21,25},{1,0,1,2,1 ,11,10,11,12,11 ,21,20,21,22,21}};

int get_random(){
   return rand()%2;
}


void read_file(){
   char data[100];
   ifstream infile;
   infile.open("initfile");
   infile >> data;
   int line=std::stoi(data);
   infile >> data;
   int col=std::stoi(data);
   cout<<line<<" "<<col<<endl;
   int i,j;
   for(i=0;i<line;i++){
        infile >> data;
    for(j=0;j<col;j++){
        cout<<data[j];
        grid_1[i][j]=data[j]=='0'?0:1;}
    cout<<endl;
    cout<<strlen(data)<<endl;}
   infile.close();
}

void init(){
    int i,j;
    for(i=0;i<grid_w;i++)
        for(j=0;j<grid_h;j++){
            if(first)
                grid_1[i][j]=0;
            else grid_2[i][j]=0;}
    read_file();
}

void save_grid(){
    int i,j;
    for(i=0;i<grid_w;i++){
        for(j=0;j<grid_h;j++){
            grid_1[i][j]=grid_2[i][j];
        }
    }
}

void die(int i,int j){
     grid_2[i][j]=0;
}

void update_matrix(){
    int i,j;
    //cout<<first<<endl;
    for(i=0;i<grid_w;i++){
        for(j=0;j<grid_h;j++){
            int q,neig=0;
            for(q=0;q<8;q++){
                int new_x=i+mov_x[q],new_y=j+mov_y[q];
                if(new_x>=0&&new_x<grid_w&&new_y>=0&&new_y<grid_h){
                    if(grid_1[new_x][new_y])
                        neig++;
                }
            }
            //cout<<first<<" "<<i<<" "<<j<<" "<<neig<<endl;
            if(grid_1[i][j]==1){
                    if(neig<2 || neig>3){
                            die(i,j);
                    }
                    else grid_2[i][j]=1;

            }
            else {
                if(neig==3)
                            grid_2[i][j]=1;
                else{
                            grid_2[i][j]=0;
                }
            }
        }
    }

    save_grid();
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
            if(grid_1[i][j])
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

int main(){
    read_file();
      int i;
    srand((unsigned) time(NULL));
  int steps=2000;
    init();
    display_grid(false);
    float runs[steps];
   while(i++<steps){
        auto t1 = std::chrono::steady_clock::now();
        update_matrix();
        auto t2 = std::chrono::steady_clock::now();
        float durat=std::chrono::duration_cast<std::chrono::nanoseconds>( t2 - t1 ).count();
        if(durat>0)
            runs[i]=durat;
        else i--;
        //display_grid(true);
}
    cout<<endl;
    display_grid(false);
    ofstream MyFile("measureserial.log");
    MyFile<<grid_h<<" "<<grid_w<<endl;
    float sum=0;
    for(i=0;i<steps;i++){
        MyFile <<i<<" "<< runs[i]<<endl;
        sum+=runs[i];
        }
    MyFile<<"Total "<<sum<<" Average "<<(float)sum/steps<<endl;
    MyFile.close();

    //cout << "Hello world!" << endl;
    return 0;
}
