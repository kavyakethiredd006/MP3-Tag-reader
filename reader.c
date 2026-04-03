#include<stdio.h>
#include "types.h"
#include<string.h>
#include<stdlib.h>

Status read_and_validate_args(char *argv[])
{
    if(argv[2]==NULL)
    {
        printf(".mp3 file not passed\n");
        return e_failure;
    }
    if(strstr(argv[2],".mp3")==NULL)
    {
        printf("Invalid file name\n");
        return e_failure;
    }
  FILE *fptr = fopen(argv[2],"r");
  if(fptr==NULL)
  {
    printf("File doesn't exist\n");
    return e_failure;
  }
  
  fseek(fptr,0,SEEK_END);
  //printf("file size: %lu\n",ftell(fptr));

  fclose(fptr);
  return e_success;
}

Status skip_header(FILE *fptr)
{
  //printf("Offset before skipping header: %lu\n",ftell(fptr));
  fseek(fptr,10,SEEK_SET);
 // printf("Skipped 10 bytes Header\n");
 // printf("Current Offset position: %lu\n",ftell(fptr));
  return e_success; 
}

Status do_reader(FILE *fptr)
{
  while(1)
  {
    char tag[5];
    if(fread(tag,1,4,fptr)!=4)
    {
      break;
    }
    tag[4]='\0';
    if(tag[0]==0)
    return e_failure;
 
    //printf("INFO: Read 4 bytes tag\n");
    //printf("INFO: offset at %lu\n",ftell(fptr));

    char size[4];
    if(fread(size,1,4,fptr)!=4)
    {
      return e_failure;
    }
  
    //printf("INFO: Size of tag\n");
    //printf("INFO: offset is at %lu\n",ftell(fptr));

   /* int tag_size;
    tag_size=(size[0]<<24 | size[1]<<16 | size[2]<<8 | size[3]);*/
    int tag_size =
    ((size[0] & 0x7F) << 21) |
    ((size[1] & 0x7F) << 14) |
    ((size[2] & 0x7F) << 7)  |
    (size[3] & 0x7F);
    //printf("INFO: Convert to little endian\n");
    //printf("INFO: Tag size- %d\n",tag_size);

    fseek(fptr,2,SEEK_CUR);
    //printf("INFO: Skip 2 bytes flag\n");
    //printf("INFO: offset after flag skip %lu\n",ftell(fptr));

    char *tag_data = malloc(tag_size + 1);
    if(!tag_data)
    return e_failure;

    if(fread(tag_data, 1, tag_size, fptr)!=tag_size)
    {
      free(tag_data);
      return e_failure;
    }
    tag_data[tag_size] = '\0'; 

     if(strcmp(tag, "COMM") == 0)
        {
            printf("%s - ", tag);

            int i = 4;   // Skip Encoding + Language

            while(i < tag_size && tag_data[i] != '\0')
                i++;

            i++;   // Skip NULL

            printf("%s\n", &tag_data[i]);
            free(tag_data);
            continue;
        }

    if ( strcmp(tag,"TIT2") == 0 || 
         strcmp(tag,"TALB") == 0 ||
         strcmp(tag,"TYER") == 0 || 
         strcmp(tag,"COMM") == 0 ||
         strcmp(tag,"TPE1") == 0 || 
         strcmp(tag,"TCON") == 0)
         {
          /*printf("%s - ",tag); 
     //printf("INFO: Offset after reading tag data: %lu\n", ftell(fptr));
           for(int i=1;i<tag_size;i++)
           {
            if(tag_data[i]!=0)
            printf("%c",tag_data[i]); 
           }
           printf("\n");*/
          printf("%s - ", tag);

unsigned char encoding = tag_data[0];

if (encoding == 0x00 || encoding == 0x03)   
{
    printf("%s", &tag_data[1]);
}
else if (encoding == 0x01 || encoding == 0x02)   
{
    int start = 1;

    // Skip BOM if present
    if(tag_data[1] == (char)0xFF && tag_data[2] == (char)0xFE)
        start = 3;

    for(int i = start; i < tag_size; i += 2)
    {
        if(tag_data[i] != 0)
            printf("%c", tag_data[i]);
    }
}
printf("\n");

free(tag_data);
         }
  }
      return e_success;
}


