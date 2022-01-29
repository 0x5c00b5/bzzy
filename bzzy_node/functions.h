// Expose Espressif SDK functionality
extern "C" {
#include "user_interface.h"
  typedef void (*freedom_outside_cb_t)(uint8 status);
  int  wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);
  void wifi_unregister_send_pkt_freedom_cb(void);
  int  wifi_send_pkt_freedom(uint8 *buf, int len, bool sys_seq);
}

#include <ESP8266WiFi.h>

#define MAX_CLIENTS_TRACKED 100
int client_count = 0;
typedef struct client{uint8_t MAC[6];} client;
client client_list[MAX_CLIENTS_TRACKED];
unsigned long timeout = 30000; // **in milliseconds 
unsigned long timer[MAX_CLIENTS_TRACKED];


int avgppl[15];
int adx = 0;
unsigned long amillis = 60000;


void printMAC(client c){
  for(int x=0;x<5;x++){
    Serial.printf("%02x:",c.MAC[x]);
  }
  Serial.printf("%02x\n",c.MAC[5]);
}

void printTop15(client cl[]){ 
  Serial.printf("MAC ADDRESSES: (Clients: %d)\n",client_count);
  Serial.printf("-----------------\n");
  for(int x=0;x<15;x++){
    printMAC(cl[x]);
  }
  Serial.printf("-----------------\n");
}

void updateClients(){
  for(int x=0;x<client_count;x++){
    if(millis() > timer[x]+timeout){
        //timeout
        client_list[x] = client_list[client_count-1];
        timer[x] = timer[client_count-1];
        timer[client_count-1] = 0;
        client_count -= 1;
        x -= 1;
       }
   }
}

void updateAverage(){
  if(millis()>amillis){
    avgppl[adx]=client_count;
    adx++;
    amillis = 60000+millis();
    if(adx == 15){adx=0;}
  }
}

int getAverage(){
  if(adx == 0){return 0;}
  int a = 0;
  for(int x=0;x<adx;x++){
    a += avgppl[x];
  }
  a = a/adx;
  return a;
}

void promisc_cb(uint8_t *buf, uint16_t len){
  // Position 12 in the array is where the packet type number is located
  // https://supportforums.cisco.com/document/52391/80211-frames-starter-guide-learn-wireless-sniffer-traces
  //     QoS Data        Probe Req        Block Ack         PS-Poll         RTS               Beacon
  if((buf[12]==0x88)||(buf[12]==0x40)||(buf[12]==0x94)||(buf[12]==0xa4)||(buf[12]==0xb4))//||(buf[12]==0x08))
  {
    //  Serial.printf("Frame Type: %02x\n",buf[12]);
    //  Serial.printf("Sig Strength: %i\n",int8_t(buf[0]));
    // Signal strength is in byte 0
    // Origin MAC address starts at byte 22
    client newClient;
    for(int i=0;i<6;i++) {
      newClient.MAC[i] = buf[22+i];
        // Serial.printf("%02x:",newClient.MAC[i]);
    }
    //Comparing MACS:
    int known = 0;
    int idx;
    for(int x=0;x<client_count;x++){
      if(! memcmp(client_list[x].MAC, newClient.MAC, 6)){
        known = 1;
        idx = x;
        break;
      }
    }
    if(! known){
      if (! ((unsigned int) client_count >= sizeof(client_list)/sizeof(client_list[0]))){
        memcpy(&client_list[client_count],&newClient, sizeof(newClient));
        timer[client_count] = millis();
        client_count++;
      }
    }else{
      timer[idx] = millis();
    }
    updateClients();
    updateAverage();
  }
  //Other packet types
  // else
  // {
  // }
}
