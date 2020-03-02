#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
int resultmatrix [10][10]={0};
int row1;
int row2;
int coloumn1;
int coloumn2;
int mat_a [10][10]={0};
int mat_b [10][10]={0};
int matmethodsec [100];
int threadcount=0;
int incrementer = 0;
void* multiplymatrix(void * argument);
void* elementmethod(void * argument);
int main(){
int i = 0;
int j = 0;
int k = 0;

FILE *fptr;

fptr = fopen("/home/abobakr/Desktop/input.txt","r");
if (fptr  == NULL)
{
    printf("error opening file");
}

fscanf(fptr,"%d %d\n",&row1,&coloumn1);
printf("mat-A : %d %d\n",row1,coloumn1);

for(i = 0;i < row1; i++){
    for ( j = 0; j < coloumn1; j++)
    {
        fscanf(fptr,"%d",&mat_a[i][j]);
    }

}

fscanf(fptr,"%d %d\n",&row2,&coloumn2);
printf("mat-B : %d %d\n",row2,coloumn2);

for(i = 0;i < row2; i++){
    for ( j = 0; j < coloumn2; j++)
    {
        fscanf(fptr,"%d",&mat_b[i][j]);
    }

}

fclose(fptr);

//matrix A
printf("matrix A :\n");
for(i=0;i<row1;i++){
    for(j=0;j<coloumn1;j++){
        printf("%d ",mat_a[i][j]);
    }
    printf("\n");
}


//matrix B
printf("matrix B :\n");
for(i=0;i<row2;i++){
    for(j=0;j<coloumn2;j++){
        printf("%d ",mat_b[i][j]);
    }
    printf("\n");
}

////////////////// each thread calculate single row //////////////////

//creating array of threads
printf("creating threads part\n");
pthread_t thread_IDarray[row1];

clock_t startrow = clock();

for ( i = 0; i < row1; i++)
{
    printf("createing thread\n");
    // if(threadcount < row1)
    pthread_create(&thread_IDarray[i],NULL,multiplymatrix,NULL);

}
//using join to make parent wait for threads to finish
printf("joining threads\n");
for ( i = 0; i < row1; i++)
{
    printf("waiting for threads\n");
    pthread_join(thread_IDarray[i],NULL);

}

clock_t endrow = clock();
double exec_time = (double) (endrow - startrow) / CLOCKS_PER_SEC;
printf("Finished row calculations\n");
/////////////// each thread compute an element in result matrix ////////
pthread_t tharr2 [row1*coloumn2];

//creating an array to hold the row and col required to generate a specific cell in result matrix
// int row_col [row1 * col2];
int threadcounter = 0;
int * passvalues ;
clock_t startelement = clock();
for ( i = 0; i < row1; i++)
{
    for ( j = 0; j < coloumn2; j++)
    {
        passvalues = (int *)malloc(100*sizeof(int));
        for ( k = 0; k < row2; k++){
            passvalues[k] = mat_a[i][k];
            passvalues[k+coloumn1] = mat_b[k][j];
            }




        pthread_create(&tharr2[threadcounter++],NULL,elementmethod,(void *)passvalues);

    }

}
printf("Finished creation of element threads \n");
int elementarr [row1*coloumn2];

for ( i = 0; i < (row1*coloumn2); i++)
{
    void * cellvlu;
    pthread_join(tharr2[i],&cellvlu);
     printf("after join cell value : %d\n",(int *)cellvlu);

    int * copy = (int *)cellvlu;
    elementarr[i] = *copy;
    printf("eachelement : %d \n",elementarr[i]);


}
clock_t endelement = clock();

double exec_time2 = (double) (endelement - startelement) / CLOCKS_PER_SEC;




// printing result array before writing it in file

printf("Resulting Matrix :: \n");
for ( i = 0; i < row1; i++)
{
    for ( j = 0; j < coloumn2; j++)
    {
        printf("%d ",resultmatrix[i][j]);
    }
    printf("\n");
}
FILE * outptr = fopen("/home/abobakr/Desktop/output.txt","w");
for ( i = 0; i < row1; i++)
{
    for ( j = 0;  j < coloumn2; j++)
    {
        fprintf(outptr,"%d ",resultmatrix[i][j]);
    }
    fprintf(outptr,"\n");
}

fprintf(outptr,"\nEND1\t[%lf]\n",exec_time);
for ( i = 0; i <(row1 * coloumn2); i++)
{
    if (i % coloumn2 == 0)
    {
        fprintf(outptr,"\n");
    }
    fprintf(outptr,"%d ",elementarr[i]);

}
fprintf(outptr,"\nEND2\t[%lf]\n",exec_time2);

fclose(outptr);
return 0;
}
void* multiplymatrix(void * argument){

    int partindex=incrementer;
    int i;
    int j;
    int k;

    //making each thread execute a single row


        for ( j = 0; j < coloumn2; j++)
        {
            for ( k = 0; k < coloumn1; k++)
            {
                resultmatrix[partindex][j] += mat_a[partindex][k] * mat_b[k][j];

            }

        }





    ++incrementer;
    threadcount++;
    pthread_exit(NULL);

}

void* elementmethod(void* argument){

    int i = 0;
    int sum = 0;
    int * ptrsendvalue = (int *) argument ;
    for ( i = 0; i < coloumn1; i++)
    {
        // sum += matmethodsec[i]*matmethodsec[i+coloumn1];
        sum += ptrsendvalue[i]*ptrsendvalue[i+coloumn1];

        printf("sum = %d\n",sum);
    }
    int * cellvalue = (int *)malloc(100*sizeof(int));
    *cellvalue = sum;
    printf("sent value to join is : %d\n",*cellvalue);
    pthread_exit(cellvalue);
}
