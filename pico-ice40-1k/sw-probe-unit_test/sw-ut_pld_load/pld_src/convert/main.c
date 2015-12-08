#include <stdio.h>

int main(int argc, char **argv)
{
  FILE *f_in, *f_out;
  char buffer[4];
  int count;
  
  f_in  = fopen("bitstream.bin", "rb");
  f_out = fopen("pld-image.h",   "w");
  
  fprintf(f_out, "const unsigned char pld_data[] = {");
  
  count = 0;
  while( ! feof(f_in) )
  {
    if ((count % 16) == 0)
      fprintf(f_out, "\r\n");
    
    fread(buffer, 1, 1, f_in);
    fprintf(f_out, "0x%.2X,", (unsigned char)buffer[0]);
    count ++;
    
    if( (count % 8) == 0)
      fprintf(f_out, " ");
  }
  fprintf(f_out, "\n };\n");
  
  fprintf(f_out, "const unsigned long pld_len = %d;\n", count - 1);
  fclose(f_in);
  fclose(f_out);
  
  printf("Finished. %d bytes read\n", count);
  return(0);
}
/* EOF */
