

int remaining_minutes = 0;
bool timer_active = false;

long one_minute = 60000;
long last_successfull_check = 0;

void set_aircon_timer(int minutes){
  remaining_minutes = minutes;
  timer_active = true;
  last_successfull_check = millis();
  String timer_answer = "Alrighty, I will turn off your AC in " + String(minutes) + " minutes :)";
  
  bot.sendMessage(CHAT_ID, timer_answer, "");
}

int get_remaining_minutes(){
  return remaining_minutes;
}

void start_aircon_timer(){
  
}

void stop_aircon_timer(){
  remaining_minutes = 0;
  timer_active = false;
}

void check_and_execute_aircon_timer(){
  if (millis() > (last_successfull_check + one_minute)){
    last_successfull_check += one_minute;
    if (timer_active == true){
      remaining_minutes -= 1;
      //bot.sendMessage(CHAT_ID, String(remaining_minutes), "");
      if (remaining_minutes <= 0){
        stop_everything();
        turn_off();
        bot.sendMessage(CHAT_ID, "Timer stopped and aircon turned off", "");
      }
    }
  }
}
