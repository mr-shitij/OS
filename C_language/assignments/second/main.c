#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"DCLL.h"

/*
Reads a CSV file with columns Name, MISID, Marks.
Creates a doubly circular linked list of all the entries from the file.  
Then sorts this doubly linked list on MISID.
Then Reads another CSV file with same columns, and inserts the entries from the second CSV file into the doubly linked list in Sorted order.  
The code should be properly divided into functions and a DCLL data type.
*/

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
    char *file_name_first = args[1]; 
    char *file_name_second = args[2]; 

    /* Going For the First File ..!! */

    printf("Openning the file : %s\n", file_name_first);
    FILE* file_first = fopen(file_name_first, "r");

    char colums_first[3][10];
    int cn_first = 0, wo_first = -1;
    char ch;
    while((ch = fgetc(file_first)) != '\n'){
        wo_first++;
        if(ch == ','){
            colums_first[cn_first][wo_first] = '\0';
            cn_first++;
            wo_first = -1;
        }
        else{
            // printf("\nch : %c, wo : %d, cn : %d", ch, wo, cn);
            colums_first[cn_first][wo_first] = ch;
        }
    }
    colums_first[cn_first][++wo_first] = '\0';

    // Assumming Colum Specified as MIS, Name, Marks
    DCLL list;
    while(getc(file_first) != EOF) {
        fseek(file_first, -1, SEEK_CUR);
        Data d = get_data_token_by_line(file_first);
        insert_last_DCLL(&list, &d);
    }
    printf("\n Constructed DCLL From The CSV ..!!");
    show_DCLL(&list);

    printf("\n Now Sorting DCLL ..!!");
    sort_DCLL(&list);
    show_DCLL(&list);


    /* Now Going For the Second File ..!! */

    printf("Openning the file : %s\n", file_name_second);
    FILE* file_second = fopen(file_name_second, "r");
    
    char colums_second[3][10];
    int cn_second = 0, wo_second = -1;
    while((ch = fgetc(file_second)) != '\n'){
        wo_second++;
        if(ch == ','){
            colums_second[cn_second][wo_second] = '\0';
            cn_second++;
            wo_second =-1;
        }
        else{
            // printf("\nch : %c, wo : %d, cn : %d", ch, wo, cn);
            colums_second[cn_second][wo_second] = ch;
        }
    }
    colums_second[cn_second][++wo_second] = '\0';

    // Assumming Colum Specified as MIS, Name, Marks
    while(getc(file_second) != EOF) {
        fseek(file_second, -1, SEEK_CUR);
        Data d = get_data_token_by_line(file_second);
        insert_sorted_DCLL(&list, &d);
    }
    printf("\n After Adding The Content From Second File ..!!");
    show_DCLL(&list);
    return 0;
}
