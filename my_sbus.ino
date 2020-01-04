
#define LEN 25
byte buf[LEN]; 

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(100000,SERIAL_8E2);
  Serial1.setTimeout(17);
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  size_t l=0;
  buf[0]=0;
  while (buf[0]!=0x0f && l!=LEN){
    l=Serial1.readBytes(buf,LEN);
    if (l!=LEN) Serial.println("Timeout");
    if (buf[0]!=0x0f) Serial.println("bad header");
  }

  unsigned int pack[16];
  int bit_ptr=0;
  int byte_ptr=1;
  
  for (int ch=0;ch<16;ch++){
    pack[ch]=0;
    for (int i=0;i<=10;i++) {
      if (bit_ptr>7){
        bit_ptr=0;
        byte_ptr++;
      }
      pack[ch]+=(unsigned int) ((buf[byte_ptr]>>bit_ptr) & 0x01) << i;
      bit_ptr++;
    }
  }
       
  

  char str[10]; 
  sprintf(str,"%3d:",l);Serial.print(str);
  for (int i=0;i<16;i++) {
    sprintf(str,"%6d",pack[i]);  Serial.print(str);
     }
  Serial.println();
}
