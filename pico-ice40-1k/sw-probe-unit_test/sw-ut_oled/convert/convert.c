#include <stdio.h>

int load_ppm(char *filename);

static unsigned char raw_img[32][128];

int main(int argc, char **argv)
{
	FILE *f_out;
	unsigned char buffer[256];
	int j;

	if (argc == 1)
	{
	        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
	        fprintf(stderr, "   Where <filename> if the ppm file source.\n");
	        return(-1);
	}
	if ( load_ppm(argv[1]) < 0)
	{
	        fprintf(stderr, "Error during source file load\n");
	        return(-1);
	}

	f_out = fopen("out.h", "w");

	/* Write precompiler test to avoid multiple includes */
	fprintf(f_out, "#ifndef IMG_H\n#define IMG_H\n");
	
	fprintf(f_out, "unsigned char img[512] = {\n");
	
	/* Compute line 0 (bottom) */
	for (j = 0; j < 128; j++)
	{
	        buffer[0] = 0;
	        buffer[0] |= (raw_img[24][j] << 7);
	        buffer[0] |= (raw_img[25][j] << 6);
	        buffer[0] |= (raw_img[26][j] << 5);
	        buffer[0] |= (raw_img[27][j] << 4);
	        buffer[0] |= (raw_img[28][j] << 3);
	        buffer[0] |= (raw_img[29][j] << 2);
	        buffer[0] |= (raw_img[30][j] << 1);
	        buffer[0] |= (raw_img[31][j] << 0);
	        fprintf(f_out, "0x%.2x, ", buffer[0]);
	}
	fprintf(f_out, "\n");
	
	/* Compute line 1 */
	for (j = 0; j < 128; j++)
	{
	        buffer[0] = 0;
	        buffer[0] |= (raw_img[16][j] << 7);
	        buffer[0] |= (raw_img[17][j] << 6);
	        buffer[0] |= (raw_img[18][j] << 5);
	        buffer[0] |= (raw_img[19][j] << 4);
	        buffer[0] |= (raw_img[20][j] << 3);
	        buffer[0] |= (raw_img[21][j] << 2);
	        buffer[0] |= (raw_img[22][j] << 1);
	        buffer[0] |= (raw_img[23][j] << 0);
	        fprintf(f_out, "0x%.2x, ", buffer[0]);
	}
	fprintf(f_out, "\n");
	
	/* Compute line 2 */
	for (j = 0; j < 128; j++)
	{
	        buffer[0] = 0;
	        buffer[0] |= (raw_img[8][j]  << 7);
	        buffer[0] |= (raw_img[9][j]  << 6);
	        buffer[0] |= (raw_img[10][j] << 5);
	        buffer[0] |= (raw_img[11][j] << 4);
	        buffer[0] |= (raw_img[12][j] << 3);
	        buffer[0] |= (raw_img[13][j] << 2);
	        buffer[0] |= (raw_img[14][j] << 1);
	        buffer[0] |= (raw_img[15][j] << 0);
	        fprintf(f_out, "0x%.2x, ", buffer[0]);
	}
	fprintf(f_out, "\n");
	
	/* Compute line 3 */
	for (j = 0; j < 128; j++)
	{
	        buffer[0] = 0;
	        buffer[0] |= (raw_img[0][j] << 7);
	        buffer[0] |= (raw_img[1][j] << 6);
	        buffer[0] |= (raw_img[2][j] << 5);
	        buffer[0] |= (raw_img[3][j] << 4);
	        buffer[0] |= (raw_img[4][j] << 3);
	        buffer[0] |= (raw_img[5][j] << 2);
	        buffer[0] |= (raw_img[6][j] << 1);
	        buffer[0] |= (raw_img[7][j] << 0);
	        fprintf(f_out, "0x%.2x", buffer[0]);
	        if (j != 127)
	                fprintf(f_out, ", ");
	}
	fprintf(f_out, "\n");
	/* Write the array end tag */
	fprintf(f_out,"};\n");
	fprintf(f_out,"#endif\n");
	fclose(f_out);
	
	return(0);
}

int load_ppm(char *filename)
{
	unsigned char buffer[256];
	FILE *f_in;
	int color;
	int i, j;
	
	f_in  = fopen(filename, "rb");
	if (f_in == 0)
	        return(-1);

        /* Try to read file singature */
	fgets((char *)buffer, 256, f_in);
	if ( (buffer[0] != 0x50) ||
	     (buffer[1] != 0x36) ||
	     (buffer[2] != 0x0A) )
	{
		fprintf(stderr, "Bad signature\n");
		return(-2);
	}
	
	/* Read (and ignore) image properties */
	fgets((char *)buffer, 256, f_in);
	fgets((char *)buffer, 256, f_in);
	fgets((char *)buffer, 256, f_in);

	/* Read the file content */
	for (i = 0; i < 32; i++)
	{
		for (j = 0; j < 128; j++)
		{
			fread(buffer, 1, 3, f_in);
			color = (buffer[0] + buffer[1] + buffer[2]) / 3;
			if (color > 0x80)
        			raw_img[i][j] = 0x00;
                        else
                                raw_img[i][j] = 0x01;
		}
	}
	fclose(f_in);
	
	return(0);
}
/* EOF */
