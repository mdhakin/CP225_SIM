enum tubeState {
  idle,
  preheating,
  shot
};

tubeState ts = idle;
int iDelay = 5;
// Used to hold data that comes from the serial port
String readString; 

// holds serial port incoming messages
String inputString = "";

// Message from the serial port is ready
bool stringComplete = false; 

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
unsigned long period = 1500;  //the value is a number of milliseconds
unsigned long startMillis2;  //some global variables available anywhere in the program
unsigned long currentMillis2;
unsigned long period2 = 1000;  //the value is a number of milliseconds
unsigned long ticks = 0;

bool bShootingRays = false;
bool bPower = false;


bool printAM = false;
bool printPH = false;
bool printER = false;
bool printMA = false;
bool printKV = false;
bool printAT = false;

String AMProto = "!AM0001,0001,0000,0100,045000,150608<\n";
String AM = "!AM";
unsigned int AM0 = 1;
unsigned int AM1 = 1;
unsigned int AM2 = 1;
unsigned int AM3 = 1;
unsigned int AM4 = 45000;
unsigned long AM5 = 150608;

String ATProto = "!AT027,024,023,019,5777<\n";
String AT = "!AT";
unsigned int AT0 = 27;
unsigned int AT1 = 24;
unsigned int AT2 = 23;
unsigned int AT3 = 19;
unsigned int AT4 = 5777;

String StrPH = "!PH";
String requestPH = "?PH";
unsigned int ph1 = 2200;
unsigned int ph2 = 600;
unsigned int ph3 = 0;


int KV = 600;
String StrKV = "!KV";
String requestKV = "?KV";
String SetKV = "#KV";
int actualKV = 0;


unsigned int MA = 1000;
String StrMA = "!MA";
String requestMA = "?MA";
String SetMA = "#MA";
int actualMA = 0;

String ERProto = "!ER0,0,0,0,0,1,0,0\n";
String ER = "!ER";

String ER0 = "0";
String ER1 = "0";
String ER2 = "0";
String ER3 = "0";
String ER4 = "0";
String ER5 = "0"; // err bit
String ER6 = "0";
String ER7 = "0";

String changeErr = "#ER";


// *******************************************************************
// *******************************************************************
// Function prototypes
// *******************************************************************
// *******************************************************************

// Flush the serial buffer
void serial_flush(void);

// read input commands
void readstring();
bool containsKVStr(String inStr);
int extractNumber(String inStr);
bool containsStr(String inStr, String substr);
byte calculateCRC(byte data[], int len);
byte* convertStringToByteArray(String str);
String MakeERString();
String PHString();
String MakeAMString();
String MakeATString();
void DumpBytes(String inStr);



void setup() {
  // put your setup code here, to run once:
  startMillis = millis();  //initial start time
  startMillis2 = millis();  //initial start time
  pinMode(LED_BUILTIN, OUTPUT);                                        
  Serial.begin(19200);
  Serial.println("Ready");
  Serial.print(MakeERString());
  Serial.print(PHString());
  Serial.print(MakeAMString());
}
void loop() {
  // put your main code here, to run repeatedly:
  readstring();
  currentMillis = millis();
  currentMillis2 = millis();
  if(KV > ph2)
  {
    ER5 = "1";
  }else
  {
    ER5 = "0";
  }
  
  if (currentMillis2 - startMillis2 >= period2)  //test whether the period has elapsed
  {
    if(bShootingRays)
    {
      String testStr = "!XO\n";
      Serial.println(testStr);
    }
    startMillis2 = millis();
  }
  
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
      updateActualKVandMAValues();
      if(bShootingRays)
      {
        Serial.println("!!XRay ON!!");
      }
      
      if(true)
      {
          String oput = MakeERString() + "\n" + PHString() + "|n" + MakeATString() + StrKV + String(KV) + "<\n" + StrMA + String(MA);
          Serial.println(oput);
          delay(200);
          Serial.println( PHString());
          Serial.println( StrKV + String(KV));
          
          Serial.flush();
      }

      if(stringComplete)
      {
        if(containsStr(inputString, "#ST"))
        {
          ts = idle;
          bPower = false;
          bShootingRays = false;
          stringComplete = false;
          inputString = "";
          Serial.println("Stop signal received");
          updateActualKVandMAValues();
        }
        
        if(containsStr(inputString, "?PW"))
        {
          if(bPower)
          {
            Serial.println("Power On");
          }else
          {
            Serial.println("Power Off");
          }
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#PW"))
        {
          if(ER5 == "0" && ts != idle)
          {
            bPower = true;
          }else
          {
            bPower = false;            
          }
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "?KV"))
        {
          String testStr = StrKV + String(KV) + "<\n";
          Serial.println(testStr);
          stringComplete = false;
          inputString = "";
          Serial.flush();
        }        
        //if(containsStr(inputString, "?MA"))
        if(true)
        {
          String testStr1 = StrMA + String(MA) + "<\n" + StrMA + String(MA) + "<\n";
          Serial.println(testStr1);
          //stringComplete = false;
          //inputString = "";
          delay(iDelay);
        }
        if(containsStr(inputString, "#ON"))
        {
          if(bPower)
          {
            bShootingRays = true;
          }
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#XO"))
        {
          Serial.println("Recieved Heartbeat Response");
          stringComplete = false;
          inputString = "";
        }
        
        if(containsStr(inputString, "?ST"))
        {
          String tempStateString = "";
          if(ts == idle)
          {
            tempStateString = "ST:idle";
          }else if (ts == preheating)
          {
            tempStateString = "ST:preheat";
          }
          else
          {
            tempStateString = "ST:shot";
          }
          Serial.println(tempStateString);
          stringComplete = false;
          inputString = "";
        }        
        if(containsStr(inputString, "#KV"))
        {
          int temp = extractNumber(inputString);
          KV = temp;
          String testStr = StrKV + String(KV) + "<\n";
          Serial.println(testStr);          
        }
        if(containsStr(inputString, "#TS0"))
        {
          ts = idle;
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#TS1") && ER5 == "1")
        {
          ts = preheating;
          actualKV = KV;
          AM0 = KV;
          actualMA = MA;
          AM1 = MA;
          stringComplete = false;
          inputString = "";
        }

        if(containsStr(inputString, "#TS2") && ER5 == "0")
        {
          ts = shot;
          
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, SetMA))
        {
          MA = extractNumber(inputString);
          
          stringComplete = false;
          inputString = "";
          String testStr = StrMA + String(MA) + "\n";
          Serial.println(testStr);
        }
        
        if(containsStr(inputString, requestPH))
        {
          String testStr = StrPH + String(ph1) + "," + String(ph2) + "," + String(ph3) + "<\n";
          Serial.println(testStr);
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#phone"))
        {
          ph1 = extractNumber(inputString);
          stringComplete = false;
          inputString = "";
          String testStr = StrPH + String(ph1) + "," + String(ph2) + "," + String(ph3) + "<\n";
          Serial.println(testStr);
        }
        if(containsStr(inputString, "#phtwo"))
        {
          ph2 = extractNumber(inputString);
          stringComplete = false;
          inputString = "";
          String testStr = StrPH + String(ph1) + "," + String(ph2) + "," + String(ph3) + "<\n";
          Serial.println(testStr);
        }        
        if(containsStr(inputString, changeErr))
        {
          
          int NewEr5 = extractNumber(inputString);
          ER5 = String(NewEr5);
          String testStr = MakeERString();
          Serial.println(testStr);
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "?ER"))
        {
          String testStr = MakeERString();
          Serial.println(testStr);
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#Moff"))
        {
          printMA = false;
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#Mon"))
        {
          printMA = true;
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#Koff"))
        {
          printKV = false;
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#Kon"))
        {
          printKV = true;
          stringComplete = false;
          inputString = "";
        }        
        if(containsStr(inputString, "#AToff"))
        {
          printAT = false;
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#ATon"))
        {
          printAT = true;
          stringComplete = false;
          inputString = "";
        }        
        if(containsStr(inputString, "#AMoff"))
        {
          printAM = false;
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#AMon"))
        {
          printAM = true;
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#PHoff"))
        {
          printPH = false;
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#PHon"))
        {
          printPH = true;
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#Eff"))
        {
          printER = false;
          Serial.println("Error off");
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#Eon"))
        {
          printER = true;
          Serial.println("Error on");
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#Allon"))
        {
          printER = true;
          printPH = true;
          printAM = true;
          printAT = true;
          printMA = true;
          printKV = true;
          Serial.println("All on");
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#Alloff"))
        {
          printER = false;
          printPH = false;
          printAM = false;
          printAT = false;
          printMA = false;
          printKV = false;
          Serial.println("All off");
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "?PER"))
        {
          Serial.print("Period: ");
          Serial.println(String(period));
          stringComplete = false;
          inputString = "";
        }
        if(containsStr(inputString, "#PER"))
        {
          long newPeriod = extractNumber(inputString);

          if(newPeriod > 300)
          {
            period = newPeriod;            
          }
          
          Serial.println(String(period));
          stringComplete = false;
          inputString = "";
        }
      }
      if(printKV)
      {
        String testStr = StrKV + String(KV) + "<\n";
          Serial.print(testStr);
          delay(iDelay);
      }
      if(printER)
      {
        Serial.println(MakeERString());
        delay(iDelay);
      }
      if(printAM)
      {
        Serial.print(MakeAMString());
      }
      if(printPH)
      {
        Serial.println(PHString());
      }
      if(printMA)
      {
         String testStr = StrMA + String(MA) + "<\n";
          Serial.println(testStr);
      }
      if(printAT)
      {
        Serial.print(MakeATString());
      }
      startMillis = millis();
  }
}

void updateActualKVandMAValues()
{
  if(ts != idle && bPower && bShootingRays && ER5 == "0")
  {
    AM0 = KV;
    AM1 = MA; 
    digitalWrite(LED_BUILTIN, HIGH);
  }else if(ts == shot && bPower && bShootingRays && ER5 == "1")
  {
    
    bPower = false;          
    bShootingRays = false;
    AM0 = 0;
    AM1 = 0;
    digitalWrite(LED_BUILTIN, LOW);
  }else
  {
    AM0 = 0;
    AM1 = 0;
    digitalWrite(LED_BUILTIN, LOW);
  }
}














String MakeATString()
{

  String sAT0 = String(AT0);
  int at0len = sAT0.length();
  int at0toadd = 3 - at0len;

  for(int i = 0;i < at0toadd;i++)
  {
    sAT0 = "0" + sAT0;
  }
  
  String sAT1 = String(AT1);
  int at1len = sAT1.length();
  int at1toadd = 3 - at1len;

  for(int i = 0;i < at1toadd;i++)
  {
    sAT1 = "0" + sAT1;
  }

   String sAT2 = String(AT2);
  int at2len = sAT2.length();
  int at2toadd = 3 - at2len;

  for(int i = 0;i < at2toadd;i++)
  {
    sAT2 = "0" + sAT2;
  }

  String sAT3 = String(AT3);
  int at3len = sAT3.length();
  int at3toadd = 3 - at3len;

  for(int i = 0;i < at3toadd;i++)
  {
    sAT3 = "0" + sAT3;
  }
  
  String newAT = AT + sAT0 + "," + sAT1 + "," + sAT2 + "," + sAT3 + "," + String(AT4) + "\n";
  return newAT;
}
String MakeAMString()
{

  String sAM0 = String(AM0);
  int am0len = sAM0.length();
  int amtoadd = 4 - am0len;

  for(int i = 0;i < amtoadd;i++)
  {
    sAM0 = "0" + sAM0;
  }
  
  String sAM1 = String(AM1);
  int am1len = sAM1.length();
  int am1toadd = 4 - am1len;

  for(int i = 0;i < am1toadd;i++)
  {
    sAM1 = "0" + sAM1;
  }

  String sAM2 = String(AM2);
  int am2len = sAM2.length();
  int am2toadd = 4 - am2len;

  for(int i = 0;i < am2toadd;i++)
  {
    sAM2 = "0" + sAM2;
  }

  String sAM3 = String(AM3);
  int am3len = sAM3.length();
  int am3toadd = 4 - am3len;

  for(int i = 0;i < am3toadd;i++)
  {
    sAM3 = "0" + sAM3;
  }
  
  String newAM = AM + sAM0 + "," + sAM1 + "," + sAM2 + "," + sAM3 + "," + "0" + String(AM4) + "," + String(AM5) + "<" + "\n";
  return newAM;
}
String MakeERString()
{
  String newER = ER + ER0 + "," + ER1+ "," + ER2+ "," + ER3+ "," + ER4 + "," + ER5 + "," + ER6 + "," + ER7 + "," + "<" + "\n";
  return newER;
}
String PHString()
{
  String newPH = "!PH" + String(ph1) + "," + String(ph2) + "," + String(ph3) + "<\n";
  return newPH;
}
void serial_flush(void) {
  while (Serial.available()) Serial.read();
}
void readstring()
{
    while (Serial.available()) {
      // get the new byte:
      char inChar = (char)Serial.read();
      // add it to the inputString:
      inputString += inChar;
        // if the incoming character is a newline, set a flag so the main loop can
        // do something about it:
        if (inChar == '\n')
        {
          stringComplete = true;
          serial_flush();
          return;
        }
    }
}
bool containsStr(String inStr, String substr)
{
  //String substr = "#KV";
  int pos = inStr.indexOf(substr);

  if (pos >= 0) {
    return true;
  } else {
    return false;
  }
  delay(1000);
}
bool containsKVStr(String inStr)
{
  String substr = "#KV";
  int pos = inStr.indexOf(substr);

  if (pos >= 0) {
    return true;
  } else {
    return false;
  }
  //delay(1000);
}
int extractNumber(String inStr)
{
  String numStr = "";
  
  // Extract numbers from the string
  for (int i = 0; i < inStr.length(); i++) {
    if (isdigit(inStr.charAt(i))) {
      numStr += inStr.charAt(i);
    }
  }

  // Convert the extracted string of numbers to an integer
  int num = atoi(numStr.c_str());
  //Serial.print(inStr);
  //Serial.print(" Extract ");
  //Serial.print(num);
  //Serial.println("---");
  return num;
}
byte calculateCRC(byte data[], int len)
{
    byte crc = 0;
    for (int i = 0; i < len; i++)
    {
        if (i == 0)
        {
            crc = (byte)(data[i] ^ data[i + 1]);
            i = 1;
        }
        else
        {
            crc = (byte)(crc ^ data[i]);
        }
    }
    return crc;
}
byte* convertStringToByteArray(String str)
{
  char charArray[str.length() + 1];
  str.toCharArray(charArray, sizeof(charArray));

  byte byteArray[sizeof(charArray)];
  for(int i = 0; i < sizeof(charArray);i++)
  {
    byteArray[i] = charArray[i];
  }

  byteArray[sizeof(byteArray) -1] = calculateCRC(byteArray, sizeof(byteArray));

  return byteArray;
}
void DumpBytes(String inStr)
{
  Serial.println("-----");
  int iLen = inStr.length();
  for(int i = 0;i<iLen;i++)
  {
    byte a = (byte)inStr[i];
      Serial.print(a);
      Serial.print(" ");
  }
  Serial.println("");
  for(int i = 0;i<iLen;i++)
  {
    byte a = (byte)inStr[i];
      Serial.print(a, HEX);
      Serial.print(" ");
  }

  Serial.print("\n");
  Serial.println("Original: " + inStr);
  Serial.print("String length: ");
  Serial.println(inStr.length());
  Serial.println("-----");
}
