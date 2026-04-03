#include <stdio.h>
#include "types.h"

OperationType check_operation_type(int argc, char *argv[]);
Status read_and_validate_args(char *argv[]);
Status skip_header(FILE *fptr);
Status do_reader(FILE *fptr);


