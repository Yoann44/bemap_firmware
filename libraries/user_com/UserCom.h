#ifndef USER_COM_H
#define USER_COM_H

#include "../device/config.h"
#include "../base_com/base_com.h"
#include "../device/device.h"

enum {_USR_SENTENCE_ORGET, _USR_SENTENCE_ORUSR, _USR_SENTENCE_ORCLR, _USR_SENTENCE_ORNUM,
      _USR_SENTENCE_ORRST, _USR_SENTENCE_ORMEM, _USR_SENTENCE_ORRTV, _USR_SENTENCE_OTHER,
      _USR_SENTENCE_ORCFG};

class User_com : public Core_communication
{
    public:
        bool    has_new(void);
        char    get_instruction(void);
        int     get_offset(void){return _offset_value;}
        int     get_nb_value(void);
        int     get_device_id(void);
        void    send_value(int _id, int _track_id, long _lat, long _lon, unsigned long _date,
                           unsigned long _time, int _gas1, int _gas2, int _hum, 
                           int _temp, char _etat);
        void    do_instructions(void);
        void    init(Config* main_config, Devices* main_devices);

    protected:
        char _instruction;
        char _new_instruction;
        int _nb_value;
        int _new_nb_value;
        int _offset_value;
        int _new_offset_value;
        int _device_id;
        int _new_device_id;
        Config _config;

        Config* bemap_config;
        Devices* devices;

        bool term_complete(void);
        int from_hex(char a);


};

#endif // TINY_GPS_H

