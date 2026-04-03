#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"

const char *get_frame_id(const char *option)
{
    if (strcmp(option, "-t") == 0) return "TIT2";
    if (strcmp(option, "-a") == 0) return "TPE1";
    if (strcmp(option, "-A") == 0) return "TALB";
    if (strcmp(option, "-y") == 0) return "TYER";
    if (strcmp(option, "-m") == 0) return "COMM";
    return NULL;
}

Status read_and_validate_editor_args(char *argv[])
{
    if (argv[2] == NULL || argv[3] == NULL || argv[4] == NULL)
        return e_failure;

    if (get_frame_id(argv[2]) == NULL)
        return e_failure;

    if (strstr(argv[4], ".mp3") == NULL)
        return e_failure;

    return e_success;
}

Status do_editor(FILE *src, const char *tag, const char *value)
{
    FILE *dest = fopen("temp.mp3", "wb");
    if (!dest)
        return e_failure;

    char header[10];
    fread(header, 1, 10, src);
    fwrite(header, 1, 10, dest);

 Status status = update_tag(src, dest, tag, value);

    fclose(dest);
    return status;
}

Status update_tag(FILE *src, FILE *dest, const char *tag, const char *value)
{
    char frame_id[5];
    unsigned char size_buf[4];
    char flags[2];
    unsigned int frame_size;
    char *buffer;
    int found = 0;

    while (1)
    {
        if (fread(frame_id, 1, 4, src) != 4)
            break;

        frame_id[4] = '\0';

        if (frame_id[0] == 0)
            break;

        fread(size_buf, 1, 4, src);

        frame_size = ((size_buf[0] & 0x7F) << 21) |
                     ((size_buf[1] & 0x7F) << 14) |
                     ((size_buf[2] & 0x7F) << 7)  |
                     (size_buf[3] & 0x7F);

        fread(flags, 1, 2, src);

        buffer = malloc(frame_size);
        fread(buffer, 1, frame_size, src);

        if (strcmp(frame_id, tag) == 0)
        {
            found = 1;

            unsigned int new_size = strlen(value) + 1;

            fwrite(frame_id, 1, 4, dest);

            unsigned char new_size_buf[4];
            new_size_buf[0] = (new_size >> 21) & 0x7F;
            new_size_buf[1] = (new_size >> 14) & 0x7F;
            new_size_buf[2] = (new_size >> 7)  & 0x7F;
            new_size_buf[3] = new_size & 0x7F;

            fwrite(new_size_buf, 1, 4, dest);
            fwrite(flags, 1, 2, dest);

            fputc(0, dest);
            fwrite(value, 1, strlen(value), dest);
        }
        else
        {
            fwrite(frame_id, 1, 4, dest);
            fwrite(size_buf, 1, 4, dest);
            fwrite(flags, 1, 2, dest);
            fwrite(buffer, 1, frame_size, dest);
        }

        free(buffer);
    }

    char ch;
    while (fread(&ch, 1, 1, src) == 1)
        fwrite(&ch, 1, 1, dest);

    if (!found)
        return e_failure;

    return e_success;
}