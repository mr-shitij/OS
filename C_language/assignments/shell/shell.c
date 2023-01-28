#include<stdio.h>
#include<string.h>
#include<ctype.h>

#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>

typedef struct history {
    int pid;
    char* command;
} history;

void history_add(history **h, char* bin_path) {
    *h = malloc(sizeof(history));
    (*h)->pid = getpid();
    (*h)->command = malloc(sizeof(char) * strlen(bin_path));
    strcpy((*h)->command, bin_path);
}

typedef struct queue_node {
	struct queue_node* next;
	void* data;
}queue_node;
typedef queue_node* queue_list;


void queue_init(queue_list *list) {
	*list = NULL;
}

void queue_enqueue(queue_list *list, void* data) { // Insert At Last
	queue_node* new_node = malloc(sizeof(queue_node));	
	new_node->next = NULL;
	new_node->data = data;
	
	if(new_node == NULL) {
		return;
	}

	if(*list  == NULL) {
		*list = new_node;
	} else {
		queue_node *last_node = *list;
		while(last_node->next != NULL) {
			last_node = last_node->next;
		}
		last_node->next = new_node;
	}
}

int queue_dequeue(queue_list *list) { // Remove From First
	if(*list == NULL) {
		return 1; // queue underflow
	} else {
		queue_node* second_node = NULL;
		
		if((*list)->next != NULL)
			second_node = (*list)->next;
		
		free((*list)->data);
		free(*list);
		*list = second_node;		
	}
	return 0;
}

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


int pid = -1; //initially set to less than 0 so that if no fork got created then program get terminated ..!!
int current_directory_flag = 0; // 0 = CWD, 1 = SHORT
int output_redirection_index = -1; // -1 for nothing else the index of rediretion sign
int output_redirection_flag = -1; // -1 = NULL, 0 = >, 1 = <
int wc;

char* output_redirection_file_name = NULL;
char** words = NULL; // to store the tokens
char command[1024];
char current_path[1024];

queue_list queue;
queue_list history_list;
queue_list path;


int pipe_count = 0;
queue_list pipe_commands;


int take_input() {
    if(scanf(" %[^\t\n]s", command) == EOF)
        return -1;

    trimString(command);
}

void print_path() {
    if(current_directory_flag == 0)
        getcwd(current_path, sizeof(current_path));

    printf("\n%s>",current_path);
}

int check_for_build_in_commands() {
    if(strcmp(command, "exit") == 0) return 0;
    if(strstr(command, "PS1=\"") != NULL) return 1;
    else if (strstr(command, "history") != NULL) return 2;
    else if(strstr(command, "PATH=") != NULL) return 3;
    return 4;
}

int built_in_command_processor(int command_id) {
    int len;
    switch(command_id) {
        case 0:
            return 0; // For exit
        case 1:
            if(strstr(command, "PS1=\"\\w$\"") != NULL) {
                current_directory_flag = 0;
                return 1; // For Continue
            }
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
                return 1;
            }
            break;
        case 2:
            if(strstr(command, "history -c") != NULL) {                
                while(history_list != NULL) {
                    queue_dequeue(&history_list);
                }
            } else {
                queue_node* node = history_list;
                while(node != NULL) {
                    history *h = (history*)(node->data);
                    printf("%d %s\n", h->pid, h->command);
                    node = node->next;
                }
            }
            return 1; // For Continue
        case 3:
            len = strlen(command);
            if(len < 5) {
                printf("Invalid Command ..!!\n");
            } else {
                // clear the path for new
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
                return 1; // For Continue
            }
            break;
    }
    return 2;

}

void create_tokens() {
    if(words != NULL)
        free(words);
    words = malloc(sizeof(char*) * (wc + 1));
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

}

int process(int* pipe_d) {
    int i;
    char bin_path[1024];

    switch(built_in_command_processor(check_for_build_in_commands())){
        case 0:
            return 0; //break
        case 1:
            return 1; // continue;
    }

    wc = get_pos_and_count_of_words(&queue, command);
    if(wc == -1)
        printf("Invalid ..!!\n");
    else {
        create_tokens(&wc);
        if(wc == 2) {
            if(check_for_cd(words[0], words[1]))
                return 1;
        } else if(wc > 2) {
            output_redirection_index = 0;
            int i, temp = 0;
            char* fuck = NULL;
            int len = 0;
            for(i = 0; i < wc; i++) {
                if(strcmp(words[i], ">") == 0) {
                    output_redirection_flag = 0;
                    break;
                }
                if(strcmp(words[i], "<") == 0) {
                    output_redirection_flag = 1;
                    break;
                } 
                if(strcmp(words[i], "|") == 0) {
                    pipe_count += 1;

                    for(int j = temp; j < i; j++) {
                        len += strlen(words[j]);
                        if(fuck == NULL) {
                            fuck = malloc(sizeof(char) * len);
                            strcpy(fuck, words[j]);
                        }
                        else{
                            fuck = realloc(fuck, sizeof(char) * len);
                            strcat(fuck, words[j]);
                        }
                        strcat(fuck, " ");
                    }
                    temp = i;

                    // printf("Temp : %d, CMP : Command : %s, i : %d\n", temp, fuck, i);
                    queue_enqueue(&pipe_commands, fuck);
                    fuck = NULL;
                    break;
                }
                // printf("W : %s\n", words[i]);
            }
            if(pipe_count > 0) {
                fuck = NULL;
                for(int j = temp + 1; j < wc; j++) {
                    len += strlen(words[j]);
                    if(fuck == NULL) {
                        fuck = malloc(sizeof(char) * len);
                        strcpy(fuck, words[j]);
                    }
                    else {
                        fuck = realloc(fuck, sizeof(char) * len);
                        strcat(fuck, words[j]);
                    }
                    strcat(fuck, " ");
                }
                temp = i;

                // printf("PIPE : Command : %s, i : %d\n", fuck, i);
                queue_enqueue(&pipe_commands, fuck);
                fuck = NULL;
            }

            if(output_redirection_flag != -1) {
                output_redirection_index = i;
                if(output_redirection_index + 1 > wc){
                    printf("Invalid Redirection ..!! : %d\n", output_redirection_index);
                    return 1;
                }
                output_redirection_file_name = words[output_redirection_index + 1];
            }
        }

        if(pipe_count == 0) {
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
                return 1;
            }
        }

        pid = fork();
        if(pid < 0) {
            printf("\nnot able to create child ..!!");
        }
        else if(pid == 0) {
            if(pipe_count != 0) {
                int temp_output_fd = dup(1);
                int temp_input_fd = dup(0);

                for(i = 0; i < pipe_count; i++) {
                    if(pipe_d == NULL) {
                        pipe_d = malloc(sizeof(int) * 2);
                        pipe(pipe_d);
                    }

                    int fd = fork();
                    if(fd < 0)
                        printf("Can not create fork..!!");
                    else if(fd == 0) {
                        close(1);
                        dup(pipe_d[1]);

                        close(pipe_d[0]);
                        strcpy(command, (char*)pipe_commands->data);
                        queue_dequeue(&pipe_commands);
                        // command = "new commadn to execute";

                        pipe_count = 0;
                        printf("Calling Main In Child For : %s\n", command);
                        main(NULL);
                        return 0;
                    } else {
                        if(pipe_commands != NULL) {
                            close(0);
                            dup(pipe_d[0]);

                            close(pipe_d[1]);

                            strcpy(command, (char*)pipe_commands->data);
                            queue_dequeue(&pipe_commands);
                            // command = "new commadn to execute";

                            printf("Calling Main In Parent For : %s\n", command);
                            main(NULL);
                        }
                        else {
                            // restore std. ip/op
                            close(1);
                            dup(temp_output_fd);
                            close(0);
                            dup(temp_input_fd);

                            close(temp_input_fd);
                            close(temp_output_fd);
                        }
                    }
                }

            }



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

                    execv(bin_path, new_args);
                } else if(output_redirection_flag == 1) {
                    int fd = open(output_redirection_file_name, O_RDONLY);
                    int tmp = dup(0);
                    close(0);
                    dup(fd);

                    execv(bin_path, new_args);
                }
            } 
            execv(bin_path, words);
        } else {
            history *h;
            history_add(&h, bin_path);
            queue_enqueue(&history_list, h);

            wait(0);
        }
        output_redirection_flag = -1;
    }
    return 1;
}

int main() {
    queue_init(&path);
    queue_init(&queue);
    queue_init(&history_list);
    queue_init(&pipe_commands);

    do {

        print_path();
        if(take_input() == -1) break;

        switch(process(NULL)){
            case 0:
                break; //break
            case 1:
                continue;; // continue;
        }

    } while (strcmp(command, "exit") != 0);
    return 0;
}
