#include "UserCom.h"

#define _ORGET_TERM   "ORGET"
#define _ORUSR_TERM   "ORUSR"
#define _ORCLR_TERM   "ORCLR"
#define _ORNUM_TERM   "ORNUM"
#define _ORRST_TERM   "ORRST"
#define _ORMEM_TERM   "ORMEM"
#define _ORRTV_TERM   "ORRTV"
#define _ORCFG_TERM   "ORCFG"

bool User_com::has_new(void)
{
  bool var = _new_instruction;
  _new_instruction = false;
  return var;
}

char User_com::get_instruction(void)
{
  return _instruction;
}

int User_com::get_nb_value(void)
{
  return _nb_value;
}

int User_com::get_device_id(void)
{
    return _device_id;
}

int User_com::from_hex(char a)
{
  if (a >= 'A' && a <= 'F')
    return a - 'A' + 10;
  else if (a >= 'a' && a <= 'f')
    return a - 'a' + 10;
  else
    return a - '0';
}


bool User_com::term_complete()
{
  if (_is_checksum_term)
  {
    byte checksum = 16 * from_hex(_term[0]) + from_hex(_term[1]);
    if (true)
    {
      if (true)
      {
        switch(_sentence_type)
        {
          case _USR_SENTENCE_ORGET:
            _new_instruction = true;
            _instruction = _USR_SENTENCE_ORGET;
            _nb_value = _new_nb_value;
            _offset_value = _new_offset_value;
            break;

          case _USR_SENTENCE_ORUSR:
            _new_instruction = true;
            _device_id = _new_device_id;
            _instruction = _USR_SENTENCE_ORUSR;
            break;

          case _USR_SENTENCE_ORCLR:
            _new_instruction = true;
            _instruction = _USR_SENTENCE_ORCLR;
            break;

          case _USR_SENTENCE_ORNUM:
            _new_instruction = true;
            _instruction = _USR_SENTENCE_ORNUM;
            break;

          case _USR_SENTENCE_ORRST:
            _new_instruction = true;
            _instruction = _USR_SENTENCE_ORRST;
            break;

          case _USR_SENTENCE_ORMEM:
            _new_instruction = true;
            _instruction = _USR_SENTENCE_ORMEM;
            break;

          case _USR_SENTENCE_ORRTV:
            _new_instruction = true;
            _instruction = _USR_SENTENCE_ORRTV;
            break;

          case _USR_SENTENCE_ORCFG:
              if( _config.sleep_time_ms != INVALID_DATA )
                bemap_config->sleep_time_ms = _config.sleep_time_ms;
              if( _config.tracking_time_ms != INVALID_DATA )
                bemap_config->tracking_time_ms = _config.tracking_time_ms;
              if( _config.sensor_sampling_rate != INVALID_DATA )
                bemap_config->sensor_sampling_rate = _config.sensor_sampling_rate;
              if( _config.sensirion_sampling_rate != INVALID_DATA )
                bemap_config->sensirion_sampling_rate = _config.sensirion_sampling_rate;
              if( _config.fix_lost_time_out != INVALID_DATA )
                bemap_config->fix_lost_time_out = _config.fix_lost_time_out;
              if( _config.preheat_time != INVALID_DATA )
                bemap_config->preheat_time = _config.preheat_time;
              if( _config.a_tap_thersh != INVALID_DATA )
                bemap_config->a_tap_thersh = _config.a_tap_thersh;
              if( _config.a_tap_time != INVALID_DATA )
                bemap_config->a_tap_time = _config.a_tap_time;
              if( _config.a_inact_thresh != INVALID_DATA )
                bemap_config->a_inact_thresh = _config.a_inact_thresh;
              if( _config.a_act_thresh != INVALID_DATA )
                bemap_config->a_act_thresh = _config.a_act_thresh;
              if( _config.a_incat_time != INVALID_DATA )
                bemap_config->a_incat_time = _config.a_incat_time;

              devices->memory.save_status((char*) bemap_config, sizeof(Config));

              _new_instruction = true;
              _instruction = _USR_SENTENCE_ORCFG;
            break;

          case _USR_SENTENCE_OTHER:
            _new_instruction = true;
            _instruction = _USR_SENTENCE_OTHER;
            break;
        }

        return true;
      }
    }

    return false;
  }

  // the first term determines the sentence type
  if (_term_number == 0)
  {
    if (!strcmp(_term, _ORGET_TERM))
      _sentence_type = _USR_SENTENCE_ORGET;
    else if (!strcmp(_term, _ORUSR_TERM))
    {
      _sentence_type = _USR_SENTENCE_ORUSR;
      _new_device_id = -1;
    }
    else if (!strcmp(_term, _ORCLR_TERM))
      _sentence_type = _USR_SENTENCE_ORCLR;
    else if (!strcmp(_term, _ORNUM_TERM))
      _sentence_type = _USR_SENTENCE_ORNUM;
    else if (!strcmp(_term, _ORRST_TERM))
      _sentence_type = _USR_SENTENCE_ORRST;
    else if (!strcmp(_term, _ORMEM_TERM))
      _sentence_type = _USR_SENTENCE_ORMEM;
    else if (!strcmp(_term, _ORRTV_TERM))
      _sentence_type = _USR_SENTENCE_ORRTV;
    else if (!strcmp(_term, _ORCFG_TERM))
    {
      _sentence_type = _USR_SENTENCE_ORCFG;

      _config.sleep_time_ms = INVALID_DATA;
      _config.tracking_time_ms = INVALID_DATA;
      _config.sensor_sampling_rate = INVALID_DATA;
      _config.sensirion_sampling_rate = INVALID_DATA;
      _config.fix_lost_time_out = INVALID_DATA;
      _config.preheat_time = INVALID_DATA;
      _config.a_tap_thersh = INVALID_DATA;
      _config.a_tap_time = INVALID_DATA;
      _config.a_inact_thresh = INVALID_DATA;
      _config.a_act_thresh = INVALID_DATA;
      _config.a_incat_time = INVALID_DATA;
    }
    else
      _sentence_type = _USR_SENTENCE_OTHER;
    return false;
  }

  if (_sentence_type != _USR_SENTENCE_OTHER && _term[0])
    switch(COMBINE(_sentence_type, _term_number))
    {
      case COMBINE(_USR_SENTENCE_ORGET, 2): // Time in both sentences
      {
        String value = _term;
        _new_nb_value = value.toInt();
        break;
      }
      case COMBINE(_USR_SENTENCE_ORGET, 1): // Time in both sentences
      {
          String value = _term;
          _new_offset_value = value.toInt();
          break;
      }
      case COMBINE(_USR_SENTENCE_ORUSR, 1): // Time in both sentences
      {
          String value = _term;
          _new_device_id = value.toInt();
          break;
      }
      case COMBINE(_USR_SENTENCE_ORCFG, 1): // Time in both sentences
      {
          String value = _term;
          _config.sleep_time_ms = value.toInt();
          break;
      }
      case COMBINE(_USR_SENTENCE_ORCFG, 2): // Time in both sentences
      {
          String value = _term;
          _config.tracking_time_ms = value.toInt();
          break;
      }
      case COMBINE(_USR_SENTENCE_ORCFG, 3): // Time in both sentences
      {
          String value = _term;
          _config.sensor_sampling_rate = value.toInt();
          break;
      }
      case COMBINE(_USR_SENTENCE_ORCFG, 4): // Time in both sentences
      {
          String value = _term;
          _config.sensirion_sampling_rate = value.toInt();
          break;
      }
      case COMBINE(_USR_SENTENCE_ORCFG, 5): // Time in both sentences
      {
          String value = _term;
          _config.fix_lost_time_out = value.toInt();
          break;
      }
      case COMBINE(_USR_SENTENCE_ORCFG, 6): // Time in both sentences
      {
          String value = _term;
          _config.preheat_time = value.toInt();
          break;
      }
      case COMBINE(_USR_SENTENCE_ORCFG, 7): // Time in both sentences
      {
          String value = _term;
          _config.a_tap_thersh = value.toInt();
          break;
      }
      case COMBINE(_USR_SENTENCE_ORCFG, 8): // Time in both sentences
      {
          String value = _term;
          _config.a_tap_time = value.toInt();
          break;
      }
      case COMBINE(_USR_SENTENCE_ORCFG, 9): // Time in both sentences
      {
          String value = _term;
          _config.a_inact_thresh = value.toInt();
          break;
      }
      case COMBINE(_USR_SENTENCE_ORCFG, 10): // Time in both sentences
      {
          String value = _term;
          _config.a_act_thresh = value.toInt();
          break;
      }
      case COMBINE(_USR_SENTENCE_ORCFG, 11): // Time in both sentences
      {
          String value = _term;
          _config.a_incat_time = value.toInt();
          break;
      }
    }
  return false;
}

void User_com::send_value(int _id, int _track_id, long _lat, long _lon, unsigned long _date,
                           unsigned long _time, int _gas1, int _gas2, int _hum, 
                           int _temp, char _etat)
{
  Serial.print("$BMVAL,");
  Serial.print(_id, DEC);
  Serial.print(",");
  Serial.print(_track_id, DEC);
  Serial.print(",");
  Serial.print(_lat, DEC);
  Serial.print(",");
  Serial.print(_lon, DEC);
  Serial.print(",");
  Serial.print(_date, DEC);
  Serial.print(",");
  Serial.print(_time, DEC);
  Serial.print(",");
  Serial.print(_gas1, DEC);
  Serial.print(",");
  Serial.print(_gas2, DEC);
  Serial.print(",");
  Serial.print(_hum, DEC);
  Serial.print(",");
  Serial.print(_temp, DEC);
  Serial.print(",");
  Serial.print(_etat, DEC);
  Serial.println(",*11");
}

void User_com::init(Config* main_config, Devices* main_devices){
  this->bemap_config = main_config;
  this->devices = main_devices;
}


void  User_com::do_instructions(void){
  if(has_new())
    switch(_instruction) {

      case _USR_SENTENCE_ORUSR:
        if(get_device_id() != -1)
          devices->memory.set_device_id(get_device_id());
        Serial.print("$BMUSR,");
        Serial.print(devices->memory.get_device_id());
        Serial.println(",*11");
      break;

      case _USR_SENTENCE_ORGET:
      {
        int nb_value = get_nb_value();
        int offset = get_offset();

        for(int i=0; i<nb_value; i++)
        {
          char packet_b[PACKET_SIZE];
          devices->memory.read_p(packet_b, 1, offset + i);

          Bemap_packet packet;
          packet.set_from_buf(packet_b);

          send_value(i, packet.Gettrack(), packet.Getlattitude(), packet.Getlongitude(), packet.Getdate(),
                packet.Gettime(), packet.Getmonoxyde(), packet.Getnitrogen(),
                packet.Gethumidity(), packet.Gettemperature(), packet.Getbounce());
        }
      }
      break;

      case _USR_SENTENCE_ORNUM:
        Serial.print("$BMNUM,");
        Serial.print(devices->memory.get_nb_value(), DEC);
        Serial.println(",*11");
      break;

      case _USR_SENTENCE_ORMEM:
      {
        long memoire = ((long)devices->memory.get_nb_value()) * PACKET_SIZE;
        Serial.print("$BMMEM,");
        Serial.print(memoire, DEC);
        Serial.print(",");
        Serial.print(MEM_SIZE, DEC);
        Serial.println(",*11");
      }
      break;
      /*
      case _USR_SENTENCE_ORGSR:
      Serial.print("$BMSSR,");
      Serial.print("");
      Serial.print("");
      Serial.print("");
      Serial.print("");
      Serial.println(",*11");
      break;
      */
      case _USR_SENTENCE_ORCLR:
      {
        devices->memory.eraseMemory();
        bemap_config->track_id = 0;
      }
      break;

      case _USR_SENTENCE_ORRTV:
      {
        long lat, lon;
        int co, no2, temp, hum;
        unsigned long _time, _date;
        float xg = 0, yg = 0, zg = 0;

        //ACCL_read(&xg, &yg, &zg);
        devices->gps.get_position(&lat, &lon);
        devices->gps.get_datetime(&_time, &_date);

        devices->sensor.get_measure(&co, &no2);
        devices->sensirion.getLastSample(&temp, &hum);

        Serial.print("$BMRTV,");

        //if(device_state == TRACKING_FIX)
        Serial.print(lat);
        Serial.print(",");
        //if(device_state == TRACKING_FIX)
        Serial.print(lon);
        Serial.print(",");
        //if(device_state == TRACKING_FIX)
        Serial.print(_date);
        Serial.print(",");
        //if(device_state == TRACKING_FIX)
        Serial.print(_time);
        Serial.print(",");

        Serial.print(co);
        Serial.print(",");
        Serial.print(no2);
        Serial.print(",");
        Serial.print(hum);
        Serial.print(",");
        Serial.print(temp);
        Serial.print(",");

        Serial.print(xg);
        Serial.print(",");
        Serial.print(yg);
        Serial.print(",");
        Serial.print(zg);
        Serial.print(",");

        Serial.println("*11");
      }
      break;

      case _USR_SENTENCE_OTHER:
        Serial.println("$BMERR,1,*11");
      break;

      case _USR_SENTENCE_ORCFG:
        Serial.print("$BMCFG,");

        Serial.print(bemap_config->fw_version, DEC);
        Serial.print(",");

        Serial.print(bemap_config->battery_state, DEC);
        Serial.print(",");

        Serial.print(bemap_config->memory_state, DEC);
        Serial.print(",");

        Serial.print(bemap_config->sleep_time_ms, DEC);
        Serial.print(",");

        Serial.print(bemap_config->tracking_time_ms, DEC);
        Serial.print(",");

        Serial.print(bemap_config->sensor_sampling_rate, DEC);
        Serial.print(",");

        Serial.print(bemap_config->sensirion_sampling_rate, DEC);
        Serial.print(",");

        Serial.print(bemap_config->fix_lost_time_out, DEC);
        Serial.print(",");

        Serial.print(bemap_config->preheat_time, DEC);
        Serial.print(",");

        Serial.print(bemap_config->a_tap_thersh, DEC);
        Serial.print(",");

        Serial.print(bemap_config->a_tap_time, DEC);
        Serial.print(",");

        Serial.print(bemap_config->a_inact_thresh, DEC);
        Serial.print(",");

        Serial.print(bemap_config->a_act_thresh, DEC);
        Serial.print(",");

        Serial.print(bemap_config->a_incat_time, DEC);
        Serial.print(",");

        Serial.println("*11");
      break;
    }
}