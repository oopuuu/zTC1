/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef _BT_H_
#define _BT_H_

/** Initialize BT driver
 *
 * This function registers BT driver with the mdev interface.
 * It initializes the BT firmware.
 * Note that firmware needs to be already downloaded
 * through the wifi driver.
 *
 * \return WM_SUCCESS on success, -WM_FAIL otherwise.
 */
int32_t bt_drv_init(void);

/** Register the receive callback
 *
 * This function is used to register a callback with the
 * BT driver which will be called when a valid HCI packet
 * is received from the card.
 *
 * \param[in] cb_func Pointer to the callback function.
 *
 * \return Always returns WM_SUCCESS.
 */
int32_t bt_drv_set_cb(void (*cb_func)(uint8_t pkt_type, uint8_t *data,
							uint32_t size));

/** Send a HCI packet
 *
 * This function is used to send a HCI packet to the card.
 *
 * \param[in] pkt_type HCI Packet Type.
 * \param[in] data Pointer to the packet contents.
 * \param[in] size Length of the packet in bytes.
 *
 * \return WM_SUCCESS on success, -WM_FAIL otherwise.
 */
int32_t bt_drv_send(uint8_t pkt_type, uint8_t *data, uint32_t size);
#endif /* !_BT_H_ */
