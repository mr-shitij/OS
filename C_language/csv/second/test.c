#include<stdio.h>

int main() {
    int arr[] = {40, 20, 30};
    printf("\n Array Currently : ");
    for(int i =0 ; i<3; i++) {
        printf("%d", arr[i]);
    }

    /*Bubble Sort*/
    for(int i=0; i<3-1; i++) {
        for(int j=i+1;j<3;j++) {
            if(arr[i] > arr[j]) {
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }

    printf("\n Array Now : ");
    for(int i =0 ; i<3; i++) {
        printf("%d", arr[i]);
    }

    return 0;
}
