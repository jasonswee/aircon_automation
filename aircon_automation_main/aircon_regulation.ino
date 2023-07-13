float target_temp = 0;
bool regulation_is_active = false;
int last_level = 0;

float level3_offset = 8.0;
float level2_offset = 5.0;
float level1_offset = 2.0;

void set_regulation_temperature(float temp) {
  target_temp = temp;
  String regulation_answer = "Will do my best to keep temperature at " + String(temp) + " °C. But the algorithm to do so could be better...";
  bot.sendMessage(CHAT_ID, regulation_answer, "");
}

float get_regulation_temperature() {
  return target_temp;
}

bool get_regulation_is_active(){
  return regulation_is_active;
}

void start_aircon_regulation() {
  regulation_is_active = true;
}

void stop_aircon_regulation() {
  regulation_is_active = false;
  last_level = 0;
}

void check_and_execute_aircon_regulation() {
  if (regulation_is_active) {

    float cur_temp = get_temperature();
    float temp_differ = cur_temp - target_temp;

    if (temp_differ > level3_offset) {
      if (last_level != 3){
      turn_on_3();
      bot.sendMessage(CHAT_ID, "Automated AC switched to setting 3", "");
      last_level = 3;
      Serial.println("cur_temp: " + String(cur_temp));
      Serial.println("target_temp: " + String(target_temp));
      Serial.println("temp_differ: " + String(temp_differ));
      Serial.println("clast_level: " + String(last_level));
      }
    }
    else if ((temp_differ > level2_offset) && (temp_differ < level3_offset-1)) {
      if (last_level != 2){
      turn_on_2();
      bot.sendMessage(CHAT_ID, "Automated AC switched to setting 2", "");
      last_level = 2;
      Serial.println("cur_temp: " + String(cur_temp));
      Serial.println("target_temp: " + String(target_temp));
      Serial.println("temp_differ: " + String(temp_differ));
      Serial.println("clast_level: " + String(last_level));
      }
    }
    else if ((temp_differ > level1_offset) && (temp_differ < level2_offset-1)){
      if (last_level != 1){
      turn_on_1();
      bot.sendMessage(CHAT_ID, "Automated AC switched to setting 1", "");
      last_level = 1;
      Serial.println("cur_temp: " + String(cur_temp));
      Serial.println("target_temp: " + String(target_temp));
      Serial.println("temp_differ: " + String(temp_differ));
      Serial.println("clast_level: " + String(last_level));
      }
    }
    else if ((temp_differ < level1_offset) && (temp_differ < level1_offset-1)) {
      if (last_level != 0){
      turn_off();
      bot.sendMessage(CHAT_ID, "Automated AC switched to setting 1", "");
      last_level = 0;
      Serial.println("cur_temp: " + String(cur_temp));
      Serial.println("target_temp: " + String(target_temp));
      Serial.println("temp_differ: " + String(temp_differ));
      Serial.println("clast_level: " + String(last_level));
      }
    }
  }
}
