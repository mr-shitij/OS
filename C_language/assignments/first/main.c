#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"DCLL.h"

/*
Reads a CSV file with columns Name, MISID, Marks; then calculates the average, total, and median marks, 
and saves all the result in another CSV file with columns  statistics, number. 
The filenames should be accepted as command-line arguments.
*/

int count = 0, mean = 0, total = 0;
int median = 0;

void updater(Data* data) {
    count++;
    total += data->marks;
}

void getStatistics(DCLL* list) {
    map(list, &updater);

    mean = total / count;
    median = get_item_DCLL(list, count / 2)->mis;

    printf("\n Total : %d", total);
    printf("\n Mean/Average : %d", mean);
    printf("\n Median : %d", median);
}

void write_statistics_to_file(char* filename, DCLL* list) {
    FILE* file = fopen(filename, "w");
    fprintf(file, "Total,Mean,Median\n");
    fprintf(file, "%d,%d,%d\n", total, mean, median);
    fclose(file);
}

Data get_data_token_by_line(FILE* file) {
    char colums[3][10];
    int cn = 0, wo= -1;
    char ch;
    while((ch = fgetc(file)) != '\n'){
        wo++;
        if(ch == ','){
            colums[cn][wo] = '\0';
            cn++;
            wo=-1;
        }
        else{
            // printf("\nch : %c, wo : %d, cn : %d", ch, wo, cn);
            colums[cn][wo] = ch;
        }
    }
    colums[cn][++wo] = '\0';

    // for(int i=0;i<3;i++)
    //     printf("\n Column : %d value is : %s", i, colums[i]);
    // printf("\n\n");

    Data data;
    data.mis = atoi(colums[0]);
    strcpy(data.name, colums[1]);
    data.marks = atoi(colums[2]);

    return data;
}


int main(int argn, char* args[]) {
    if(argn < 3)
        exit(1);
    char *file_name = args[1]; 
    char *file_name_out = args[2]; 

    printf("Openning the file : %s\n", file_name);
    FILE* file = fopen(file_name, "r");
    
    char colums[3][10];
    int cn = 0, wo= -1;
    char ch;
    while((ch = fgetc(file)) != '\n'){
        wo++;
        if(ch == ','){
            colums[cn][wo] = '\0';
            cn++;
            wo=-1;
        }
        else{
            // printf("\nch : %c, wo : %d, cn : %d", ch, wo, cn);
            colums[cn][wo] = ch;
        }
    }
    colums[cn][++wo] = '\0';
    // for(int i=0;i<3;i++)
    //     printf("\n Column : %d value is : %s", i, colums[i]);
    // printf("\n\n");


    // Assumming Colum Specified as MIS, Name, Marks
    DCLL list;
    while(getc(file) != EOF) {
        fseek(file, -1, SEEK_CUR);
        Data d = get_data_token_by_line(file);
        insert_sorted_DCLL(&list, &d);
    }
    show_DCLL(&list);
    getStatistics(&list);
    write_statistics_to_file(file_name_out, &list);
    return 0;
}
/*

char *file_name, *file_name_out;
int count = 0, mean = 0, total = 0;
int median = 0;

void updater(Data* data) {
    count++;
    total += data->marks;
}

void getStatistics(DCLL* list) {
    map(list, &updater);
    printf("\n Total : %d", total);
    printf("\n Mean/Average : %d", total/count);
    printf("\n Median : %d", get_item_DCLL(list, count/2)->mis);
}

Data get_data_token_by_line(FILE* file) {
    char colums[3][10];
    int cn = 0, wo= -1;
    char ch;
    while((ch = fgetc(file)) != '\n'){
        wo++;
        if(ch == ','){
            colums[cn][wo] = '\0';
            cn++;
            wo=-1;
        }
        else{
            // printf("\nch : %c, wo : %d, cn : %d", ch, wo, cn);
            colums[cn][wo] = ch;
        }
    }
    colums[cn][++wo] = '\0';


    // for(int i=0;i<3;i++)
    //     printf("\n Column : %d value is : %s", i, colums[i]);
    // printf("\n\n");


    Data data;
    data.mis = atoi(colums[0]);
    strcpy(data.name, colums[1]);
    data.marks = atoi(colums[2]);

    return data;
}

void write_data_to_file(Data* data) {
    if(file_name == NULL)
        return;
    FILE* file = fopen(file_name_out, "a+");
    fprintf(file, "%d,%s,%d", data->mis, data->name, data->marks);
    fclose(file);
}

void write_DCLL_to_file(char* filename, DCLL* list) {
    char* temp = file_name_out;
    file_name_out = filename;
    map(list, &write_data_to_file);
    file_name_out = temp;
}

void write_statistics_to_file(char* filename, DCLL* list) {
    FILE* file = fopen(filename, "w");
    fprintf(file, "Total,Mean,Median");
    fprintf(file, "%d,%d,%d", total, mean, median);
    fclose(file);
}

int main(int argn, char* args[]) {
    file_name = NULL;
    file_name_out = NULL;

    if(argn < 3)
        exit(1);

    file_name = args[1]; 
    file_name_out = args[2]; 

    printf("Openning the file : %s\n", file_name);
    FILE* file = fopen(file_name, "r");
    
    char colums[3][10];
    int cn = 0, wo= -1;
    char ch;
    while((ch = fgetc(file)) != '\n'){
        wo++;
        if(ch == ','){
            colums[cn][wo] = '\0';
            cn++;
            wo=-1;
        }
        else{
            // printf("\nch : %c, wo : %d, cn : %d", ch, wo, cn);
            colums[cn][wo] = ch;
        }
    }
    colums[cn][++wo] = '\0';

    // for(int i=0;i<3;i++)
    //     printf("\n Column : %d value is : %s", i, colums[i]);
    // printf("\n\n");

    // Assumming Colum Specified as MIS, Name, Marks
    DCLL list;
    while(getc(file) != EOF) {
        fseek(file, -1, SEEK_CUR);
        Data d = get_data_token_by_line(file);
        printf("\n\nDATA:%s\n\n", d.name);
        insert_sorted_DCLL(&list, &d);
        printf("\n\n\n\ndaHELLLO..!!\n\n\n\n");
    }
    fclose(file);
    printf("\n\nHELLLO..!!\n\n");
    show_DCLL(&list);
    getStatistics(&list);
    write_statistics_to_file(file_name_out, &list);
    return 0;
}


// File me write kar na ka bacha hai only ie. (saves all the result in another CSV file with columns  statistics, number). 

*/