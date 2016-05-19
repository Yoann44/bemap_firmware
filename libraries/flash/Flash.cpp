#include "Flash.h"
#include <SPI.h>

/*
*   DATASHEET CONSTANTS
*/
#define READ        0x03    //Read Memory
#define HIGH_READ   0x0B    //Read Memory at higher speed
#define ERASE_4K    0x20    //Erase 4 Kchar of memory array
#define ERASE_32K   0x52    //Erase 32 Kchar block of memory array
#define ERASE_64K   0xD8    //Erase 64 Kchar block of memory array
#define CHIP_ERASE  0x60    //Erase Full Memory Array
//#define CHIP_ERASE  0xC7  //Two different implementation of the "chip erase" constant
#define WRITE_ONE   0x02    //To Program One Data char
#define WRITE_MANY  0xAD    //Auto Address Increment Programming
#define RDSR        0x05    //Read-Status-Register
#define EWSR        0x50    //Enable-Write-Status-Register
#define WRSR        0x01    //Write-Status-Register
#define WREN        0x06    //Write-Enable
#define WRDI        0x04    //Write-Disable
#define RDID        0x90    //Read-ID
//#define RDID      0xAB    //Two different implementation of the "read-id" constant
#define JEDEC_ID    0x9F    //JEDEC ID read
#define EBSY        0x70    //Enable SO to output RY/BY# status during AAI programming
#define DBSY        0x80    //Disable SO to output RY/BY# status during AAI programming
#define NULL_CHAR   0x00

#define BUSY        0x01
#define WEL         0x02
#define BP0         0x04
#define BP1         0x08
#define BP2         0x10
#define BP3         0x20
#define AAI         0x40
#define BPL         0x80

/*
*   APPLICATION CONSTANTS
*   This is the list of the class var in the flash memory
*/
#define DEVICE_ID_OFFSET    0x00000000  //Where the device ID is stored
#define PACKET_NUMBER       0x00000002  //Where the number of existing packet is stored
#define LAST_DATA           0x00000006  //Where the adresse of the last stored DATA is stored
#define TRACK_ID_OFFSET     0x0000000A  //Where the actual track id is stored
#define FW_VERSION_OFFSET   0x0000000C
#define CFG_SIZE            0x0000000E

#define CFG_START           0x00000100
#define DATA_MEMORY         0x00001000  //Where the storage memory really begin

F_memory::F_memory(unsigned char p_size, unsigned short hold_pin, unsigned short write_pin, unsigned short cs_pin)
{
    this->adresse = DATA_MEMORY;
    this->nb_value = 0;

    this->p_size = p_size;

    this->hold_pin = hold_pin;
    this->cs_pin = cs_pin;
    this->write_pin = write_pin;

    this->device_id = -1;
    this->track_id = -1;
    this->fw_version = -1;
    this->cfg_size = -1;
}

F_memory::~F_memory()
{

}

void F_memory::init(int _fw_version, char* config_buffer, int buffer_size)
{
  //Init the physical PINs
	pinMode(hold_pin, OUTPUT);
	pinMode(cs_pin, OUTPUT);
	pinMode(write_pin, OUTPUT);
	digitalWrite(write_pin, HIGH);
	digitalWrite(cs_pin, HIGH);
	digitalWrite(hold_pin, HIGH);

  //Wait device to initialize
	delay(10);


  //Read the actual memory state, if first launch erase memory
	unprotect();

	char buffer[4];
	this->read(DEVICE_ID_OFFSET, buffer, 2);
	this->device_id = *((int*) buffer);

  this->read(TRACK_ID_OFFSET, buffer, 2);
  this->track_id = *((int*) buffer);

  this->read(FW_VERSION_OFFSET, buffer, 2);
  this->fw_version = *((int*) buffer);

  this->read(CFG_SIZE, buffer, 2);
  this->cfg_size = *((int*) buffer);

	if(this->device_id != INVALID_DEVICE_ID && this->track_id != INVALID_TRACK_ID && _fw_version == fw_version && buffer_size == this->cfg_size)
  {
      this->read(PACKET_NUMBER, buffer, 4);
      this->nb_value = *((long*) buffer);
      this->adresse = (long) this->nb_value * p_size + DATA_MEMORY;
      this->read(CFG_START, config_buffer, this->cfg_size);
  }
  else
  {
      this->cfg_size = buffer_size;
      this->fw_version = _fw_version;

      if(this->track_id == INVALID_TRACK_ID)
        this->track_id = 0;

      //eraseMemory();

      save_status(config_buffer, buffer_size);
  }
}

char F_memory::get_memory_state(void)
{
  return (char) (( this->adresse * 100 ) / MEM_SIZE);
}


/*
*   Getter and setter for external access
*/
int F_memory::get_device_id(void)
{
    return this->device_id;
}

void F_memory::set_device_id(int new_id)
{
    //if(this->device_id == INVALID_DEVICE_ID)
        this->device_id = new_id;

    save_status();
}

int F_memory::get_track_id(void)
{
    return this->track_id;
}

void F_memory::set_track_id(int new_id)
{
    if(this->track_id < new_id && new_id != INVALID_TRACK_ID)
    {
      this->track_id = new_id;
      save_status();
    }
}


/*
*   Save the new meta data of the memory : 
*     - Packets number
*     - Device id
*     - Last data address
*/
void F_memory::save_status(char* new_config, int cfg_size)
{
    char buffer[4];
    char cfg_buffer[this->cfg_size];

    //If a new cfg has to be saved set the new size else make a copy of the actual cfg
    if(new_config && cfg_size)
      this->cfg_size = cfg_size;
    else
      this->read(CFG_START, cfg_buffer, this->cfg_size);

    this->erase4KPage(0, true);

    //Write the new/saved configuration
    if(new_config && cfg_size)
      this->write(CFG_START, new_config, this->cfg_size);
    else
      this->write(CFG_START, cfg_buffer, this->cfg_size);

    *((int*) buffer) = this->cfg_size;
    this->write(CFG_SIZE, buffer, 2);

    *((int*) buffer) = this->fw_version;
    this->write(FW_VERSION_OFFSET, buffer, 2);

    *((int*) buffer) = this->device_id;
    this->write(DEVICE_ID_OFFSET, buffer, 2);

    *((int*) buffer) = this->track_id;
    this->write(TRACK_ID_OFFSET, buffer, 2);

    *((long*) buffer) = this->nb_value;
    this->write(PACKET_NUMBER, buffer, 4);
}


bool F_memory::write_p(char* buffer_p)
{
    if(this->write(this->adresse, buffer_p, p_size) == p_size)
    {
        this->adresse += p_size;
        this->nb_value++;
        return false;
    }
    else
    {
        this->adresse += p_size;
        return true;
    }
}

char* F_memory::read_p(char* buffer_p, char p_nb = 1, long p_offset = 0)
{
    long r_addr = (long) p_offset * p_size + DATA_MEMORY;

    if(read(r_addr, buffer_p, p_nb * p_size) == p_nb + 1)
        return buffer_p;
    else
        return buffer_p;
}

char F_memory::eraseMemory(bool keep_settings)
{
    char cfg_buffer[this->cfg_size];

    if(keep_settings)
    {
      this->read(CFG_START, cfg_buffer, this->cfg_size);
    }

    //noInterrupts();

    writeEnable();

    digitalWrite(cs_pin, LOW);
    SPI.transfer(CHIP_ERASE);
    digitalWrite(cs_pin, HIGH);

    writeDisable();

    //interrupts();

    this->adresse = DATA_MEMORY;
    this->nb_value = 0;

    if(keep_settings)
      save_status(cfg_buffer, this->cfg_size);
    else
      save_status();

    return true;
}

/* This function reads the SST25VF080B status register */
char F_memory::readStatusRegister()
{
  char val;
  digitalWrite(cs_pin, LOW);
  SPI.transfer(RDSR);
  val = SPI.transfer(NULL_CHAR);
  digitalWrite(cs_pin, HIGH);

  return val;
}

/* This function returns true if the SST25VF080B is busy
 * writing. Otherwise it returns false.
 */
bool F_memory::isBusy()
{
  if (this->readStatusRegister() & BUSY)
  {
    return true;
  }
  return false;
}

/* Enables write/erase operations */
void F_memory::writeEnable()
{
  digitalWrite(cs_pin, LOW);
  SPI.transfer(WREN);
  digitalWrite(cs_pin, HIGH);
}

/* By default, the SST25VF080B starts with all the sectors
 * write-protected. We disable the protection calling this
 * function */
void F_memory::unprotect()
{
  writeEnable();

  digitalWrite(cs_pin, LOW);
  SPI.transfer(WRSR);
  SPI.transfer(NULL_CHAR);
  digitalWrite(cs_pin, HIGH);
}

/* Disables write/erase operations */
void F_memory::writeDisable()
{
  digitalWrite(cs_pin, LOW);
  SPI.transfer(WRDI);
  digitalWrite(cs_pin, HIGH);
}

/* Erases a 4Kchars page (sets all 4Kchars to 0xFF) */
void F_memory::erase4KPage(unsigned long page, bool wait = true)
{
  //noInterrupts();
  unsigned long addr = (unsigned long) page * 0x1000;
  writeEnable();

  digitalWrite(cs_pin, LOW);
  SPI.transfer(ERASE_4K);
  SPI.transfer((addr >> 16) & 0xFF);
  SPI.transfer((addr >> 8) & 0xFF);
  SPI.transfer(addr & 0xFF);
  digitalWrite(cs_pin, HIGH);

  if(wait)
    while (isBusy());
  //interrupts();
}

/* Reads the given quantity of chars from 'addr' address into
 * 'buf' buffer.
 * Returns the quantity of read chars.
 */
unsigned long F_memory::read(unsigned long addr, char* buf, unsigned long qty)
{
  //noInterrupts();
  unsigned long read_b = 0;

  while (isBusy());

  digitalWrite(cs_pin, LOW);
  SPI.transfer(READ);
  SPI.transfer((addr >> 16) & 0xFF);
  SPI.transfer((addr >> 8) & 0xFF);
  SPI.transfer(addr & 0xFF);

  while (read_b < qty)
  {
    buf[read_b++] = SPI.transfer(NULL_CHAR);
  }

  digitalWrite(cs_pin, HIGH);
  //interrupts();
  return read_b;
}

/* Writes the 'qty' quantity of chars from 'buf' buffer into
 * 'addr' address.
 * This function will use the AAI (Auto Address Increment)
 * feature if qty >= 2.
 */
unsigned long F_memory::write(unsigned long addr, char* buf, unsigned long qty)
{
  //noInterrupts();
  unsigned long cnt = 0;
  bool aai = false;

  /* Enable write operation */
  writeEnable();

  while (cnt < qty)
  {
    /* Check if busy and wait for every write */
    while (isBusy());

    if (qty - cnt >= 2)
    {
      /* Auto Address Increment */
      digitalWrite(cs_pin, LOW);

      SPI.transfer(WRITE_MANY);

      if (aai == false)
      {
        /* First time issuing AAI command; send
         * the destination address. */
        SPI.transfer((addr >> 16) & 0xFF);
        SPI.transfer((addr >> 8) & 0xFF);
        SPI.transfer(addr & 0xFF);
        aai = true;
      }

      /* If aai = true, then we don't need to send
       * the address again. It will be automatically
       * incremented.
       */
      SPI.transfer(buf[cnt++]);
      SPI.transfer(buf[cnt++]);

      digitalWrite(cs_pin, HIGH);

      /* Keep track of the last written address */
      addr += 2;
    } else {

      if (aai)
      {
        /* If an AAI was issued, we need to terminate
         * it by calling WriteDisable().
         */
        writeDisable();
        /* And... since we're writing again, call.. */
        writeEnable();
        aai = false;
      }

      /* Single-char write */
      digitalWrite(cs_pin, LOW);

      SPI.transfer(WRITE_ONE);
      SPI.transfer((addr >> 16) & 0xFF);
      SPI.transfer((addr >> 8) & 0xFF);
      SPI.transfer(addr & 0xFF);
      SPI.transfer(buf[cnt++]);

      digitalWrite(cs_pin, HIGH);
    }
  }

  /* Disable write operations */
  writeDisable();
  return cnt;
  //interrupts();
}
