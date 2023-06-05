#define OutCharge GPIO_NUM_22    //發射
#define OutFire GPIO_NUM_23      //充能
#define KeyCharge GPIO_NUM_34    //充能按鍵
#define KeyFire GPIO_NUM_35       //發射按鍵
int DefaultChargeTime = 1000;    //預設衝能時間
String String_Fire = "FIRE";   //發射指令設置
String String_Charge = "CHE";  //充能指令設置
String String_Exit = "EXIT";   //充能跳脫指令設置

void setup() {
  Serial.begin(115200);  //啟動端口監聽
  gpio_set_direction(KeyCharge, GPIO_MODE_INPUT);
  gpio_set_direction(KeyFire, GPIO_MODE_INPUT);
  gpio_set_direction(OutCharge,GPIO_MODE_OUTPUT_OD);
  gpio_set_direction(OutFire,GPIO_MODE_OUTPUT_OD);
  gpio_set_level(OutCharge, 1);
  gpio_set_level(OutFire, 1);
}

void loop() {
  //指令控制
  if (Serial.available()) {          //檢測串口狀態
    String Input = Serial.readString();  //獲取串口輸入
    if (Input == String_Fire) {
      Trigger_Fire();
    }
    if (Input.startsWith(String_Charge, 0)) {  //字串處理
      Input.replace(String_Charge, "");
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
  while (!digitalRead(KeyFire));
  delay(100);
}

void Trigger_Charge(long T) {  //充能
  long i = 0;
  gpio_set_level(OutCharge, 0);
  for (i = 0; T >= i || !digitalRead(KeyCharge); i++) {
    if (Serial.available()) {          //檢測串口狀態
      String Input = Serial.readString();  //跳脫檢測
      if (Input.startsWith(String_Exit, 0)){
         T = 0;
         Serial.println("Charge Escaped");
      }
    }
    delay(1);
  }
  gpio_set_level(OutCharge, 1);
  Serial.println("Charge Completed");
  Serial.print("Charge Time:");
  Serial.println(i / 1000);
}

void Trigger_Fire() {  //發射
  gpio_set_level(OutFire, 0);
  delay(100);
  gpio_set_level(OutFire, 1);
  Serial.println("Fired");
}