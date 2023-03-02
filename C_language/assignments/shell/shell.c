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

void queue_enqueue_first(queue_list *list, void* data) { // Insert At Last
	queue_node* new_node = malloc(sizeof(queue_node));
	new_node->next = NULL;
	new_node->data = data;

	if(new_node == NULL) {
		return;
	}

	if(*list  == NULL) {
		*list = new_node;
	} else {
        new_node->next = *list;
        *list = new_node;
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


queue_list jobs_list;

int pipe_count = 0;
int* pipe_d = NULL;
queue_list pipe_commands;


int take_input() {
    printf("TAKING IP: %d \n", getpid());
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
    else if(strcmp(command, "fg") == 0) return 4;
    return 5;
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
        case 4:
            if(jobs_list == NULL)
                break;
            int pid = *((int*)jobs_list->data), status;
            queue_dequeue(&jobs_list);
            kill(pid, SIGCONT);
            waitpid(pid, &status, 0);
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

int custom_execv(const char *pathname, char *const argv[]) {
    if(fork() == 0) {
        execv(pathname, argv);
    } else {
        wait(NULL);
        exit(0);
    }
}

void handle_sigtstp(int sig) {
    printf("HELOOOOOOOOO >>>>>> %d >>>> %d >>>\n\n\n\n", getppid(), getpid());
    kill(SIGSTOP, getpid());
}

void handle_sigtint(int sig) {
    printf("KILE");
    kill(SIGINT, getppid());
}

// If it is executd by child then 1 else 0;
int process(int is_child) {
    printf("Process Of: %d By: %d, P1\n", getpid(), is_child);
    int i;
    char bin_path[1024];

    switch(built_in_command_processor(check_for_build_in_commands())){
        case 0:
            return 0; //break
        case 1:
            return 1; // continue;
    }

    printf("Process Of: %d By: %d, P2\n", getpid(), is_child);
    wc = get_pos_and_count_of_words(&queue, command);
    if(wc == -1)
        printf("Invalid ..!!\n");
    else {
        printf("Process Of: %d By: %d, P3\n", getpid(), is_child);
        create_tokens(&wc);
        printf("Process Of: %d By: %d, P4\n", getpid(), is_child);
        if(wc == 2) {
            if(check_for_cd(words[0], words[1]))
                return 1;
        } else if(wc > 2) {
            printf("Process Of: %d By: %d, P5\n", getpid(), is_child);
            output_redirection_index = 0;
            int i, temp = 0;
            char* st = NULL;
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
                    printf("Process Of: %d By: %d, P6\n", getpid(), is_child);
                    pipe_count += 1;

                    for(int j = temp; j < i; j++) {
                        len += strlen(words[j]);
                        if(st == NULL) {
                            st = malloc(sizeof(char) * len);
                            strcpy(st, words[j]);
                        }
                        else{
                            st = realloc(st, sizeof(char) * len);
                            strcat(st, words[j]);
                        }
                        strcat(st, " ");
                    }
                    temp = i;

                    // printf("Temp : %d, CMP : Command : %s, i : %d\n", temp, st, i);
                    queue_enqueue(&pipe_commands, st);
                    st = NULL;
                    break;
                }
                // printf("W : %s\n", words[i]);
            }
            if(pipe_count > 0) {
                printf("Process Of: %d By: %d, P7\n", getpid(), is_child);
                st = NULL;
                for(int j = temp + 1; j < wc; j++) {
                    len += strlen(words[j]);
                    if(st == NULL) {
                        st = malloc(sizeof(char) * len);
                        strcpy(st, words[j]);
                    }
                    else {
                        st = realloc(st, sizeof(char) * len);
                        strcat(st, words[j]);
                    }
                    strcat(st, " ");
                }
                temp = i;

                // printf("PIPE : Command : %s, i : %d\n", st, i);
                queue_enqueue(&pipe_commands, st);
                st = NULL;
            }

            if(output_redirection_flag != -1) {
                printf("Process Of: %d By: %d, P8\n", getpid(), is_child);
                output_redirection_index = i;
                if(output_redirection_index + 1 > wc){
                    printf("Invalid Redirection ..!! : %d\n", output_redirection_index);
                    return 1;
                }
                output_redirection_file_name = words[output_redirection_index + 1];
            }
            printf("Process Of: %d By: %d, P9\n", getpid(), is_child);
        }

        if(pipe_count == 0) {
            printf("Process Of: %d By: %d, P10\n", getpid(), is_child);
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
            printf("Process Of: %d By: %d, P11\n", getpid(), is_child);
        }

        printf("Process Of: %d By: %d, P12\n", getpid(), is_child);
        pid = fork();

        int* p = malloc(sizeof(int));
        *p = getpid();
        queue_enqueue_first(&jobs_list, p);

        if(pid < 0) {
            printf("\nnot able to create child ..!!");
        }
        else if(pid == 0) {
            signal(SIGTSTP, handle_sigtstp);

            printf("Process Of: %d By: %d, P13\n", getpid(), is_child);
            if(pipe_count != 0) {
                printf("Of: %d, Pipe Count : %d\n", getpid(), pipe_count);
                int temp_output_fd = dup(1);
                int temp_input_fd = dup(0);
                int temp_pipe_count = pipe_count;

                int io_flag = 0, temp_pip_flag = 1;
                pipe_count = 0;

                for(i = 0; i < temp_pipe_count + 1; i++) {
                    if(pipe_d == NULL) {
                        pipe_d = malloc(sizeof(int) * 2);
                        pipe(pipe_d);
                    }
                    strcpy(command, (char*)pipe_commands->data);
                    queue_dequeue(&pipe_commands);

                    printf("Process Of: %d By: %d, P14\n", getpid(), is_child);
                    int fd = fork();
                    if(fd < 0)
                        printf("Can not create fork..!!");
                    else if(fd == 0) {
                        printf("IN CHILD\n");
                        printf("Calling  Of: %d Main In Child For : %s\n", getpid(), command);

                        if(io_flag == 0) {
                            close(pipe_d[0]);
                            close(1);

                            dup(pipe_d[1]);
                            close(pipe_d[1]);

                            printf("Starting for output Of: %d\n",getpid());
                            process(1);

                            printf("MIDDLE for output Of: %d\n",getpid());
                            close(0);
                            wait(NULL);

                            printf("Ending for output Of: %d\n", getpid());

                            close(1);
                        } else {
                            close(pipe_d[1]);
                            close(0);
                            dup(pipe_d[0]);
                            close(pipe_d[0]);

                            printf("Starting for input Of: %d\n", getpid());
                            process(1);

                            printf("Middle for input Of: %d\n", getpid());
                            close(1);
                            //wait(NULL);
                            
                            printf("Ending for input Of: %d\n", getpid());
                        }
                        printf("After Process IN CHI Of: %d\n", getpid());
                        return 0;
                    }
                    else {
                        int status;
                        printf("Parent waiting: %d\n", getpid());
                        waitpid(fd, &status, WNOHANG);
                        printf("Parent Waiting Done: %d\n", getpid());
                        if(io_flag == 0) 
                            io_flag = 1;
                        else
                            io_flag = 0;

                        if(temp_pip_flag % 2 == 0) {
                            free(pipe_d);
                            pipe_d = NULL;
                        }
                    }
                    temp_pip_flag++;
                }
                printf("Process Of: %d By: %d, P15\n", getpid(), is_child);

                // int status;
                pid_t wpid;
                printf("WAITING  Of: %d FOR CHILDS\n\n", getpid());
                while ((wpid = wait(NULL)) > 0);
                // waitpid(pid, &status, 0);
                printf("PROCESS Of: %d FOR CHILDS EXITED\n\n", getpid());

                close(1);
                dup(temp_output_fd);
                close(0);
                dup(temp_input_fd);

                close(temp_input_fd);
                close(temp_output_fd);

                return 0;
            }

            printf("Process Of: %d By: %d, P16\n", getpid(), is_child);

            // combie_strings_upto_offsets(final_commad, bin_path, command, strlen(bin_path), strlen(words[0]));
            if(output_redirection_flag != -1) {
                printf("Process Of: %d By: %d, P17\n", getpid(), is_child);
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

                    custom_execv(bin_path, new_args);
                } else if(output_redirection_flag == 1) {
                    int fd = open(output_redirection_file_name, O_RDONLY);
                    int tmp = dup(0);
                    close(0);
                    dup(fd);

                    custom_execv(bin_path, new_args);
                    return 0;
                }
                printf("Process Of: %d By: %d, P18\n", getpid(), is_child);
            }
            printf("Process Of: %d By: %d, P19\n", getpid(), is_child);
            custom_execv(bin_path, words);
            return 0;
        } else {
            printf("Process Of: %d By: %d, P20\n", getpid(), is_child);
            history *h;
            history_add(&h, bin_path);
            queue_enqueue(&history_list, h);

            if(!is_child) {
                printf("Process Of: %d By: %d, P21\n", getpid(), is_child);
                int status;
                printf("WAITING  Of: %d FOR SIGNLA\n\n", getpid());
                waitpid(pid, &status, 0);
                printf("PROCESS Of: %d EXITED\n\n", getpid());
                if(WIFSTOPPED(status)) {
                    printf("STOP Of: %d SIGNLA\n\n", getpid());
                    int* id = malloc(sizeof(int));
                    *id = pid;
                    queue_enqueue(&jobs_list, id);
                }
                printf("Process Of: %d By: %d, P22\n", getpid(), is_child);
            }
            printf("Process Of: %d By: %d, P23\n", getpid(), is_child);
        }
        printf("Process Of: %d By: %d, P24\n", getpid(), is_child);
        output_redirection_flag = -1;
    }
    if(is_child){
        printf("Process Of: %d By: %d, P25\n", getpid(), is_child);
        return 0;
    }
    else {
        printf("Process Of: %d By: %d, P26\n", getpid(), is_child);
        pipe_count = 0;
        while(pipe_commands != NULL) {
            queue_node* node = pipe_commands;
            free(node);
            pipe_commands = pipe_commands->next;
        }
        pipe_commands = NULL;
        return 1;
    }
    printf("Process Of: %d By: %d, P27\n", getpid(), is_child);
}

int main() {
    signal(SIGINT, handle_sigtint);

    queue_init(&path);
    queue_init(&queue);
    queue_init(&history_list);
    queue_init(&jobs_list);
    queue_init(&pipe_commands);


    int* p = malloc(sizeof(int));
    *p = getpid();
    queue_enqueue_first(&jobs_list, p);

    do {
        print_path();
        if(take_input() == -1) break;

        switch(process(0)){
            case 0:
                exit(0); //break
            case 1:
                continue;; // continue;
        }

    } while (strcmp(command, "exit") != 0);
    printf("EXITING ..!!\n");
    return 0;
}
