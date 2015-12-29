/**
 * eCow-logic - Embedded probe main firmware
 *
 * This file is based on httpParser.c from Wiznet ioLibrary
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation. (See COPYING.GPL for details.)
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "libc.h"
#include "socket.h"
#include "httpParser.h"
#include "uart.h"

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
uint8_t BUFPUB[256];

/*****************************************************************************
 * Private functions
 ****************************************************************************/
static uint8_t C2D(uint8_t c); 												/* Convert a character to HEX */

/**
 @brief	convert escape characters(%XX) to ASCII character
 */ 
void unescape_http_url(
	char * url	/**< pointer to be converted ( escape characters )*/
	)
{
	int x, y;

	for (x = 0, y = 0; url[y]; ++x, ++y) {
		if ((url[x] = url[y]) == '%') {
			url[x] = C2D(url[y+1])*0x10+C2D(url[y+2]);
			y+=2;
		}
	}
	url[x] = '\0';
}


/**
 @brief	make response header such as html, gif, jpeg,etc.
 */ 
void make_http_response_head(
	char * buf, 	/**< pointer to response header to be made */
	char type, 	/**< response type */
	uint32_t len	/**< size of response header */
	)
{
	char * head;
	char tmp[10];
			
	/*  file type*/
	if 	(type == PTYPE_HTML) 		head = RES_HTMLHEAD_OK;
	else if (type == PTYPE_GIF)		head = RES_GIFHEAD_OK;
	else if (type == PTYPE_TEXT)	head = RES_TEXTHEAD_OK;
	else if (type == PTYPE_JPEG)	head = RES_JPEGHEAD_OK;
	else if (type == PTYPE_FLASH)	head = RES_FLASHHEAD_OK;
	else if (type == PTYPE_XML) 	head =  RES_XMLHEAD_OK;
	else if (type == PTYPE_CSS) 	head = RES_CSSHEAD_OK;
	else if (type == PTYPE_JSON)	head = RES_JSONHEAD_OK;
	else if (type == PTYPE_JS)		head = RES_JSHEAD_OK;
	else if (type == PTYPE_CGI)		head = RES_CGIHEAD_OK;
	else if (type == PTYPE_PNG)		head = RES_PNGHEAD_OK;
	else if (type == PTYPE_ICO)		head = RES_ICOHEAD_OK;
	else if (type == PTYPE_TTF)		head = RES_TTFHEAD_OK;
	else if (type == PTYPE_OTF)		head = RES_OTFHEAD_OK;
	else if (type == PTYPE_WOFF)	head = RES_WOFFHEAD_OK;
	else if (type == PTYPE_EOT)		head = RES_EOTHEAD_OK;
	else if (type == PTYPE_SVG)		head = RES_SVGHEAD_OK;
//#ifdef _HTTPPARSER_DEBUG_
	else
	{
		head = NULL;
		uart_puts("\r\n\r\n-MAKE HEAD UNKNOWN-\r\n");
	}
//#else
//	else head = NULL;
//#endif	

//GSG	sprintf(tmp, "%ld", len);
	strcpy(buf, head);
	strcat(buf, tmp);
	strcat(buf, "\r\n\r\n");
}

/**
 @brief	parse http request from a peer
 */ 
void parse_http_request(st_http_request * request)
{
  char *token;
  char *pnt;
  
  pnt = (char *)request->buffer;
  /* Search the end of header */
  while(pnt)
  {
    if ( (pnt[0] == 0x0d) && (pnt[1] == 0x0a) &&
         (pnt[2] == 0x0d) && (pnt[3] == 0x0a) )
    {
    	/* Get a pointer on HTTP body (after header) */
    	request->body = (uint8_t *)(pnt + 4);
    	/* Cut the string between header and body */
    	pnt[0] = 0;
    	/* Compute header length */
    	request->header_len = ( (u32)pnt - (u32)request->buffer );
    	break;
    }
    /* Search the next CR */
    pnt = strchr(pnt + 1, 0x0d);
  }
  /* If no cr-lf-cr-lf found, bad header */
  if (pnt == 0)
  	goto parse_error;
  
  token = 0;
  
  pnt = strchr((char *)request->buffer, ' ');
  if (pnt == 0)
  	goto parse_error;
  *pnt = 0; /* Cut the string */
  
  token = (char *)request->buffer;
  
  if(!strcmp(token, "GET") || !strcmp(token, "get"))
    request->METHOD = METHOD_GET;
  else if (!strcmp(token, "POST") || !strcmp(token, "post"))
    request->METHOD = METHOD_POST;
  else
    goto parse_error;

  /* Now, search for URI */
  
  token = pnt;
  pnt = strchr(pnt + 1, ' ');
  if (pnt == 0)
  	goto parse_error;
  *pnt = 0; /* Cut the string */
  token++;

  // ToDo : add copy length protection to avoid overflow !!
  strcpy((char *)request->URI, token);
  
  /* Search the end of line */
  pnt = strchr(pnt + 1, 0x0A);
  if (pnt)
    request->header = (uint8_t *)(pnt + 1);
  else
    request->header = 0;
  
  return;
  
parse_error:
  request->METHOD = METHOD_ERR;
  return;
}

uint8_t get_http_uri_name(uint8_t * uri, uint8_t * uri_buf)
{
	//uint8_t * uri_ptr;
	if(!uri)
		return 0;

	strcpy((char *)uri_buf, (char *)uri);

//	if(strcmp((char *)uri_ptr,"/")) uri_ptr++;
//	strcpy((char *)uri_buf, (char *)uri_ptr);

	return 1;
}

void inet_addr_(uint8_t * addr, uint8_t *ip)
{
	uint8_t i;
	uint8_t taddr[30];
	uint8_t * nexttok;
	uint8_t num;

	strcpy((char *)taddr, (char *)addr);

	nexttok = taddr;
	for(i = 0; i < 4 ; i++)
	{
// GSG 		nexttok = (uint8_t *)strtok((char *)nexttok, ".");
		if(nexttok[0] == '0' && nexttok[1] == 'x') num = ATOI(nexttok+2,0x10);
		else num = ATOI(nexttok,10);
		ip[i] = num;
		nexttok = NULL;
	}
}


/**
@brief	CONVERT STRING INTO INTEGER
@return	a integer number
*/
uint16_t ATOI(
	uint8_t * str,	/**< is a pointer to convert */
	uint8_t base	/**< is a base value (must be in the range 2 - 16) */
	)
{
        unsigned int num = 0;
// debug_2013_11_25
//        while (*str !=0)
        while ((*str !=0) && (*str != 0x20)) // not include the space(0x020)
                num = num * base + C2D(*str++);
	return num;
}

/**
 * @brief Check strings and then execute callback function by each string.
 * @param src The information of URI
 * @param s1 The start string to be researched
 * @param s2 The end string to be researched
 * @param sub The string between s1 and s2
 * @return The length value atfer working
 */
void mid(char* src, char* s1, char* s2, char* sub)
{
//	char* sub1;
//	char* sub2;
//	uint16_t n;

	uart_puts("httpParser.c mid()\r\n");
	
//	sub1=strstr((char*)src,(char*)s1);
//	sub1+=strlen((char*)s1);
//	sub2=strstr((char*)sub1,(char*)s2);

//	n=sub2-sub1;
//	strncpy((char*)sub,(char*)sub1,n);
//	sub[n]='\0';
}

////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////

/**
@brief	CONVERT CHAR INTO HEX
@return	HEX

This function converts HEX(0-F) to a character
*/
static uint8_t C2D(
		uint8_t c	/**< is a character('0'-'F') to convert to HEX */
	)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return 10 + c -'a';
	if (c >= 'A' && c <= 'F')
		return 10 + c -'A';

	return (char)c;
}
