// Used to hold data that comes from the serial port
String readString; 

// holds serial port incoming messages
String inputString = "";

// Message from the serial port is ready
bool stringComplete = false; 

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
unsigned long period = 1500;  //the value is a number of milliseconds
unsigned long ticks = 0;

bool printAM = false;
bool printPH = false;
bool printER = false;
bool printMA = false;
bool printKV = false;

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
int actualKV = 600;


unsigned int MA = 1000;
String StrMA = "!MA";
String requestMA = "?MA";
String SetMA = "#MA";
int actualMA = 1000;

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
void DumpBytes(String inStr);



void setup() {
  // put your setup code here, to run once:
  startMillis = millis();  //initial start time
                                           
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
  if(KV > ph2)
  {
    ER5 = "1";
  }else
  {
    ER5 = "0";
  }
  
  
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
      if(stringComplete)
      {
        if(containsKVStr(inputString))
        {
          
          KV = extractNumber(inputString);
          Serial.println(inputString);
          stringComplete = false;
          inputString = "";
          String testStr = StrKV + String(KV) + "\n";
          Serial.print(testStr);
        }
     
        if(containsStr(inputString, requestKV))
        {
          String testStr = StrKV + String(KV) + "\n";
          Serial.print(testStr);
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
        if(containsStr(inputString, requestMA) || containsStr(inputString, "?MA3"))
        {
          String testStr = StrMA + String(MA) + "\n";
          Serial.println(testStr);
          stringComplete = false;
          inputString = "";
        }

        if(containsStr(inputString, requestPH))
        {
          String testStr = StrPH + String(ph1) + "," + String(ph2) + "," + String(ph3) + "<\n";
          Serial.println(testStr);
          stringComplete = false;
          inputString = "";
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
        String testStr = StrKV + String(KV) + "\n";
          Serial.print(testStr);
      }

      if(printER)
      {
        Serial.println(MakeERString());
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
         String testStr = StrMA + String(MA) + "\n";
          Serial.println(testStr);
      }
      startMillis = millis();
  }
   
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
