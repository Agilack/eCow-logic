/**
 * eCow-logic - Embedded probe main firmware
 *
 * This file is based on httpServer.c from Wiznet ioLibrary
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
#include "uart.h"

#include "httpServer.h"
#include "httpParser.h"

#ifndef DATA_BUF_SIZE
	#define DATA_BUF_SIZE		1536
#endif

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
static uint8_t HTTPSock_Num[4] = {0, };
static st_http_request * http_request;				/**< Pointer to received HTTP request */
static uint8_t * http_response;						/**< Pointer to HTTP response */

// Number of registered web content in code flash memory
static uint16_t total_content_cnt;
/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
uint8_t * pHTTP_TX;
uint8_t * pHTTP_RX;

volatile uint32_t httpServer_tick_1s;
st_http_socket HTTPSock_Status[4];
httpServer_webContent web_content[MAX_CONTENT_CALLBACK];

/*****************************************************************************
 * Private functions
 ****************************************************************************/
void httpServer_Sockinit(uint8_t cnt, uint8_t * socklist);
static uint8_t getHTTPSocketNum(uint8_t seqnum);
static int8_t getHTTPSequenceNum(uint8_t socket);
static int8_t http_disconnect(uint8_t sn);

static void http_process_handler(uint8_t s, st_http_request * p_http_request);
static void send_http_response_header(uint8_t s, uint8_t content_type, uint32_t body_len, uint16_t http_status);
static void send_http_response_body(uint8_t s, uint8_t * uri_name, uint8_t * buf, uint32_t start_addr, uint32_t file_len);
static void send_http_response_cgi(uint8_t s, uint8_t * buf, uint8_t * http_body, uint16_t file_len, uint16_t file_type, uint32_t full_len);

/*****************************************************************************
 * Public functions
 ****************************************************************************/

static uint8_t getHTTPSocketNum(uint8_t seqnum)
{
	// Return the 'H/W socket number' corresponding to the index number
	return HTTPSock_Num[seqnum];
}

static int8_t getHTTPSequenceNum(uint8_t socket)
{
	uint8_t i;

	for(i = 0; i < 4; i++)
		if(HTTPSock_Num[i] == socket) return i;

	return -1;
}

void httpServer_init(uint8_t * tx_buf, uint8_t * rx_buf, uint8_t cnt, uint8_t * socklist)
{
	int i;
	
	total_content_cnt  = 0;
	httpServer_tick_1s = 0;
	for (i = 0; i < 4; i++)
		HTTPSock_Status[i].sock_status = STATE_HTTP_IDLE;
	
	// User's shared buffer
	pHTTP_TX = tx_buf;
	pHTTP_RX = rx_buf;

	// H/W Socket number mapping
	HTTPSock_Num[0] = 4;
	HTTPSock_Num[1] = 5;
	HTTPSock_Num[2] = 6;
	HTTPSock_Num[3] = 7;
}

void httpServer_run(uint8_t seqnum)
{
	uint8_t s;	// socket number
	uint16_t len;
	uint32_t gettime = 0;

	http_request = (st_http_request *)pHTTP_RX;		// Structure of HTTP Request

	// Get the H/W socket number
	s = getHTTPSocketNum(seqnum);

	/* HTTP Service Start */
	switch(getSn_SR(s))
	{
		case SOCK_ESTABLISHED:
			// Interrupt clear
			if(getSn_IR(s) & Sn_IR_CON)
			{
				setSn_IR(s, Sn_IR_CON);
			}

			// HTTP Process states
			switch(HTTPSock_Status[seqnum].sock_status)
			{

				case STATE_HTTP_IDLE :
					if ((len = getSn_RX_RSR(s)) > 0)
					{
						http_request->buffer = (pHTTP_RX + 128);
						
						if (len > DATA_BUF_SIZE)
							len = DATA_BUF_SIZE;
						
						len = recv(s, http_request->buffer, len);

						http_request->buffer[len] = '\0';
						http_request->length  = len;
						http_request->status  = 0;
						http_request->handler = 0;
						http_request->priv    = 0;
						http_request->response_len = 0;

						parse_http_request(http_request);
#ifdef _HTTPSERVER_DEBUG_
						uart_puthex8(s);
						uart_puts("> HTTPSocket : HTTP Request received\r\n");
#endif
						// HTTP 'response' handler; includes send_http_response_header / body function
						http_process_handler(s, http_request);

						if (http_request->status == 0)
						{
						gettime = get_httpServer_timecount();
						// Check the TX socket buffer for End of HTTP response sends
						while(getSn_TX_FSR(s) != (getSn_TXBUF_SIZE(s)*1024))
						{
							if((get_httpServer_timecount() - gettime) > 3)
							{
#ifdef _HTTPSERVER_DEBUG_
								uart_puthex8(s);
								uart_puts("> HTTPSocket : [State] STATE_HTTP_REQ_DONE: TX Buffer clear timeout\r\n");
#endif
								break;
							}
						}

							if (http_request->response_len > 0)
								HTTPSock_Status[seqnum].sock_status = STATE_HTTP_RES_INPROC_CGI;
							else
							{
								if(HTTPSock_Status[seqnum].file_len > 0)
									HTTPSock_Status[seqnum].sock_status = STATE_HTTP_RES_INPROC;
								else
									HTTPSock_Status[seqnum].sock_status = STATE_HTTP_RES_DONE; // Send the 'HTTP response' end
							}
						}
						else
							HTTPSock_Status[seqnum].sock_status = STATE_HTTP_REQ_INPROC;
					}
					break;
				
				case STATE_HTTP_REQ_INPROC:
					len = getSn_RX_RSR(s);
					if (len == 0)
						break;
					if (len > DATA_BUF_SIZE)
						len = DATA_BUF_SIZE;
					len = recv(s, http_request->buffer, len);
					http_request->length = len;
					if (http_request->handler)
					{
						char *buffer_tx = (pHTTP_TX + 128);
						u32 file_len;
						u32 file_type;
						http_request->handler((void*)http_request, (char *)(pHTTP_TX + 128), (u32 *)&file_len, (u32*)&file_type);
						if (http_request->status == 0)
						{
							u32 content_len = file_len + http_request->response_len;
							send_http_response_cgi(s, 0, (uint8_t *)buffer_tx, (uint16_t)file_len, file_type, content_len);
							// Send the 'HTTP response' end
							HTTPSock_Status[seqnum].sock_status = STATE_HTTP_RES_DONE;
						}
					}
					break;
					
				case STATE_HTTP_RES_INPROC :
					/* Repeat: Send the remain parts of HTTP responses */
#ifdef _HTTPSERVER_DEBUG_
					uart_puthex8(s);
					uart_puts("> HTTPSocket : [State] STATE_HTTP_RES_INPROC\r\n");
#endif
					// Repeatedly send remaining data to client
					send_http_response_body(s, 0, http_response, 0, 0);
					/* Wait until datas are sent */
					while(getSn_TX_FSR(s) != (getSn_TXBUF_SIZE(s)*1024))
						;
					if(HTTPSock_Status[seqnum].file_len == 0) HTTPSock_Status[seqnum].sock_status = STATE_HTTP_RES_DONE;
					break;
				
				case STATE_HTTP_RES_INPROC_CGI:
				{
					u32 file_len;
					u32 file_type;
					uart_puts("> HTTPSocket : [State] STATE_HTTP_RES_INPROC_CGI\r\n");
					http_request->handler((void*)http_request, (char *)pHTTP_TX, (u32 *)&file_len, (u32*)&file_type);
					send(s, pHTTP_TX, file_len);
					if (http_request->response_len == 0)
						HTTPSock_Status[seqnum].sock_status = STATE_HTTP_RES_DONE;
					break;
				}

				case STATE_HTTP_RES_DONE :
#ifdef _HTTPSERVER_DEBUG_
					uart_puthex8(s);
					uart_puts("> HTTPSocket : [State] STATE_HTTP_RES_DONE\r\n");
#endif
					// Socket file info structure re-initialize
					HTTPSock_Status[seqnum].file_len = 0;
					HTTPSock_Status[seqnum].file_offset = 0;
					HTTPSock_Status[seqnum].file_start = 0;
					HTTPSock_Status[seqnum].sock_status = STATE_HTTP_IDLE;

					http_disconnect(s);
					break;

				default :
					break;
			}
			break;

		case SOCK_CLOSE_WAIT:
			disconnect(s);
			break;

		case SOCK_CLOSED:
			/* Reinitialize the socket */
			socket(s, Sn_MR_TCP, HTTP_SERVER_PORT, 0x00);
			break;

		case SOCK_INIT:
			listen(s);
			break;

		case SOCK_LISTEN:
			break;

		default :
			break;

	} // end of switch
}

////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////
static void send_http_response_header(uint8_t s, uint8_t content_type, uint32_t body_len, uint16_t http_status)
{
	switch(http_status)
	{
		case STATUS_OK: 		// HTTP/1.1 200 OK
			if((content_type != PTYPE_CGI) && (content_type != PTYPE_XML)) // CGI/XML type request does not respond HTTP header
			{
#ifdef _HTTPSERVER_DEBUG_
			uart_puthex8(s);
			uart_puts("> HTTPSocket : HTTP Response Header - STATUS_OK\r\n");
			uart_puthex(content_type); uart_puts("\r\n");
#endif
				make_http_response_head((char*)http_response, content_type, body_len);
			}
			else
			{
#ifdef _HTTPSERVER_DEBUG_
			uart_puthex8(s);
			uart_puts("> HTTPSocket : HTTP Response Header - NONE / CGI or XML\r\n");
#endif
				// CGI/XML type request does not respond HTTP header to client
				http_status = 0;
			}
			break;
		case STATUS_BAD_REQ: 	// HTTP/1.1 400 OK
#ifdef _HTTPSERVER_DEBUG_
			uart_puthex8(s);
			uart_puts("> HTTPSocket : HTTP Response Header - STATUS_BAD_REQ\r\n");
#endif
			memcpy(http_response, ERROR_REQUEST_PAGE, sizeof(ERROR_REQUEST_PAGE));
			break;
		case STATUS_NOT_FOUND:	// HTTP/1.1 404 Not Found
#ifdef _HTTPSERVER_DEBUG_
			uart_puthex8(s);
			uart_puts("> HTTPSocket : HTTP Response Header - STATUS_NOT_FOUND\r\n");
#endif
			memcpy(http_response, ERROR_HTML_PAGE, sizeof(ERROR_HTML_PAGE));
			break;
		default:
			break;
	}

	// Send the HTTP Response 'header'
	if(http_status)
	{
#ifdef _HTTPSERVER_DEBUG_
		uart_puthex8(s);
		uart_puts("> HTTPSocket : [Send] HTTP Response Header [ 0x");
		uart_puthex16( (uint16_t)strlen((char *)http_response) );
		uart_puts(" ]byte\r\n");
#endif
		send(s, http_response, strlen((char *)http_response));
	}
}

static void send_http_response_body(uint8_t s, uint8_t * uri_name, uint8_t * buf, uint32_t start_addr, uint32_t file_len)
{
	int8_t get_seqnum;
	uint32_t send_len;

	uint8_t flag_datasend_end = 0;

	if((get_seqnum = getHTTPSequenceNum(s)) == -1) return; // exception handling; invalid number

	// Send the HTTP Response 'body'; requested file
	if(!HTTPSock_Status[get_seqnum].file_len) // ### Send HTTP response body: First part ###
	{
		if (file_len > DATA_BUF_SIZE - 1)
		{
			HTTPSock_Status[get_seqnum].file_start = start_addr;
			HTTPSock_Status[get_seqnum].file_len = file_len;
			send_len = DATA_BUF_SIZE - 1;

/////////////////////////////////////////////////////////////////////////////////////////////////
// ## 20141219 Eric added, for 'File object structure' (fs) allocation reduced (8 -> 1)
			memset(HTTPSock_Status[get_seqnum].file_name, 0x00, MAX_CONTENT_NAME_LEN);
			strcpy((char *)HTTPSock_Status[get_seqnum].file_name, (char *)uri_name);
#ifdef _HTTPSERVER_DEBUG_
			uart_puthex8(s);
			uart_puts("> HTTPSocket : HTTP Response body - file name : ");
			uart_puts( (char *)HTTPSock_Status[get_seqnum].file_name);
			uart_puts("\r\n");
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _HTTPSERVER_DEBUG_
			uart_puthex8(s);
			uart_puts("> HTTPSocket : HTTP Response body - file len [ 0x");
			uart_puthex(file_len);
			uart_puts(" ]byte\r\n");
#endif
		}
		else
		{
			// Send process end
			send_len = file_len;

#ifdef _HTTPSERVER_DEBUG_
			// GSG printf("> HTTPSocket[%d] : HTTP Response end - file len [ %ld ]byte\r\n", s, send_len);
#endif
		}
	}
	else // remained parts
	{
		send_len = HTTPSock_Status[get_seqnum].file_len - HTTPSock_Status[get_seqnum].file_offset;

		if(send_len > DATA_BUF_SIZE - 1)
		{
			send_len = DATA_BUF_SIZE - 1;
			//HTTPSock_Status[get_seqnum]->file_offset += send_len;
		}
		else
		{
#ifdef _HTTPSERVER_DEBUG_
			// GSG printf("> HTTPSocket[%d] : HTTP Response end - file len [ %ld ]byte\r\n", s, HTTPSock_Status[get_seqnum].file_len);
#endif
			// Send process end
			flag_datasend_end = 1;
		}
#ifdef _HTTPSERVER_DEBUG_
			// GSG printf("> HTTPSocket[%d] : HTTP Response body - send len [ %ld ]byte\r\n", s, send_len);
#endif
	}

/*****************************************************/
	//HTTPSock_Status[get_seqnum]->storage_type == NONE
	//HTTPSock_Status[get_seqnum]->storage_type == CODEFLASH
	//HTTPSock_Status[get_seqnum]->storage_type == SDCARD
	//HTTPSock_Status[get_seqnum]->storage_type == DATAFLASH
/*****************************************************/

	if(HTTPSock_Status[get_seqnum].storage_type == CODEFLASH)
	{
		if(HTTPSock_Status[get_seqnum].file_len) start_addr = HTTPSock_Status[get_seqnum].file_start;
		read_userReg_webContent(start_addr, &buf[0], HTTPSock_Status[get_seqnum].file_offset, send_len);
	}
	else
	{
		send_len = 0;
	}
	// Requested content send to HTTP client
#ifdef _HTTPSERVER_DEBUG_
	// GSG printf("> HTTPSocket[%d] : [Send] HTTP Response body [ %ld ]byte\r\n", s, send_len);
#endif

	if(send_len) send(s, buf, send_len);
	else flag_datasend_end = 1;

	if(flag_datasend_end)
	{
		HTTPSock_Status[get_seqnum].file_start = 0;
		HTTPSock_Status[get_seqnum].file_len = 0;
		HTTPSock_Status[get_seqnum].file_offset = 0;
		flag_datasend_end = 0;
	}
	else
	{
		HTTPSock_Status[get_seqnum].file_offset += send_len;
#ifdef _HTTPSERVER_DEBUG_
		// GSG printf("> HTTPSocket[%d] : HTTP Response body - offset [ %ld ]\r\n", s, HTTPSock_Status[get_seqnum].file_offset);
#endif
	}
}

int b2ds(char *d, int n);

static void send_http_response_cgi(uint8_t s, uint8_t * buf, uint8_t * http_body, uint16_t file_len, uint16_t file_type, uint32_t content_len)
{
	uint16_t send_len = 0;
	char content_length[8];
	int i;

#ifdef _HTTPSERVER_DEBUG_
	uart_puthex8(s);
	uart_puts("> HTTPSocket : HTTP Response Header + Body - CGI\r\n");
#endif
	i = b2ds(content_length, content_len);
	content_length[i] = 0;
	
	if (file_type == 1)
		i = strlen(RES_JSONHEAD_OK) + strlen(content_length) + 4;
	else if (file_type == 2)
		i = strlen(RES_PNGHEAD_OK)  + strlen(content_length) + 4;
	else if (file_type == 3)
		i = strlen(RES_CSSHEAD_OK)  + strlen(content_length) + 4;
	else
		i = strlen(RES_CGIHEAD_OK)  + strlen(content_length) + 4;
	if (i > 128)
	{
		uart_puts("send_http_response_cgi() FATAL: response header > 128\r\n");
		return;
	}
	/* Go backward into buffer to get space for header */
	http_body -= i;
	/* Add the header before datas */
	if (file_type == 1)
		strcpy((char *)http_body, RES_JSONHEAD_OK);
	else if (file_type == 2)
		strcpy((char *)http_body, RES_PNGHEAD_OK);
	else if (file_type == 3)
		strcpy((char *)http_body, RES_CSSHEAD_OK);
	else
		strcpy((char *)http_body, RES_CGIHEAD_OK);
	strcat((char *)buf, content_length);
	/**/
	http_body[i-4] = '\r';
	http_body[i-3] = '\n';
	http_body[i-2] = '\r';
	http_body[i-1] = '\n';

	send_len = file_len + i;
	
#ifdef _HTTPSERVER_DEBUG_
	uart_puthex8(s);
	uart_puts("> HTTPSocket : HTTP Response Header + Body - send len [ ");
	uart_puthex(send_len);
	uart_puts(" ]byte\r\n");
#endif

	send(s, http_body, send_len);
}


static int8_t http_disconnect(uint8_t sn)
{
	setSn_CR(sn,Sn_CR_DISCON);
	/* wait to process the command... */
	while(getSn_CR(sn));

	return SOCK_OK;
}


static void http_process_handler(uint8_t s, st_http_request * p_http_request)
{
	uint8_t * uri_name;
	uint32_t content_addr = 0;
	uint16_t content_num = 0;
	uint32_t file_len = 0;
	uint32_t file_type = 0;

	uint8_t *buffer_tx;

	uint8_t uri_buf[MAX_URI_SIZE]={0x00, };

	uint16_t http_status;
	int8_t get_seqnum;
	uint8_t content_found;

	if((get_seqnum = getHTTPSequenceNum(s)) == -1)
		return; // exception handling; invalid number

	http_status = 0;
	buffer_tx = (pHTTP_TX + 128);
	http_response = pHTTP_TX;
	file_len = 0;

	//method Analyze
	switch (p_http_request->METHOD)
	{
		case METHOD_ERR :
			http_status = STATUS_BAD_REQ;
			send_http_response_header(s, 0, 0, http_status);
			break;

		case METHOD_HEAD :
		case METHOD_GET :
			get_http_uri_name(p_http_request->URI, uri_buf);
			uri_name = uri_buf;

			// If URI is "/", respond by index.html
			if (!strcmp((char *)uri_name, "/"))
				strcpy((char *)uri_name, INITIAL_WEBPAGE);

#ifdef _HTTPSERVER_DEBUG_
			uart_puthex8(s);
			uart_puts("> HTTPProcess : GET ");
			uart_puts("Request URI = ");
			uart_puts((char *)uri_name);
			uart_puts("\r\n");
#endif
			if (find_userReg_webContent(uri_buf, &content_num, &file_len))
			{
				if(web_content[content_num].cgi)
					p_http_request->TYPE = PTYPE_CGI;
				else
					p_http_request->TYPE = PTYPE_HTML;
			}
			
			if(p_http_request->TYPE == PTYPE_CGI)
			{
				p_http_request->handler = web_content[content_num].cgi;
				content_found = p_http_request->handler((void*)p_http_request, (char *)buffer_tx, (u32 *)&file_len, (u32*)&file_type);

				if(content_found && (file_len <= (DATA_BUF_SIZE-(strlen(RES_CGIHEAD_OK)+8))))
				{
					u32 content_len = file_len +  http_request->response_len;
					send_http_response_cgi(s, http_response, buffer_tx, (uint16_t)file_len, file_type, content_len);
				}
				else
				{
					send_http_response_header(s, PTYPE_CGI, 0, STATUS_NOT_FOUND);
				}
			}
			else
			{
				// Find the User registered index for web content
				if(find_userReg_webContent(uri_buf, &content_num, &file_len))
				{
					uart_puts("Content found len=");
					uart_puthex(file_len); uart_puts(" cgi=");
					uart_puthex((u32)web_content[content_num].cgi); uart_puts("\r\n");
					http_status = STATUS_OK;
					content_addr = (uint32_t)content_num;
					HTTPSock_Status[get_seqnum].storage_type = CODEFLASH;
				}
				else
					http_status = STATUS_NOT_FOUND;

				// Send HTTP header
				if(http_status)
				{
#ifdef _HTTPSERVER_DEBUG_
					uart_puthex8(s);
					uart_puts("> HTTPSocket : Requested content len = [ ");
					uart_puthex16(file_len);
					uart_puts(" ]byte\r\n");
#endif
					send_http_response_header(s, p_http_request->TYPE, file_len, http_status);
				}

				// Send HTTP body (content)
				if(http_status == STATUS_OK)
				{
					send_http_response_body(s, uri_name, http_response, content_addr, file_len);
				}
			}
			break;

		case METHOD_POST :
			strcpy((char *)uri_buf, (char *)p_http_request->URI);
			uri_name = uri_buf;
			
			if (find_userReg_webContent(uri_buf, &content_num, &file_len))
			{
				if(web_content[content_num].cgi)
					p_http_request->TYPE = PTYPE_CGI;
				else
					p_http_request->TYPE = PTYPE_HTML;
			}

#ifdef _HTTPSERVER_DEBUG_
			uart_puthex8(s);
			uart_puts("> HTTPSocket : POST ");
			uart_puts("Request URI = ");
			uart_puts((char *)uri_name);
			uart_puts("\r\n");
#endif
			p_http_request->handler = web_content[content_num].cgi;
			
			if(p_http_request->TYPE == PTYPE_CGI)	// HTTP POST Method; CGI Process
			{
				content_found = web_content[content_num].cgi((void *)p_http_request, (char *)http_response, (u32 *)&file_len, (u32*)&file_type);
#ifdef _HTTPSERVER_DEBUG_
				// GSG printf("> HTTPSocket[%d] : [CGI: %s] / Response len [ %ld ]byte\r\n", s, content_found?"Content found":"Content not found", file_len);
#endif
				if (p_http_request->status == 0)
				{
				if(content_found && (file_len <= (DATA_BUF_SIZE-(strlen(RES_CGIHEAD_OK)+8))))
				{
					u32 content_len = file_len +  http_request->response_len;
					send_http_response_cgi(s, pHTTP_TX, http_response, (uint16_t)file_len, file_type, content_len);
				}
				else
				{
					send_http_response_header(s, PTYPE_CGI, 0, STATUS_NOT_FOUND);
				}
				}
			}
			else	// HTTP POST Method; Content not found
			{
				send_http_response_header(s, 0, 0, STATUS_NOT_FOUND);
			}
			break;

		default :
			uart_puts("http_process_handler() default BAD_REQ\r\n");
			http_status = STATUS_BAD_REQ;
			send_http_response_header(s, 0, 0, http_status);
			break;
	}
}

void httpServer_time_handler(void)
{
	httpServer_tick_1s++;
}

uint32_t get_httpServer_timecount(void)
{
	return httpServer_tick_1s;
}

void reg_httpServer_webCgi(uint8_t * content_name, u32 fctaddr)
{
	if(content_name == NULL)
		return;
	if(total_content_cnt >= MAX_CONTENT_CALLBACK)
		return;

	strcpy((char *)web_content[total_content_cnt].content_name, (const char *)content_name);
	web_content[total_content_cnt].content_len = 0;
	web_content[total_content_cnt].content = 0;
	web_content[total_content_cnt].cgi = (u8 (*)(void*,char*,u32*, u32*))fctaddr;

	total_content_cnt++;
}

void reg_httpServer_webContent(uint8_t * content_name, uint8_t * content)
{
//	uint16_t name_len;
	uint32_t content_len;

	if(content_name == NULL || content == NULL)
	{
		return;
	}
	else if(total_content_cnt >= MAX_CONTENT_CALLBACK)
	{
		return;
	}

//	name_len = strlen((char *)content_name);
	content_len = strlen((char *)content);

	//web_content[total_content_cnt].content_name = malloc(name_len+1);
	strcpy((char *)web_content[total_content_cnt].content_name, (const char *)content_name);
	web_content[total_content_cnt].content_len = content_len;
	web_content[total_content_cnt].content = content;
	web_content[total_content_cnt].cgi = 0;
	
	total_content_cnt++;
}

uint8_t display_reg_webContent_list(void)
{
	uint16_t i;
	uint8_t ret;

	if(total_content_cnt == 0)
	{
		uart_puts(">> Web content file not found\r\n");
		ret = 0;
	}
	else
	{
		uart_puts("\r\n=== List of Web content in code flash ===\r\n");
		for(i = 0; i < total_content_cnt; i++)
		{
			uart_puthex8(i+1);
			uart_puts("  ");
			uart_puts( (char *)web_content[i].content_name );
			//printf("%ld byte, ", web_content[i].content_len);

			//if(web_content[i].content_len < 30) printf("[%s]\r\n", web_content[i].content);
			//else printf("[ ... ]\r\n");
			uart_puts("\r\n");
		}
		uart_puts("=========================================\r\n\r\n");
		ret = 1;
	}

	return ret;
}

uint8_t find_userReg_webContent(uint8_t * content_name, uint16_t * content_num, uint32_t * file_len)
{
	uint16_t i;
	uint8_t ret = 0; // '0' means 'File Not Found'

	for(i = 0; i < total_content_cnt; i++)
	{
//		if(!strcmp((char *)content_name, (char *)web_content[i].content_name))
		if(!strncmp((char *)content_name, (char *)web_content[i].content_name, strlen((char *)web_content[i].content_name)))
		{
			*file_len = web_content[i].content_len;
			*content_num = i;
			ret = 1; // If the requested content found, ret set to '1' (Found)
			break;
		}
	}
	return ret;
}


uint16_t read_userReg_webContent(uint16_t content_num, uint8_t * buf, uint32_t offset, uint16_t size)
{
	uint16_t ret = 0;
	uint8_t * ptr;

	if(content_num > total_content_cnt) return 0;

	ptr = web_content[content_num].content;
	if(offset) ptr += offset;

	strncpy((char *)buf, (char *)ptr, size);
	*(buf+size) = 0; // Insert '/0' for indicates the 'End of String' (null terminated)

	ret = strlen((void *)buf);
	return ret;
}
