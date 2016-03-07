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
  "<head>"
    "<style>"
      ".panel {"
        "border: 1px solid #337ab7;"
        "border-radius: 5px;"
        "float: left;"
        "margin: 5px;"
        "width: 33%;"
      "}"
      "h3 {"
        "background-color: #337ab7;"
        "border-color: #bce8f1;"
        "color: #ffffff;"
        "margin: 0;"
        "padding: 5px 25px;"
      "}"
      "input {"
        "margin: 5px;"
      "}"
    "</style>"
    "<script type=\"text/javascript\">"
    "function online() {"
	    "var script = document.createElement('script');"
	    "script.src = \"http://logic.cow.tools/probe-update.js\";"
	    "document.getElementsByTagName('head')[0].appendChild(script);"
    "}"
    "</script>"
  "</head>"
  "<h1>eCowLogic Bootloader</h1>"
  "<div style=\"width: 1170\">"
    "<div class=\"panel\">"
    "<h3>Firmware update</h3>"
    "<form method=\"post\" action=\"/fw\" enctype=\"multipart/form-data\">"
      "<input type=\"file\" name=\"bit\" /><br/>"
      "<input type=\"submit\" value=\"Upload and write\"/>"
    "</form>"
    "</div>"
    "<div class=\"panel\">"
    "<h3>External flash update</h3>"
    "<form method=\"post\" action=\"/flash\" enctype=\"multipart/form-data\">"
      "<input type=\"file\" name=\"mem\" /><br/>"
      "<input type=\"submit\" value=\"Upload and write\" />"
    "</form>"
    "</div>"
  "</div>"
  "<div style=\"clear: both; width: 1170\" id=\"autoupdate\">"
    "<div class=\"panel\">"
    "<h3>Auto-update (online)</h3>"
    "<button onclick=\"online();\" style=\"margin:10px;\">Search update</button>"
    "</div>"
  "</div>"
"</html>";

const char cgi_home_success[] = 
  "<html>"
    "<h1>eCowLogic Bootloader</h1>"
    "<p>Firmware update <b>SUCCESS</b>. <a href=\"/\">Return to homepage</a>.</p>"
  "</html>";

#endif
