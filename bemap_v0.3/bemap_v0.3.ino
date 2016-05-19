//TODO POWER_SAVE_MODE && ACCL && REINITIALISATION AFTER CHANGE OF CONFIG && RESET MILLIS && CHECK DES VARIABLES && SAVE DATA AFTER SOME TIME && detect pc connection

#include <SPI.h>
#include <Wire.h>
//#include <avr/sleep.h>
//#include <avr/wdt.h>

#include <config.h>
#include <device.h>

#include <Thread.h>
#include <ThreadController.h>

#include <base_com.h>
#include <UserCom.h>
#include <Bemap_packet.h>
#include <state_controller.h>
#include <Accelerometer.h>
#include <Tiny_gps.h>
#include <Flash.h>
#include <LibHumidity.h>
#include <gaz_sensor.h>
#include <bemap_packet.h>
#include <rg_led.h>

/*
*  THREAD CONTROL
*/
Thread* thread_gaz_sensor;
Thread* thread_temp_sensor;
Thread* thread_save_data;
Thread* thread_gps;
Thread* thread_accelerometer;
Thread* thread_user;
Thread* thread_state;
Thread* thread_led;


/*
* DEVICE STATE
*/
Config bemap_config = {DEFAULT_SLEEP_RATE, DEFAULT_TRACKING_RATE,
                       INVALID_DATA, INVALID_DATA, 
                       DEFAULT_SENSOR_RATE, DEFAULT_SENSIRION_RATE,
                       INVALID_DEVICE_ID, INVALID_TRACK_ID, FW_VERSION,
                       DEFAULT_NO_FIX_TIME_OUT, DEFAULT_PREHEAT_TIME, 
                       DEFAULT_A_TAP_THERSH, DEFAULT_A_TAP_TIME, 
                       DEFAULT_A_ACT_THERSH, DEFAULT_A_INACT_THERSH, 
                       DEFAULT_A_INACT_TIME, SLEEPING};

/*
* FONCTION PROTOTYPE
*/
void io_ports_init(void);
void init_thread(void);
void devices_sensor_handler(void);
void state_controller_handler(void);
void gps_handler(void);
void user_handler(void);
void save_packet(void);
void temp_handler(void);
void led_driver_handler(void);
//void accl_handler(void);
bool load_config(void);


/*
* GLOBAL VARIABLE
*/

User_com* user = new User_com();
State_controller* state_controller = new State_controller();
Devices devices;

/*
* WATCHDOG TIMER
*/
/*
volatile int f_wdt=1;

ISR(WDT_vect){
  if(f_wdt == 0){
    f_wdt=1;
  }
}
*/


void setup(){
  io_ports_init();

  devices.sensor.init();
  devices.gps.init();
  devices.memory.init(FW_VERSION, (char*) &bemap_config, sizeof(Config));
  devices.rg_led.init();
  devices.accel.init(&bemap_config);
  
  user->init(&bemap_config, &devices);
  state_controller->init(&bemap_config, &devices);

  if(load_config())
    devices.rg_led.set_blink_color(C_ORANGE);
  else
    devices.rg_led.set_blink_color(C_RED);

  devices.rg_led.set_blink_freq(200);
  devices.rg_led.set_blink_time(6);

  init_thread();

  //wdt_enable(WDTO_2S);
  //Serial.println("Reset");
}

void loop(){
  /*
  if(f_wdt == 1)
  {
    Serial.println("WDT activated");
  }
  */
  if(thread_state->shouldRun())
    thread_state->run();

  if(thread_user->shouldRun())
    thread_user->run();

  if(thread_led->shouldRun())
    thread_led->run();

  if(bemap_config.actual_state != SLEEPING)
  {
    if(thread_gps->shouldRun())
      thread_gps->run();

    if(thread_gaz_sensor->shouldRun())
      thread_gaz_sensor->run();
  
    if(thread_temp_sensor->shouldRun())
      thread_temp_sensor->run();

    //wdt_reset();
  }

  if(bemap_config.actual_state == FIX_LOST || bemap_config.actual_state == TRACKING)
  {
    if(thread_save_data->shouldRun())
      thread_save_data->run();
  }

}

void io_ports_init(void){
  SPI.begin();
  SPI.setDataMode(SPI_MODE3);

  Serial.begin(115200);
}

bool load_config()
{
  if(devices.memory.get_device_id() == INVALID_DEVICE_ID)
    return false;
  return true;
}

void init_thread(void){
  
  thread_gaz_sensor = new Thread(devices_sensor_handler, bemap_config.sensor_sampling_rate);
  thread_temp_sensor = new Thread(temp_handler, 200);//Temp & hum sensor
  thread_save_data = new Thread(save_packet, 2000);
  thread_gps = new Thread(gps_handler, 10);
  thread_user = new Thread(user_handler, 10);
  thread_state = new Thread(state_controller_handler, 10);
  thread_led = new Thread(led_driver_handler, 50);
}

void save_packet(void){

  //devices.accel.int_stop();
  
  Bemap_packet packet;

  static long last_gps_time = 0, last_gps_time_call = 0;

  long lattitude, longitude;
  unsigned long time, date;
  devices.gps.get_position(&lattitude, &longitude);
  devices.gps.get_datetime(&date, &time);

  if( bemap_config.actual_state == TRACKING)
  {
    last_gps_time_call = millis();
    last_gps_time = time;
  }
  else if(bemap_config.actual_state == FIX_LOST)
    time = last_gps_time + (millis() - last_gps_time_call) / 10;

  packet.Setlattitude(lattitude);
  packet.Setlongitude(longitude);
  packet.Setdate(date);
  packet.Settime(time);

  int co, no2;
  devices.sensor.get_measure(&co, &no2);
  packet.Setmonoxyde(co);
  packet.Setnitrogen(no2);

  int temp, humidity;
  devices.sensirion.getLastSample(&temp, &humidity);
  packet.Sethumidity(humidity);
  packet.Settemperature(temp);

  packet.Settrack(bemap_config.track_id);

  packet.Setbounce(devices.accel.get_bounce());

  char flash_buf[PACKET_SIZE];
  packet.get_buf(flash_buf);
  devices.memory.write_p(flash_buf);

  devices.accel.reset_bounce();

  //devices.memory.save_status();
  //devices.accel.int_start();
}

void state_controller_handler(void){
  state_controller->update_state();
}
void devices_sensor_handler(void){
  devices.sensor.measure();
}

void user_handler(void){
  while(Serial.available())
    user->encode(Serial.read());
    
  user->do_instructions();
}

void gps_handler(){
  while(Serial1.available())
  {
    char c = Serial1.read();
    //Serial.write(c);
    devices.gps.encode(c);
  }
}

void temp_handler()
{
  devices.sensirion.rollMeasure();
}

void led_driver_handler()
{
  devices.rg_led.update();
}
/*
void accl_handler()
{
  devices.accel.new_int();
}
*/
