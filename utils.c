#include "utils.h"

char * read_file(const char * filename)
{
  FILE * fp;
  char ch;
  char * output;
  long int filesize;
  int i;
  printf("Opening file %s to read\n", filename);
  fp = fopen(filename, "r+");
  if (fp == NULL)
    {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
    }
  /* get size of file */
  fseek(fp, 0L, SEEK_END);
  filesize = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  output = (char *) malloc(filesize + 1);
  i = 0;
  while((ch = fgetc(fp)) != EOF)
    {
      output[i] = ch;
      i++;
    }
  output[i] = '\0';
  fclose(fp);
  //printf("Output is \n%s",output);
  /*someone should free the memory*/
  return output;
}
