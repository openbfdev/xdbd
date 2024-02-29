#ifndef __ADB__H__
#define __ADB__H__

#include <stdbool.h>
#include <xdbd.h>

typedef struct adb_command_s adb_command_t;
typedef struct adb_pcmd_s adb_pcmd_t;
typedef struct xdbd_adb_header_s  xdbd_adb_header_t;
typedef struct xdbd_adb_packet_s  xdbd_adb_packet_t;
typedef struct xdbd_adb_request_s xdbd_adb_request_t;
// https://android.googlesource.com/platform/packages/modules/adb/+/master/protocol.txt

/* AUTH packets first argument */
/* Request */
#define ADB_AUTH_TOKEN         1
/* Response */
#define ADB_AUTH_SIGNATURE     2
#define ADB_AUTH_RSAPUBLICKEY  3

#define ADB_VERSION_MIN 0x01000000
#define ADB_VERSION_SKIP_CHECKSUM 0x01000001
#define ADB_VERSION 0x01000001


enum TransportType {
    kTransportUsb,
    kTransportLocal,
    kTransportAny,
    kTransportHost,
};

#define TOKEN_SIZE 20

enum ConnectionState {
    kCsAny = -1,

    kCsConnecting = 0,  // Haven't received a response from the device yet.
    kCsAuthorizing,     // Authorizing with keys from ADB_VENDOR_KEYS.
    kCsUnauthorized,    // ADB_VENDOR_KEYS exhausted, fell back to user prompt.
    kCsNoPerm,          // Insufficient permissions to communicate with the device.
    kCsDetached,        // USB device that's detached from the adb server.
    kCsOffline,

    // After CNXN packet, the ConnectionState describes not a state but the type of service
    // on the other end of the transport.
    kCsBootloader,  // Device running fastboot OS (fastboot) or userspace fastboot (fastbootd).
    kCsDevice,      // Device running Android OS (adbd).
    kCsHost,        // What a device sees from its end of a Transport (adb host).
    kCsRecovery,    // Device with bootloader loaded but no ROM OS loaded (adbd).
    kCsSideload,    // Device running Android OS Sideload mode (minadbd sideload mode).
    kCsRescue,      // Device running Android OS Rescue mode (minadbd rescue mode).
};

inline bool ConnectionStateIsOnline(enum ConnectionState state) {
    switch (state) {
        case kCsBootloader:
        case kCsDevice:
        case kCsHost:
        case kCsRecovery:
        case kCsSideload:
        case kCsRescue:
            return true;
        default:
            return false;
    }
}

struct adb_s {

};


#endif  /*__ADB__H__*/
