/* Environment Management Routines Example *
   https://computing.llnl.gov/tutorials/mpi
Compile with:
   gcc -I"c:\Program Files\MPICH2\include" -L"c:\Program Files\MPICH2\lib" -o Env.exe Env.c -lmpi
Run with:
   mpiexec Env
   mpiexec -n 4 Env
   mpiexec -host USER-PC-2 Env
   mpiexec -hosts 2 USER-PC-2 USER-PC-4 Env
*/

    #include "mpi.h"
    #include <stdio.h>
    #include <windows.h>
    #include <chrono>

    using namespace std;
    int n;

    int decomp_h = 1;
    int decomp_w = 3;

    time_t t;

    const int grid_h=30,grid_w=30;
    int grid_1[grid_h][grid_w],grid_2[grid_h][grid_w];
    bool first=true;
    int mov_x[8]={-1,-1,0,1,1,1,0,-1},mov_y[8]={0,-1,-1,-1,0,1,1,1};
    int col,line;

    int **alloc_2d_int(int rows, int cols) {
        int *data = (int *)malloc(rows*cols*sizeof(int));
        int **array= (int **)malloc(rows*sizeof(int*));
        for (int i=0; i<rows; i++)
            array[i] = &(data[cols*i]);

        return array;
    }

    void save_file(){
        int i,j;
        FILE *fp;
        fp = fopen("last_State", "w");
        fprintf(fp, "%d %d \n", col,line);
        char c;
        for(i=0;i<line;i++){
                for(j=0;j<col;j++){
                    fprintf(fp, "%c",grid_1[i][j]+'0');
                }
             fprintf(fp, "\n");
        }
    }

    void read_file(){
        int i,j;
        FILE *fp;
        fp = fopen("initfile", "r");
        fscanf(fp, "%d %d \n", &col,&line);
        char c;
        for(i=0;i<line;i++){
                for(j=0;j<col;j++){
                    c=fgetc(fp);
                    grid_1[i][j]=c=='0'?0:1;
                }
            fgetc(fp);
        }
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
                    printf("*");
                else printf("O");
            printf("\n");}
    }

    void save_grid(int rank){
        int i,j;

        int decomp_data_size_i=grid_h/decomp_h;
        int decomp_data_size_j=grid_w/decomp_w;

        int ind_i=rank/decomp_w;
        int ind_j=rank%decomp_w;

        int inter_start_i=ind_i*decomp_data_size_i;
        int inter_end_i=rank==decomp_h-1?grid_h:(ind_i+1)*decomp_data_size_i;

        int inter_start_j=ind_j*decomp_data_size_j;
        int inter_end_j=rank==decomp_w-1?grid_w:(ind_j+1)*decomp_data_size_j;
        //printf("save grid rank %d inter_start_i %d inter_end_i %d inter_start_j %d inter_end_j %d\n",rank,inter_start_i,inter_end_i,inter_start_j,inter_end_j);
        for(i=inter_start_i;i<inter_end_i;i++){
            for(j=inter_start_j;j<inter_end_j;j++){
                    grid_1[i][j]=grid_2[i][j];}
        }
    }

    void save_rec_grid(int rank,int **aux_mat){
        int i,j;

        int decomp_data_size_i=grid_h/decomp_h;
        int decomp_data_size_j=grid_w/decomp_w;

        int ind_i=rank/decomp_w;
        int ind_j=rank%decomp_w;

        int inter_start_i=ind_i*decomp_data_size_i;
        int inter_end_i=rank==decomp_h-1?grid_h:(ind_i+1)*decomp_data_size_i;

        int inter_start_j=ind_j*decomp_data_size_j;
        int inter_end_j=rank==decomp_w-1?grid_w:(ind_j+1)*decomp_data_size_j;
        //printf("save rec grid rank %d inter_start_i %d inter_end_i %d inter_start_j %d inter_end_j %d\n",rank,inter_start_i,inter_end_i,inter_start_j,inter_end_j);
        for(i=inter_start_i;i<inter_end_i;i++){
            for(j=inter_start_j;j<inter_end_j;j++){
                    grid_1[i][j]=aux_mat[i-inter_start_i][j-inter_start_j];
                    if(grid_1[i][j]!=0&&grid_1[i][j]!=1){
                        //printf("Strange value %d at i=%d j=%d idex rec : i=%d j=%d\n",grid_1[i][j],i,j,i-inter_start_i,j-inter_start_j);
                    }
                    }
        }
    }

    void die(int i,int j){
    if(first){
        grid_2[i][j]=0;}
    else{
        grid_1[i][j]=0;}
}

    void update_matrix(int rank,int world_size){
    int i,j;
    //cout<<first<<endl;
    int decomp_data_size_i=grid_h/decomp_h;
    int decomp_data_size_j=grid_w/decomp_w;

    int ind_i=rank/decomp_w;
    int ind_j=rank%decomp_w;

    int inter_start_i=ind_i*decomp_data_size_i;
    int inter_end_i=rank==decomp_h-1?grid_h:(ind_i+1)*decomp_data_size_i;

    int inter_start_j=ind_j*decomp_data_size_j;
    int inter_end_j=rank==decomp_w-1?grid_w:(ind_j+1)*decomp_data_size_j;
    //printf("rank %d inter_start_i %d inter_end_i %d inter_start_j %d inter_end_j %d\n",rank,inter_start_i,inter_end_i,inter_start_j,inter_end_j);
    for(i=inter_start_i;i<inter_end_i;i++){
        for(j=inter_start_j;j<inter_end_j;j++){
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
    }

    void broadcast(int rank,int world_size,int line_size,int col_size,int i_start,int j_start){
        //printf("Rank %d broadcast i_start %d j_start %d line_size %d col_size %d\n",rank,i_start,j_start,line_size,col_size);
        int **aux_matrix=alloc_2d_int(line_size,col_size);
        int i,a,b;
        for(a=0;a<line_size&&a<line;a++)
            for(b=0;b<col_size&&b<col;b++)
                aux_matrix[a][b]=grid_1[i_start+a][j_start+b];
        for (i = 0; i < world_size; i++) {
            if (i != rank) {
                MPI_Send(&line_size, 1, MPI_INT, i, rank, MPI_COMM_WORLD);
                MPI_Send(&col_size, 1, MPI_INT, i, rank, MPI_COMM_WORLD);
                MPI_Send(&(aux_matrix[0][0]), line_size*col_size, MPI_INT, i, rank, MPI_COMM_WORLD);
            }
        }
    }

   int main(int argc,char *argv[]){
    int  numtasks, rank, len, rc;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    rc = MPI_Init(&argc,&argv);
    if (rc != MPI_SUCCESS) {
     printf ("Error starting MPI program. Terminating.\n");
     MPI_Abort(MPI_COMM_WORLD, rc);
     }

   MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
   MPI_Comm_rank(MPI_COMM_WORLD,&rank);
   MPI_Get_processor_name(hostname, &len);
   printf ("Number of tasks= %d My rank= %d Running on %s\n", numtasks,rank,hostname);


   if(numtasks<decomp_h*decomp_w){
        printf("More processes are needed ( %d )",decomp_h*decomp_w);
        MPI_Finalize();
        return 0;}
    int loop=0;

    init();
    display_grid(false);
    clock_t start, end;
    start = clock();
    while(loop++<2000){
        update_matrix(rank,numtasks);
        save_grid(rank);
        int decomp_data_size_i=grid_h/decomp_h;
        int decomp_data_size_j=grid_w/decomp_w;

        int ind_i=rank/decomp_w;
        int ind_j=rank%decomp_w;

        int inter_start_i=ind_i*decomp_data_size_i;
        int inter_end_i=rank==decomp_h-1?grid_h:(ind_i+1)*decomp_data_size_i;

        int inter_start_j=ind_j*decomp_data_size_j;
        int inter_end_j=rank==decomp_w-1?grid_w:(ind_j+1)*decomp_data_size_j;

        int prev_index;
        int aux_col,aux_line;
        for(prev_index=0;prev_index<rank;prev_index++){
            //printf("Rank %d receiving previous %d \n",rank,prev_index);
            MPI_Recv(&aux_line, 1, MPI_INT, prev_index, prev_index, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            MPI_Recv(&aux_col, 1, MPI_INT, prev_index, prev_index, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            int **aux_mat=alloc_2d_int(aux_line,aux_col);
            MPI_Recv(&(aux_mat[0][0]), aux_line*aux_col, MPI_INT, prev_index, prev_index, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            save_rec_grid(prev_index,aux_mat);
            free(aux_mat[0]);
            free(aux_mat);}
        broadcast(rank,decomp_h*decomp_w,inter_end_i-inter_start_i,inter_end_j-inter_start_j,inter_start_i,inter_start_j);
        for(prev_index=rank+1;prev_index<decomp_h*decomp_w;prev_index++){
            //printf("Rank %d receiving next %d \n",rank,prev_index);
            MPI_Recv(&aux_line, 1, MPI_INT, prev_index, prev_index, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            MPI_Recv(&aux_col, 1, MPI_INT, prev_index, prev_index, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            int **aux_mat=alloc_2d_int(aux_line,aux_col);
            MPI_Recv(&(aux_mat[0][0]), aux_line*aux_col, MPI_INT, prev_index, prev_index, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            save_rec_grid(prev_index,aux_mat);
            free(aux_mat[0]);
            free(aux_mat);}
    }
   end = clock();
   double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
   printf("Rank %d Total time : %f s Loops: %d Decomp_h: %d Decomp_w: %d\n",rank,time_taken,loop,decomp_h,decomp_w);
   if (rank == 0) {
    display_grid(false);
    save_file();}

   MPI_Finalize();
   }
