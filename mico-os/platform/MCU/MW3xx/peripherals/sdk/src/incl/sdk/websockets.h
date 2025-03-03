/*! \file websockets.h
 * \brief Websocket Implementation in WMSDK using the WSLAY websocket library.
 *
 * This file exposes the API's to be used for websocket communication and which
 * internally call the WSLAY library functions
 *
 */
/*
 *  Copyright (C) 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef _WEBSOCKETS_H_
#define _WEBSOCKETS_H_

#include <httpc.h>

/** Websocket events **/
typedef enum {
	WS_SEND_DATA = 1,
	WS_RECV_DATA,
	WS_STOP_TRANSACTION,
} ws_event_t;

typedef enum {
	WS_CONT_FRAME = 0x0,
	WS_TEXT_FRAME = 0x1,
	WS_BIN_FRAME  = 0x2,
	WS_PING_FRAME = 0x9,
	WS_PONG_FRAME = 0xa,

} ws_opcode_t;

typedef int ws_context_t;

/** Web Sockets Transfer Frame
 *
 */
typedef struct wslay_frame {
	/** Frame Finish. Set to 1 if this is the final fragment of the frame */
	uint8_t fin;
	/** The Opcode of this data */
	uint8_t opcode;
	/** Pointer to data */
	const uint8_t *data;
	/** Length of the data */
	size_t data_len;
} ws_frame_t;

/** Upgrade a socket to websocket
 *
 * This function upgrades an already created socket to websocket.
 *
 * @param[in] handle Pointer to the handle retrieved from the
 * call to http_open_session()
 * @param[in] req Pointer to the HTTP request retrieved from the
 * call to http_prepare_req()
 * @param[in] protocol Websocket subprotocol to be used.
 * @param[out] ws_ctx The Web Socket context to be used from here onwards
 * @return WM_SUCCESS is returned on success. -WM_FAIL in case of an error.
 *
 */
int ws_upgrade_socket(http_session_t *handle, http_req_t *req,
		      const char *protocol, ws_context_t *ws_ctx);

/** Send a websocket frame
 *
 * @param[in] ws_ctx WS Context returned from a call to ws_upgrade_socket()
 * @param[in] f The frame (or fragment) to send out
 * @return Number of bytes written. -WM_FAIL in case of an error.
 *
 */
int ws_frame_send(ws_context_t *ws_ctx, ws_frame_t *f);

/** Receive a websocket frame
 *
 * @param[in] ws_ctx WS Context returned from a call to ws_upgrade_socket()
 * @param[out] f The frame (or fragment) that is received. After this call,
 * f->data contains the data that is received, while f->data_len is the length
 * of this data.
 *
 * @return Number of bytes read. -WM_FAIL in case of an error
 *
 */
int ws_frame_recv(ws_context_t *ws_ctx, ws_frame_t *f);

/** Close Web Socket context
 *
 * @param[in] ws_ctx WS Context returned from a call to ws_upgrade_socket()
 */
void ws_close_ctx(ws_context_t *ws_ctx);


/** Set websocket mask callback to generate mask key
 *
 * The SDK has a default masking function that uses \ref get_random_sequence()
 * to generate mask key. This function can be used to override it. It is
 * executed while sending masked data.
 *
 * @param[in] cb A user defined callback to generate mask key. The
 * implementation of this function must write exactly len bytes of mask key to
 * buf. This function should return WM_SUCCESS if successful, -WM_FAIL in case
 * of an error. Note that user_data is not required in this case.
 *
 */
void ws_set_genmask_callback(int (*cb)(uint8_t *buf, size_t len,
	void *user_data));
#endif /* _WEBSOCKETS_H_ */
