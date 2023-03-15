#include <global.h>
#include <isa.h>

FILE *_input_file = NULL;
FILE *_output_file = NULL;

int main(int argc, char** argv) {
        // Options:
        // -i = input file
        // -o = output file

        int input_file_name = -1;

        for (int i = 1; i < argc; i++) {
                if (strcmp("-i", argv[i]) == 0) _input_file = fopen(argv[input_file_name = ++i], "r");
                if (strcmp("-o", argv[i]) == 0) _output_file = fopen(argv[++i], "w");
        }

        ASSERT(_input_file != NULL);
        
        if (_output_file == NULL && input_file_name != -1) {
                char *tmp = malloc(strlen(argv[input_file_name]) + 4);
                strcpy(tmp, argv[input_file_name]);
                strcat(tmp + strlen(argv[input_file_name]), ".out");
                _output_file = fopen(tmp, "w");
                free(tmp);
        }

        ASSERT(_output_file != NULL);
        
        init_isa();
}