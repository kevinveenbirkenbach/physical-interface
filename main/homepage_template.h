String homepage_template(void){
  String homepage = "";
  homepage = homepage +
  "<!DOCTYPE html>"
  "<html>"
    "<head>"
      "<meta charset=\"UTF-8\">"
      "<title>"+String(titel)+"("+ String(hostname) +")</title>"
      "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css\" integrity=\"sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm\" crossorigin=\"anonymous\">"
      "<script src=\"https://code.jquery.com/jquery-3.2.1.slim.min.js\" integrity=\"sha384-KJ3o2DKtIkvYIK3UENzmM7KCkRr/rE9/Qpg6aAZGJwFDMVNA/GpGFF93hXpG5KkN\" crossorigin=\"anonymous\"></script>"
      "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.12.9/umd/popper.min.js\" integrity=\"sha384-ApNbgh9B+Y1QKtv3Rn7W3mgPxhU9K/ScQsAP7hUibX39j7fakFPskvXusvfa0b4Q\" crossorigin=\"anonymous\"></script>"
      "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/js/bootstrap.min.js\" integrity=\"sha384-JZR6Spejh4U02d8jOt6vLEHfe/JQGiRRSQQxSfFWpi1MquVdAyjUar5+76PVCmYl\" crossorigin=\"anonymous\"></script>"
      "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.13.0/css/all.min.css\">"
      "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.13.0/js/all.min.js\"></script>"
      "<link href=\"https://gitcdn.github.io/bootstrap-toggle/2.2.2/css/bootstrap-toggle.min.css\" rel=\"stylesheet\">"
      "<script src=\"https://gitcdn.github.io/bootstrap-toggle/2.2.2/js/bootstrap-toggle.min.js\"></script>"
    "</head>"
    "<body>"
      "<div class=\"container\">"
        "<h1>"+String(titel)+"</h1>"
        "<p>An REST-API Arduino microcontroller, which allows you to interact with hardware and the physical environment.</p>"
        "<h2>Controller <i class=\"fas fa-cogs\"></i></h2>"
        "<form method=\"get\" >";
        for(const char* parameter : PARAMETER_LIST){
          String parameter_type=getParameterType(parameter);
          if(parameter_type.equals("boolean")){
            homepage = homepage +
            "<div class=\"form-row\">"
              "<label for=\""+ parameter +"\" class=\"col-sm-2 col-form-label\">"+ parameter +":</label>"
              "<div class=\"col-sm-10\" >"
                "<input type=\"checkbox\" data-toggle=\"toggle\" name=\""+ parameter +"\" " + ((server.arg(parameter)=="on") ? String("checked=\"checked\""):String("")) + ">"
              "</div>"
            "</div>";
          }else{
            homepage = homepage +
            "<div class=\"form-group row\" >"
              "<label for=\""+ parameter +"\" class=\"col-sm-2 col-form-label\">"+ parameter +":</label>"
              "<div class=\"col-sm-10\" >"
                "<input type=\"" + ((parameter_type=="integer") ? String("number") : String("text")) + "\" class= \"form-control\" id=\""+ parameter +"\" value=\"" +  server.arg(parameter) +"\" name=\""+ parameter +"\">"
              "</div>"
            "</div>";
          }
        }
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
