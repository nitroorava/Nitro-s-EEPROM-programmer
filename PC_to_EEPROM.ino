#define DATA 2
#define CLK 3
#define LATCH 4
#define WRITE 13

#define EEPROM_D0 5
#define EEPROM_D7 12

byte receivedByte;
bool newData=false;

void setAddress(int address, bool outputEnable){
  shiftOut(DATA ,CLK ,MSBFIRST ,(address>>8)|(outputEnable ? 0x00:0x80));
  shiftOut(DATA ,CLK ,MSBFIRST ,address);

  digitalWrite(LATCH,LOW);
  digitalWrite(LATCH,HIGH);
  digitalWrite(LATCH,LOW);
  }

byte readEEPROM(int address){
  for(int pin=EEPROM_D0;pin<=EEPROM_D7;pin++){
    pinMode(pin,INPUT);
    }
  
  setAddress(address,true);

  delay(10);
  byte data =0;
  for(int pin = EEPROM_D7; pin >=EEPROM_D0; pin-=1){
    data= (data<<1)+digitalRead(pin);
    }
  
  return data;
  }

void writeEEPROM(int address, byte data){
  for(int pin=EEPROM_D0;pin<=EEPROM_D7;pin++){
    pinMode(pin,OUTPUT);
    }
  
  setAddress(address,false);
  for(int pin = EEPROM_D0; pin<=EEPROM_D7; pin++){
    digitalWrite(pin,data & 1);
    data= data>>1;
    }
    digitalWrite(WRITE,LOW);
    delayMicroseconds(1);
    digitalWrite(WRITE,HIGH);
    delay(5);
  }
byte buf[8];
byte dataFromSerial(){
  
  if (Serial.available()>0){
    Serial.readBytes(buf,1);
    receivedByte=buf[0];
    newData=true;
  }
}


//give printContents a start value and a finish value and it will print everything in between 
//including the start and excluding finish values

void printContents(int start,int finish){
  byte data[16];
  for(int base=start; base<finish; base+=16){
      for(int offset =0; offset<=15; offset++){
        data[offset] = readEEPROM(base+offset);
        }
      char buf[80];  
      sprintf(buf, "%03x: %02x %02x %02x %02x %02x %02x %02x %02x     %02x %02x %02x %02x %02x %02x %02x %02x",
      base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
      data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
      
      Serial.println(buf);
    }
}

void printNoFormat(int start, int finish){
  byte data=0;
  for(int n=start;n<finish;n++){
    data= readEEPROM(n);
    Serial.print(data,HEX);
    
    }
  }

byte digits[] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b, 0x77, 0x1f, 0x4e, 0x3d, 0x4f, 0x47 };

void setup() {
  // put your setup code here, to run once:
  digitalWrite(DATA,LOW);
  digitalWrite(CLK,LOW);
  digitalWrite(LATCH,LOW);
  digitalWrite(WRITE,HIGH);
  
  pinMode(DATA,OUTPUT);
  pinMode(CLK,OUTPUT);
  pinMode(LATCH,OUTPUT);
  pinMode(WRITE,OUTPUT);

  Serial.begin(19200);
  //Serial.println("Serial OK");
  //printContents(0,255);
}

int i =0;
void loop() {
//   put your main code here, to run repeatedly:

  dataFromSerial();
  if(newData==true){
    writeEEPROM(i,receivedByte);
    
    newData=false;
    Serial.write(readEEPROM(i));
    i++;
  }  
}
