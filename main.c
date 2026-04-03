#include <stdio.h>
#include <string.h>
#include "reader.h"
#include "editor.h"
#include "types.h"

int main(int argc, char *argv[]) 
{
    OperationType ret = check_operation_type(argc, argv);

    if(ret == e_unsupported)
    {
        printf("Invalid arguments\n");
        return 1;
    }

    
    if(ret == e_reader)
    {
        ret = read_and_validate_args(argv);
        if(ret == e_failure)
        {
            printf("Invalid reader arguments\n");
            return 1;
        }

        FILE *fptr = fopen(argv[2], "rb");
        if(fptr == NULL)
        {
            printf("File open failed\n");
            return 1;
        }

        ret = skip_header(fptr);
        if(ret == e_failure)
        {
            printf("Skip header failed\n");
            fclose(fptr);
            return 1;
        }

        ret = do_reader(fptr);
        fclose(fptr);

        if(ret == e_success)
        {
            printf("Read successful\n");
            return 0;
        }
    }

    if(ret == e_editor)
    {
        ret = read_and_validate_editor_args(argv);
        if(ret == e_failure)
        {
            printf("Invalid editor arguments\n");
            return 1;
        }

        FILE *fptr = fopen(argv[4], "rb");
        if(fptr == NULL)
        {
            printf("File open failed\n");
            return 1;
        }
            const char *frame = NULL;

             if (strcmp(argv[2], "-t") == 0)
                frame = "TIT2";
             else if (strcmp(argv[2], "-a") == 0)
             frame = "TPE1";
            else if (strcmp(argv[2], "-A") == 0)
              frame = "TALB";
            else if (strcmp(argv[2], "-y") == 0)
              frame = "TYER";
            else if (strcmp(argv[2], "-m") == 0)
             frame = "COMM";
            else
            {
              printf("Invalid edit option\n");
              fclose(fptr);
              return 1;
            }
        
        ret = do_editor(fptr, frame, argv[3]);
        fclose(fptr);

        if(ret == e_success)
        {
            remove(argv[4]);
            rename("temp.mp3",argv[4]);
            printf("Tag updated successfully\n");
            return 0;
        }
        else
        {
            printf("Failed to update tag\n");
            return 1;
        }
    }

    return 0;
}

OperationType check_operation_type(int argc, char *argv[])
{
    if (argv[1] == NULL)
        return e_unsupported;

    if (strcmp(argv[1], "-r") == 0)
        return e_reader;

    if (strcmp(argv[1], "-e") == 0)
        return e_editor;

    return e_unsupported;
}
