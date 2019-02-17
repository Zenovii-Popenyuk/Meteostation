#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>


ESP8266WebServer server(80); // 80 is the port number

const char* ssid="NodeMCU";
const char* password="11111111";

String Website,XML,Javascript,temp1, temp2, humidity, pressure, time1;
int send_data;
int data[17] = {0};
String Argument_Name, Clients_Response1, Clients_Response2;
int year, month, day, hour, minute, second, dayOfWeek;

bool Start = false;
void javascriptContent(){
    Javascript ="<SCRIPT>\n";
        Javascript+="function graphData(clicked_id) {var temp_but = document.getElementsByTagName('temp');\n";
    Javascript+="if (clicked_id === 'temp1'){dps = tempData}else if (clicked_id === 'humm'){dps = hummData}else if (clicked_id === 'press'){dps = pressData}else if (clicked_id === 'temp2'){dps = temp2Data}}\n";
        Javascript +="var hh;\n";
    Javascript +="var mm;\n";
    Javascript +="var ss;\n";
    Javascript +="function myFunction() {\n";
    Javascript +="hh = document.getElementById('date_hh').value\n";
    Javascript +="mm = document.getElementById('date_mm').value\n";
    Javascript +="ss = document.getElementById('date_ss').value\n";
    Javascript +="}\n";
    Javascript+="var dps = [{x: 0, y: 0}];\n";
Javascript+="var tempData = [];\n";
Javascript+="var hummData = [];\n";
Javascript+="var pressData = [];\n";
Javascript+="var temp2Data = [];\n";
Javascript+="var temperature, humidity, pressure, temperature_bmp;\n";
Javascript+="var chart = null;\n";
Javascript+="var xVal = 0;\n";
Javascript+="var yVal = 20;\n";
Javascript+="var updateInterval = 3000;\n";
Javascript+="var dataLength = 20;\n";
Javascript+="var updateChart = function (count) {\n";
Javascript+="for (var j = 0; j < count; j++) {\n";
Javascript+="if (temperature) {\n";
Javascript+="tempData.push({x: xVal, y: temperature});\n";
Javascript+="}\n";
Javascript+="if (humidity){\n";
Javascript+="hummData.push({x: xVal, y: humidity});\n";
Javascript+="}\n";
Javascript+="if (pressure){\n";
Javascript+="pressData.push({x: xVal, y: pressure});\n";
        Javascript+="}\n";
        Javascript+="if (temperature_bmp){\n";
            Javascript+="temp2Data.push({x: xVal, y: temperature_bmp});\n";
        Javascript+="}\n";
        Javascript+="\n";
        Javascript+="xVal++\n";
    Javascript+="}\n";
Javascript+="if (dps.length > dataLength) {\n";
        Javascript+="dps.shift();\n";
    Javascript+="}\n";
    Javascript+="chart.render();\n";
Javascript+="};\n";

Javascript+="function process() {\n";
    Javascript+="chart = new CanvasJS.Chart('chartContainer', {\n";
        Javascript+="title: {text: ''},\n";
        Javascript+="axisY: {includeZero: false},\n";
        Javascript+="data: [{type: 'line', dataPoints: dps}]\n";
    Javascript+="});\n";
    Javascript+="xmlHttp.open('PUT', 'xml', true);\n";
    Javascript+="xmlHttp.onreadystatechange = response;\n";
    Javascript+="xmlHttp.send(null);\n";
    Javascript+="try {\n";
        Javascript+="updateChart(1);\n";
    Javascript+="} catch (e) {\n";
Javascript+="    }\n";
    Javascript+="setTimeout('process()', 1000);\n";
Javascript+="}\n";

Javascript+="var xmlHttp = createXmlHttpObject();\n";

Javascript+="function createXmlHttpObject() {\n";
    Javascript+="if (window.XMLHttpRequest) {\n";
        Javascript+="xmlHttp = new XMLHttpRequest();\n";
    Javascript+="} else {\n";
        Javascript+="xmlHttp = new ActiveXObject('Microsoft.XMLHTTP');\n";
    Javascript+="}\n";
    Javascript+="return xmlHttp;\n";
Javascript+="}\n";

Javascript+="function response() {\n";
    Javascript+="xmlResponse = xmlHttp.responseXML;\n";
Javascript+="    xmldoc = xmlResponse.getElementsByTagName('temp1');\n";
    Javascript+="message = xmldoc[0].firstChild.nodeValue;\n";
    Javascript+="if (message) {\n";
        Javascript+="temperature = parseInt(message.substring(0, message.length - 1));\n";
    Javascript+="}\n";
    Javascript+="document.getElementById('div1').innerHTML = message;\n";

    Javascript+="xmldoc = xmlResponse.getElementsByTagName('humidity');\n";
    Javascript+="humm = xmldoc[0].firstChild.nodeValue;\n";
    Javascript+="if (humm){\n";
        Javascript+="humidity = parseInt(humm.substring(0, humm.length - 1));\n";
    Javascript+="}\n";
    Javascript+="document.getElementById('div2').innerHTML = humm;\n";

    Javascript+="xmldoc = xmlResponse.getElementsByTagName('pressure');\n";
    Javascript+="preess = xmldoc[0].firstChild.nodeValue;\n";
    Javascript+="if (preess){\n";
        Javascript+="pressure = parseInt(preess.substring(0, preess.length - 1));\n";
    Javascript+="}\n";
    Javascript+="document.getElementById('div3').innerHTML = preess;\n";

    Javascript+="xmldoc = xmlResponse.getElementsByTagName('temp2');\n";
    Javascript+="temper = xmldoc[0].firstChild.nodeValue;\n";
    Javascript+="if (temper){\n";
        Javascript+="temperature_bmp = parseInt(temper.substring(0, temper.length - 1));\n";
    Javascript+="}\n";

    Javascript+="document.getElementById('div4').innerHTML = temper;\n";
    Javascript+="xmldoc = xmlResponse.getElementsByTagName('time1');\n";
    Javascript+="timer = xmldoc[0].firstChild.nodeValue;\n";
    Javascript+="document.getElementById('div5').innerHTML = timer;\n";
Javascript+="}\n";
    Javascript+="</SCRIPT>\n";
    
  
  }

void WebsiteContent(){
    javascriptContent();
    
    Website="<html>\n";
    Website+="<script src='https://canvasjs.com/assets/script/canvasjs.min.js'></script>\n";
    Website+="<style>\n";
    Website+=".content {display: flex; flex-direction: row; justify-content: space-between; }\n";
    Website+=".block {display: flex; flex-direction: row}\n";
    Website+="#div1, #div2, #div3, #div4{font-size: xx-large;}\n";
    Website+=".round-button {margin: auto; width:5%; }\n";
    Website+=".round-button-circle {margin-top: 20px;width: 100%;height:0;padding-bottom: 100%;border-radius: 50%;border:10px solid #cfdcec;overflow:hidden;background: #4679BD; box-shadow: 0 0 3px gray;}\n";
    Website+=".round-button-circle:hover {background:#30588e;}\n";
    Website+=".round-button a {display:block;float:left;width:100%;padding-top:50%;padding-bottom:50%;line-height:1em;margin-top:-0.5em;text-align:center;color:#e2eaf3;font-family:Verdana;font-size:1.2em;font-weight:bold; text-decoration:none;}\n";
    Website+="font-size:100%;\n";
    Website+="color:#000000;\n";
    Website+="}\n";
    Website+="font-size:100%;\n";
    Website+="color:#000000;\n";
    Website+="}\n";
    Website+="a.button { display: block; width: 115px; height: 25px; background: #4E9CAF; padding: 10px; text-align: center; border-radius: 5px; color: white; font-weight: bold; }\n";
    Website+="</style>\n";
    Website+=Javascript;
    Website+="<body onload='process()'>\n";

    Website+="<div id='div5'></div>\n ";
    Website+="<div class='content'>\n";
    Website+="<div class='block'>\n";
    Website+="<img src='https://image.flaticon.com/icons/svg/56/56295.svg' width='40' height='40'>\n";
    Website+="<div id='div1'>"+temp1+"</div>\n";
    Website+="</div>\n";
    Website+="<div class='block'>\n";
    Website+="<img src='https://cdn1.iconfinder.com/data/icons/weather-line-icon-set-3/100/humidity-512.png' width='40' height='40'>\n";
    Website+="<div id='div2'>"+humidity+"</div>\n";
    Website+="</div>\n";
    Website+="<div class='block'>\n";
    Website+="<img src='https://cdn4.iconfinder.com/data/icons/proglyphs-science-and-medicine/512/Pressure-512.png' width='40' height='40'>\n";
    Website+="<div id='div3'>"+pressure+"</div>\n";
    Website+="</div>\n";
    Website+="<div class='block'>\n";
    Website+="<img src='https://cdn4.iconfinder.com/data/icons/weather-conditions/512/hot_temperature-512.png' width='40' height='40'>\n";
    Website+="<div id='div4'>"+temp2+"</div>\n";
    Website+="</div>\n";
    Website+="</div>\n";
    Website+="<div style='margin-top: 20px; margin-left: 43%;'><a href=\"fullpw\"><button>Full power</button></a><a href=\"lowpw\"><button>Low power</button></a><a href=\"pwoff\"><button>Power off</button></a></div>\n";
    Website+="<div style='margin-top: 20px; margin-left: 43%;'><h1>Time</h1>";
    String IPaddress = WiFi.localIP().toString();
    Website+="<form action='http://"+IPaddress+"' method='POST'><input type='text' name='date_year' id='date_year' size=2 autofocus>year<input type='text' name='date_month' id='date_month' size=2 autofocus>month<input type='text' name='date_day' id='date_day' size=2 autofocus>day<input type='text' name='date_dayOfWeek' id='date_dayOfWeek' size=2 autofocus>day of week<input type='text' name='date_hh' id='date_hh' size=2 autofocus>hh<input type='text' name='date_mm' id='date_mm' size=2 autofocus>mm<input type='text' name='date_ss' id='date_ss' size=2 autofocus>ss\n";
    Website+="<input type='submit' value='Enter'>";
    Website+="</form></div>";
    Website+="<div>\n";
    Website+="<div id='chartContainer' style='height: 300px; width: 100%;'></div>\n";
Website+="<script src='https://canvasjs.com/assets/script/canvasjs.min.js'></script>\n";
Website+="<script src='https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js'></script>\n";

    Website+="<button id='temp1' onclick='graphData(this.id)'>Temperature</button>\n";
    Website+="<button id='humm' onclick='graphData(this.id)'>Hummidity</button>\n";
    Website+="<button id='press' onclick='graphData(this.id)'>Pressure</button>\n";
    Website+="<button id='temp2' onclick='graphData(this.id)'>Temperature BMP</button></div>\n";
    Website+="</body></html>\n";
    
    server.send(200,"text/html",Website);
    if (server.args() > 0 ) { // Arguments were received
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.arg(i) != "" && server.arg(i) != " "){
        if (server.argName(i) == "date_year") {
          year = server.arg(i).toInt();
          if(year >= 0 && year < 100){
              uint8_t buff[3] = {2, (uint8_t)year, (uint8_t)year};
              Serial.write(buff, sizeof(buff));
            }
        
        }
        if (server.argName(i) == "date_month") {
          month = server.arg(i).toInt();
          if(month >= 0 && month < 100){
              uint8_t buff[3] = {3, (uint8_t)month, (uint8_t)month};
              Serial.write(buff, sizeof(buff));
            }
  
        }
        if (server.argName(i) == "date_day") {
          day = server.arg(i).toInt();
          if(day >= 0 && day <= 31){
            uint8_t buff[3] = {4, (uint8_t)day, (uint8_t)day};
              Serial.write(buff, sizeof(buff));
            }
        }
        if (server.argName(i) == "date_hh") {
          hour = server.arg(i).toInt();
          if(hour >= 0 && hour <= 24){
              uint8_t buff[3] = {5, (uint8_t)hour, (uint8_t)hour};
              Serial.write(buff, sizeof(buff));
           }
        }
        if (server.argName(i) == "date_mm") {
          minute = server.arg(i).toInt();
          if(minute >= 0 && minute <= 60){
             uint8_t buff[3] = {6, (uint8_t)minute, (uint8_t)minute};
              Serial.write(buff, sizeof(buff));
            }
        }
        if (server.argName(i) == "date_ss") {
          second = server.arg(i).toInt();
          if (second >= 0 && second <= 60){
              uint8_t buff[3] = {7, (uint8_t)second, (uint8_t)second};
              Serial.write(buff, sizeof(buff));
            }
        }
        if (server.argName(i) == "date_dayOfWeek") {
          dayOfWeek = server.arg(i).toInt();
          if(dayOfWeek >= 0 && dayOfWeek <= 7){
              uint8_t buff[3] = {8, (uint8_t)dayOfWeek, (uint8_t)dayOfWeek};
              Serial.write(buff, sizeof(buff));
            }
        }
      }
    }
  }
}
void XMLcontent(){

  XML ="<?xml version='1.0'?>";
  XML+="<datas>";
  XML+="<temp1>";
  XML+=temp1;
  XML+="</temp1>";

  XML+="<humidity>";
  XML+=humidity;
  XML+="</humidity>";

  XML+="<pressure>";
  XML+=pressure;
  XML+="</pressure>";

  XML+="<temp2>";
  XML+=temp2;
  XML+="</temp2>";

  XML+="<time1>";
  XML+=time1;
  XML+="</time1>";
  
  
  XML+="</datas>";
  

  server.send(200,"text/xml",XML);

  
  }

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
  }
  WiFi.mode(WIFI_STA);
  server.on("/",WebsiteContent);
  server.on("/xml",XMLcontent);
  server.on("/fullpw", [](){
    server.send(200, "text/html", Website);
    uint8_t buff[2] = {1, (uint8_t)0};
    Serial.write(buff, sizeof(buff));
  });
  server.on("/lowpw", [](){
  server.send(200, "text/html", Website);
  uint8_t buff[2] = {1, (uint8_t)1};
  Serial.write(buff, sizeof(buff));
  });
  
  server.on("/pwoff", [](){
  server.send(200, "text/html", Website); 
  uint8_t buff[2] = {1, (uint8_t)2};
  Serial.write(buff, sizeof(buff));
  });
  server.begin();
}

void loop() {
  server.handleClient();
  while (Serial.available()){
    int id = (int)Serial.read();
    int number = (int)Serial.read();
    int CRF = (int)Serial.read();
    if(number == CRF && number >= 0){
      data[id] = number;
    }
  }

  temp1 =" " + String(data[0]) + "." + String(data[1]) + "C ";
  humidity =" " + String(data[2]) + "." + String(data[3]) + "% ";
  pressure =" " + String(data[4]) + String(data[5]) + String(data[6]) + "." + String(data[7])+ "Pa ";
  temp2 =" " + String(data[8]) +  "." + String(data[9]) + "C ";
  time1 = "Time: 20" + String(data[16]) + "/" + String(data[15]) + "/" + String(data[14]) + " ";
  switch(data[13]){
    case 1: time1 += "Monday"; break;
    case 2: time1 += "Tuesday"; break;
    case 3: time1 += "Wednesday"; break;
    case 4: time1 += "Thursday"; break;
    case 5: time1 += "Friday"; break;
    case 6: time1 += "Saturday"; break;
    case 7: time1 += "Sunday"; break;
    default: time1 += "Unknown day!"; break;
    }
    time1 += " " + String(data[12]) + ":" + String(data[11]) + ":" + String(data[10]);
}
