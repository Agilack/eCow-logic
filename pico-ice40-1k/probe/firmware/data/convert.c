/**
 * eCow-logic - Tool - Merge data files and make a flash image
 *
 * Copyright (c) 2016 Saint-Genest Gwenael <gwen@agilack.fr>
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation. (See COPYING.GPL for details.)
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int insert_file(char *path, char *name);

char *image;
long  current_offset;
long  current_entry;

int main(int argc, char **argv)
{
	FILE *f_out;

	f_out = fopen("ecow.mem", "wb");
	if (f_out == NULL)
	{
		fprintf(stderr, "ERROR: could not open ecow.mem file.\n");
		return (-1);
	}
	
	image = malloc(2 * 1024 * 1024);
	if (image == NULL)
	{
		fprintf(stderr, "ERROR: could not allocate memory for image.\n");
		fclose(f_out);
		return(-1);
	}
	current_offset = 1024;
	current_entry = 0;
	
	insert_file("web/index.html", "home.htm");
	insert_file("web/styl.css",   "styl.css");
	insert_file("web/ecow.png",   "ecow.png");
	insert_file("web/home.png",   "home.png");
	insert_file("web/rand.png",   "rand.png");
	insert_file("web/qust.png",   "qust.png");
	
	fwrite(image, 1, current_offset, f_out);
	
	fclose(f_out);

	return(0);
}

int insert_file(char *path, char *name)
{
	FILE  *f_in = NULL;
	struct stat sb;
	char  *buffer = NULL;
	int    len;
	
	if ( stat(path, &sb) != 0)
		goto err_out;
	/* Allocate a buffer for file content */
	buffer = malloc(sizeof(char) * sb.st_size);
	if (buffer == NULL)
		goto err_out;
	/* Read the file content */
	f_in = fopen(path, "rb");
	if (f_in == NULL)
		goto err_out;
	len = fread(buffer, 1, sb.st_size, f_in);
	fclose(f_in);
	/* Copy datas into image */
	memcpy(image + current_offset, buffer, len);
	
	/* Insert new entry into table of contents */
	strncpy(image + current_entry, name, 8);
	memcpy(image + current_entry + 8, &current_offset, 4);
	memcpy(image + current_entry +12, &sb.st_size,     4);
	current_entry += 16;

	printf(" * File inserted %s size %.6X offset %.8X\n", name, (int)sb.st_size, (int)current_offset);
	
	/* Update image offset */
	current_offset += sb.st_size;
	if (current_offset & 0x0F)
		current_offset = (current_offset & 0xFFFFF0) + 16;
	
	return(0);
	
err_out:
	if (buffer)
		free(buffer);
	if (f_in)
		fclose(f_in);
	return(-1);
}
/* EOF */
