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

String AM = "!AM0001,0001,0000,0100,045000,150608<\n";
String AT = "!AT027,024,023,019,5777<\n";


int ph1 = 2200;
int ph2 = 600;
int ph3 = 0;


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


void setup() {
  // put your setup code here, to run once:
  startMillis = millis();  //initial start time
                                           
  Serial.begin(19200);
  Serial.println("Ready");
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
        
        

      }
    startMillis = millis();
  }
    //Serial.write("!ER0,0,0,0,0,1,0,0\n");
    //Serial.write("!XO,024,023,019,5777<\n");
    //Serial.write("!PH1200,600,0<\n");
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
