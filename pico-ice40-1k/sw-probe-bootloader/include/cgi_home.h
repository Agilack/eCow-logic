/**
 * eCow-logic - Bootloader
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
#ifndef CGI_HOME_H
#define CGI_HOME_H

const char cgi_home_content[] = 
  "<html>"
    "<h1>eCowLogic Bootloader</h1>"
    "<h3>Firmware update</h3>"
    "<form method=\"post\" action=\"/fw\" enctype=\"multipart/form-data\">"
      "<input type=\"file\" name=\"bit\" />"
      "<input type=\"submit\" />"
    "</form>"
    "<h3>External flash update</h3>"
    "<form method=\"post\" action=\"/flash\" enctype=\"multipart/form-data\">"
      "<input type=\"file\" name=\"mem\" />"
      "<input type=\"submit\" />"
    "</form>"
  "</html>";

const char cgi_home_success[] = 
  "<html>"
    "<h1>eCowLogic Bootloader</h1>"
    "<p>Firmware update <b>SUCCESS</b>. <a href=\"/\">Return to homepage</a>.</p>"
  "</html>";

#endif
