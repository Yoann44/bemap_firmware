#ifndef FLASH_MEM_H
#define FLASH_MEM_H

#define MEM_SIZE 1000000

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "../device/config.h"

/*
*   The Flash memory class work by saving data in a fixed packet size. The packet size has to be defined on the creation phase.
*   The first 4Kbyte of the memory are reserved for metadata, this actually contain the device ID and number of stored packets. 
*/

class F_memory
{
    public:
        F_memory(unsigned char p_size, unsigned short hold_pin, unsigned short write_pin, unsigned short cs_pin);
        virtual ~F_memory();
        void init(int _fw_version, char* config_buffer, int buffer_size);
        void save_status(char* new_config = NULL, int cfg_size = 0);

        void set_device_id(int new_id);
        int get_device_id(void);

        int get_track_id(void);
        void set_track_id(int new_id);

        bool write_p(char* packet);
        char* read_p(char* buffer_p, char p_nb, long p_offset);
        int get_nb_value(void){return nb_value;}
        char eraseMemory(bool keep_settings = false);

        char get_memory_state(void);
    protected:
    private:
        char p_size;
        unsigned long adresse;

        long nb_value;
        int device_id;
        int track_id;
        int fw_version;
        int cfg_size;

        int hold_pin;
        int cs_pin;
        int write_pin;

        bool isBusy(void);
        void writeEnable(void);
        void unprotect(void);
        void writeDisable(void);
        char readStatusRegister(void);
        void erase4KPage(unsigned long page, bool wait);
        unsigned long read(unsigned long addr, char* buf, unsigned long qty);
        unsigned long write(unsigned long addr, char* buf, unsigned long qty);
};

#endif // FLASH_H
