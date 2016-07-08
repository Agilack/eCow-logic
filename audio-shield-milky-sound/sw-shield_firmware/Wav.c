 /* This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation. (See COPYING.GPL for details.)
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
 
#include "hardware.h"
#include "spi.h"
#include "uart.h"

void data(const unsigned char *bitstream, u32 len)
{
	int i;
	int a;
	static char data=0;
	static int data_length=0;
	char [10] data_length_string;
	//maybe add a step varaiable allowing to restart from the right step if the lenght is cut short in the middle
	while (i<len)
	{	
		if (data==0)
		{
			if (*bitstream==0x63)// find "data" in the wav file
			{
				bitstream++;
				i++;
				if (*bitstream==0x61)
				{
					bitstream++;
					i++;
					if (*bitstream==0x74)
					{
						bitstream++;
						i++;
						if (*bitstream==0x61)
						{
							for(a=3; a>0; a--)
							{
								bitstream++;
								i++;
								data_length=data_length+(*bitstream<<8*a);
							}
							sprintf(data_length_string,"%.9g",data_length);
							uart_puts(data_length_string); uart_puts("\r\n");
							data=1;
						}
					}
				}
			}	
		}
		else
		{
			if (data_length>0)
			{
				spi_wr(*bitstream);
				bitstream++;
			}
			else { data=0;}
		}
	}
	return;
}