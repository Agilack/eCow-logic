
const char web_01[] =
  "<html>"
  "<body>"
    "<div><h3>Charger PLD</h3>"
    "<form action=\"/pld\" method=\"POST\" enctype=\"multipart/form-data\">"
      "<input type=\"file\" name=\"bit\" accept=\"\">"
      "<input type=\"submit\">"
    "</form>"
    "</div>"
    "<div><h3>Commande SPI</h3>"
    "<form action=\"/spi\" method=\"POST\">"
      "<input type=\"text\" name=\"cmd\">"
      "<input type=\"submit\">"
    "</form>"
    "</div>"
  "</body>"
  "</html>";
