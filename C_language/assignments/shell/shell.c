#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

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

void copy_string_from_offset(char* dest_str, char* source_str, int offset) {
    int dest_offset = 0;
    while(source_str[offset] != '\0') {
        dest_str[dest_offset] = source_str[offset];
        offset++;
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

int check_for_cd(char* command, char*  arguments) {
    if(strcmp(command, "cd") != 0) {
        return 0;   
    }
    // printf("\n\ncommand: %s, argument: %s\n\n", command, arguments);
    chdir(arguments);
    return 1;
}

int main() {
    int pid = -1; //initially set to less than 0 so that if no fork got created then program get terminated ..!!

    char command[1024];
    char final_commad[1024];

    char bin_path[1024] = "/bin/";
    char arguments[1024];
    char current_path[1024];
    do {
        getcwd(current_path, sizeof(current_path));
        printf("\n%s>",current_path);
        scanf(" %[^\t\n]s", command);

        if(strcmp(command, "exit") == 0) return 0;

        int pos = get_white_spce_pos(command);
        if(pos != -1) {
            copy_string_from_offset(arguments, command, pos + 1);
            command[pos] = '\0';
            if(check_for_cd(command, arguments)) {
                continue;
            }
        }

        pid = fork();
        if(pid < 0) {
            printf("\nnot able to create child ..!!");
        }
        else if(pid == 0) {
            if(pos == -1) {
                combie_strings_upto_offsets(final_commad, bin_path, command, strlen(bin_path), strlen(command));
                execl(final_commad, command, NULL);
            }

            combie_strings_upto_offsets(final_commad, bin_path, command, strlen(bin_path), pos);
            command[pos] = '\0';

            execl(final_commad, command, arguments, NULL);
        } else {
            wait(0);
        }
    } while (strcmp(command, "exit") != 0);
    return 0;
}
