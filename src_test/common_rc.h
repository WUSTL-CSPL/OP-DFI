
#pragma once

#include <stdint.h>

// #include "crsf.h"
// #include "dsm.h"
// #include "ghst.hpp"
// #include "sbus.h"
// #include "st24.h"
// #include "sumd.h"

#pragma pack(push, 1)
typedef  struct rc_decode_buf_ {
	union {
		crsf_frame_t crsf_frame;
		// ghst_frame_t ghst_frame;
		// dsm_decode_t dsm;
		// sbus_frame_t sbus_frame;
		// ReceiverFcPacket _strxpacket;
		// ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#pragma pack(pop)

extern rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a){
	crc ^= a;
	for (int i = 0; i < 8; i++){
		if (crc & 0x80){
			crc = (crc << 1) ^ 0xD5;
		}else{
			crc <<= 1;
		}
	}
	return crc;
}
uint8_t crc8_dvb_s2_buf(uint8_t *buf, int len){
	uint8_t crc = 0;
	for (int i = 0; i < len; i++){
		crc = crc8_dvb_s2(crc, buf[i]);
	}
	return crc;
}
