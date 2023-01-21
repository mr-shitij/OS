#include<stdio.h>
#include<string.h>
#include<ctype.h>

#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>

#include"./queue/queue.h"

char *trimString(char *str) {
    char *end;

    while(isspace((unsigned char)*str)) str++;
    if(*str == 0)
       return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';
    return str;
}
int get_white_spce_pos(char* str) {
    if(str == NULL) 
        return -1;
    int pos = 0;
    while(str[pos] != ' ') {
        if(str[pos] == '\0')
            return -1;
        pos++;
    }
    return pos;
}

int get_pos_and_count_of_words(queue_list* queue, char* str) {
    if(str == NULL) 
        return -1;
    int pos = 0;
    int count = 0, w_start = -1, w_end = 0;

    while(str[pos] != '\0') {
        if(str[pos] != ' ') {
            w_start++;
            w_end = 0;
        }

        if(str[pos] == ' ') {
            w_start = -1;
            w_end++;
        }

        if(w_start == 0 && w_end == 0) {
            int* p = malloc(sizeof(int));
            *p = pos;
            queue_enqueue(queue, p);
            count++;
        }
        else if(w_start == -1 && w_end == 1) {
            int* p = malloc(sizeof(int));
            *p = pos;
            queue_enqueue(queue, p);
            count++;
        }

        pos++;
    }
    int* p = malloc(sizeof(int));
    *p = pos;
    queue_enqueue(queue, p);
    count++;
    return count/2;
}

void copy_string_from_offset(char* dest_str, char* source_str, int start_offset, int end_offset) {
    int dest_offset = 0;
    while(source_str[start_offset] != '\0') {
        if(start_offset == end_offset)
            break;
        dest_str[dest_offset] = source_str[start_offset];
        start_offset++;
        dest_offset++;
    }
    dest_str[dest_offset] = '\0';
}

void combie_strings_upto_offsets(char* out, char* string_1, char* string_2, int string_1_offset, int string_2_offset) {
    int string_1_start, string_2_start, string_out_start;
    string_1_start = string_2_start = string_out_start = 0;

    while (string_1_start != string_1_offset) {
        out[string_out_start] = string_1[string_1_start];
        string_out_start++;
        string_1_start++;
    }

    while (string_2_start != string_2_offset) {
        out[string_out_start] = string_2[string_2_start];
        string_out_start++;
        string_2_start++;
    }
    out[string_out_start] = '\0';
}

char* substr(char* command, int first, int second) {
    int len = strlen(command);
    if(len < second)
        return NULL;

    int i;
    int c = 0;
    char* str = malloc(sizeof(char) * ((second - first) + 1));
    for(i = first; i < second; i++) {
        str[c] = command[i];
        c++;
    }
    str[c] = '\0';
    return str;
}

int is_file_exists_access(const char *path){
    if (access(path, F_OK) == -1)
        return 0;
    return 1;
}

int check_for_cd(char* command, char*  arguments) {
    if(strcmp(command, "cd") != 0) {
        return 0;   
    }

    chdir(arguments);
    return 1;
}

int main() {
    int pid = -1; //initially set to less than 0 so that if no fork got created then program get terminated ..!!

    int output_redirection_index = -1; // -1 for nothing else the index of rediretion sign
    int output_redirection_flag = -1; // -1 = NULL, 0 = >, 1 = <
    char* output_redirection_file_name = NULL;

    char command[1024];

    char bin_path[1024];
    queue_list path;
    queue_init(&path);

    int current_directory_flag = 0; // 0 = CWD, 1 = SHORT
    char current_path[1024];

    int i;

    queue_list queue;
    queue_init(&queue);

    do {
        if(current_directory_flag == 0)
            getcwd(current_path, sizeof(current_path));

        printf("\n%s>",current_path);    
        scanf(" %[^\t\n]s", command);

        if(strcmp(command, "exit") == 0) return 0;
        if(strstr(command, "PS1=\"") != NULL) {
            int len = strlen(command);
            if(len < 5) {
                printf("Invalid Command ..!!\n");
            } else {
                int i = 5, offset = 0;
                while(command[i] != '\0') {
                    if(command[i] == '"') {
                        current_path[offset] = '\0';
                        current_directory_flag = 1;
                        break;
                    }
                    current_path[offset] = command[i];
                    i++;
                    offset++;
                }
                continue;
            }
        }
        else if(strstr(command, "PATH=") != NULL) {
            int len = strlen(command);
            if(len < 5) {
                printf("Invalid Command ..!!\n");
            } else {
                while(path != NULL) {
                    queue_dequeue(&path);
                }

                int i = 5, offset = 0, temp_offset, current_len = 0;
                temp_offset = i;
                while(command[i] != '\0') {
                    if(command[i] == ':') {
                        char* pa = malloc(sizeof(char) * current_len);
                        copy_string_from_offset(pa, command, temp_offset, i);

                        queue_enqueue(&path, pa);

                        temp_offset = i + 1;
                        current_len = 0;
                    }
                    current_len++;
                    i++;
                }
                char* pa = malloc(sizeof(char) * current_len);
                copy_string_from_offset(pa, command, temp_offset, i);
                queue_enqueue(&path, pa);
                continue;
            }
        }

        int wc = get_pos_and_count_of_words(&queue, command);
        if(wc == -1)
            printf("Invalid ..!!\n");
        else {
            char** words = malloc(sizeof(char*) * (wc + 1));
            int c = 0;

            words[wc] = NULL;
            while(wc != 0){
                int first = *((int*)queue->data);
                queue_dequeue(&queue);

                int second = *((int*)queue->data);
                queue_dequeue(&queue);

                int len = (second - first) + 1;

                words[c] = malloc(sizeof(char) * len);

                char *s = substr(command, first, second);
                words[c] = s;

                c++;
                wc--;
            }
            wc = c;

            if(wc == 2) {
                if(check_for_cd(words[0], words[1]))
                    continue;
            } else if(wc > 2) {
                output_redirection_index = 0;
                for(i = 0; i < wc; i++) {
                    if(strcmp(words[i], ">") == 0){
                        output_redirection_flag = 0;
                        break;
                    }
                    if(strcmp(words[i], "<") == 0){
                        output_redirection_flag = 1;
                        break;
                    }
                }
                if(output_redirection_flag != -1) {
                    output_redirection_index = i;
                    if(output_redirection_index + 1 > wc){
                        printf("Invalid Redirection ..!! : %d\n", output_redirection_index);
                        continue;
                    }
                    output_redirection_file_name = words[output_redirection_index + 1];
                }
            }


            queue_node* node = path;
            int found = 0;
            while(node != NULL) {
                char* st = malloc(sizeof(char) * (strlen((char*)node->data) + strlen(words[0])));
                strcpy(st, node->data);
                strcat(st, words[0]);


                if(is_file_exists_access(st)){
                    strcpy(bin_path, st);
                    found = 1;
                }
                free(st);
                node = node->next;
            }
            if(!found){
                printf("Command Not Found ..!!\n");
                continue;
            }


            pid = fork();
            if(pid < 0) {
                printf("\nnot able to create child ..!!");
            }
            else if(pid == 0) {
                // combie_strings_upto_offsets(final_commad, bin_path, command, strlen(bin_path), strlen(words[0]));
                if(output_redirection_flag != -1) {
                    char **new_args = malloc(sizeof(char*) * (output_redirection_index));
                    for(i = 0; i < output_redirection_index; i++) {
                        new_args[i] = words[i];
                    }
                    new_args[i] = NULL;

                    if(output_redirection_flag == 0) {
                        int fd = open(output_redirection_file_name, O_WRONLY);
                        int tmp = dup(1);
                        close(1);
                        dup(fd);

                        return execv(bin_path, new_args);
                    } else if(output_redirection_flag == 1) {
                        int fd = open(output_redirection_file_name, O_RDONLY);
                        int tmp = dup(0);
                        close(0);
                        dup(fd);

                        return execv(bin_path, new_args);
                    }
                } 
                return execv(bin_path, words);
            } else {
                wait(0);
            }
            output_redirection_flag = -1;
        }
    } while (strcmp(command, "exit") != 0);
    return 0;
}
