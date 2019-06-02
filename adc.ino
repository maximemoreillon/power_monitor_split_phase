void read_current(){

  RMS_current_phase_1 = get_RMS_current(CT_1_PIN);
  RMS_current_phase_2 = get_RMS_current(CT_2_PIN);

  static long last_ws_send_time;

  if(millis()- last_ws_send_time> WS_SEND_PERIOD){
    last_ws_send_time = millis();

    StaticJsonDocument<300> outbound_JSON_message; 
  
    outbound_JSON_message["RMS_current_phase_1"] = (String) RMS_current_phase_1;
    outbound_JSON_message["RMS_current_phase_2"] = (String) RMS_current_phase_2;
    
    char JSONmessageBuffer[300];
    serializeJson(outbound_JSON_message, JSONmessageBuffer, sizeof(JSONmessageBuffer));
    ws_server.broadcastTXT(JSONmessageBuffer);
  }
  
}

float lpf(float input, float filter_constant){
  // !!! lpf is shared among all the uses of the function
  static float lpf;
  lpf = lpf * (1.00 - filter_constant) + input * filter_constant;
  return lpf;
}

float hpf(float input, float filter_constant) {
  return (input - lpf(input, filter_constant));
}

float get_instantaneous_current(float adc_hpf){
  return adc_hpf * (CT_RATIO * VCC) / (ADC_MAX * BURDEN_VALUE);
}


float get_RMS_current(int adc_pin){

  // Resetting
  float sum_of_squares = 0;

  // Computing sum of squares
  for(int i=0; i<SAMPLE_COUNT; i++){

    float adc_hpf = hpf(analogRead(adc_pin),FILTER_CONSTANT);
    float instantaneous_current = get_instantaneous_current(adc_hpf);
    
    sum_of_squares = sum_of_squares + (instantaneous_current * instantaneous_current);
  }

  // averaging and square root
  return  sqrt(sum_of_squares / SAMPLE_COUNT);
}
