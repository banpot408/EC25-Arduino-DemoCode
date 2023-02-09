
#define RXD1         35//34
#define TXD1         33//32

#define Lora_RST 12
#define EN_Pow   14

//String phone_number = "+66624820181";
String phone_number = "+66618055704";
//SoftwareSerial mySerial(8, 9); // RX, TX
//AltSoftSerial mySerial;



unsigned char SignalQuality();
String GetOperator();

void debug(String data)
{
  Serial.println(data);
}

void setup() 
{
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1); 
  pinMode(EN_Pow, OUTPUT);
  pinMode(Lora_RST, OUTPUT);
  Serial.print("ESP32 On!");
  digitalWrite(EN_Pow, HIGH);
  digitalWrite(Lora_RST, LOW); 
  
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println(F("UC25 SMS TEST"));
  Serial.println();
  Serial.println();
  
 // gsm.PowerOn();
//  while(gsm.WaitReady()){}

  delay(1000);

  Serial.print(F("GetOperator --> "));
  Serial.println(GetOperator());
  Serial.print(F("SignalQuality --> "));
  Serial.println(SignalQuality());
  Serial.println("Send SMS");

  DefaultSetting();
  Start(phone_number);  
  Sendln("Hello");
  Sendln(" My neme is Banpot ");
  Sendln(" This is SMS for Test EC25 Module ");
  Stop();

  Serial.println("END"); 
 
}

void loop() 
{
 if (Serial1.available())
  {
    Serial.write(Serial1.read());
  } 
  if (Serial.available())
  {
    char c = Serial.read();
    Serial1.write(c);
    
  }
}



String GetOperator()
{
  Serial1.println(F("AT+COPS?"));
  while(!Serial1.available())
  {}
  start_time_out();
  while(1)
  {
    String req = Serial1.readStringUntil('\n');   
    if(req.indexOf(F("+COPS")) != -1)
    {
      //+COPS: 0,0,"TRUE-H",2
      /*char comma1 = req.indexOf(F(","));
      char comma2 = req.indexOf(F(","),comma1+1);
      char comma3 = req.indexOf(F(","),comma2+1);
      String  operate_name = req.substring(comma2+2,comma3-1);
      String  acc_tech = req.substring(comma3+1);
      return(operate_name + "," + acc_tech);
      */
      return(req.substring(req.indexOf(F("\""))));
    }
    if(time_out(3000))
    {
      return(F(""));
    }
    
  }
  return(F(""));
}
unsigned char SignalQuality()
{
  unsigned char ret = 101;
  Serial1.println(F("AT+CSQ"));
  while(!Serial1.available())
  {}
  start_time_out();
  while(1)
  {
    String req = Serial1.readStringUntil('\n');   
    if(req.indexOf(F("+CSQ")) != -1)
    {
      ret = req.substring(req.indexOf(F(" "))+1,req.indexOf(F(","))).toInt();
      return(ret);
    }
    if(time_out(3000))
    {
      return(ret);
    }
    
  }
  return(ret);
}

unsigned long previousMillis_timeout =0;
void start_time_out()
{
  previousMillis_timeout = millis();
}

bool time_out(long timeout_interval)
{
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis_timeout >= timeout_interval) 
  {
    previousMillis_timeout = currentMillis;
    return(true);
  }
  return(false);
}
bool wait_ok(long time)
{
  return wait_ok_(time,true);
}
bool wait_ok_ndb(long time)
{
  return wait_ok_(time,false);
}
  
bool wait_ok_(long time,bool ack)
{
  unsigned long previousMillis = millis(); 
  while(1)
  {
    String req = Serial1.readStringUntil('\n');
    if(req.indexOf(F("OK")) != -1)
    {
      if(ack)
  //    debug(F("OK"));
      return(1);
    }
    if(req.indexOf(F("ERROR")) != -1)
    {
      if(ack)
 //     debug(F("Error"));
      return(0);
    }
    //debug(req); 
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= time) 
    {
      previousMillis = currentMillis;
      if(ack)
 //     debug(F("Error"));
      return(0);
    }     

  }
  
}

void DefaultSetting()
{
  Serial1.println(F("AT+CMGF=1"));
//  Serial1.debug(F("AT+CMGF=1"));
  wait_ok(3000);
  Serial1.println(F("AT+CSMP=17,167,0,8"));
//  Serial1.debug(F("AT+CSMP=17,167,0,8"));
  wait_ok(3000);
  Serial1.println(F("AT+CSCS=\"UCS2\""));
//  Serial1.debug(F("AT+CSCS=\"UCS2\""));
  wait_ok(3000);
}

void Start(String rx_number)
{
  Serial1.print(F("AT+CMGS=\""));
  unsigned char i=0;
  while(rx_number[i])
  {
    Serial1.print(F("00"));
    Serial1.print(rx_number[i],HEX);
    i++;
  }
  Serial1.println(F("\""));
  while(!Serial1.available())
  {}
  String req = Serial1.readStringUntil('>');
  
}
void Send(String data)
{
  unsigned char i=0;
  String buf="";
  while(data[i])
  {
    unsigned char c = data[i]&0xFF;
    if(c==0xE0)
    {
      Serial1.print(F("0E"));
      //gsm.debug("0E");
      i++;
      c = data[i];
      if(c == 0xB8)
      {
        i++;
        c = data[i]-0x80;
        if(c <= 0x0F)
        {
          Serial1.print(F("0"));
          //gsm.debug(F("0"));          
        }
        buf = String(c,HEX);
        buf.toUpperCase();
        Serial1.print(buf);
        //gsm.debug(buf);
      }
      else
      {
        i++;
        c = data[i]-0x40;
        if(c <= 0x0F)
        {
          Serial1.print(F("0"));
          //gsm.debug(F("0"));
        }
        buf = String(c,HEX);
        buf.toUpperCase();
        Serial1.print(buf);
        //gsm.debug(buf);
      }     
    }
    else
    {

      Serial1.print(F("00"));
      if(c == 0x0A)
      {
       Serial1.print("0A");
      // gsm.debug("0A");
      }
      else if(c == 0x0D)
      {
       Serial1.print("0D");
      // gsm.debug("0D");
      }
      else
      {
        buf = String(c,HEX);
        buf.toUpperCase();
        Serial1.print(buf);
        //gsm.debug(buf);
      }
        
    }
    i++;
  }
  
}
void Sendln(String data)
{
  Send(data+"\r\n");
}

void Stop()
{
  Serial1.write(0x1A);
  if(wait_ok(10000))
  {
    //gsm.debug("Send OK");
  }
}
