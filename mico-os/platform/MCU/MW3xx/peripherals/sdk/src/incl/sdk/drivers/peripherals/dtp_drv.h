/*! \file dtp_drv.h
 *  \brief DTP (Driver Tunneling Protocol) Driver
 *
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef _DTP_DRV_H_
#define _DTP_DRV_H_

/* I2C Slave Address*/
#define I2C_ADDR 0xC8

/** DTP supported protocols */
typedef enum {
	DTP_UART = 0,	/*!< UART DTP mode define */
	DTP_I2C,	/*!< I2C DTP mode define */
	DTP_SPI,	/*!< SPI DTP mode define */
} dtp_proto_t;

/* Driver Tunneling Diagnostic Statistics
 * This Structure is used to store diagnostic information for driver
 * tunneling protocol
 */
struct dtp_drv_diag_stat {
	/* Total number of transmitted packets */
	unsigned short dtp_tx_pkt;
	/* Total number of received packets */
	unsigned short dtp_rx_pkt;
	/* Errors due to checksum failure */
	unsigned short dtp_chksum_err;
	/* Errors due to receive data timeout */
	unsigned short dtp_timeout_err;
	/* Framing errors */
	unsigned short dtp_framing_err;
	/* Total number of transmission failures */
	unsigned short dtp_tx_fail;
	/* Total number of retry counts */
	unsigned short dtp_retry;
};

/* Driver Tunnel Protocol Config Parameters */
#define DTP_FRAME_MAX (4 * 1024) /* Length can be upto 2^(DATA_LEN*8) */
#define SOF_LEN 1
#define CMD_LEN 2
#define DATA_LEN 2
#define CRC_LEN 1
#define EOF_LEN 1

/* Bytes that should be escaped with 0xfd because they are either
 * Control or Prohibited bytes
 */
#define DTP_ESCAPE_VALUES { 0xfe, 0xfc, 0xfd }

/* Start of frame identifier */
#define DTP_SOF 0xfc

/* End of frame identifier */
#define DTP_EOF 0xfe

/* Escape byte */
#define DTP_ESCAPE_BYTE 0xfd

/* Escape value */
#define DTP_ESCAPE_VAL 0x20

/* RX state machine states */
enum rx_sm_states {
	NO_PKT_RECV,
	PKT_START,
	PKT_CMD_RECV,
	PKT_DATA_RECV,
	PKT_ESCAPE_CHR_RECV,
	PKT_DATA_RECV_DONE,
	PKT_ESCAPE_CKSUM,
	PKT_CKSUM_RECV,
};

/* TX state machine states */
enum tx_sm_states {
	PKT_TX_START,
	PKT_TX_SOF,
	PKT_TX_CMD,
	PKT_TX_LEN,
	PKT_TX_PAYLOAD,
	PKT_TX_EOF,
};

#ifdef CONFIG_DTP_DEBUG
#define DTP_DBG_SM(...) wmlog("dtp_drv_sm", __VA_ARGS__);
#else
#define DTP_DBG_SM(...)
#endif

#define DTP_DBG(...) wmlog("dtp_drv", __VA_ARGS__);

/** Initialize dtp driver for specified protocol
 *
 * \param[in] proto protocol name from supported protocol list
 * \param[in] port_id for protocol specific interface
 * \param[in] slave slave or master mode (optional for uart protocol)
 *
 * \return WM_SUCCESS on success or -WM_FAIL on error
 */
int dtp_drv_init(dtp_proto_t proto, int port_id, int slave);

/** Send data to other board
 *
 * This function sends data to the connected board using the underlying
 * driver
 *
 * \param msg buffer containing data to be sent
 * \param len data length of buffer to be sent
 *
 * \return WM_SUCCESS on success or -WM_FAIL on error
 */
int dtp_drv_send_data(uint8_t *msg, int len);

/** Register call back function
 *
 * This function registers the call back function which will be called
 * when the data will be received
 *
 * \param[in] fn function pointer to callback function.
 */
void dtp_drv_register_recv_cb(void (*fn) (uint8_t*, uint16_t));

/** Driver diagnostic function
 *
 * This function provides the information of diagnostic of
 * dtp driver's packet transactions
 *
 * \param stat dtp driver diagnostics stat struct to be filled
 */
void dtp_drv_diag_info(struct dtp_drv_diag_stat *stat);

#endif /* _DTP_DRV_H_ */
