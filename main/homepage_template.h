String homepage_template(void){
  String homepage = "";
  homepage = homepage +
  "<!DOCTYPE html>"
  "<html>"
    "<head>"
      "<meta charset=\"UTF-8\">"
      "<title>"+String(titel)+"("+ String(hostname) +")</title>"
      "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css\">"
      "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js\"></script>"
      "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/js/bootstrap.min.js\"></script>"
      "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.13.0/css/all.min.css\">"
      "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.13.0/js/all.min.js\"></script>"
    "</head>"
    "<body>"
      "<div class=\"container\">"
        "<h1>"+String(titel)+"</h1>"
        "<p>An REST-API Arduino microcontroller, which allows you to interact with hardware and the physical environment.</p>"
        "<h2>Controller <i class=\"fas fa-cogs\"></i></h2>"
        "<form method=\"get\" >";
        for(const char* parameter : parameter_list)
          homepage = homepage +
          "<div class=\"form-group\">"
            "<label for=\""+ parameter +"\">"+ parameter +":</label>"
            "<input type=\"number\" class= \"form-control\" id=\""+ parameter +"\" value=\"" +  server.arg(parameter) +"\" name=\""+ parameter +"\">"
          "</div>";
          homepage = homepage +
          "<input type=\"submit\" class=\"btn btn-secondary\">"
        "</form>"
        "<h2>Data <i class=\"fas fa-database\"></i></h2>"
        "<pre>"
           + getJson() +
        "</pre>"
        "<p>The sensor datas in plane text  can be reached <a href=\"?format=json\">here</a>.</p>"
        "<hr />"
        "<p><small>Please check out the <a href=\"https://github.com/kevinveenbirkenbach/physical-interface\">git-repository</a> to get more information about this software.</small></p>"
        "</div>"
    "<body>"
  "<html>";
  return homepage;
}
