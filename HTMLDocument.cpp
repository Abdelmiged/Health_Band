#include "Arduino.h"
#include "HTMLDocument.h"

HTMLDocument::HTMLDocument(WiFiClient client) {
  _client = client;
}

void HTMLDocument::HTTPResponseHeaders() {
  _client.println("HTTP/1.1 200 OK");
  _client.println("Content-type:text/html");
//  _client.println("Refresh: 2");
  _client.println();
}

String HTMLDocument::HTMLStyle() {
  String htmlStyle = R"===(
    <style>
          * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: sans-serif;
          }
          
          header {
            padding: 1rem;
            background-color: #333;
            display: grid;
            place-items: center;
          }
          
          header h1 {
            color: white;
            font-size: 2.3rem;
            text-transform: uppercase;
            letter-spacing: .15rem;
          }
          
          .readings-container {
            padding: 1.2rem;
            display: flex;
            justify-content: space-around;
            align-items: center;
          }
          
          .readings-container section {
            background-color: #eee;
            width: 30rem;
            height: 10rem;
            border-radius: .5rem;
            display: flex;
            flex-direction: column;
            justify-content: space-around;
            align-items: center;
          }
          
          .container {
            display: flex;
            flex-direction: column;
            align-items: center;
          }
          
          .container > h1 {
            margin-bottom: .7rem;
          }
          
          section > h2 {
            font-size: 1.7rem;
          }

          .email-container {
        padding: 1rem;
        display: flex;
        align-items: center;
      }
      
      .email-container h3 {
        font-size: 1.2rem;
        font-weight: 600;
        margin-right: 1rem;
      }
      
      .email-container input[type="text"] {
        height: 1.5rem;
        width: 15rem;
        margin-right: 1rem;
      }
      
      .email-container a {
        padding: 0.3rem;
        font-weight: 600;
        border-radius: 0.3rem;
        background-color: #eee;
        text-decoration: none;
      }

      .email-container a:active {
        background-color: #333;
      }
        </style>
  )===";
  return htmlStyle;
}

String HTMLDocument::setSpanDesign(String postFix, float value, String toBeChecked) {
  RangeChecker checker(0);
  String checkerResponse = checker.checker(value, toBeChecked);

  Serial.println(checkerResponse);
  
  String spanStyle = "";
  if(checkerResponse == "normal") {
    spanStyle = R"===(
      <span style="color: green;">
    )===";
    spanStyle += String(value) + postFix + "</span>";
  }
  else if(checkerResponse == "above") {
    spanStyle = R"===(
      <span style="color: red;">
    )===";
    spanStyle += String(value) + postFix + "</span>";
  }
  else {
    spanStyle = R"===(
      <span style="color: blue;">
    )===";
    spanStyle += String(value) + postFix + "</span>";  
  }

  return spanStyle;
}

void HTMLDocument::HTMLDocumentResponse(float temperatureReading, float humidityReading, int heartRateReading, float oxygenlevelReading) {
  HTTPResponseHeaders();
  
  String htmlForm = R"===(
  <!DOCTYPE html>
  <head>)===";

  htmlForm += HTMLStyle();
  
  htmlForm += R"===(<title>Health Band</title>
  </head>
  <body>
    <header>
    <h1>Health Band</h1>
    </header>
    <div class="readings-container">
      <div class="container">
        <h1>Room Readings</h1>
        <section class="room-readings">
        <h2 class="temperature">Temperature: )===";
        htmlForm += setSpanDesign("C", temperatureReading, "temperature") + "</h2>";

        htmlForm += R"===(<h2 class="humidity">Humidity: )===";
            
        htmlForm += setSpanDesign("%", humidityReading, "humidity") + "</h2>";
        
      htmlForm += R"===(</section>
      </div>
      <div class="container">
        <h1>Health Readings</h1>
        <section class="health-readings">
        <h2 class="heart-rate">Heart Rate: )===";

        htmlForm += setSpanDesign("bpm", heartRateReading, "heartrate") + "</h2>";
            
        htmlForm += R"===(<h2 class="oxygen-level">Oxygen Level: )===";
        
        htmlForm += setSpanDesign("%", oxygenlevelReading, "oxygenlevel") + "</h2>";
        
      htmlForm += R"===(</section>
      </div>
    </div>

    <div class="email-container">
        <h3>Enter preferred email</h3>
        <input type="text" id="choosen-email">
        <a href="/on">Submit</a>
    </div>
    <script>
        function getValue() {
            let textValue = document.querySelector("input[type='text']").value

            if(textValue == "")
                return

            let a = document.querySelector("a")
            a.href = "/"
            a.href += textValue
        }
        let anchor = document.querySelector("a")
        anchor.addEventListener("click", getValue)
    </script>
  </body>
  )===";
  
  _client.print(htmlForm);
  _client.println();
}
