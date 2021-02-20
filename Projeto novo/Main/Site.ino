String siteMain() {
  const String HTML = "<!DOCTYPE html>"
                "<html>"

                "<head>"
                "<meta charset='utf-8'>"
                "<title>Bem-vindo!</title>"
                "<style type='text/css'>"
                "body {"
                "font-family: arial;"
                "color: #D8DBE2;"
                "background-color: #599392;"
                "}"

                ".panelHead {"
                "margin-left: auto;"
                "margin-right: auto;"
                "margin-top: 30px;"
                "width: 330px;"
                "height: auto;"
                "background-color: #125559;"
                "padding-left: 50px;"
                "padding-bottom: 50px;"
                "padding-top: 30px;"
                "border-radius: 6px;"
                "}"

                ".txtbox {"
                "border: 0px;"
                "height: 20px;"
                "background-color: #D8DBE2;"
                "color: #484A47;"
                "margin-top: 10px;"
                "margin-bottom: 10px;"
                "border-radius: 15px;"
                "}"

                ".colorDialog {"
                "width: 20px;"
                "border-style: none;"
                "}"

                ".subtitle {"
                "font-size: 20px;"
                "margin: 5px;"
                "}"

                ".btn {"
                "width: 70px;"
                "height: 30px;"
                "color: #484A47;"
                "background-color: #D8DBE2;"
                "border: 0;"
                "border-radius: 15px;"
                "}"
                "</style>"
                "</head>"

                "<body>"
                "<div class='panelHead'>"

                "<form>"
                "<table>"
                "<tr>"
                "<td>"
                "<h2 class='subtitle'>Conexão</h2>"
                "</td>"
                "</tr>"
                "<tr align=right>"
                "<td>SSID:</td>"
                "<td><input class='txtbox' type='text' required name='SIID' size='20' /></td>"
                "</tr>"

                "<tr align=right>"
                "<td>Senha:</td>"
                "<td><input class='txtbox' type='text' name='Pass' size='20' /></td>"
                "</tr>"

                "<tr align=right>"
                "<td>Server IP:</td>"
                "<td><input class='txtbox' type='text' required name='ServerIP' size='20' /></td>"
                "</tr>"

                "<tr align=right>"
                "<td>Porta:</td>"
                "<td><input class='txtbox' type='text' required name='Port' size='20' /></td>"
                "</tr>"

                "<tr>"
                "<td>"
                "<h2 class='subtitle'>Mensagens</h2>"
                "</td>"
                "</tr>"

                "<tr align=right>"
                "<td>Identificador:</td>"
                "<td><input class='txtbox' type='text' required name='Ident' size='20' /></td>"
                "</tr>"

                "<tr align=right>"
                "<td>"
                "Botão 1:"
                "<input class='colorDialog' type='color' id='head' name='ColorButton1' value='#599392'>"
                "</td>"
                "<td><input class='txtbox' type='text' required name='MessageButton1' size='20' /></td>"
                "</tr>"

                "<tr align=right>"
                "<td>"
                "Botão 2:"
                "<input class='colorDialog' type='color' id='head' name='ColorButton2' value='#599392'>"
                "</td>"
                "<td><input class='txtbox' type='text' required name='MessageButton2' size='20' /></td>"
                "</tr>"

                "<tr align=right>"
                "<td>"
                "Botão 3:"
                "<input class='colorDialog' type='color' id='head' name='ColorButton3' value='#599392'>"
                "</td>"
                "<td><input class='txtbox' type='text' required name='MessageButton3' size='20' /></td>"
                "</tr>"

                "<tr align=right>"
                "<td></td>"
                "<td><input class='btn' type='submit' value='Enviar'></td>"
                "</tr>"
                "</table>"
                "</form>"
                "</div>"
                "</body>"
                "</html>"
                ;
                return HTML;
}

String siteResposta(){
  String HTML="";
  
  HTML+="<!DOCTYPE html>";
  HTML+="<html>";
  HTML+="<head>";
  HTML+="<title>Setup</title>";
  HTML+="<meta charset='utf-8'>";
  HTML+="<style type='text/css'>";
  HTML+="body {";
  HTML+="font-family:arial;";
  HTML+="color: #D8DBE2;";
  HTML+="background-color: #599392;";
  HTML+="}";
  HTML+=".titulo{";
  HTML+="margin-right: 15px;";
  HTML+="margin-left: 15px;";
  HTML+="}";
  HTML+=".panelHead{";
  HTML+="padding-top:30px;";
  HTML+="margin-left: auto;";
  HTML+="margin-right: auto;";
  HTML+="margin-top: 30px;";
  HTML+="width:400px;";
  HTML+="height: 200px;";
  HTML+="background-color: #125559;";
  HTML+="border-radius: 6px;";
  HTML+="}";
  HTML+="</style>";
  HTML+="</head>";
  HTML+="<body>";
  HTML+="<div class='panelHead'>";
  HTML+="<h1 class='titulo' align=Center>Configurado!</h1>";
  HTML+="<h3 class='titulo' align=center >Caso o painel não conecte-se a rede em alguns segundos, algo correu mal, resete o painel e tente novamente.</h3>";
  HTML+="</div>";
  HTML+="</body>";
  HTML+="</html>";
  
  return HTML;
}
