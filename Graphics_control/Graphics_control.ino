void setup() {
  // put your setup code here, to run once:

}

void loop() {
  //Graphics control
  if (temperature < preheat_setpoint) {
    temp_setpoint = seconds * pendent_preheat;                  //Preheat zone
  }

  if (temperature > preheat_setpoint && seconds < soak_time) { //soak temperature zone (time here)
    temp_setpoint = soak_setpoint;
  }

  if (seconds > soaktime && seconds < reflow_time && temperature >= soak_setpoint) {
    temp_setpoint = soak_setpoint + (seconds * (reflow_setpoint - soak_setpoint) / (reflow_time - soaktime);
  }
  if (seconds > reflow_time && seconds < manteinance_time) {
    temp_setpoint = reflow_setpoint
  }
  if (seconds > manteinance_time && seconds < finish_time) {
    temp_setpoint = reflow_setpoint - (seconds * pendent_cooling);
  }
  if (seconds > finish_time && temperateure < cooldown_temp) {
    temp_setpoint = 0;
    running_mode=10
  }
  if running_mode == 10){
    seconds=0
    delay(3000)
  }

  }


  //Seconds counting
  millis_now = millis();
  if (millis_now - millis_before > refresh_rate) {        //Refresh rate of prntiong on the LCD
    millis_before = millis();
    seconds = seconds + (refresh_rate / 1000);            //We count time in seconds
  }
