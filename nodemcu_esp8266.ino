#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>

#include <vector>

#define RXPIN 13
#define TXPIN 15

#define RXPIN2 5
#define TXPIN2 16

#define BAUD_RATE 115200

SoftwareSerial * arduinoSerial;
SoftwareSerial * arduino2Serial;

std::vector<String> AvailableTypes = {"TEMP", "HUM", "CO2", "COV", "NO2", "PM1", "PM2p5", "PM10"};

bool isDigit(const String& str);

struct Param
{
  String pname;
  String pvalue;
};

class Serv
{
   public :

      Serv(String servSSID, String servPass, String servAPIUrl)
      {
        WiFi.begin(servSSID, servPass);
        this->serverUrl = servAPIUrl;

        Serial.print("Connecting");
  
  
        while (WiFi.status() != WL_CONNECTED) {
          delay(500) ;
          Serial.print(".");
        }
      
        Serial.println() ;
        Serial.print("Connected, IP address: ");
        Serial.println(WiFi.localIP()) ;

        this->whttp = new HTTPClient();
        this->wclient = new WiFiClient();

        this->whttp->begin(*wclient, serverUrl);
          
        this->whttp->addHeader("Content-Type", "application/x-www-form-urlencoded");
      }

      void Send_POST_Data(const std::vector<Param>& _args)
      {
        this->requestBody = "";
        for(auto& _arg : _args)
        {
          if(this->requestBody == "")
          {
             this->requestBody += "type=" + _arg.pname + "&value=" + _arg.pvalue;
          }
          else
          {
            this->requestBody += "&type=" + _arg.pname + "&value=" + _arg.pvalue;
          }
          Serial.println("req body : " + requestBody);
        }

        
        if (WiFi.status() == WL_CONNECTED) {
          this->httpResponseCode = whttp->POST(requestBody);
          Serial.println("RESPONSE CODE : " + String(httpResponseCode));
          delay(1000);
          Serial.println("Resetting...");
          this->resetServ();
          delay(1000);
          return;
         }
      }

   private :

      void resetServ()
      {
        this->whttp->end();
        delete this->whttp;
        delete this->wclient;
        this->wclient = new WiFiClient();
        this->whttp = new HTTPClient();
        this->whttp->begin(*wclient, serverUrl);
        this->whttp->addHeader("Content-Type", "application/x-www-form-urlencoded");
      }
     
      String serverUrl;
      String requestBody;
      HTTPClient* whttp;
      WiFiClient* wclient;
      int httpResponseCode;
};

class ArduinoSoftwareSerial : public SoftwareSerial 
{
   public :   
      enum ReadingStates {START_TYPE, END_TYPE, START_DATA, END_DATA, UNKNOWN_STATE};

      ArduinoSoftwareSerial(uint8_t rxPin, uint8_t txPin, int _baudRate, String _name) : SoftwareSerial(rxPin, txPin) {
        delay(500);
        this->begin(_baudRate);
        this->current_reading_state = UNKNOWN_STATE;
        this->current_reading_type = "";
        this->current_reading_data = ""; 
        this->arduino_name = _name;
      }


      void SetServer(Serv* _serv)
      {
        this->serv = _serv;
      }

      String GetName() const
      {
        return this->arduino_name;
      }

      void ReadBuffer()
      {
        if(this->available() > 0)
        {
          this->current_reading_char = static_cast<char>(this->read());
          switch((int)this->current_reading_char)
          {
            case (int)'{': this->current_reading_state = START_TYPE; break;
            case (int)'}': this->current_reading_state = END_TYPE;   break;
            case (int)'[': this->current_reading_state = START_DATA; break;
            case (int)']': this->current_reading_state = END_DATA;   break;
            
            default :
              if(this->current_reading_state == START_TYPE) { current_reading_type += current_reading_char; }
              else if(this->current_reading_state == START_DATA) { current_reading_data += current_reading_char; }
              else if(this->current_reading_state == END_DATA)
              {
                this->SendDataToAPI(current_reading_type, current_reading_data);
                current_reading_type = "";
                current_reading_data = "";
                current_reading_state == UNKNOWN_STATE;
              }
              break;
          } 
        }
      }
      

      void SendDataToAPI(String type, String data) const
      {
        if(type == "PM2.5"){ type = "PM2p5";}
        for(auto& t : AvailableTypes)
        {
          if(t == type && isDigit(data))
          {
            this->serv->Send_POST_Data({{type, data}});
            break;
          }
        }
      }
      
   private :

   int current_reading_state;
   char current_reading_char;
   String current_reading_type;
   String current_reading_data;
   String arduino_name;

   Serv* serv;
   
};

std::vector<ArduinoSoftwareSerial*> arduinoSerials;
Serv* purifeyeAPI;

void setup(){

  Serial.begin(115200);
  Serial.println() ;
  
  
  arduinoSerials.push_back(new ArduinoSoftwareSerial(RXPIN, TXPIN, BAUD_RATE, "arduino 1"));
  arduinoSerials.push_back(new ArduinoSoftwareSerial(RXPIN2, TXPIN2, BAUD_RATE, "arduino 2"));

  purifeyeAPI = new Serv("Amaury", "12345678", "http://purifeye-app.herokuapp.com/api/entries");
  
  delay(2500);

  for(ArduinoSoftwareSerial* arduino: arduinoSerials)
  {
    arduino->SetServer(purifeyeAPI);
  }
  
  Serial.println("SoftwareSerial enabled.");

}

void loop() {
  digitalWrite(0, HIGH); // ENABLE PIN
  digitalWrite(4, HIGH); // ENABLE PIN
  
  for(ArduinoSoftwareSerial* arduino: arduinoSerials)
  {
    arduino->ReadBuffer();
  }
}

bool isDigit(const String& str) {
  bool hasDecimalPoint = false;
  bool hasDigit = false;

  for (size_t i = 0; i < str.length(); i++) {
    char c = str.charAt(i);

    if (c == '.') {
      if (hasDecimalPoint || i == 0 || i == str.length() - 1) {
        return false;
      }
      hasDecimalPoint = true;
    } else if (isdigit(c)) {
      hasDigit = true;
    } else {
      return false;
    }
}

return hasDigit;
}
