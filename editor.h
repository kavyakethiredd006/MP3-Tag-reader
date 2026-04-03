#include <stdio.h>
#include "types.h"

Status read_and_validate_editor_args(char *argv[]);
Status do_editor(FILE *src, const char *tag, const char *value);
Status update_tag(FILE *src, FILE *dest, const char *tag, const char *value);
const char *get_frame_id(const char *option);

