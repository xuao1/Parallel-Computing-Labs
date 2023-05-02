#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));
    int num_lines = rand() % 100 + 2; 
    FILE* fp;
    fp = fopen("input3.txt", "w");
    char word[105];
    for (int i = 0; i < num_lines; i++) {
        for (int j = 0; j < 10; j++) {
            int length = rand() % 20 + 1;  // 单词长度
            for (int k = 0; k < length; k++) {
                word[k] = 'a' + rand() % 26; 
            }
            word[length] = '\0';
            fprintf(fp, "%s ", word);  // 用空格隔开
        }
        fprintf(fp, "\n");  // 换行
    }
    fclose(fp);
    return 0;
}
