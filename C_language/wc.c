#include<stdio.h>
/*Counting total words, characters and lines*/
int main() {
    int c, wc, lc, cc;
    wc=cc=lc=0;

    while((c=getchar()) != EOF) {
        printf("%c", c);
        if(c == '\n')
            lc++;
        else if (c == '\t' || c == ' ')
            wc++;
        else if(c != '\n' || c != '\t' || c != ' ')
            cc++; 

        if(c == 'e')
            break;
    }
    printf("\n word count: %d, character count is: %d, line count is: %d", wc, cc, lc);
    return 0;
}
