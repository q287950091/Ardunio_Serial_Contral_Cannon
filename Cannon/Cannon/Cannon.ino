int KeyCharge = 2,             //充能按鍵
  KeyFire = 3,                 //發射按鍵
  OutCharge = A0,              //充能IGBT
  OutFire = A1,                //發射IGBT
  DefaultChargeTime = 1000;    //預設衝能時間
String String_Fire = "FIRE";   //發射指令設置
String String_Charge = "CHE";  //充能指令設置
String String_Exit = "EXIT";   //充能跳脫指令設置

void setup() {
  Serial.begin(9600);  //啟動端口監聽
  pinMode(KeyCharge, INPUT_PULLUP);
  pinMode(KeyFire, INPUT_PULLUP);
  pinMode(OutCharge, OUTPUT);
  pinMode(OutFire, OUTPUT);
}

void loop() {

  //指令控制
  if (Serial.available() > 0) {          //檢測串口狀態
    String Input = Serial.readString();  //獲取串口輸入
    Input.trim();
    if (Input == String_Fire) {
      Trigger_Fire();
    }
    if (Input.startsWith(String_Charge, 0)) {  //字串處理
      Input.replace(String_Charge, "");
      Input.replace(" ", "");
      Serial.println(Input.toInt());  //使用預設時間或自訂時間長度
      if (Input.toInt() != 0) {
        Trigger_Charge(Input.toInt() * 1000);
      } else {
        Trigger_Charge(DefaultChargeTime);
      }
    }
  }
  //物理控制
  if (!digitalRead(KeyCharge)) {
    Trigger_Charge(DefaultChargeTime);
  }
  if (!digitalRead(KeyFire)) {
    Trigger_Fire();
  }
  while (!digitalRead(KeyFire))
    ;
  delay(100);
}

void Trigger_Charge(long T) {  //充能
  long i = 0;
  digitalWrite(OutCharge, HIGH);
  for (i = 0; T >= i || !digitalRead(KeyCharge); i++) {
    if (Serial.available() > 0) {          //檢測串口狀態
      String Input = Serial.readString();  //跳脫檢測
      if (Input.startsWith(String_Exit, 0)) T = 0;
      Serial.println("Charge Escaped");
    }
    delay(1);
  }
  digitalWrite(OutCharge, LOW);
  Serial.println("Charge Completed");
  Serial.print("Charge Time:");
  Serial.println(i / 1000);
}

void Trigger_Fire() {  //發射
  digitalWrite(OutFire, HIGH);
  delay(100);
  digitalWrite(OutFire, LOW);
  Serial.println("Fired");
}