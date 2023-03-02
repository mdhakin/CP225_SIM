// Used to hold data that comes from the serial port
String readString; 

// holds serial port incoming messages
String inputString = "";

// Message from the serial port is ready
bool stringComplete = false; 

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 500;  //the value is a number of milliseconds
unsigned long ticks = 0;

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


int KV = 1000;
String StrKV = "!KV";
String requestKV = "?KV";
String SetKV = "#KV";
int actualKV = 1000;


int MA = 9000;
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
  
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
      if(stringComplete)
      {
        if(containsKVStr(inputString))
        {
          KV = extractNumber(inputString);
          stringComplete = false;
          inputString = "";
          String testStr = StrKV + String(KV) + "\n";
          Serial.println(testStr);
        }
     
        if(containsStr(inputString, requestKV))
        {
          String testStr = StrKV + String(KV) + "\n";
          Serial.println(testStr);
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
        if(containsStr(inputString, requestMA))
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
        
      }
      Serial.print(MakeERString());
      Serial.print(MakeAMString());
      Serial.print(PHString());
      startMillis = millis();
  }
    //Serial.write("!ER0,0,0,0,0,1,0,0\n");
    //Serial.write("!XO,024,023,019,5777<\n");
    //Serial.write("!PH1200,600,0<\n");
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
  String newER = ER + ER0 + "," + ER1+ "," + ER2+ "," + ER3+ "," + ER4+ "," + ER5+ "," + ER6+ "," + ER7+ "," + "<" + "\n";
  return newER;
}

String PHString()
{
  String newPH = "!PH" + String(ph1) + "," + String(ph2) + "," + String(ph3) + "<" + "\n";
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
  delay(1000);
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
