void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
}

void loop() {
  if(Serial.available()){
    char c = Serial.read(); 
    if( c == 'w'){
      //start water sensing tasks 
    }else if( c == 'e'){
      //start earth tasks
    }else if(c == 'f'){
      //start fire tasks
    }else if(c == 'a'){
      //start air tasks 
    }
  }

}
