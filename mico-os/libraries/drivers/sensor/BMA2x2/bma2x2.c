/*!
****************************************************************************
* Copyright (C) 2011 - 2014 Bosch Sensortec GmbH
*
* bma2x2.c
* Date: 2014/12/12
* Revision: 2.0.3 $
*
* Usage: Sensor Driver for BMA2x2 sensor
*
****************************************************************************
* License:
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*   Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
*   Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the distribution.
*
*   Neither the name of the copyright holder nor the names of the
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
* OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
* OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
*
* The information provided is believed to be accurate and reliable.
* The copyright holder assumes no responsibility
* for the consequences of use
* of such information nor for any infringement of patents or
* other rights of third parties which may result from its use.
* No license is granted by implication or otherwise under any patent or
* patent rights of the copyright holder.
**************************************************************************/
/*! file <BMA2x2 >
    brief <Sensor driver for BMA2x2> */
#include "bma2x2.h"
/*! user defined code to be added here ... */
static struct bma2x2_t *p_bma2x2;
/*! Based on Bit resolution v_value_u8 should be modified */
u8 V_BMA2x2RESOLUTION_U8 = BMA2x2_14_RESOLUTION;

/*!
 * @brief
 *	This API reads the data from
 *	the given register continuously
 *
 *
 *	@param v_addr_u8 -> Address of the register
 *	@param v_data_u8 -> The data from the register
 *	@param v_len_u32 -> no of bytes to read
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_burst_read(u8 v_addr_u8,
u8 *v_data_u8, u32 v_len_u32)
{
	/* Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* Read the data from the register*/
			com_rslt = p_bma2x2->BMA2x2_BURST_READ_FUNC
			(p_bma2x2->dev_addr, v_addr_u8, v_data_u8, v_len_u32);
		}
	return com_rslt;
}
/*!
 *	@brief
 *	This function is used for initialize
 *	bus read and bus write functions
 *	assign the chip id and device address
 *	chip id is read in the register 0x00 bit from 0 to 7
 *
 *	@param bma2x2 : structure pointer
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *	@note
 *	While changing the parameter of the bma2x2_t
 *	consider the following point:
 *	Changing the reference value of the parameter
 *	will changes the local copy or local reference
 *	make sure your changes will not
 *	affect the reference value of the parameter
 *	(Better case don't change the reference value of the parameter)
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_init(struct bma2x2_t *bma2x2)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	u8 v_config_data_u8 = C_BMA2x2_ZERO_U8X;
	/* assign bma2x2 ptr */
	p_bma2x2 = bma2x2;
	/* read Chip Id */
	com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
	(p_bma2x2->dev_addr,
	BMA2x2_CHIP_ID__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
	p_bma2x2->chip_id = v_data_u8;    /* get bit slice */
	/* read the fifo config register and update
	the value to the fifo_config*/
	com_rslt += bma2x2_read_reg(BMA2x2_FIFO_MODE_REG,
	&v_config_data_u8, C_BMA2x2_ONE_U8X);
	p_bma2x2->fifo_config = v_config_data_u8;
	return com_rslt;
}
/*!
 * @brief
 *	This API gives data to the given register and
 *	the data is written in the corresponding register address
 *
 *
 *	@param v_adr_u8  -> Address of the register
 *	@param v_data_u8 -> The data to the register
 *	@param v_len_u8 -> no of bytes to read
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_write_reg(u8 v_adr_u8,
u8 *v_data_u8, u8 v_len_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		/* Write the data to the register*/
		com_rslt = p_bma2x2->BMA2x2_BUS_WRITE_FUNC
		(p_bma2x2->dev_addr, v_adr_u8, v_data_u8, v_len_u8);
	}
	return com_rslt;
}
/*!
 * @brief This API reads the data from
 *           the given register address
 *
 *
 *	@param v_adr_u8 -> Address of the register
 *	@param v_data_u8 -> The data from the register
 *	@param v_len_u8 -> no of bytes to read
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_read_reg(u8 v_adr_u8,
u8 *v_data_u8, u8 v_len_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/*Read the data from the register*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, v_adr_u8, v_data_u8, v_len_u8);
		}
	return com_rslt;
}
/*!
 * @brief
 *	This API reads acceleration data X values
 *	from location 02h and 03h
 *
 *
 *  @param   v_accel_x_s16 : pointer holding the data of accel X
 *		       value       |   resolution
 *       ----------------- | --------------
 *              0          | BMA2x2_12_RESOLUTION
 *              1          | BMA2x2_10_RESOLUTION
 *              2          | BMA2x2_14_RESOLUTION
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_read_accel_x(s16 *v_accel_x_s16)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	/* Array holding the accel x value
	v_data_u8[0] - x->LSB
	v_data_u8[MSB_ONE] - x->MSB
	*/
	u8	v_data_u8[ARRAY_SIZE_TWO] = {
	C_BMA2x2_ZERO_U8X, C_BMA2x2_ZERO_U8X};
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (V_BMA2x2RESOLUTION_U8) {
		/* This case used for the resolution bit 12*/
		case BMA2x2_12_RESOLUTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ACCEL_X12_LSB__REG, v_data_u8, C_BMA2x2_TWO_U8X);
			*v_accel_x_s16 = (s16)((((s32)((s8)v_data_u8[MSB_ONE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_ZERO] & RESOLUTION_12_MASK));
			*v_accel_x_s16 = *v_accel_x_s16 >> C_BMA2x2_FOUR_U8X;
		break;
		/* This case used for the resolution bit 10*/
		case BMA2x2_10_RESOLUTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ACCEL_X10_LSB__REG, v_data_u8, C_BMA2x2_TWO_U8X);
			*v_accel_x_s16 = (s16)((((s32)((s8)v_data_u8[MSB_ONE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_ZERO] & RESOLUTION_10_MASK));
			*v_accel_x_s16 = *v_accel_x_s16 >> C_BMA2x2_SIX_U8X;
		break;
		/* This case used for the resolution bit 14*/
		case BMA2x2_14_RESOLUTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ACCEL_X14_LSB__REG, v_data_u8, C_BMA2x2_TWO_U8X);
			*v_accel_x_s16 = (s16)((((s32)((s8)v_data_u8[MSB_ONE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_ZERO] & RESOLUTION_14_MASK));
			*v_accel_x_s16 = *v_accel_x_s16 >> C_BMA2x2_TWO_U8X;
		break;
		default:
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief
 *	This API reads acceleration data X values
 *	from location 02h and 03h bit resolution support 8bit
 *
 *
 *  @param   v_accel_x_s8 : pointer holding the data of accel X
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_read_accel_elight_resolution_x(
s8 *v_accel_x_s8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8	data = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* Read the sensor X data*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_X_AXIS_MSB_REG, &data, C_BMA2x2_ONE_U8X);
			*v_accel_x_s8 = BMA2x2_GET_BITSLICE(data,
			BMA2x2_ACCEL_X_MSB);
		}
	return com_rslt;
}
/*!
 * @brief
 *	This API reads acceleration data Y values
 *	from location 04h and 05h
 *
 *  @param   v_accel_y_s16 : pointer holding the data of accel Y
 *		       value       |   resolution
 *       ----------------- | --------------
 *              0          | BMA2x2_12_RESOLUTION
 *              1          | BMA2x2_10_RESOLUTION
 *              2          | BMA2x2_14_RESOLUTION
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_read_accel_y(s16 *v_accel_y_s16)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	/* Array holding the accel y value
	v_data_u8[LSB_ZERO] - y->LSB
	v_data_u8[MSB_ONE] - y->MSB
	*/
	u8 v_data_u8[ARRAY_SIZE_TWO] = {C_BMA2x2_ZERO_U8X, C_BMA2x2_ZERO_U8X};
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (V_BMA2x2RESOLUTION_U8) {
		/* This case used for the resolution bit 12*/
		case BMA2x2_12_RESOLUTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ACCEL_Y12_LSB__REG, v_data_u8, C_BMA2x2_TWO_U8X);
			*v_accel_y_s16 = (s16)((((s32)((s8)v_data_u8[MSB_ONE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_ZERO] & BMA2x2_12_BIT_SHIFT));
			*v_accel_y_s16 = *v_accel_y_s16 >> C_BMA2x2_FOUR_U8X;
		break;
		/* This case used for the resolution bit 10*/
		case BMA2x2_10_RESOLUTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ACCEL_Y10_LSB__REG, v_data_u8, C_BMA2x2_TWO_U8X);
			*v_accel_y_s16 = (s16)((((s32)((s8)v_data_u8[MSB_ONE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_ZERO] & BMA2x2_10_BIT_SHIFT));
			*v_accel_y_s16 = *v_accel_y_s16 >> C_BMA2x2_SIX_U8X;
		break;
		/* This case used for the resolution bit 14*/
		case BMA2x2_14_RESOLUTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ACCEL_Y14_LSB__REG, v_data_u8, C_BMA2x2_TWO_U8X);
			*v_accel_y_s16 = (s16)((((s32)((s8)v_data_u8[MSB_ONE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_ZERO] & BMA2x2_14_BIT_SHIFT));
			*v_accel_y_s16 = *v_accel_y_s16 >> C_BMA2x2_TWO_U8X;
		break;
		default:
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API reads acceleration data Y values of
 * 8bit  resolution  from location 05h
 *
 *
 *
 *
 *  @param v_accel_y_s8   The data of y
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_read_accel_elight_resolution_y(
s8 *v_accel_y_s8)
{
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8	data = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_Y_AXIS_MSB_REG, &data, C_BMA2x2_ONE_U8X);
			*v_accel_y_s8 = BMA2x2_GET_BITSLICE(data,
			BMA2x2_ACCEL_Y_MSB);
		}
	return com_rslt;
}
/*!
 * @brief This API reads acceleration data Z values
 *                          from location 06h and 07h
 *
 *
 *  @param   v_accel_z_s16 : pointer holding the data of accel Z
 *		       value       |   resolution
 *       ----------------- | --------------
 *              0          | BMA2x2_12_RESOLUTION
 *              1          | BMA2x2_10_RESOLUTION
 *              2          | BMA2x2_14_RESOLUTION
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_read_accel_z(s16 *v_accel_z_s16)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	/* Array holding the accel z value
	v_data_u8[LSB_ZERO] - z->LSB
	v_data_u8[MSB_ONE] - z->MSB
	*/
	u8 v_data_u8[ARRAY_SIZE_TWO] = {C_BMA2x2_ZERO_U8X, C_BMA2x2_ZERO_U8X};
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (V_BMA2x2RESOLUTION_U8) {
		case BMA2x2_12_RESOLUTION:
			/* This case used for the resolution bit 12*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ACCEL_Z12_LSB__REG, v_data_u8, C_BMA2x2_TWO_U8X);
			*v_accel_z_s16 = (s16)((((s32)((s8)v_data_u8[MSB_ONE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_ZERO] & BMA2x2_12_BIT_SHIFT));
			*v_accel_z_s16 = *v_accel_z_s16 >> C_BMA2x2_FOUR_U8X;
		break;
		/* This case used for the resolution bit 10*/
		case BMA2x2_10_RESOLUTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ACCEL_Z10_LSB__REG, v_data_u8, C_BMA2x2_TWO_U8X);
			*v_accel_z_s16 = (s16)((((s32)((s8)v_data_u8[MSB_ONE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_ZERO] & BMA2x2_10_BIT_SHIFT));
			*v_accel_z_s16 = *v_accel_z_s16 >> C_BMA2x2_SIX_U8X;
		break;
		/* This case used for the resolution bit 14*/
		case BMA2x2_14_RESOLUTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ACCEL_Z14_LSB__REG, v_data_u8, C_BMA2x2_TWO_U8X);
			*v_accel_z_s16 = (s16)((((s32)((s8)v_data_u8[MSB_ONE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_ZERO] & BMA2x2_14_BIT_SHIFT));
			*v_accel_z_s16 = *v_accel_z_s16 >> C_BMA2x2_TWO_U8X;
		break;
		default:
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief
 *	This API reads acceleration data Z values of
 *	8bit  resolution  from location 07h
 *
 *
 *
 *
 *  \@aram  v_accel_z_s8 : the data of z
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_read_accel_elight_resolution_z(
s8 *v_accel_z_s8)
{
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8	data = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_Z_AXIS_MSB_REG, &data, C_BMA2x2_ONE_U8X);
			*v_accel_z_s8 = BMA2x2_GET_BITSLICE(data,
			BMA2x2_ACCEL_Z_MSB);
		}
	return com_rslt;
}
/*!
 *	@brief This API reads acceleration data X,Y,Z values
 *	from location 02h to 07h
 *
 *  @param accel : pointer holding the data of accel
 *		       value       |   resolution
 *       ----------------- | --------------
 *              0          | BMA2x2_12_RESOLUTION
 *              1          | BMA2x2_10_RESOLUTION
 *              2          | BMA2x2_14_RESOLUTION
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_read_accel_xyz(
struct bma2x2_accel_data *accel)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	/* Array holding the accel xyz value
	v_data_u8[LSB_ZERO] - x->LSB
	v_data_u8[MSB_ONE] - x->MSB
	v_data_u8[2] - y->MSB
	v_data_u8[3] - y->MSB
	v_data_u8[4] - z->MSB
	v_data_u8[5] - z->MSB
	*/
	u8 v_data_u8[ARRAY_SIZE_SIX] = {
	C_BMA2x2_ZERO_U8X, C_BMA2x2_ZERO_U8X,
	C_BMA2x2_ZERO_U8X, C_BMA2x2_ZERO_U8X,
	C_BMA2x2_ZERO_U8X, C_BMA2x2_ZERO_U8X};
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (V_BMA2x2RESOLUTION_U8) {
		/* This case used for the resolution bit 12*/
		case BMA2x2_12_RESOLUTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_ACCEL_X12_LSB__REG,
			v_data_u8, C_BMA2x2_SIX_U8X);
			/* read the x v_data_u8*/
			accel->x = (s16)((((s32)((s8)v_data_u8[MSB_ONE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_ZERO] & BMA2x2_12_BIT_SHIFT));
			accel->x = accel->x >> C_BMA2x2_FOUR_U8X;

			/* read the y v_data_u8*/
			accel->y = (s16)((((s32)((s8)v_data_u8[MSB_THREE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_TWO] & BMA2x2_12_BIT_SHIFT));
			accel->y = accel->y >> C_BMA2x2_FOUR_U8X;

			/* read the z v_data_u8*/
			accel->z = (s16)((((s32)((s8)v_data_u8[MSB_FIVE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_FOUR] & BMA2x2_12_BIT_SHIFT));
			accel->z = accel->z >> C_BMA2x2_FOUR_U8X;

		break;
		case BMA2x2_10_RESOLUTION:
		/* This case used for the resolution bit 10*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_ACCEL_X10_LSB__REG,
			v_data_u8, C_BMA2x2_SIX_U8X);
			/* read the x v_data_u8*/
			accel->x = (s16)((((s32)((s8)v_data_u8[MSB_ONE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_ZERO] & BMA2x2_10_BIT_SHIFT));
			accel->x = accel->x >> C_BMA2x2_SIX_U8X;

			/* read the y v_data_u8*/
			accel->y = (s16)((((s32)((s8)v_data_u8[MSB_THREE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_TWO] & BMA2x2_10_BIT_SHIFT));
			accel->y = accel->y >> C_BMA2x2_SIX_U8X;

			/* read the z v_data_u8*/
			accel->z = (s16)((((s32)((s8)v_data_u8[MSB_FIVE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_FOUR] & BMA2x2_10_BIT_SHIFT));
			accel->z = accel->z >> C_BMA2x2_SIX_U8X;
		break;
		/* This case used for the resolution bit 14*/
		case BMA2x2_14_RESOLUTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_ACCEL_X14_LSB__REG,
			v_data_u8, C_BMA2x2_SIX_U8X);

			/* read the x v_data_u8*/
			accel->x = (s16)((((s32)((s8)v_data_u8[MSB_ONE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_ZERO] & BMA2x2_14_BIT_SHIFT));
			accel->x = accel->x >> C_BMA2x2_TWO_U8X;

			/* read the y v_data_u8*/
			accel->y = (s16)((((s32)((s8)v_data_u8[MSB_THREE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_TWO] & BMA2x2_14_BIT_SHIFT));
			accel->y = accel->y >> C_BMA2x2_TWO_U8X;

			/* read the z v_data_u8*/
			accel->z = (s16)((((s32)((s8)v_data_u8[MSB_FIVE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_FOUR] & BMA2x2_14_BIT_SHIFT));
			accel->z = accel->z >> C_BMA2x2_TWO_U8X;
		break;
		default:
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API reads acceleration of 8 bit resolution
 * data of X,Y,Z values
 * from location 03h , 05h and 07h
 *
 *
 *
 *
 *  @param accel : pointer holding the data of accel
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_read_accel_eight_resolution_xyz(
struct bma2x2_accel_eight_resolution *accel)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8	v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
		(p_bma2x2->dev_addr,
		BMA2x2_X_AXIS_MSB_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		accel->x = BMA2x2_GET_BITSLICE(v_data_u8,
		BMA2x2_ACCEL_X_MSB);

		com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
		(p_bma2x2->dev_addr,
		BMA2x2_Y_AXIS_MSB_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		accel->y = BMA2x2_GET_BITSLICE(v_data_u8,
		BMA2x2_ACCEL_Y_MSB);

		com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
		(p_bma2x2->dev_addr,
		BMA2x2_Z_AXIS_MSB_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		accel->z = BMA2x2_GET_BITSLICE(v_data_u8,
		BMA2x2_ACCEL_Z_MSB);
		}
	return com_rslt;
}
/*!
 *	@brief This API read tap-sign, tap-first-xyz
 *	slope-sign, slope-first-xyz status register byte
 *	from location 0Bh
 *
 *   @param v_stat_tap_u8 : The status of tap and slope
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr_tap_stat(
u8 *v_stat_tap_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* Read the interrupt status register 0x0B*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_STAT_TAP_SLOPE_REG,
			v_stat_tap_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API read orient, high-sign and high-first-xyz
 *	status register byte from location 0Ch
 *
 *
 *  @param v_stat_orient_u8 : The status of orient and high
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr_orient_stat(
u8 *v_stat_orient_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* Read the interrupt status register 0x0C*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_STAT_ORIENT_HIGH_REG,
			v_stat_orient_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API reads fifo overrun and fifo frame counter
 *	status register byte  from location 0Eh
 *
 *  @param v_stat_fifo_u8 : The status of fifo overrun and frame counter
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_fifo_stat(
u8 *v_stat_fifo_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* Read the interrupt status register 0x0E*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC(
			p_bma2x2->dev_addr,
			BMA2x2_STAT_FIFO_REG,
			v_stat_fifo_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API read fifo frame count
 *	from location 0Eh bit position 0 to 6
 *
 *
 * @param v_frame_count_u8 : The status of fifo frame count
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_fifo_frame_count(
u8 *v_frame_count_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* Read the FIFO frame count*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC(
			p_bma2x2->dev_addr,
			BMA2x2_FIFO_FRAME_COUNT_STAT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_frame_count_u8 = BMA2x2_GET_BITSLICE(v_data_u8,
			BMA2x2_FIFO_FRAME_COUNT_STAT);
		}
	return com_rslt;
}
/*!
 *	@brief This API read fifo overrun
 *	from location 0Eh bit position 7
 *
 *
 * @param v_fifo_overrun_u8 : The status of fifo overrun
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_fifo_overrun(
u8 *v_fifo_overrun_u8)
{
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* Read the status of fifo over run*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC(
			p_bma2x2->dev_addr,
			BMA2x2_FIFO_OVERRUN_STAT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_fifo_overrun_u8 = BMA2x2_GET_BITSLICE(v_data_u8,
			BMA2x2_FIFO_OVERRUN_STAT);
		}
	return com_rslt;
}
/*!
 *	@brief This API read interrupt status of flat, orient, single tap,
 *	double tap, slow no motion, slope, highg and lowg from location 09h
 *
 *
 *
 *	@param  v_intr_stat_u8 : The value of interrupt status
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr_stat(
u8 *v_intr_stat_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* Read the interrupt status register 0x09*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC(
			p_bma2x2->dev_addr,
			BMA2x2_STAT1_REG, v_intr_stat_u8, C_BMA2x2_FOUR_U8X);
		}
	return com_rslt;
}
/*!
 * @brief This API is used to get the ranges(g values) of the sensor
 *	in the register 0x0F bit from 0 to 3
 *
 *
 *	@param v_range_u8 : The value of range
 *		  v_range_u8       |   result
 *       ----------------- | --------------
 *              0x00       | BMA2x2_RANGE_2G
 *              0x01       | BMA2x2_RANGE_4G
 *              0x02       | BMA2x2_RANGE_8G
 *              0x03       | BMA2x2_RANGE_16G
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_range(u8 *v_range_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		/* Read the range register 0x0F*/
		com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC(p_bma2x2->dev_addr,
		BMA2x2_RANGE_SELECT__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		v_data_u8 = BMA2x2_GET_BITSLICE(v_data_u8, BMA2x2_RANGE_SELECT);
		*v_range_u8 = v_data_u8;
	}
	return com_rslt;
}
/*!
 * @brief This API is used to set the ranges(g values) of the sensor
 *	in the register 0x0F bit from 0 to 3
 *
 *
 *	@param v_range_u8 : The value of range
 *		  v_range_u8       |   result
 *       ----------------- | --------------
 *              0x00       | BMA2x2_RANGE_2G
 *              0x01       | BMA2x2_RANGE_4G
 *              0x02       | BMA2x2_RANGE_8G
 *              0x03       | BMA2x2_RANGE_16G
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_range(u8 v_range_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		if ((v_range_u8 == C_BMA2x2_THREE_U8X) ||
		(v_range_u8 == C_BMA2x2_FIVE_U8X) ||
		(v_range_u8 == C_BMA2x2_EIGHT_U8X) ||
		(v_range_u8 == C_BMA2x2_TWELVE_U8X)) {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_RANGE_SELECT_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			switch (v_range_u8) {
			case BMA2x2_RANGE_2G:
				v_data_u8  = BMA2x2_SET_BITSLICE(v_data_u8,
				BMA2x2_RANGE_SELECT, C_BMA2x2_THREE_U8X);
			break;
			case BMA2x2_RANGE_4G:
				v_data_u8  = BMA2x2_SET_BITSLICE(v_data_u8,
				BMA2x2_RANGE_SELECT, C_BMA2x2_FIVE_U8X);
			break;
			case BMA2x2_RANGE_8G:
				v_data_u8  = BMA2x2_SET_BITSLICE(v_data_u8,
				BMA2x2_RANGE_SELECT, C_BMA2x2_EIGHT_U8X);
			break;
			case BMA2x2_RANGE_16G:
				v_data_u8  = BMA2x2_SET_BITSLICE(v_data_u8,
				BMA2x2_RANGE_SELECT, C_BMA2x2_TWELVE_U8X);
			break;
			default:
			break;
			}
			/* Write the range register 0x0F*/
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_RANGE_SELECT_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		} else {
		com_rslt = E_OUT_OF_RANGE;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get the bandwidth of the sensor in the register
 *	0x10 bit from 0 to 4
 *
 *
 *  @param v_bw_u8 : The value of bandwidth
 *		  v_bw_u8          |   result
 *       ----------------- | --------------
 *              0x08       | BMA2x2_BW_7_81HZ
 *              0x09       | BMA2x2_BW_15_63HZ
 *              0x0A       | BMA2x2_BW_31_25HZ
 *              0x0B       | BMA2x2_BW_62_50HZ
 *              0x0C       | BMA2x2_BW_125HZ
 *              0x0D       | BMA2x2_BW_250HZ
 *              0x0E       | BMA2x2_BW_500HZ
 *              0x0F       | BMA2x2_BW_1000HZ
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_bw(u8 *v_bw_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* Read the bandwidth register 0x10*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_BW__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_GET_BITSLICE(v_data_u8, BMA2x2_BW);
			*v_bw_u8 = v_data_u8;
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set the bandwidth of the sensor in the register
 *	0x10 bit from 0 to 4
 *
 *
 *  @param v_bw_u8 : The value of bandwidth
 *		  v_bw_u8          |   result
 *       ----------------- | --------------
 *              0x08       | BMA2x2_BW_7_81HZ
 *              0x09       | BMA2x2_BW_15_63HZ
 *              0x0A       | BMA2x2_BW_31_25HZ
 *              0x0B       | BMA2x2_BW_62_50HZ
 *              0x0C       | BMA2x2_BW_125HZ
 *              0x0D       | BMA2x2_BW_250HZ
 *              0x0E       | BMA2x2_BW_500HZ
 *              0x0F       | BMA2x2_BW_1000HZ
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_bw(u8 v_bw_u8)
{
/*  Variable used to return value of
communication routine*/
BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
u8 v_data_bw_u8 = C_BMA2x2_ZERO_U8X;
if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
	} else {
	/* Check the chip id 0xFB, it support upto 500Hz*/
	if (p_bma2x2->chip_id == BANDWIDTH_DEFINE) {
		if (v_bw_u8 > C_BMA2x2_SEVEN_U8X &&
		v_bw_u8 < C_BMA2x2_FIFETEEN_U8X) {
			switch (v_bw_u8) {
			case BMA2x2_BW_7_81HZ:
				v_data_bw_u8 = BMA2x2_BW_7_81HZ;

				/*  7.81 Hz      64000 uS   */
			break;
			case BMA2x2_BW_15_63HZ:
				v_data_bw_u8 = BMA2x2_BW_15_63HZ;

			/*  15.63 Hz     32000 uS   */
			break;
			case BMA2x2_BW_31_25HZ:
				v_data_bw_u8 = BMA2x2_BW_31_25HZ;

			/*  31.25 Hz     16000 uS   */
			break;
			case BMA2x2_BW_62_50HZ:
				v_data_bw_u8 = BMA2x2_BW_62_50HZ;

			/*  62.50 Hz     8000 uS   */
			break;
			case BMA2x2_BW_125HZ:
				v_data_bw_u8 = BMA2x2_BW_125HZ;

			/*  125 Hz       4000 uS   */
			break;
			case BMA2x2_BW_250HZ:
				v_data_bw_u8 = BMA2x2_BW_250HZ;

			/*  250 Hz       2000 uS   */
			break;
			case BMA2x2_BW_500HZ:
				v_data_bw_u8 = BMA2x2_BW_500HZ;

			/*  500 Hz       1000 uS   */
			break;
			default:
			break;
			}
			/* Write the bandwidth register */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_BW__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_BW, v_data_bw_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_BW__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			} else {
			com_rslt = E_OUT_OF_RANGE;
			}
		} else {
		if (v_bw_u8 > C_BMA2x2_SEVEN_U8X &&
		v_bw_u8 < C_BMA2x2_SIXTEEN_U8X) {
			switch (v_bw_u8) {
			case BMA2x2_BW_7_81HZ:
				v_data_bw_u8 = BMA2x2_BW_7_81HZ;

			/*  7.81 Hz      64000 uS   */
			break;
			case BMA2x2_BW_15_63HZ:
				v_data_bw_u8 = BMA2x2_BW_15_63HZ;

			/*  15.63 Hz     32000 uS   */
			break;
			case BMA2x2_BW_31_25HZ:
				v_data_bw_u8 = BMA2x2_BW_31_25HZ;

			/*  31.25 Hz     16000 uS   */
			break;
			case BMA2x2_BW_62_50HZ:
				v_data_bw_u8 = BMA2x2_BW_62_50HZ;

			/*  62.50 Hz     8000 uS   */
			break;
			case BMA2x2_BW_125HZ:
				v_data_bw_u8 = BMA2x2_BW_125HZ;

			/*  125 Hz       4000 uS   */
			break;
			case BMA2x2_BW_250HZ:
				v_data_bw_u8 = BMA2x2_BW_250HZ;

			/*  250 Hz       2000 uS   */
			break;
			case BMA2x2_BW_500HZ:
				v_data_bw_u8 = BMA2x2_BW_500HZ;

			/*!  500 Hz       1000 uS   */
			break;
			case BMA2x2_BW_1000HZ:
				v_data_bw_u8 = BMA2x2_BW_1000HZ;

			/*  1000 Hz      500 uS   */
			break;
			default:
			break;
			}
			/* Write the bandwidth register */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_BW__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_BW, v_data_bw_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_BW__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			} else {
			com_rslt = E_OUT_OF_RANGE;
			}
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get the operating
 *	modes of the sensor in the register 0x11 and 0x12
 *	@note Register 0x11 - bit from 5 to 7
 *	@note Register 0x12 - bit from 5 and 6
 *
 *
 *  @param v_power_mode_u8 : The value of power mode
 *	v_power_mode_u8           |value  |   0x11  |   0x12
 *  ------------------------- |-------| --------|--------
 *  BMA2x2_MODE_NORMAL        |  0    |  0x00   |  0x00
 *  BMA2x2_MODE_LOWPOWER1     |  1    |  0x02   |  0x00
 *  BMA2x2_MODE_SUSPEND       |  2    |  0x06   |  0x00
 *  BMA2x2_MODE_DEEP_SUSPEND  |  3    |  0x01   |  0x00
 *  BMA2x2_MODE_LOWPOWER2     |  4    |  0x02   |  0x01
 *  BMA2x2_MODE_STANDBY       |  5    |  0x04   |  0x00
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_power_mode(
u8 *v_power_mode_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
u8 v_data2_u8 = C_BMA2x2_ZERO_U8X;
if (p_bma2x2 == BMA2x2_NULL) {
	/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
	} else {
		com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
		(p_bma2x2->dev_addr, BMA2x2_MODE_CTRL_REG,
		&v_data_u8, C_BMA2x2_ONE_U8X);
		com_rslt += p_bma2x2->BMA2x2_BUS_READ_FUNC
		(p_bma2x2->dev_addr, BMA2x2_LOW_NOISE_CTRL_REG,
		&v_data2_u8, C_BMA2x2_ONE_U8X);

		v_data_u8  = (v_data_u8 &
		POWER_MODE_HEX_E_ZERO_MASK) >> C_BMA2x2_FIVE_U8X;
		v_data2_u8  = (v_data2_u8 &
		POWER_MODE_HEX_4_ZERO_MASK) >> C_BMA2x2_SIX_U8X;

	if ((v_data_u8 ==
	POWER_MODE_HEX_ZERO_ZERO_MASK) &&
	(v_data2_u8 ==
	POWER_MODE_HEX_ZERO_ZERO_MASK)) {
		*v_power_mode_u8  = BMA2x2_MODE_NORMAL;
		} else {
		if ((v_data_u8 ==
		POWER_MODE_HEX_ZERO_TWO_MASK) &&
		(v_data2_u8 ==
		POWER_MODE_HEX_ZERO_ZERO_MASK)) {
			*v_power_mode_u8  =
			BMA2x2_MODE_LOWPOWER1;
			} else {
			if ((v_data_u8 ==
			POWER_MODE_HEX_ZERO_FOUR_MASK
			|| v_data_u8 ==
			POWER_MODE_HEX_ZERO_SIX_MASK) &&
			(v_data2_u8 ==
			POWER_MODE_HEX_ZERO_ZERO_MASK)) {
				*v_power_mode_u8  =
				BMA2x2_MODE_SUSPEND;
				} else {
				if (((v_data_u8 &
				POWER_MODE_HEX_ZERO_ONE_MASK)
				== POWER_MODE_HEX_ZERO_ONE_MASK)) {
					*v_power_mode_u8  =
					BMA2x2_MODE_DEEP_SUSPEND;
					} else {
					if ((v_data_u8 ==
					POWER_MODE_HEX_ZERO_TWO_MASK)
					&& (v_data2_u8 ==
					POWER_MODE_HEX_ZERO_ONE_MASK)) {
						*v_power_mode_u8  =
						BMA2x2_MODE_LOWPOWER2;
					} else {
					if ((v_data_u8 ==
					POWER_MODE_HEX_ZERO_FOUR_MASK) &&
						(v_data2_u8 ==
						POWER_MODE_HEX_ZERO_ONE_MASK))
							*v_power_mode_u8  =
							BMA2x2_MODE_STANDBY;
					else
						*v_power_mode_u8 =
						BMA2x2_MODE_DEEP_SUSPEND;
						}
					}
				}
			}
		}
	}
	p_bma2x2->v_power_mode_u8 = *v_power_mode_u8;
return com_rslt;
}
/*!
 *	@brief This API is used to set the operating
 *	modes of the sensor in the register 0x11 and 0x12
 *	@note Register 0x11 - bit from 5 to 7
 *	@note Register 0x12 - bit from 5 and 6
 *
 *
 *  @param v_power_mode_u8 : The value of power mode
 *	v_power_mode_u8           |value  |   0x11  |   0x12
 *  ------------------------- |-------| --------|--------
 *  BMA2x2_MODE_NORMAL        |  0    |  0x00   |  0x00
 *  BMA2x2_MODE_LOWPOWER1     |  1    |  0x02   |  0x00
 *  BMA2x2_MODE_SUSPEND       |  2    |  0x06   |  0x00
 *  BMA2x2_MODE_DEEP_SUSPEND  |  3    |  0x01   |  0x00
 *  BMA2x2_MODE_LOWPOWER2     |  4    |  0x02   |  0x01
 *  BMA2x2_MODE_STANDBY       |  5    |  0x04   |  0x00
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_power_mode(u8 v_power_mode_u8)
{
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 mode_ctr_eleven_reg = C_BMA2x2_ZERO_U8X;
	u8 mode_ctr_twel_reg = C_BMA2x2_ZERO_U8X;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	u8 v_data2_u8 = C_BMA2x2_ZERO_U8X;
	u8 v_pre_fifo_config_data = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
	} else {
		com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
		(p_bma2x2->dev_addr,
		BMA2x2_MODE_CTRL_REG,
		&v_data_u8, C_BMA2x2_ONE_U8X);
		com_rslt += p_bma2x2->BMA2x2_BUS_READ_FUNC
		(p_bma2x2->dev_addr,
		BMA2x2_LOW_POWER_MODE__REG,
		&v_data2_u8, C_BMA2x2_ONE_U8X);

		com_rslt += bma2x2_set_mode_value(v_power_mode_u8);
		mode_ctr_eleven_reg = p_bma2x2->ctrl_mode_reg;
		mode_ctr_twel_reg =  p_bma2x2->low_mode_reg;
		/* write the power mode to
		register 0x12*/
		v_data2_u8  = BMA2x2_SET_BITSLICE
		(v_data2_u8, BMA2x2_LOW_POWER_MODE,
		mode_ctr_twel_reg);
		com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
		(p_bma2x2->dev_addr, BMA2x2_LOW_POWER_MODE__REG,
		&v_data2_u8, C_BMA2x2_ONE_U8X);
		/*A minimum delay of
		atleast 450us is required for
		the low power modes,
		as per the data sheet.*/
		p_bma2x2->delay_msec(C_BMA2x2_ONE_U8X);
		/* Enter the power mode to suspend*/
		v_data_u8  = BMA2x2_SET_BITSLICE
		(v_data_u8, BMA2x2_MODE_CTRL,
		C_BMA2x2_FOUR_U8X);
		/* write the power mode to suspend*/
		com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
		(p_bma2x2->dev_addr, BMA2x2_MODE_CTRL_REG,
		&v_data_u8, C_BMA2x2_ONE_U8X);
		/*A minimum delay of
		atleast 450us is required for
		the low power modes,
		as per the data sheet.*/
		p_bma2x2->delay_msec(C_BMA2x2_ONE_U8X);
		/* write the previous FIFO mode and data select*/
		v_pre_fifo_config_data = p_bma2x2->fifo_config;
		com_rslt += bma2x2_write_reg(BMA2x2_FIFO_MODE_REG,
		&v_pre_fifo_config_data, C_BMA2x2_ONE_U8X);
		/*A minimum delay of
		atleast 450us is required for
		the low power modes,
		as per the data sheet.*/
		p_bma2x2->delay_msec(C_BMA2x2_ONE_U8X);
		com_rslt += p_bma2x2->BMA2x2_BUS_READ_FUNC
		(p_bma2x2->dev_addr,
		BMA2x2_MODE_CTRL_REG,
		&v_data_u8, C_BMA2x2_ONE_U8X);
		/* write the power mode to 11th register*/
		v_data_u8  = BMA2x2_SET_BITSLICE
		(v_data_u8, BMA2x2_MODE_CTRL,
		mode_ctr_eleven_reg);
		com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
		(p_bma2x2->dev_addr, BMA2x2_MODE_CTRL_REG,
		&v_data_u8, C_BMA2x2_ONE_U8X);
		/*A minimum delay of
		atleast 450us is required for
		the low power modes,
		as per the data sheet.*/
		p_bma2x2->delay_msec(C_BMA2x2_ONE_U8X);
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to assign the power mode values
 *	modes of the sensor in the register 0x11 and 0x12
 *	@note Register 0x11 - bit from 5 to 7
 *	@note Register 0x12 - bit from 5 and 6
 *
 *
 *  @param v_power_mode_u8 : The value of power mode
 *	v_power_mode_u8           |value  |   0x11  |   0x12
 *  ------------------------- |-------| --------|--------
 *  BMA2x2_MODE_NORMAL        |  0    |  0x00   |  0x00
 *  BMA2x2_MODE_LOWPOWER1     |  1    |  0x02   |  0x00
 *  BMA2x2_MODE_SUSPEND       |  2    |  0x06   |  0x00
 *  BMA2x2_MODE_DEEP_SUSPEND  |  3    |  0x01   |  0x00
 *  BMA2x2_MODE_LOWPOWER2     |  4    |  0x02   |  0x01
 *  BMA2x2_MODE_STANDBY       |  5    |  0x04   |  0x00
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_mode_value(u8 v_power_mode_u8)
{
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_SUCCESS;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
	} else {
	if (v_power_mode_u8 < C_BMA2x2_SIX_U8X) {
		switch (v_power_mode_u8)	{
		case BMA2x2_MODE_NORMAL:
			p_bma2x2->ctrl_mode_reg =
			POWER_MODE_HEX_ZERO_ZERO_MASK;
			p_bma2x2->low_mode_reg =
			POWER_MODE_HEX_ZERO_ZERO_MASK;
		break;
		case BMA2x2_MODE_LOWPOWER1:
			p_bma2x2->ctrl_mode_reg =
			POWER_MODE_HEX_ZERO_TWO_MASK;
			p_bma2x2->low_mode_reg =
			POWER_MODE_HEX_ZERO_ZERO_MASK;
		break;
		case BMA2x2_MODE_LOWPOWER2:
			p_bma2x2->ctrl_mode_reg =
			POWER_MODE_HEX_ZERO_TWO_MASK;
			p_bma2x2->low_mode_reg =
			POWER_MODE_HEX_ZERO_ONE_MASK;
		break;
		case BMA2x2_MODE_SUSPEND:
			p_bma2x2->ctrl_mode_reg =
			POWER_MODE_HEX_ZERO_FOUR_MASK;
			p_bma2x2->low_mode_reg =
			POWER_MODE_HEX_ZERO_ZERO_MASK;
		break;
		case BMA2x2_MODE_STANDBY:
			p_bma2x2->ctrl_mode_reg =
			POWER_MODE_HEX_ZERO_FOUR_MASK;
			p_bma2x2->low_mode_reg =
			POWER_MODE_HEX_ZERO_ONE_MASK;
		break;
		case BMA2x2_MODE_DEEP_SUSPEND:
			p_bma2x2->ctrl_mode_reg =
			POWER_MODE_HEX_ZERO_ONE_MASK;
		break;
		}
		} else {
			com_rslt = E_OUT_OF_RANGE;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the sleep duration of the sensor in the register 0x11
 *	Register 0x11 - bit from 0 to 3
 *
 *
 *  @param  v_sleep_durn_u8 : The value of sleep duration time
 *         v_sleep_durn_u8 |   result
 *       ----------------- | ----------------------
 *              0x05       | BMA2x2_SLEEP_DURN_0_5MS
 *              0x06       | BMA2x2_SLEEP_DURN_1MS
 *              0x07       | BMA2x2_SLEEP_DURN_2MS
 *              0x08       | BMA2x2_SLEEP_DURN_4MS
 *              0x09       | BMA2x2_SLEEP_DURN_6MS
 *              0x0A       | BMA2x2_SLEEP_DURN_10MS
 *              0x0B       | BMA2x2_SLEEP_DURN_25MS
 *              0x0C       | BMA2x2_SLEEP_DURN_50MS
 *              0x0D       | BMA2x2_SLEEP_DURN_100MS
 *              0x0E       | BMA2x2_SLEEP_DURN_500MS
 *              0x0F       | BMA2x2_SLEEP_DURN_1S
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_sleep_durn(u8 *v_sleep_durn_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read the sleep duration */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_SLEEP_DURN__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_sleep_durn_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_SLEEP_DURN);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the sleep duration of the sensor in the register 0x11
 *	Register 0x11 - bit from 0 to 3
 *
 *
 *
 *
 *  @param  v_sleep_durn_u8 : The value of sleep duration time
 *        v_sleep_durn_u8  |   result
 *       ----------------- | ----------------------
 *              0x05       | BMA2x2_SLEEP_DURN_0_5MS
 *              0x06       | BMA2x2_SLEEP_DURN_1MS
 *              0x07       | BMA2x2_SLEEP_DURN_2MS
 *              0x08       | BMA2x2_SLEEP_DURN_4MS
 *              0x09       | BMA2x2_SLEEP_DURN_6MS
 *              0x0A       | BMA2x2_SLEEP_DURN_10MS
 *              0x0B       | BMA2x2_SLEEP_DURN_25MS
 *              0x0C       | BMA2x2_SLEEP_DURN_50MS
 *              0x0D       | BMA2x2_SLEEP_DURN_100MS
 *              0x0E       | BMA2x2_SLEEP_DURN_500MS
 *              0x0F       | BMA2x2_SLEEP_DURN_1S
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_sleep_durn(u8 v_sleep_durn_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_sleep_durn_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		if (v_sleep_durn_u8 > C_BMA2x2_FOUR_U8X &&
		v_sleep_durn_u8 < C_BMA2x2_SIXTEEN_U8X) {
			switch (v_sleep_durn_u8) {
			case BMA2x2_SLEEP_DURN_0_5MS:
				v_data_sleep_durn_u8 = BMA2x2_SLEEP_DURN_0_5MS;

				/*  0.5 MS   */
			break;
			case BMA2x2_SLEEP_DURN_1MS:
				v_data_sleep_durn_u8 = BMA2x2_SLEEP_DURN_1MS;

				/*  1 MS  */
			break;
			case BMA2x2_SLEEP_DURN_2MS:
				v_data_sleep_durn_u8 = BMA2x2_SLEEP_DURN_2MS;

				/*  2 MS  */
			break;
			case BMA2x2_SLEEP_DURN_4MS:
				v_data_sleep_durn_u8 = BMA2x2_SLEEP_DURN_4MS;

				/*  4 MS   */
			break;
			case BMA2x2_SLEEP_DURN_6MS:
				v_data_sleep_durn_u8 = BMA2x2_SLEEP_DURN_6MS;

				/*  6 MS  */
			break;
			case BMA2x2_SLEEP_DURN_10MS:
				v_data_sleep_durn_u8 = BMA2x2_SLEEP_DURN_10MS;

				/*  10 MS  */
			break;
			case BMA2x2_SLEEP_DURN_25MS:
				v_data_sleep_durn_u8 = BMA2x2_SLEEP_DURN_25MS;

				/*  25 MS  */
			break;
			case BMA2x2_SLEEP_DURN_50MS:
				v_data_sleep_durn_u8 = BMA2x2_SLEEP_DURN_50MS;

				/*  50 MS   */
			break;
			case BMA2x2_SLEEP_DURN_100MS:
				v_data_sleep_durn_u8 = BMA2x2_SLEEP_DURN_100MS;

				/*  100 MS  */
			break;
			case BMA2x2_SLEEP_DURN_500MS:
				v_data_sleep_durn_u8 = BMA2x2_SLEEP_DURN_500MS;

				/*  500 MS   */
			break;
			case BMA2x2_SLEEP_DURN_1S:
				v_data_sleep_durn_u8 = BMA2x2_SLEEP_DURN_1S;

				/*!  1 SECS   */
			break;
			default:
			break;
			}
			/* write the sleep duration */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_SLEEP_DURN__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_SLEEP_DURN, v_data_sleep_durn_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr, BMA2x2_SLEEP_DURN__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		} else {
		com_rslt = E_OUT_OF_RANGE;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get the sleep timer mode
 *	in the register 0x12 bit 5
 *
 *
 *
 *
 *  @param  v_sleep_timer_u8 : The value of sleep timer mode
 *        v_sleep_timer_u8 |   result
 *       ----------------- | ----------------------
 *              0          | enable EventDrivenSampling(EDT)
 *              1          | enable Equidistant sampling mode(EST)
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_sleep_timer_mode(
u8 *v_sleep_timer_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/*Read the SLEEP TIMER MODE*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_SLEEP_TIMER__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_sleep_timer_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_SLEEP_TIMER);
		}
	return com_rslt;
}
/*!
 * @brief This API is used to set the sleep timer mode
 *	in the register 0x12 bit 5
 *
 *
 *
 *
 *  @param  v_sleep_timer_u8 : The value of sleep timer mode
 *        v_sleep_timer_u8 |   result
 *       ----------------- | ----------------------
 *              0          | enable EventDrivenSampling(EDT)
 *              1          | enable Equidistant sampling mode(EST)
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_sleep_timer_mode(u8 v_sleep_timer_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		if (v_sleep_timer_u8 < C_BMA2x2_TWO_U8X) {
			/* write the SLEEP TIMER MODE*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_SLEEP_TIMER__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_SLEEP_TIMER, v_sleep_timer_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr, BMA2x2_SLEEP_TIMER__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		} else {
		com_rslt = E_OUT_OF_RANGE;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get high bandwidth
 *		in the register 0x13 bit 7
 *
 *  @param  v_high_bw_u8 : The value of high bandwidth
 *         v_high_bw_u8    |   result
 *       ----------------- | ----------------------
 *              0          | Unfiltered High Bandwidth
 *              1          | Filtered Low Bandwidth
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_high_bw(u8 *v_high_bw_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		return  E_BMA2x2_NULL_PTR;
		} else {
			/* Read the high bandwidth*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_ENABLE_DATA_HIGH_BW__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_high_bw_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_DATA_HIGH_BW);
		}
	return com_rslt;
}
/*!
 * @brief This API is used to write high bandwidth
 *		in the register 0x13 bit 7
 *
 *  @param  v_high_bw_u8 : The value of high bandwidth
 *         v_high_bw_u8    |   result
 *       ----------------- | ----------------------
 *              0          | Unfiltered High Bandwidth
 *              1          | Filtered Low Bandwidth
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_high_bw(u8 v_high_bw_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		return  E_BMA2x2_NULL_PTR;
		}  else {
			/* Write the high bandwidth*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_ENABLE_DATA_HIGH_BW__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_ENABLE_DATA_HIGH_BW, v_high_bw_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_DATA_HIGH_BW__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get shadow dis
 *	in the register 0x13 bit 6
 *
 *  @param  v_shadow_dis_u8 : The value of shadow dis
 *        v_shadow_dis_u8  |   result
 *       ----------------- | ------------------
 *              0          | MSB is Locked
 *              1          | No MSB Locking
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_shadow_dis(u8 *v_shadow_dis_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		return  E_BMA2x2_NULL_PTR;
		} else {
			/*Read the shadow dis*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_DIS_SHADOW_PROC__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_shadow_dis_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_DIS_SHADOW_PROC);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set shadow dis
 *	in the register 0x13 bit 6
 *
 *  @param  v_shadow_dis_u8 : The value of shadow dis
 *        v_shadow_dis_u8  |   result
 *       ----------------- | ------------------
 *              0          | MSB is Locked
 *              1          | No MSB Locking
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_shadow_dis(u8 v_shadow_dis_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		return  E_BMA2x2_NULL_PTR;
		} else {
			/* Write the shadow dis*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_DIS_SHADOW_PROC__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_DIS_SHADOW_PROC, v_shadow_dis_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr, BMA2x2_DIS_SHADOW_PROC__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This function is used for the soft reset
 *	The soft reset register will be written
 *	with 0xB6 in the register 0x14.
 *
 *
 *
 *  \param : None
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_soft_rst(void)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = BMA2x2_ENABLE_SOFT_RESET_VALUE;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		}  else {
			/*! To reset the sensor
			0xB6 v_value_u8 will be written */
			com_rslt = p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr, BMA2x2_RST_REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 * @brief This API is used to update the register values
 *
 *
 *
 *
 *  @param : None
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_update_image(void)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		return  E_BMA2x2_NULL_PTR;
		} else {
			/* Write the update image*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_UPDATE_IMAGE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_UPDATE_IMAGE, C_BMA2x2_ONE_U8X);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr, BMA2x2_UPDATE_IMAGE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *  interrupt enable bits of the sensor in the registers 0x16 and 0x17
 *	@note It reads the flat enable, orient enable,
 *	@note single tap enable, double tap enable
 *	@note slope-x enable, slope-y enable, slope-z enable,
 *	@note fifo watermark enable,
 *	@note fifo full enable, data enable, low-g enable,
 *	@note high-z enable, high-y enable
 *	@note high-z enable
 *
 *
 *
 *  @param v_intr_type_u8: The value of interrupts
 *        v_intr_type_u8   |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_LOW_G_INTR
 *              1          | BMA2x2_HIGH_G_X_INTR
 *              2          | BMA2x2_HIGH_G_Y_INTR
 *              3          | BMA2x2_HIGH_G_Z_INTR
 *              4          | BMA2x2_DATA_ENABLE
 *              5          | SLOPE_X_INTR
 *              6          | SLOPE_Y_INTR
 *              7          | SLOPE_Z_INTR
 *              8          | SINGLE_TAP_INTR
 *              9          | SINGLE_TAP_INTR
 *              10         | ORIENT_INT
 *              11         | FLAT_INT
 *
 *  @param v_value_u8 : The value of interrupts enable
 *        v_value_u8       |   result
 *       ----------------- | ------------------
 *              0x00       | INTR_DISABLE
 *              0x01       | INTR_ENABLE
 *
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr_enable(u8 v_intr_type_u8,
u8 *v_value_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_intr_type_u8) {
		case BMA2x2_LOW_G_INTR:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_LOW_G_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_value_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_LOW_G_INTR);
		break;
		case BMA2x2_HIGH_G_X_INTR:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_HIGH_G_X_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_value_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_HIGH_G_X_INTR);
		break;
		case BMA2x2_HIGH_G_Y_INTR:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_HIGH_G_Y_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_value_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_HIGH_G_Y_INTR);
		break;
		case BMA2x2_HIGH_G_Z_INTR:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_HIGH_G_Z_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_value_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_HIGH_G_Z_INTR);
		break;
		case BMA2x2_DATA_ENABLE:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_NEW_DATA_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_value_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_NEW_DATA_INTR);
		break;
		case BMA2x2_SLOPE_X_INTR:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SLOPE_X_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_value_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_SLOPE_X_INTR);
		break;
		case BMA2x2_SLOPE_Y_INTR:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SLOPE_Y_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_value_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_SLOPE_Y_INTR);
		break;
		case BMA2x2_SLOPE_Z_INTR:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SLOPE_Z_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_value_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_SLOPE_Z_INTR);
		break;
		case BMA2x2_SINGLE_TAP_INTR:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SINGLE_TAP_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_value_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_SINGLE_TAP_INTR);
		break;
		case BMA2x2_DOUBLE_TAP_INTR:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_DOUBLE_TAP_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_value_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_DOUBLE_TAP_INTR);
		break;
		case BMA2x2_ORIENT_INTR:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_ORIENT_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_value_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_ORIENT_INTR);
		break;
		case BMA2x2_FLAT_INTR:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_FLAT_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_value_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_FLAT_INTR);
		break;
		default:
		com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *  interrupt enable bits of the sensor in the registers 0x16 and 0x17
 *	@note It reads the flat enable, orient enable,
 *	@note single tap enable, double tap enable
 *	@note slope-x enable, slope-y enable, slope-z enable,
 *	@note fifo watermark enable,
 *	@note fifo full enable, data enable, low-g enable,
 *	@note high-z enable, high-y enable
 *	@note high-z enable
 *
 *
 *
 *  @param v_intr_type_u8: The value of interrupts
 *        v_intr_type_u8   |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_LOW_G_INTR
 *              1          | BMA2x2_HIGH_G_X_INTR
 *              2          | BMA2x2_HIGH_G_Y_INTR
 *              3          | BMA2x2_HIGH_G_Z_INTR
 *              4          | BMA2x2_DATA_ENABLE
 *              5          | SLOPE_X_INTR
 *              6          | SLOPE_Y_INTR
 *              7          | SLOPE_Z_INTR
 *              8          | SINGLE_TAP_INTR
 *              9          | SINGLE_TAP_INTR
 *              10         | ORIENT_INT
 *              11         | FLAT_INT
 *
 *  @param v_value_u8 : The value of interrupts enable
 *        v_value_u8       |   result
 *       ----------------- | ------------------
 *              0x00       | INTR_DISABLE
 *              0x01       | INTR_ENABLE
 *
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr_enable(u8 v_intr_type_u8,
u8 v_value_u8)
{
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	u8 v_data2_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
		(p_bma2x2->dev_addr, BMA2x2_INTR_ENABLE1_REG,
		&v_data_u8, C_BMA2x2_ONE_U8X);
		com_rslt += p_bma2x2->BMA2x2_BUS_READ_FUNC
		(p_bma2x2->dev_addr, BMA2x2_INTR_ENABLE2_REG,
		&v_data2_u8, C_BMA2x2_ONE_U8X);
		v_value_u8 = v_value_u8 & C_BMA2x2_ONE_U8X;
		switch (v_intr_type_u8) {
		case BMA2x2_LOW_G_INTR:
			/* Low G Interrupt  */
			v_data2_u8 = BMA2x2_SET_BITSLICE(v_data2_u8,
			BMA2x2_ENABLE_LOW_G_INTR, v_value_u8);
		break;
		case BMA2x2_HIGH_G_X_INTR:
			/* High G X Interrupt */
			v_data2_u8 = BMA2x2_SET_BITSLICE(v_data2_u8,
			BMA2x2_ENABLE_HIGH_G_X_INTR, v_value_u8);
		break;
		case BMA2x2_HIGH_G_Y_INTR:
			/* High G Y Interrupt */
			v_data2_u8 = BMA2x2_SET_BITSLICE(v_data2_u8,
			BMA2x2_ENABLE_HIGH_G_Y_INTR, v_value_u8);
		break;
		case BMA2x2_HIGH_G_Z_INTR:
			/* High G Z Interrupt */
			v_data2_u8 = BMA2x2_SET_BITSLICE(v_data2_u8,
			BMA2x2_ENABLE_HIGH_G_Z_INTR, v_value_u8);
		break;
		case BMA2x2_DATA_ENABLE:
			/*Data En Interrupt  */
			v_data2_u8 = BMA2x2_SET_BITSLICE(v_data2_u8,
			BMA2x2_ENABLE_NEW_DATA_INTR, v_value_u8);
		break;
		case BMA2x2_SLOPE_X_INTR:
			/* Slope X Interrupt */
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_ENABLE_SLOPE_X_INTR, v_value_u8);
		break;
		case BMA2x2_SLOPE_Y_INTR:
			/* Slope Y Interrupt */
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_ENABLE_SLOPE_Y_INTR, v_value_u8);
		break;
		case BMA2x2_SLOPE_Z_INTR:
			/* Slope Z Interrupt */
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_ENABLE_SLOPE_Z_INTR, v_value_u8);
		break;
		case BMA2x2_SINGLE_TAP_INTR:
			/* Single Tap Interrupt */
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
				BMA2x2_ENABLE_SINGLE_TAP_INTR, v_value_u8);
		break;
		case BMA2x2_DOUBLE_TAP_INTR:
			/* Double Tap Interrupt */
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
				BMA2x2_ENABLE_DOUBLE_TAP_INTR, v_value_u8);
		break;
		case BMA2x2_ORIENT_INTR:
			/* Orient Interrupt  */
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_ENABLE_ORIENT_INTR, v_value_u8);
		break;
		case BMA2x2_FLAT_INTR:
			/* Flat Interrupt */
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_ENABLE_FLAT_INTR, v_value_u8);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
		/* write the interrupt*/
		com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
		(p_bma2x2->dev_addr, BMA2x2_INTR_ENABLE1_REG,
		&v_data_u8, C_BMA2x2_ONE_U8X);
		com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
		(p_bma2x2->dev_addr, BMA2x2_INTR_ENABLE2_REG,
		&v_data2_u8, C_BMA2x2_ONE_U8X);
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the interrupt fifo full enable interrupt status
 *	in the register 0x17 bit 5
 *
 *
 *  @param v_fifo_full_u8 The value of fifo full interrupt enable
 *        v_fifo_full_u8   |   result
 *       ----------------- | ------------------
 *              0x00       | INTR_DISABLE
 *              0x01       | INTR_ENABLE
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr_fifo_full(u8 *v_fifo_full_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* Read fifo full interrupt */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_FIFO_FULL_ENABLE_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_fifo_full_u8 = BMA2x2_GET_BITSLICE(v_data_u8,
			BMA2x2_INTR_FIFO_FULL_ENABLE_INTR);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the interrupt fifo full enable interrupt status
 *	in the register 0x17 bit 5
 *
 *
 *  @param v_fifo_full_u8 The value of fifo full interrupt enable
 *        v_fifo_full_u8   |   result
 *       ----------------- | ------------------
 *              0x00       | INTR_DISABLE
 *              0x01       | INTR_ENABLE
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr_fifo_full(u8 v_fifo_full_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		if (v_fifo_full_u8 < C_BMA2x2_TWO_U8X) {
			/* Write fifo full interrupt */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_FIFO_FULL_ENABLE_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_INTR_FIFO_FULL_ENABLE_INTR,
			v_fifo_full_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_FIFO_FULL_ENABLE_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		} else {
		com_rslt = E_OUT_OF_RANGE;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get
 *	the interrupt fifo watermark enable interrupt status
 *	in the register 0x17 bit 6
 *
 *
 *
 *
 *  @param v_fifo_wm_u8 : the value FIFO Water Mark
 *        v_fifo_wm_u8     |   result
 *       ----------------- | ------------------
 *              0x00       | INTR_DISABLE
 *              0x01       | INTR_ENABLE
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr_fifo_wm(u8 *v_fifo_wm_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* Read the fifo water mark*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_FIFO_WM_ENABLE_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_fifo_wm_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_INTR_FIFO_WM_ENABLE_INTR);
		}
	return com_rslt;
}
/*!
 * @brief This API is used to set
 *	the interrupt fifo watermark enable interrupt status
 *	in the register 0x17 bit 6
 *
 *
 *
 *
 *  @param v_fifo_wm_u8 : the value FIFO Water Mark
 *        v_fifo_wm_u8     |   result
 *       ----------------- | ------------------
 *              0x00       | INTR_DISABLE
 *              0x01       | INTR_ENABLE
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr_fifo_wm(u8 v_fifo_wm_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		if (v_fifo_wm_u8 < C_BMA2x2_TWO_U8X) {
			/* Write the fifo water mark interrupt*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_FIFO_WM_ENABLE_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_INTR_FIFO_WM_ENABLE_INTR,
			v_fifo_wm_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_FIFO_WM_ENABLE_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		} else {
		com_rslt = E_OUT_OF_RANGE;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get
 * the interrupt status of slow/no motion select and slow no motion
 * enable xyz interrupt in the register 0x18 bit from 0 to 3
 *
 *
 *  @param  v_channel_u8 : The value of slow/no motion select
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_SLOW_NO_MOTION_ENABLE_X
 *              1          | BMA2x2_ACCEL_SLOW_NO_MOTION_ENABLE_Y
 *              2          | BMA2x2_ACCEL_SLOW_NO_MOTION_ENABLE_Z
 *              3          | BMA2x2_ACCEL_SLOW_NO_MOTION_ENABLE_SEL
 *
 *	@param v_slow_no_motion_u8 : The value of slow no motion interrupt enable
 *        v_slow_no_motion_u8     |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_slow_no_motion(u8 v_channel_u8,
u8 *v_slow_no_motion_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		/* Read the slow no motion interrupt */
		switch (v_channel_u8) {
		case BMA2x2_SLOW_NO_MOTION_ENABLE_X:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_X_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_slow_no_motion_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_X_INTR);
		break;
		case BMA2x2_SLOW_NO_MOTION_ENABLE_Y:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_Y_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_slow_no_motion_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_Y_INTR);
		break;
		case BMA2x2_SLOW_NO_MOTION_ENABLE_Z:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_Z_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_slow_no_motion_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_Z_INTR);
		break;
		case BMA2x2_SLOW_NO_MOTION_ENABLE_SELECT:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_SELECT_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_slow_no_motion_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_SELECT_INTR);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to set
 * the interrupt status of slow/no motion select and slow no motion
 * enable xyz interrupt in the register 0x18 bit from 0 to 3
 *
 *
 *  @param  v_channel_u8 : The value of slow/no motion select
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_SLOW_NO_MOTION_ENABLE_X
 *              1          | BMA2x2_ACCEL_SLOW_NO_MOTION_ENABLE_Y
 *              2          | BMA2x2_ACCEL_SLOW_NO_MOTION_ENABLE_Z
 *              3          | BMA2x2_ACCEL_SLOW_NO_MOTION_ENABLE_SEL
 *
 *	@param v_slow_no_motion_u8 : The value of slow no motion interrupt enable
 *        v_slow_no_motion_u8     |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_slow_no_motion(u8 v_channel_u8,
u8 v_slow_no_motion_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		/* Write the slow no motion interrupt*/
		switch (v_channel_u8) {
		case BMA2x2_SLOW_NO_MOTION_ENABLE_X:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_X_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_X_INTR,
			v_slow_no_motion_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_X_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_SLOW_NO_MOTION_ENABLE_Y:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_Y_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_Y_INTR,
			v_slow_no_motion_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_Y_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_SLOW_NO_MOTION_ENABLE_Z:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_Z_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_Z_INTR,
			v_slow_no_motion_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_Z_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_SLOW_NO_MOTION_ENABLE_SELECT:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_SELECT_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_SELECT_INTR,
			v_slow_no_motion_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR_SLOW_NO_MOTION_ENABLE_SELECT_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief  This API is used to get
 * the interrupt enable of low_g interrupt in the register 0x19 and 0x1B
 * @note INTR1_Low_g -> register 0x19 bit 0
 * @note INTR2_Low_g -> register 0x1B bit 0
 *
 *
 *
 *
 * @param v_channel_u8 : The value of low interrupt selection channel
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_INTR1_LOW_G
 *              1          | BMA2x2_ACCEL_INTR2_LOW_G
 *
 * @param v_intr_low_g_u8 : the value of low_g interrupt
 *        v_intr_low_u8           |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr_low_g(u8 v_channel_u8,
u8 *v_intr_low_g_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* Read the low_g interrupt*/
		case BMA2x2_INTR1_LOW_G:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_LOW_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_low_g_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR1_PAD_LOW_G);
		break;
		case BMA2x2_INTR2_LOW_G:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_LOW_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_low_g_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR2_PAD_LOW_G);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief  This API is used to set
 * the interrupt enable of low_g interrupt in the register 0x19 and 0x1B
 * @note INTR1_Low_g -> register 0x19 bit 0
 * @note INTR2_Low_g -> register 0x1B bit 0
 *
 *
 *
 *
 * @param v_channel_u8 : The value of low interrupt selection channel
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_INTR1_LOW_G
 *              1          | BMA2x2_ACCEL_INTR2_LOW_G
 *
 * @param v_intr_low_u8 : the value of low_g interrupt
 *        v_intr_low_u8           |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr_low_g(u8 v_channel_u8,
u8 v_intr_low_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		case BMA2x2_INTR1_LOW_G:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_LOW_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_ENABLE_INTR1_PAD_LOW_G, v_intr_low_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_LOW_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_INTR2_LOW_G:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_LOW_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR2_PAD_LOW_G,
			v_intr_low_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_LOW_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get
 * the interrupt enable of high_g interrupt in the register 0x19 and 0x1B
 * @note INTR1_high_g -> register 0x19 bit 1
 * @note INTR2_high_g -> register 0x1B bit 1
 *
 *
 *  @param  v_channel_u8: The value of high_g interrupt selection
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_INTR1_HIGH_G
 *              1          | BMA2x2_ACCEL_INTR2_HIGH_G
 *
 * @param v_intr_high_g_u8 : the value of high_g interrupt
 *        v_intr_high_g_u8        |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr_high_g(u8 v_channel_u8,
u8 *v_intr_high_g_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* read the high_g interrupt*/
		case BMA2x2_INTR1_HIGH_G:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_HIGH_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_high_g_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR1_PAD_HIGH_G);
		break;
		case BMA2x2_INTR2_HIGH_G:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_HIGH_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_high_g_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR2_PAD_HIGH_G);
		break;
		default:
		com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to set
 * the interrupt enable of high_g interrupt in the register 0x19 and 0x1B
 * @note INTR1_high_g -> register 0x19 bit 1
 * @note INTR2_high_g -> register 0x1B bit 1
 *
 *
 *  @param  v_channel_u8: The value of high_g interrupt selection
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_INTR1_HIGH_G
 *              1          | BMA2x2_ACCEL_INTR2_HIGH_G
 *
 * @param v_intr_high_g_u8 : the value of high_g interrupt
 *        v_intr_high_g_u8        |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr_high_g(u8 v_channel_u8,
u8 v_intr_high_g_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		/* write the high_g interrupt*/
		switch (v_channel_u8) {
		case BMA2x2_INTR1_HIGH_G:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_HIGH_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR1_PAD_HIGH_G,
			v_intr_high_g_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_HIGH_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_INTR2_HIGH_G:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_HIGH_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR2_PAD_HIGH_G,
			v_intr_high_g_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_HIGH_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
		com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get
 * the interrupt enable of slope interrupt in the register 0x19 and 0x1B
 * @note INTR1_slope -> register 0x19 bit 2
 * @note INTR2_slope -> register 0x1B bit 2
 *
 *
 *
 * @param v_channel_u8: the value of slope channel select
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_INTR1_SLOPE
 *              1          | BMA2x2_ACCEL_INTR2_SLOPE
 *
 * @param v_intr_slope_u8 : The slope value enable value
 *        v_intr_slope_u8         |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr_slope(u8 v_channel_u8,
u8 *v_intr_slope_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		/* Read the slope value */
		switch (v_channel_u8) {
		case BMA2x2_INTR1_SLOPE:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_SLOPE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_slope_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR1_PAD_SLOPE);
		break;
		case BMA2x2_INTR2_SLOPE:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_SLOPE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_slope_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR2_PAD_SLOPE);
		break;
		default:
		com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to set
 * the interrupt enable of slope interrupt in the register 0x19 and 0x1B
 * @note INTR1_slope -> register 0x19 bit 2
 * @note INTR2_slope -> register 0x1B bit 2
 *
 *
 *
 * @param v_channel_u8: the value of slope channel select
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_INTR1_SLOPE
 *              1          | BMA2x2_ACCEL_INTR2_SLOPE
 *
 * @param v_intr_slope_u8 : The slope value enable value
 *        v_intr_slope_u8         |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr_slope(u8 v_channel_u8,
u8 v_intr_slope_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* Write the slope value */
		case BMA2x2_INTR1_SLOPE:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_SLOPE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR1_PAD_SLOPE,
			v_intr_slope_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_SLOPE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_INTR2_SLOPE:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_SLOPE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR2_PAD_SLOPE,
			v_intr_slope_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_SLOPE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get
 * the interrupt enable of slow/no motion interrupt in
 * the register 0x19 and 0x1B
 * @note INTR1_slow_no_motion -> register 0x19 bit 3
 * @note INTR2_slow_no_motion -> register 0x1B bit 3
 *
 *
 *
 *
 *  @param v_channel_u8 : The value of slow/no motion selection
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_INTR1_SLOW_NO_MOTION
 *              1          | BMA2x2_INTR2_SLOW_NO_MOTION
 *
 *  @param v_intr_slow_no_motion_u8:  the slow_no_motion enable value
 *       v_intr_slow_no_motion_u8 |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr_slow_no_motion(u8 v_channel_u8,
u8 *v_intr_slow_no_motion_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		/* Read the slow no motion interrupt */
		switch (v_channel_u8) {
		case BMA2x2_INTR1_SLOW_NO_MOTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_SLOW_NO_MOTION__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_slow_no_motion_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR1_PAD_SLOW_NO_MOTION);
		break;
		case BMA2x2_INTR2_SLOW_NO_MOTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_SLOW_NO_MOTION__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_slow_no_motion_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR2_PAD_SLOW_NO_MOTION);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to set
 * the interrupt enable of slow/no motion interrupt in
 * the register 0x19 and 0x1B
 * @note INTR1_slow_no_motion -> register 0x19 bit 3
 * @note INTR2_slow_no_motion -> register 0x1B bit 3
 *
 *
 *
 *
 *  @param v_channel_u8 : The value of slow/no motion selection
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_INTR1_SLOW_NO_MOTION
 *              1          | BMA2x2_INTR2_SLOW_NO_MOTION
 *
 *  @param v_intr_slow_no_motion_u8:  the slow_no_motion enable value
 *       v_intr_slow_no_motion_u8 |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr_slow_no_motion(u8 v_channel_u8,
u8 v_intr_slow_no_motion_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* Write the slow no motion interrupt */
		case BMA2x2_INTR1_SLOW_NO_MOTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_SLOW_NO_MOTION__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_INTR1_PAD_SLOW_NO_MOTION,
			v_intr_slow_no_motion_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_SLOW_NO_MOTION__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_INTR2_SLOW_NO_MOTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_SLOW_NO_MOTION__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_INTR2_PAD_SLOW_NO_MOTION,
			v_intr_slow_no_motion_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_SLOW_NO_MOTION__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get
 * the interrupt enable of double tap interrupt
 * in the register 0x19 and 0x1B
 * @note INTR1_double -> register 0x19 bit 4
 * @note INTR2_double -> register 0x1B bit 4
 *
 *
 *
 *
 *  @param v_channel_u8: The value of double tap selection
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_INTR1_DOUBLE_TAP
 *              1          | BMA2x2_ACCEL_INTR2_DOUBLE_TAP
 *
 *	@param v_intr_double_tap_u8: The double tap interrupt enable value
 *       v_intr_double_tap_u8     |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr_double_tap(u8 v_channel_u8,
u8 *v_intr_double_tap_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		/* read the double tap*/
		switch (v_channel_u8) {
		case BMA2x2_INTR1_DOUBLE_TAP:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_DOUBLE_TAP__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_double_tap_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR1_PAD_DOUBLE_TAP);
		break;
		case BMA2x2_INTR2_DOUBLE_TAP:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_DOUBLE_TAP__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_double_tap_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR2_PAD_DOUBLE_TAP);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to set
 * the interrupt enable of double tap interrupt
 * in the register 0x19 and 0x1B
 * @note INTR1_double -> register 0x19 bit 4
 * @note INTR2_double -> register 0x1B bit 4
 *
 *
 *
 *
 *  @param v_channel_u8: The value of double tap selection
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_INTR1_DOUBLE_TAP
 *              1          | BMA2x2_ACCEL_INTR2_DOUBLE_TAP
 *
 *	@param v_intr_double_tap_u8: The double tap interrupt enable value
 *       v_intr_double_tap_u8     |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr_double_tap(u8 v_channel_u8,
u8 v_intr_double_tap_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* write the double tap*/
		case BMA2x2_INTR1_DOUBLE_TAP:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_DOUBLE_TAP__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_INTR1_PAD_DOUBLE_TAP,
			v_intr_double_tap_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_DOUBLE_TAP__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_INTR2_DOUBLE_TAP:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_DOUBLE_TAP__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_INTR2_PAD_DOUBLE_TAP,
			v_intr_double_tap_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_DOUBLE_TAP__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
		com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get
 * the interrupt enable of single tap
 * interrupt in the register 0x19 and 0x1B
 * @note INTR1_sigle_tap -> register 0x19 bit 5
 * @note INTR2_sigle_tap -> register 0x1B bit 5
 *
 *
 *  @param v_channel_u8: The value of single tap interrupt select
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_INTR1_SINGLE_TAP
 *              1          | BMA2x2_ACCEL_INTR2_SINGLE_TAP
 *
 *  @param v_intr_single_tap_u8: The single tap interrupt enable value
 *       v_intr_single_tap_u8     |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr_single_tap(u8 v_channel_u8,
u8 *v_intr_single_tap_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* Read the single tap value*/
		case BMA2x2_INTR1_SINGLE_TAP:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_SINGLE_TAP__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_single_tap_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR1_PAD_SINGLE_TAP);
		break;
		case BMA2x2_INTR2_SINGLE_TAP:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_SINGLE_TAP__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_single_tap_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR2_PAD_SINGLE_TAP);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to set
 * the interrupt enable of single tap
 * interrupt in the register 0x19 and 0x1B
 * @note INTR1_sigle_tap -> register 0x19 bit 5
 * @note INTR2_sigle_tap -> register 0x1B bit 5
 *
 *
 *  @param v_channel_u8: The value of single tap interrupt select
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_INTR1_SINGLE_TAP
 *              1          | BMA2x2_ACCEL_INTR2_SINGLE_TAP
 *
 *  @param v_intr_single_tap_u8: The single tap interrupt enable value
 *       v_intr_single_tap_u8     |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr_single_tap(u8 v_channel_u8,
u8 v_intr_single_tap_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* write the single tap value*/
		case BMA2x2_INTR1_SINGLE_TAP:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_SINGLE_TAP__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_ENABLE_INTR1_PAD_SINGLE_TAP,
			v_intr_single_tap_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_SINGLE_TAP__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_INTR2_SINGLE_TAP:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_SINGLE_TAP__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_INTR2_PAD_SINGLE_TAP,
			v_intr_single_tap_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_SINGLE_TAP__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get
 * the interrupt status of orient interrupt in the register 0x19 and 0x1B
 * @note INTR1_orient -> register 0x19 bit 6
 * @note INTR2_orient -> register 0x1B bit 6
 *
 *
 * @param v_channel_u8: The value of orient interrupt select
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_INTR1_ORIENT
 *              1          | BMA2x2_ACCEL_INTR2_ORIENT
 *
 *  @param v_intr_orient_u8: The value of orient interrupt enable
 *       v_intr_orient_u8         |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr_orient(u8 v_channel_u8,
u8 *v_intr_orient_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* Read orient interrupt*/
		case BMA2x2_INTR1_ORIENT:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_ORIENT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_orient_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR1_PAD_ORIENT);
		break;
		case BMA2x2_INTR2_ORIENT:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_ORIENT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_orient_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR2_PAD_ORIENT);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to set
 * the interrupt status of orient interrupt in the register 0x19 and 0x1B
 * @note INTR1_orient -> register 0x19 bit 6
 * @note INTR2_orient -> register 0x1B bit 6
 *
 *
 * @param v_channel_u8: The value of orient interrupt select
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_INTR1_ORIENT
 *              1          | BMA2x2_ACCEL_INTR2_ORIENT
 *
 *  @param v_intr_orient_u8: The value of orient interrupt enable
 *       v_intr_orient_u8         |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr_orient(u8 v_channel_u8,
u8 v_intr_orient_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* Write orient interrupt */
		case BMA2x2_INTR1_ORIENT:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_ORIENT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_INTR1_PAD_ORIENT, v_intr_orient_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_ORIENT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_INTR2_ORIENT:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_ORIENT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_INTR2_PAD_ORIENT, v_intr_orient_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_ORIENT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get
 * the interrupt enable of flat interrupt in the register 0x19 and 0x1B
 * @note INTR1_flat -> register 0x19 bit 7
 * @note INTR2_flat -> register 0x1B bit 7
 *
 *
 *
 *
 * @param v_channel_u8: The value of flat interrupt select
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_INTR1_FLAT
 *              1          | BMA2x2_ACCEL_INTR2_FLAT
 *
 * @param v_intr_flat_u8: The flat interrupt enable value
 *       v_intr_flat_u8           |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr_flat(u8 v_channel_u8,
u8 *v_intr_flat_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* Read flat interrupt */
		case BMA2x2_INTR1_FLAT:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_FLAT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_flat_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR1_PAD_FLAT);
		break;
		case BMA2x2_INTR2_FLAT:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_FLAT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_flat_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR2_PAD_FLAT);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to set
 * the interrupt enable of flat interrupt in the register 0x19 and 0x1B
 * @note INTR1_flat -> register 0x19 bit 7
 * @note INTR2_flat -> register 0x1B bit 7
 *
 *
 *
 *
 * @param v_channel_u8: The value of flat interrupt select
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_INTR1_FLAT
 *              1          | BMA2x2_ACCEL_INTR2_FLAT
 *
 * @param v_intr_flat_u8: The flat interrupt enable value
 *       v_intr_flat_u8           |   result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr_flat(u8 v_channel_u8,
u8 v_intr_flat_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* write flat interrupt */
		case BMA2x2_INTR1_FLAT:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_FLAT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_INTR1_PAD_FLAT, v_intr_flat_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_FLAT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_INTR2_FLAT:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_FLAT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_INTR2_PAD_FLAT, v_intr_flat_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_FLAT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get
 * the interrupt status of new data in the register 0x19
 * @note INTR1_data -> register 0x19 bit 0
 * @note INTR2_data -> register 0x19 bit 7
 *
 *
 *
 *  @param v_channel_u8: The value of new data interrupt select
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_INTR1_NEWDATA
 *              1          | BMA2x2_ACCEL_INTR2_NEWDATA
 *
 *	@param intr_newdata_u8: The new data interrupt enable value
 *       intr_newdata_u8          |    result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_new_data(u8 v_channel_u8,
u8 *intr_newdata_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* Read the data interrupt*/
		case BMA2x2_INTR1_NEWDATA:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_NEWDATA__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*intr_newdata_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR1_PAD_NEWDATA);
		break;
		case BMA2x2_INTR2_NEWDATA:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_NEWDATA__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*intr_newdata_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR2_PAD_NEWDATA);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to set
 * the interrupt status of new data in the register 0x19
 * @note INTR1_data -> register 0x19 bit 0
 * @note INTR2_data -> register 0x19 bit 7
 *
 *
 *
 *  @param v_channel_u8: The value of new data interrupt select
 *        v_channel_u8     |   result
 *       ----------------- | ------------------
 *              0          | BMA2x2_ACCEL_INTR1_NEWDATA
 *              1          | BMA2x2_ACCEL_INTR2_NEWDATA
 *
 *	@param intr_newdata_u8: The new data interrupt enable value
 *       intr_newdata_u8          |    result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_new_data(u8 v_channel_u8,
u8 intr_newdata_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* write the new data interrupt */
		case BMA2x2_INTR1_NEWDATA:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_NEWDATA__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_INTR1_PAD_NEWDATA, intr_newdata_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_NEWDATA__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_INTR2_NEWDATA:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_NEWDATA__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_INTR2_PAD_NEWDATA, intr_newdata_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_NEWDATA__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get the fifo watermark interrupt1 data
 * in the register 0x1A bit 1
 *
 *  @param  v_intr1_fifo_wm_u8 : The value of interrupt1 FIFO watermark enable
 *       v_intr1_fifo_wm_u8       |    result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr1_fifo_wm(u8 *v_intr1_fifo_wm_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read the fifo watermark interrupt */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_FIFO_WM__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr1_fifo_wm_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR1_PAD_FIFO_WM);
		}
	return com_rslt;
}
/*!
 * @brief This API is used to set the fifo watermark interrupt1 data
 * in the register 0x1A bit 1
 *
 *  @param  v_intr1_fifo_wm_u8 : The value of interrupt1 FIFO watermark enable
 *       v_intr1_fifo_wm_u8       |    result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr1_fifo_wm(u8 v_intr1_fifo_wm_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		if (v_intr1_fifo_wm_u8 < C_BMA2x2_TWO_U8X) {
			/* write the fifo watermark interrupt */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_FIFO_WM__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_INTR1_PAD_FIFO_WM, v_intr1_fifo_wm_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_FIFO_WM__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		} else {
		com_rslt = E_OUT_OF_RANGE;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get the fifo watermark interrupt2 data
 * in the register 0x1A bit 6
 *
 *  @param  v_intr2_fifo_wm_u8 : The value of interrupt1 FIFO watermark enable
 *       v_intr2_fifo_wm_u8       |    result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr2_fifo_wm(u8 *v_intr2_fifo_wm_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read the fifo watermark interrupt2*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_FIFO_WM__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr2_fifo_wm_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR2_PAD_FIFO_WM);
		}
	return com_rslt;
}
/*!
 * @brief This API is used to set the fifo watermark interrupt2 data
 * in the register 0x1A bit 6
 *
 *  @param  v_intr2_fifo_wm_u8 : The value of interrupt1 FIFO watermark enable
 *       v_intr2_fifo_wm_u8       |    result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr2_fifo_wm(u8 v_intr2_fifo_wm_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		if (v_intr2_fifo_wm_u8 < C_BMA2x2_TWO_U8X) {
			/* write the fifo watermark interrupt2*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_FIFO_WM__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_INTR2_PAD_FIFO_WM, v_intr2_fifo_wm_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_FIFO_WM__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		} else {
		com_rslt = E_OUT_OF_RANGE;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the fifo full interrupt1 in the register 0x1A bit 2
 *
 *
 *
 *  @param v_intr1_fifo_full_u8 : The value of fifo full interrupt enable
 *       v_intr1_fifo_full_u8     |    result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr1_fifo_full(u8 *v_intr1_fifo_full_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read the fifo full interrupt1*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_FIFO_FULL__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr1_fifo_full_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR1_PAD_FIFO_FULL);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the fifo full interrupt1 in the register 0x1A bit 2
 *
 *
 *
 *  @param v_intr1_fifo_full_u8 : The value of fifo full interrupt enable
 *       v_intr1_fifo_full_u8     |    result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr1_fifo_full(u8 v_intr1_fifo_full_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		if (v_intr1_fifo_full_u8 < C_BMA2x2_TWO_U8X) {
			/* write the fifo full interrupt1*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_FIFO_FULL__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR1_PAD_FIFO_FULL,
			v_intr1_fifo_full_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR1_PAD_FIFO_FULL__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			} else {
			com_rslt = E_OUT_OF_RANGE;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the fifo full interrupt2 in the register 0x1A bit 5
 *
 *
 *
 *  @param v_intr2_fifo_full_u8 : Thee vale of fifo full enable
 *       v_intr2_fifo_full_u8     |    result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr2_fifo_full(u8 *v_intr2_fifo_full_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read the fifo full interrupt2*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_FIFO_FULL__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr2_fifo_full_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_INTR2_PAD_FIFO_FULL);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the fifo full interrupt2 in the register 0x1A bit 5
 *
 *
 *
 *  @param v_intr2_fifo_full_u8 : Thee vale of fifo full enable
 *       v_intr2_fifo_full_u8     |    result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr2_fifo_full(u8 v_intr2_fifo_full_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		if (v_intr2_fifo_full_u8 < C_BMA2x2_TWO_U8X) {
			/* write the fifo full interrupt2*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_FIFO_FULL__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_INTR2_PAD_FIFO_FULL,
			v_intr2_fifo_full_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_INTR2_PAD_FIFO_FULL__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			} else {
			com_rslt = E_OUT_OF_RANGE;
			}
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the source data status of source data,
 *	source slow no motion, source slope, source high
 *	and source low in the register 0x1E bit from 0 to 5
 *
 *
 *
 *  @param v_channel_u8 : The value of source select
 *       v_channel_u8     |    result
 *       -----------------| ------------------
 *               0        | BMA2x2_ACCEL_SOURCE_LOW_G
 *               1        | BMA2x2_ACCEL_SOURCE_HIGH_G
 *               2        | BMA2x2_ACCEL_SOURCE_SLOPE
 *               3        | BMA2x2_ACCEL_SOURCE_SLOW_NO_MOTION
 *               4        | BMA2x2_ACCEL_SOURCE_TAP
 *               5        | BMA2x2_ACCEL_SOURCE_DATA
 *
 *	@param v_intr_source_u8: The source status enable value
 *       v_intr_source_u8         |    result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_source(u8 v_channel_u8,
u8 *v_intr_source_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		return  E_BMA2x2_NULL_PTR;
		} else {
		/* read the source interrupt register */
		switch (v_channel_u8) {
		case BMA2x2_SOURCE_LOW_G:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_LOW_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_source_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_UNFILT_INTR_SOURCE_LOW_G);
		break;
		case BMA2x2_SOURCE_HIGH_G:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_HIGH_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_source_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_UNFILT_INTR_SOURCE_HIGH_G);
		break;
		case BMA2x2_SOURCE_SLOPE:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_SLOPE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_source_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_UNFILT_INTR_SOURCE_SLOPE);
		break;
		case BMA2x2_SOURCE_SLOW_NO_MOTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_SLOW_NO_MOTION__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_source_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_UNFILT_INTR_SOURCE_SLOW_NO_MOTION);
		break;
		case BMA2x2_SOURCE_TAP:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_TAP__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_source_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_UNFILT_INTR_SOURCE_TAP);
		break;
		case BMA2x2_SOURCE_DATA:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_DATA__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_source_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_UNFILT_INTR_SOURCE_DATA);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
			}
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the source data status of source data,
 *	source slow no motion, source slope, source high
 *	and source low in the register 0x1E bit from 0 to 5
 *
 *
 *
 *  @param v_channel_u8 : The value of source select
 *       v_channel_u8     |    result
 *       -----------------| ------------------
 *               0        | BMA2x2_ACCEL_SOURCE_LOW_G
 *               1        | BMA2x2_ACCEL_SOURCE_HIGH_G
 *               2        | BMA2x2_ACCEL_SOURCE_SLOPE
 *               3        | BMA2x2_ACCEL_SOURCE_SLOW_NO_MOTION
 *               4        | BMA2x2_ACCEL_SOURCE_TAP
 *               5        | BMA2x2_ACCEL_SOURCE_DATA
 *
 *	@param v_intr_source_u8: The source status enable value
 *       v_intr_source_u8         |    result
 *       ------------------------ | ------------------
 *              0x00              | INTR_DISABLE
 *              0x01              | INTR_ENABLE
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_source(u8 v_channel_u8,
u8 v_intr_source_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
		if (p_bma2x2 == BMA2x2_NULL) {
			return  E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* write the source interrupt register*/
		case BMA2x2_SOURCE_LOW_G:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_LOW_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_UNFILT_INTR_SOURCE_LOW_G, v_intr_source_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_LOW_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_SOURCE_HIGH_G:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_HIGH_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_UNFILT_INTR_SOURCE_HIGH_G, v_intr_source_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_HIGH_G__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_SOURCE_SLOPE:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_SLOPE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_UNFILT_INTR_SOURCE_SLOPE, v_intr_source_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_SLOPE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_SOURCE_SLOW_NO_MOTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_SLOW_NO_MOTION__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_UNFILT_INTR_SOURCE_SLOW_NO_MOTION,
			v_intr_source_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_SLOW_NO_MOTION__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_SOURCE_TAP:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_TAP__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_UNFILT_INTR_SOURCE_TAP,
			v_intr_source_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_TAP__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_SOURCE_DATA:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_DATA__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_UNFILT_INTR_SOURCE_DATA,
			v_intr_source_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_UNFILT_INTR_SOURCE_DATA__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the interrupt output type in the register 0x20.
 *	@note INTR1 -> bit 1
 *	@note INTR2 -> bit 3
 *
 *  @param v_channel_u8: The value of output type select
 *       v_channel_u8     |    result
 *       -----------------| ------------------
 *               0        | BMA2x2_ACCEL_INTR1_OUTPUT
 *               1        | BMA2x2_ACCEL_INTR2_OUTPUT
 *
 *	@param v_intr_output_type_u8: The value of output type select
 *       v_intr_source_u8         |    result
 *       ------------------------ | ------------------
 *              0x01              | OPEN_DRAIN
 *              0x00              | PUSS_PULL
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr_output_type(u8 v_channel_u8,
u8 *v_intr_output_type_u8)
{
		u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
		communication routine*/
		BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
		if (p_bma2x2 == BMA2x2_NULL) {
			return  E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* read the output type */
		case BMA2x2_INTR1_OUTPUT:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR1_PAD_OUTPUT_TYPE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_output_type_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_INTR1_PAD_OUTPUT_TYPE);
		break;
		case BMA2x2_INTR2_OUTPUT:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR2_PAD_OUTPUT_TYPE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_output_type_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_INTR2_PAD_OUTPUT_TYPE);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the interrupt output type in the register 0x20.
 *	@note INTR1 -> bit 1
 *	@note INTR2 -> bit 3
 *
 *  @param v_channel_u8: The value of output type select
 *         v_channel_u8   |    result
 *       -----------------| ------------------
 *               0        | BMA2x2_ACCEL_INTR1_OUTPUT
 *               1        | BMA2x2_ACCEL_INTR2_OUTPUT
 *
 *	@param v_intr_output_type_u8: The value of output type select
 *       v_intr_source_u8         |    result
 *       ------------------------ | ------------------
 *              0x01              | OPEN_DRAIN
 *              0x00              | PUSS_PULL
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr_output_type(u8 v_channel_u8,
u8 v_intr_output_type_u8)
{
		u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
		communication routine*/
		BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
		if (p_bma2x2 == BMA2x2_NULL) {
			return  E_BMA2x2_NULL_PTR;
		}  else {
		switch (v_channel_u8) {
		/* write the output type*/
		case BMA2x2_INTR1_OUTPUT:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR1_PAD_OUTPUT_TYPE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_INTR1_PAD_OUTPUT_TYPE, v_intr_output_type_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR1_PAD_OUTPUT_TYPE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_INTR2_OUTPUT:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR2_PAD_OUTPUT_TYPE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_INTR2_PAD_OUTPUT_TYPE, v_intr_output_type_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR2_PAD_OUTPUT_TYPE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	Active Level status in the register 0x20
 *	@note INTR1 -> bit 0
 *	@note INTR2 -> bit 2
 *
 *  @param v_channel_u8: The value of Active Level select
 *       v_channel_u8     |    result
 *       -----------------| ------------------
 *               0        | BMA2x2_ACCEL_INTR1_LEVEL
 *               1        | BMA2x2_ACCEL_INTR2_LEVEL
 *
 *  @param v_intr_level_u8: The Active Level status enable value
 *        v_intr_level_u8         |    result
 *       ------------------------ | ------------------
 *              0x01              | ACTIVE_HIGH
 *              0x00              | ACTIVE_LOW
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_intr_level(u8 v_channel_u8,
u8 *v_intr_level_u8)
{
		u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
		communication routine*/
		BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
		if (p_bma2x2 == BMA2x2_NULL) {
			return  E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* read the active level*/
		case BMA2x2_INTR1_LEVEL:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR1_PAD_ACTIVE_LEVEL__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_level_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_INTR1_PAD_ACTIVE_LEVEL);
		break;
		case BMA2x2_INTR2_LEVEL:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR2_PAD_ACTIVE_LEVEL__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_intr_level_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_INTR2_PAD_ACTIVE_LEVEL);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	Active Level status in the register 0x20
 *	@note INTR1 -> bit 0
 *	@note INTR2 -> bit 2
 *
 *  @param v_channel_u8: The value of Active Level select
 *       v_channel_u8     |    result
 *       -----------------| ------------------
 *               0        | BMA2x2_ACCEL_INTR1_LEVEL
 *               1        | BMA2x2_ACCEL_INTR2_LEVEL
 *
 *  @param v_intr_level_u8: The Active Level status enable value
 *       v_intr_level_u8          |    result
 *       ------------------------ | ------------------
 *              0x01              | ACTIVE_HIGH
 *              0x00              | ACTIVE_LOW
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_intr_level(u8 v_channel_u8,
u8 v_intr_level_u8)
{
		u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
		communication routine*/
		BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
		if (p_bma2x2 == BMA2x2_NULL) {
			return  E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* write the active level */
		case BMA2x2_INTR1_LEVEL:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR1_PAD_ACTIVE_LEVEL__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_INTR1_PAD_ACTIVE_LEVEL, v_intr_level_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR1_PAD_ACTIVE_LEVEL__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_INTR2_LEVEL:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR2_PAD_ACTIVE_LEVEL__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_INTR2_PAD_ACTIVE_LEVEL, v_intr_level_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_INTR2_PAD_ACTIVE_LEVEL__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the reset interrupt in the register 0x21 bit 7
 *
 *
 *
 *  @param  v_rst_intr_u8: The value of reset interrupt
 *          v_rst_intr_u8         |  result
 *       ------------------------ | ------------------
 *              0x01              | clear any latch interrupt
 *              0x00              | keep latch interrupt active
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_rst_intr(u8 v_rst_intr_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_RESET_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_RESET_INTR, v_rst_intr_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr, BMA2x2_RESET_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the latch duration in the register 0x21 bit from 0 to 3
 *
 *	@param v_latch_intr_u8: The value of latch duration
 *        v_latch_intr_u8 |  result
 *       -----------------| ------------------
 *               0x00     | BMA2x2_LATCH_DURN_NON_LATCH
 *               0x01     | BMA2x2_LATCH_DURN_250MS
 *               0x02     | BMA2x2_LATCH_DURN_500MS
 *               0x03     | BMA2x2_LATCH_DURN_1S
 *               0x04     | BMA2x2_LATCH_DURN_2S
 *               0x05     | BMA2x2_LATCH_DURN_4S
 *               0x06     | BMA2x2_LATCH_DURN_8S
 *               0x07     | BMA2x2_LATCH_DURN_LATCH
 *               0x08     | BMA2x2_LATCH_DURN_NON_LATCH1
 *               0x09     | BMA2x2_LATCH_DURN_250US
 *               0x0A     | BMA2x2_LATCH_DURN_500US
 *               0x0B     | BMA2x2_LATCH_DURN_1MS
 *               0x0C     | BMA2x2_LATCH_DURN_12_5MS
 *               0x0D     | BMA2x2_LATCH_DURN_25MS
 *               0x0E     | BMA2x2_LATCH_DURN_50MS
 *               0x0F     | BMA2x2_LATCH_DURN_LATCH1
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_latch_intr(u8 *v_latch_intr_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read the latch duration */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_LATCH_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_latch_intr_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_LATCH_INTR);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the latch duration in the register 0x21 bit from 0 to 3
 *
 *	@param v_latch_intr_u8: The value of latch duration
 *        v_latch_intr_u8 |  result
 *       -----------------| ------------------
 *               0x00     | BMA2x2_LATCH_DURN_NON_LATCH
 *               0x01     | BMA2x2_LATCH_DURN_250MS
 *               0x02     | BMA2x2_LATCH_DURN_500MS
 *               0x03     | BMA2x2_LATCH_DURN_1S
 *               0x04     | BMA2x2_LATCH_DURN_2S
 *               0x05     | BMA2x2_LATCH_DURN_4S
 *               0x06     | BMA2x2_LATCH_DURN_8S
 *               0x07     | BMA2x2_LATCH_DURN_LATCH
 *               0x08     | BMA2x2_LATCH_DURN_NON_LATCH1
 *               0x09     | BMA2x2_LATCH_DURN_250US
 *               0x0A     | BMA2x2_LATCH_DURN_500US
 *               0x0B     | BMA2x2_LATCH_DURN_1MS
 *               0x0C     | BMA2x2_LATCH_DURN_12_5MS
 *               0x0D     | BMA2x2_LATCH_DURN_25MS
 *               0x0E     | BMA2x2_LATCH_DURN_50MS
 *               0x0F     | BMA2x2_LATCH_DURN_LATCH1
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_latch_intr(u8 v_latch_intr_u8)
{
u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
/*  Variable used to return value of
communication routine*/
BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
u8 v_latch_durn_u8 = C_BMA2x2_ZERO_U8X;
if (p_bma2x2 == BMA2x2_NULL)  {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else  {
		if (v_latch_intr_u8 < C_BMA2x2_SIXTEEN_U8X) {
			switch (v_latch_intr_u8) {
			case BMA2x2_LATCH_DURN_NON_LATCH:
				v_latch_durn_u8 = BMA2x2_LATCH_DURN_NON_LATCH;

				/*  NON LATCH   */
			break;
			case BMA2x2_LATCH_DURN_250MS:
				v_latch_durn_u8 = BMA2x2_LATCH_DURN_250MS;

				/*  250 MS  */
			break;
			case BMA2x2_LATCH_DURN_500MS:
				v_latch_durn_u8 = BMA2x2_LATCH_DURN_500MS;

				/*  500 MS  */
			break;
			case BMA2x2_LATCH_DURN_1S:
				v_latch_durn_u8 = BMA2x2_LATCH_DURN_1S;

				/*  1 S   */
			break;
			case BMA2x2_LATCH_DURN_2S:
				v_latch_durn_u8 = BMA2x2_LATCH_DURN_2S;

				/*  2 S  */
			break;
			case BMA2x2_LATCH_DURN_4S:
				v_latch_durn_u8 = BMA2x2_LATCH_DURN_4S;

				/*  4 S  */
			break;
			case BMA2x2_LATCH_DURN_8S:
				v_latch_durn_u8 = BMA2x2_LATCH_DURN_8S;

				/*  8 S  */
			break;
			case BMA2x2_LATCH_DURN_LATCH:
				v_latch_durn_u8 = BMA2x2_LATCH_DURN_LATCH;

				/*  LATCH  */
			break;
			case BMA2x2_LATCH_DURN_NON_LATCH1:
				v_latch_durn_u8 = BMA2x2_LATCH_DURN_NON_LATCH1;

				/*  NON LATCH1  */
			break;
			case BMA2x2_LATCH_DURN_250US:
				v_latch_durn_u8 = BMA2x2_LATCH_DURN_250US;

				/*  250 US   */
			break;
			case BMA2x2_LATCH_DURN_500US:
				v_latch_durn_u8 = BMA2x2_LATCH_DURN_500US;

				/*  500 US   */
			break;
			case BMA2x2_LATCH_DURN_1MS:
				v_latch_durn_u8 = BMA2x2_LATCH_DURN_1MS;

				/*  1 MS   */
			break;
			case BMA2x2_LATCH_DURN_12_5MS:
				v_latch_durn_u8 = BMA2x2_LATCH_DURN_12_5MS;

				/*  12.5 MS   */
			break;
			case BMA2x2_LATCH_DURN_25MS:
				v_latch_durn_u8 = BMA2x2_LATCH_DURN_25MS;

				/*  25 MS   */
			break;
			case BMA2x2_LATCH_DURN_50MS:
				v_latch_durn_u8 = BMA2x2_LATCH_DURN_50MS;

				/*  50 MS   */
			break;
			case BMA2x2_LATCH_DURN_LATCH1:
				v_latch_durn_u8 = BMA2x2_LATCH_DURN_LATCH1;

				/*  LATCH1   */
			break;
			default:
			break;
			}
			/* write the latch duration */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_LATCH_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_LATCH_INTR, v_latch_durn_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr, BMA2x2_LATCH_INTR__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		} else {
		com_rslt = E_OUT_OF_RANGE;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get the duration of
 *	Low, High, Slope and slow no motion interrupts in the registers
 *	@note LOW_DURN		-> register 0x22 bit form 0 to 7
 *	@note HIGH_DURN		-> register 0x25 bit form 0 to 7
 *	@note SLOPE_DURN		-> register 0x27 bit form 0 to 1
 *	@note SLO_NO_MOT_DURN -> register 0x27 bit form 2 to 7
 *
 *  @param v_channel_u8: The value of duration select
 *     v_channel_u8   | result
 *   -----------------| ------------------
 *               0    | BMA2x2_ACCEL_LOW_DURN
 *               1    | BMA2x2_ACCEL_HIGH_DURN
 *               2    | BMA2x2_ACCEL_SLOPE_DURN
 *               3    | BMA2x2_ACCEL_SLOW_NO_MOTION_DURN
 *
 *	@param v_durn_u8: The value of duration
 *
 *	@note :
 *     Duration           |    result
 * -----------------------| ------------------
 * BMA2x2_ACCEL_LOW_DURN  | Low-g interrupt trigger
 *         -              | delay according to([v_durn_u8 +1]*2)ms
 *         -              | range from 2ms to 512ms. default is 20ms
 * BMA2x2_ACCEL_HIGH_DURN | high-g interrupt trigger
 *         -              | delay according to([v_durn_u8 +1]*2)ms
 *         -              | range from 2ms to 512ms. default is 32ms
 * BMA2x2_ACCEL_SLOPE_DURN| slope interrupt trigger
 *         -              | if[v_durn_u8<1:0>+1] consecutive data points
 *         -              | are above the slope interrupt threshold
 * SLO_NO_MOT_DURN        | Refer data sheet for clear information
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_durn(u8 v_channel_u8,
u8 *v_durn_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		/* write the duration data */
		switch (v_channel_u8) {
		case BMA2x2_LOW_DURN:
			/*LOW DURATION*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_LOW_DURN_REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_durn_u8 = v_data_u8;
		break;
		case BMA2x2_HIGH_DURN:
			/*HIGH DURATION*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_HIGH_DURN_REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_durn_u8 = v_data_u8;
		break;
		case BMA2x2_SLOPE_DURN:
			/*SLOPE DURATION*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_SLOPE_DURN__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_durn_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_SLOPE_DURN);
		break;
		case BMA2x2_SLOW_NO_MOTION_DURN:
			/*SLO NO MOT DURATION*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_SLOW_NO_MOTION_DURN__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_durn_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_SLOW_NO_MOTION_DURN);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to set the duration of
 *	Low, High, Slope and slow no motion interrupts in the registers
 *	@note LOW_DURN		-> register 0x22 bit form 0 to 7
 *	@note HIGH_DURN		-> register 0x25 bit form 0 to 7
 *	@note SLOPE_DURN		-> register 0x27 bit form 0 to 1
 *	@note SLO_NO_MOT_DURN -> register 0x27 bit form 2 to 7
 *
 *  @param v_channel_u8: The value of duration select
 *     v_channel_u8   | result
 *   -----------------| ------------------
 *               0    | BMA2x2_ACCEL_LOW_DURN
 *               1    | BMA2x2_ACCEL_HIGH_DURN
 *               2    | BMA2x2_ACCEL_SLOPE_DURN
 *               3    | BMA2x2_ACCEL_SLOW_NO_MOTION_DURN
 *
 *	@param v_durn_u8: The value of duration
 *
 *	@note :
 *     Duration           |    result
 * -----------------------| ------------------
 * BMA2x2_ACCEL_LOW_DURN  | Low-g interrupt trigger
 *         -              | delay according to([v_durn_u8 +1]*2)ms
 *         -              | range from 2ms to 512ms. default is 20ms
 * BMA2x2_ACCEL_HIGH_DURN | high-g interrupt trigger
 *         -              | delay according to([v_durn_u8 +1]*2)ms
 *         -              | range from 2ms to 512ms. default is 32ms
 * BMA2x2_ACCEL_SLOPE_DURN| slope interrupt trigger
 *         -              | if[v_durn_u8<1:0>+1] consecutive data points
 *         -              | are above the slope interrupt threshold
 * SLO_NO_MOT_DURN        | Refer data sheet for clear information
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_durn(u8 v_channel_u8,
u8 v_durn_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL)  {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		}  else  {
		/* write duration data */
		switch (v_channel_u8)   {
		case BMA2x2_LOW_DURN:
			/*LOW DURATION*/
			v_data_u8 = v_durn_u8;
			com_rslt = p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr, BMA2x2_LOW_DURN_REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_HIGH_DURN:
			/*HIGH DURATION*/
			v_data_u8 = v_durn_u8;
			com_rslt = p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_HIGH_DURN_REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_SLOPE_DURN:
			/*SLOPE DURATION*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_SLOPE_DURN__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_SLOPE_DURN, v_durn_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_SLOPE_DURN__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_SLOW_NO_MOTION_DURN:
			/*SLO NO MOT DURATION*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_SLOW_NO_MOTION_DURN__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_SLOW_NO_MOTION_DURN, v_durn_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_SLOW_NO_MOTION_DURN__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get the threshold of
 *	Low, High, Slope and slow no motion interrupts in the registers
 *	@note LOW_THRES		-> register 0x23 bit form 0 to 7
 *	@note HIGH_THRES		-> register 0x26 bit form 0 to 7
 *	@note SLOPE_THRES		-> register 0x28 bit form 0 to 7
 *	@note SLO_NO_MOT_THRES -> register 0x29 bit form 0 to 7
 *
 *  @param v_channel_u8: The value of threshold selection
 *     v_channel_u8   | result
 *   -----------------| ------------------
 *               0    | BMA2x2_ACCEL_LOW_THRES
 *               1    | BMA2x2_ACCEL_HIGH_THRES
 *               2    | BMA2x2_ACCEL_SLOPE_THRES
 *               3    | BMA2x2_ACCEL_SLOW_NO_MOTION_THRES
 *
 *  @param v_thres_u8: The threshold value of selected interrupts
 *
 *	@note : LOW-G THRESHOLD
 *     Threshold                    |    result
 * ---------------------------------| ------------------
 * BMA2x2_ACCEL_LOW_THRES           | Low-threshold interrupt trigger
 *                                  | according to(v_thres_u8 * 7.81) mg
 *                                  | range from 0g to 1.992g
 *                                  | default is 375mg
 *	@note : HIGH-G THRESHOLD
 *	@note Threshold of high-g interrupt according to accel g range
 *    g-range           |      High-g threshold
 *  --------------------|----------------------------
 *     2g               |    (v_thres_u8 * 7.81) mg
 *     4g               |    (v_thres_u8 * 15.63) mg
 *     8g               |    (v_thres_u8 * 31.25) mg
 *     16g              |    (v_thres_u8 * 62.5) mg
 *
 *	@note : SLOPE THRESHOLD
 *	@note Threshold of slope interrupt according to accel g range
 *    g-range           |      Slope threshold
 *  --------------------|----------------------------
 *     2g               |    (v_thres_u8 * 3.19) mg
 *     4g               |    (v_thres_u8 * 7.81) mg
 *     8g               |    (v_thres_u8 * 15.63) mg
 *     16g              |    (v_thres_u8 * 31.25) mg
 *
 *	@note : SLOW NO MOTION THRESHOLD
 *	@note Threshold of slow no motion interrupt according to accel g range
 *    g-range           |   slow no motion threshold
 *  --------------------|----------------------------
 *     2g               |    (v_thres_u8 * 3.19) mg
 *     4g               |    (v_thres_u8 * 7.81) mg
 *     8g               |    (v_thres_u8 * 15.63) mg
 *     16g              |    (v_thres_u8 * 31.25) mg
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_thres(u8 v_channel_u8,
u8 *v_thres_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* Read the threshold value */
		case BMA2x2_LOW_THRES:
			/*LOW THRESHOLD*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_LOW_THRES_REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_thres_u8 = v_data_u8;
		break;
		case BMA2x2_HIGH_THRES:
			/*HIGH THRESHOLD*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_HIGH_THRES_REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_thres_u8 = v_data_u8;
		break;
		case BMA2x2_SLOPE_THRES:
			/*SLOPE THRESHOLD*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_SLOPE_THRES_REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_thres_u8 = v_data_u8;
		break;
		case BMA2x2_SLOW_NO_MOTION_THRES:
			/*SLO NO MOT THRESHOLD*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_SLOW_NO_MOTION_THRES_REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_thres_u8 = v_data_u8;
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to set the threshold of
 *	Low, High, Slope and slow no motion interrupts in the registers
 *	@note LOW_THRES		-> register 0x23 bit form 0 to 7
 *	@note HIGH_THRES		-> register 0x26 bit form 0 to 7
 *	@note SLOPE_THRES		-> register 0x28 bit form 0 to 7
 *	@note SLO_NO_MOT_THRES -> register 0x29 bit form 0 to 7
 *
 *  @param v_channel_u8: The value of threshold selection
 *     v_channel_u8   | result
 *   -----------------| ------------------
 *               0    | BMA2x2_ACCEL_LOW_THRES
 *               1    | BMA2x2_ACCEL_HIGH_THRES
 *               2    | BMA2x2_ACCEL_SLOPE_THRES
 *               3    | BMA2x2_ACCEL_SLOW_NO_MOTION_THRES
 *
 *  @param v_thres_u8: The threshold value of selected interrupts
 *
 *	@note : LOW-G THRESHOLD
 *     Threshold                    |    result
 * ---------------------------------| ------------------
 * BMA2x2_ACCEL_LOW_THRES           | Low-threshold interrupt trigger
 *                                  | according to(v_thres_u8 * 7.81) mg
 *                                  | range from 0g to 1.992g
 *                                  | default is 375mg
 *	@note : HIGH-G THRESHOLD
 *	@note Threshold of high-g interrupt according to accel g range
 *    g-range           |      High-g threshold
 *  --------------------|----------------------------
 *     2g               |    (v_thres_u8 * 7.81) mg
 *     4g               |    (v_thres_u8 * 15.63) mg
 *     8g               |    (v_thres_u8 * 31.25) mg
 *     16g              |    (v_thres_u8 * 62.5) mg
 *
 *	@note : SLOPE THRESHOLD
 *	@note Threshold of slope interrupt according to accel g range
 *    g-range           |      Slope threshold
 *  --------------------|----------------------------
 *     2g               |    (v_thres_u8 * 3.19) mg
 *     4g               |    (v_thres_u8 * 7.81) mg
 *     8g               |    (v_thres_u8 * 15.63) mg
 *     16g              |    (v_thres_u8 * 31.25) mg
 *
 *	@note : SLOW NO MOTION THRESHOLD
 *	@note Threshold of slow no motion interrupt according to accel g range
 *    g-range           |   slow no motion threshold
 *  --------------------|----------------------------
 *     2g               |    (v_thres_u8 * 3.19) mg
 *     4g               |    (v_thres_u8 * 7.81) mg
 *     8g               |    (v_thres_u8 * 15.63) mg
 *     16g              |    (v_thres_u8 * 31.25) mg
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_thres(u8 v_channel_u8,
u8 v_thres_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* write the threshold value*/
		case BMA2x2_LOW_THRES:
			/*LOW THRESHOLD*/
			v_data_u8 = v_thres_u8;
			com_rslt = p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_LOW_THRES_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_HIGH_THRES:
			/*HIGH THRESHOLD*/
			v_data_u8 = v_thres_u8;
			com_rslt = p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_HIGH_THRES_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_SLOPE_THRES:
			/*SLOPE THRESHOLD*/
			v_data_u8 = v_thres_u8;
			com_rslt = p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_SLOPE_THRES_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_SLOW_NO_MOTION_THRES:
			/*SLO NO MOT THRESHOLD*/
			v_data_u8 = v_thres_u8;
			com_rslt = p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_SLOW_NO_MOTION_THRES_REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the low high hysteresis in the registers 0x24
 *	@note LOW_G_HYST  -> bit form 0 to 1
 *	@note HIGH_G_HYST  -> bit from 6 to 7
 *
 *  @param v_channel_u8: The value of hysteresis selection
 *     v_channel_u8   | result
 *   -----------------| ------------------
 *           0        | BMA2x2_ACCEL_LOW_G_HYST
 *           1        | BMA2x2_ACCEL_HIGH_G_HYST
 *
 *  @param v_hyst_u8: The hysteresis data
 *
 *	@note LOW HYSTERESIS
 *	@note Hysteresis of low-g interrupt according to (v_hyst_u8 * 125)mg
 *
 *	@note HIGH HYSTERESIS
 *	@note High hysteresis depends on the accel range selection
 *    g-range           |    High Hysteresis
 *  --------------------|----------------------------
 *     2g               |    (v_thres_u8 * 125) mg
 *     4g               |    (v_thres_u8 * 250) mg
 *     8g               |    (v_thres_u8 * 500) mg
 *     16g              |    (v_thres_u8 * 1000) mg
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_low_high_g_hyst(u8 v_channel_u8,
u8 *v_hyst_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* read the hysteresis data */
		case BMA2x2_LOW_G_HYST:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_LOW_G_HYST__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_hyst_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_LOW_G_HYST);
		break;
		case BMA2x2_HIGH_G_HYST:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_HIGH_G_HYST__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_hyst_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_HIGH_G_HYST);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the low high hysteresis in the registers 0x24
 *	@note LOW_G_HYST  -> bit form 0 to 1
 *	@note HIGH_G_HYST  -> bit from 6 to 7
 *
 *  @param v_channel_u8: The value of hysteresis selection
 *     v_channel_u8   | result
 *   -----------------| ------------------
 *           0        | BMA2x2_ACCEL_LOW_G_HYST
 *           1        | BMA2x2_ACCEL_HIGH_G_HYST
 *
 *  @param v_hyst_u8: The hysteresis data
 *
 *	@note LOW HYSTERESIS
 *	@note Hysteresis of low-g interrupt according to (v_hyst_u8 * 125)mg
 *
 *	@note HIGH HYSTERESIS
 *	@note High hysteresis depends on the accel range selection
 *    g-range           |    High Hysteresis
 *  --------------------|----------------------------
 *     2g               |    (v_thres_u8 * 125) mg
 *     4g               |    (v_thres_u8 * 250) mg
 *     8g               |    (v_thres_u8 * 500) mg
 *     16g              |    (v_thres_u8 * 1000) mg
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_low_high_g_hyst(u8 v_channel_u8,
u8 v_hyst_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* write the hysteresis data  */
		case BMA2x2_LOW_G_HYST:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_LOW_G_HYST__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_LOW_G_HYST, v_hyst_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_LOW_G_HYST__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_HIGH_G_HYST:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_HIGH_G_HYST__REG, &v_data_u8,
			C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_HIGH_G_HYST, v_hyst_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_HIGH_G_HYST__REG,
			&v_data_u8,  C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	low_g  mode in the registers 0x24 bit 2
 *
 *
 *	@param v_low_g_mode_u8: The value of Low_G mode
 *    g-v_low_g_mode_u8 |    result
 *  --------------------|----------------------------
 *     0x00             | LOW_G_SINGLE_AXIS_MODE
 *     0x01             | LOW_G_SUMMING_MODE
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_low_g_mode(u8 *v_low_g_mode_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read the low-g mode*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_LOW_G_INTR_MODE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_low_g_mode_u8 = BMA2x2_GET_BITSLICE(v_data_u8,
			BMA2x2_LOW_G_INTR_MODE);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	low_g  mode in the registers 0x24 bit 2
 *
 *
 *	@param v_low_g_mode_u8: The value of Low_G mode
 *    v_low_g_mode_u8   |    result
 *  --------------------|----------------------------
 *     0x00             | LOW_G_SINGLE_AXIS_MODE
 *     0x01             | LOW_G_SUMMING_MODE
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_low_g_mode(u8 v_low_g_mode_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* write the low-g mode*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_LOW_G_INTR_MODE__REG, &v_data_u8,
			C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_LOW_G_INTR_MODE, v_low_g_mode_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_LOW_G_INTR_MODE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the tap duration in the register 0x2A bit form 0 to 2
 *
 *
 *	@param v_tap_durn_u8: The value of tap duration
 *    v_tap_durn_u8     |    result
 *  --------------------|----------------------------
 *     0x00             | TAP_DURN_50_MS
 *     0x01             | TAP_DURN_100_MS
 *     0x02             | TAP_DURN_150_MS
 *     0x03             | TAP_DURN_200_MS
 *     0x04             | TAP_DURN_250_MS
 *     0x05             | TAP_DURN_375_MS
 *     0x06             | TAP_DURN_500_MS
 *     0x07             | TAP_DURN_700_MS
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_tap_durn(u8 *v_tap_durn_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read the tap duration*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_TAP_DURN__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			*v_tap_durn_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_TAP_DURN);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the tap duration in the register 0x2A bit form 0 to 2
 *
 *
 *	@param v_tap_durn_u8: The value of tap duration
 *    v_tap_durn_u8     |    result
 *  --------------------|----------------------
 *     0x00             | TAP_DURN_50_MS
 *     0x01             | TAP_DURN_100_MS
 *     0x02             | TAP_DURN_150_MS
 *     0x03             | TAP_DURN_200_MS
 *     0x04             | TAP_DURN_250_MS
 *     0x05             | TAP_DURN_375_MS
 *     0x06             | TAP_DURN_500_MS
 *     0x07             | TAP_DURN_700_MS
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_tap_durn(u8 v_tap_durn_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* write the tap duration */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC(
			p_bma2x2->dev_addr,
			BMA2x2_TAP_DURN__REG, &v_data_u8,
			C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_TAP_DURN, v_tap_durn_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_TAP_DURN__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the tap shock form the register 0x2A bit 6
 *
 *
 *
 *	@param v_tap_shock_u8: The value of tap shock
 *    v_tap_shock_u8    |    result
 *  --------------------|----------------------
 *     0x00             | TAP_SHOCK_50_MS
 *     0x01             | TAP_SHOCK_75_MS
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_tap_shock(u8 *v_tap_shock_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read tap shock value */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_TAP_SHOCK_DURN__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_tap_shock_u8 = BMA2x2_GET_BITSLICE(v_data_u8,
			BMA2x2_TAP_SHOCK_DURN);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the tap shock form the register 0x2A bit 6
 *
 *
 *
 *	@param v_tap_shock_u8: The value of tap shock
 *    v_tap_shock_u8    |    result
 *  --------------------|----------------------
 *     0x00             | TAP_SHOCK_50_MS
 *     0x01             | TAP_SHOCK_75_MS
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_tap_shock(u8 v_tap_shock_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* write tap shock value*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_TAP_SHOCK_DURN__REG, &v_data_u8,
			C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_TAP_SHOCK_DURN, v_tap_shock_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_TAP_SHOCK_DURN__REG, &v_data_u8,
			C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the tap quiet in the register 0x2A bit 7
 *
 *
 *
 *  @param  v_tap_quiet_u8 : The value of tap quiet
 *    v_tap_quiet_u8    |    result
 *  --------------------|----------------------
 *     0x00             | TAP_QUIET_30_MS
 *     0x01             | TAP_QUIET_20_MS
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_tap_quiet(u8 *v_tap_quiet_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read the tap quiet value*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_TAP_QUIET_DURN__REG, &v_data_u8,
			C_BMA2x2_ONE_U8X);
			*v_tap_quiet_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_TAP_QUIET_DURN);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the tap quiet in the register 0x2A bit 7
 *
 *
 *
 *  @param  v_tap_quiet_u8 : The value of tap quiet
 *    v_tap_quiet_u8    |    result
 *  --------------------|----------------------
 *     0x00             | TAP_QUIET_30_MS
 *     0x01             | TAP_QUIET_20_MS
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_tap_quiet(u8 v_tap_quiet_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* write the tap quiet value*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_TAP_QUIET_DURN__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_TAP_QUIET_DURN, v_tap_quiet_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_TAP_QUIET_DURN__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the tap threshold in the register 0x2B bit from 0 to 4
 *
 *
 *
 *  @param v_tap_thres_u8 : The value of tap threshold
 *	@note Tap threshold of single and double tap corresponding to accel range
 *     range            |    Tap threshold
 *  --------------------|----------------------
 *     2g               | (v_tap_thres_u8 * 62.5)mg
 *     4g               | (v_tap_thres_u8 * 125)mg
 *     8g               | (v_tap_thres_u8 * 250)mg
 *     16g              | (v_tap_thres_u8 * 500)mg
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_tap_thres(u8 *v_tap_thres_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read the tap threshold*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_TAP_THRES__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_tap_thres_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_TAP_THRES);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the tap threshold in the register 0x2B bit from 0 to 4
 *
 *
 *
 *  @param v_tap_thres_u8 : The value of tap threshold
 *	@note Tap threshold of single and double tap corresponding to accel range
 *     range            |    Tap threshold
 *  --------------------|----------------------
 *     2g               | (v_tap_thres_u8 * 62.5)mg
 *     4g               | (v_tap_thres_u8 * 125)mg
 *     8g               | (v_tap_thres_u8 * 250)mg
 *     16g              | (v_tap_thres_u8 * 500)mg
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_tap_thres(u8 v_tap_thres_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_TAP_THRES__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_TAP_THRES, v_tap_thres_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_TAP_THRES__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the tap sample in the register 0x2B bit 6 and 7
 *
 *
 *
 *  @param   *v_tap_sample_u8 : The value of tap sample
 *     v_tap_sample_u8  |    result
 *  --------------------|----------------------
 *     0x00             | 2 samples
 *     0x01             | 4 samples
 *     0x02             | 8 samples
 *     0x03             | 16 samples
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_tap_sample(u8 *v_tap_sample_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read tap samples */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_TAP_SAMPLES__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			*v_tap_sample_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_TAP_SAMPLES);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the tap sample in the register 0x2B bit 6 and 7
 *
 *
 *
 *  @param   *v_tap_sample_u8 : The value of tap sample
 *     v_tap_sample_u8  |    result
 *  --------------------|----------------------
 *     0x00             | 2 samples
 *     0x01             | 4 samples
 *     0x02             | 8 samples
 *     0x03             | 16 samples
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_tap_sample(u8 v_tap_sample_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* write tap samples */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_TAP_SAMPLES__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_TAP_SAMPLES, v_tap_sample_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_TAP_SAMPLES__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the orient mode in the register 0x2C bit 0 and 1
 *
 *
 *
 *  @param v_orient_mode_u8 : The value of orient mode
 *     v_orient_mode_u8 |    result
 *  --------------------|------------------
 *     0x00             | symmetrical
 *     0x01             | high asymmetrical
 *     0x02             | low asymmetrical
 *     0x03             | symmetrical
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_orient_mode(u8 *v_orient_mode_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC(
			p_bma2x2->dev_addr,
			BMA2x2_ORIENT_MODE__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			*v_orient_mode_u8 = BMA2x2_GET_BITSLICE(
			v_data_u8, BMA2x2_ORIENT_MODE);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the orient mode in the register 0x2C bit 0 and 1
 *
 *
 *
 *  @param v_orient_mode_u8 : The value of orient mode
 *     v_orient_mode_u8 |    result
 *  --------------------|------------------
 *     0x00             | symmetrical
 *     0x01             | high asymmetrical
 *     0x02             | low asymmetrical
 *     0x03             | symmetrical
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_orient_mode(u8 v_orient_mode_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ORIENT_MODE__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_ORIENT_MODE, v_orient_mode_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ORIENT_MODE__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the orient block in the register 0x2C bit 2 and 3
 *
 *
 *
 *	@param v_orient_block_u8 : The value of orient block
 *     v_orient_mode_u8 |    result
 *  --------------------|------------------
 *     0x00             | no blocking
 *     0x01             | theta blocking or
 *                      | acceleration slope in any axis > 1.5g
 *     0x02             | theta blocking or
 *                      | acceleration slope in any axis > 0.2g
 *                      | acceleration in any axis > 1.5g
 *     0x03             | theta blocking or
 *                      | acceleration slope in any axis > 0.4g
 *                      | acceleration in any axis > 1.5g
 *                      | value of orient is not stable for at lease 100ms
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_orient_block(
u8 *v_orient_block_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* Read the orient block data */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ORIENT_BLOCK__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			*v_orient_block_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ORIENT_BLOCK);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the orient block in the register 0x2C bit 2 and 3
 *
 *
 *
 *	@param v_orient_block_u8 : The value of orient block
 *     v_orient_mode_u8 |    result
 *  --------------------|------------------
 *     0x00             | no blocking
 *     0x01             | theta blocking or
 *                      | acceleration slope in any axis > 1.5g
 *     0x02             | theta blocking or
 *                      | acceleration slope in any axis > 0.2g
 *                      | acceleration in any axis > 1.5g
 *     0x03             | theta blocking or
 *                      | acceleration slope in any axis > 0.4g
 *                      | acceleration in any axis > 1.5g
 *                      | value of orient is not stable for at lease 100ms
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_orient_block(u8 v_orient_block_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* write the orient block data */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ORIENT_BLOCK__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_ORIENT_BLOCK, v_orient_block_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ORIENT_BLOCK__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the orient hysteresis in the register 0x2C bit 4 to 6
 *
 *
 *
 *  @param v_orient_hyst_u8 : The value of orient hysteresis
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_orient_hyst(u8 *v_orient_hyst_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read the orient hysteresis data*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ORIENT_HYST__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			*v_orient_hyst_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ORIENT_HYST);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the orient hysteresis in the register 0x2C bit 4 to 6
 *
 *
 *
 *  @param v_orient_hyst_u8 : The value of orient hysteresis
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_orient_hyst(u8 v_orient_hyst_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* write the orient hysteresis data */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ORIENT_HYST__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_ORIENT_HYST, v_orient_hyst_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC(
			p_bma2x2->dev_addr,
			BMA2x2_ORIENT_HYST__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief  This API is used to get
 *	the theta value of orient and flat interrupts
 *	@note ORIENT_THETA -> register 0x2D bit 0 to 5
 *	@note FLAT_THETA   -> register 0x2E bit 0 to 5
 *
 *  @param v_channel_u8: The value of theta selection
 *     v_channel_u8     |    result
 *  --------------------|------------------
 *     0x00             | BMA2x2_ACCEL_ORIENT_THETA
 *     0x01             | BMA2x2_ACCEL_FLAT_THETA
 * @note
 * @note FLAT_THETA : Defines a blocking angle between 0 deg to 44.8 deg
 * @note ORIENT_THETA : Defines threshold for detection of flat position
 *                in range from 0 deg to 44.8 deg
 *
 *  @param v_theta_u8: The value of theta
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_theta(u8 v_channel_u8,
u8 *v_theta_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* write theta value*/
		case BMA2x2_ORIENT_THETA:
			/*ORIENT THETA*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_THETA_BLOCK__REG, &v_data_u8,
			C_BMA2x2_ONE_U8X);
			*v_theta_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_THETA_BLOCK);
		break;
		case BMA2x2_FLAT_THETA:
			/*FLAT THETA*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_THETA_FLAT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_theta_u8 = v_data_u8;
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief  This API is used to set
 *	the theta value of orient and flat interrupts
 *	@note ORIENT_THETA -> register 0x2D bit 0 to 5
 *	@note FLAT_THETA   -> register 0x2E bit 0 to 5
 *
 *  @param v_channel_u8: The value of theta selection
 *     v_channel_u8     |    result
 *  --------------------|------------------
 *     0x00             | BMA2x2_ACCEL_ORIENT_THETA
 *     0x01             | BMA2x2_ACCEL_FLAT_THETA
 * @note
 * @note FLAT_THETA : Defines a blocking angle between 0 deg to 44.8 deg
 * @note ORIENT_THETA : Defines threshold for detection of flat position
 *                in range from 0 deg to 44.8 deg
 *
 *  @param v_theta_u8: The value of theta
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_theta(u8 v_channel_u8,
u8 v_theta_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		/* write flat value */
		case BMA2x2_ORIENT_THETA:
			/*ORIENT THETA*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_THETA_BLOCK__REG, &v_data_u8,
			C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_THETA_BLOCK, v_theta_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_THETA_BLOCK__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_FLAT_THETA:
			/*FLAT THETA*/
			v_data_u8 = v_theta_u8;
			com_rslt = p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_THETA_FLAT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *  the interrupt enable of orient ud_enable in the register 0x2D bit 6
 *
 *
 *  @param v_orient_enable_u8 : The value of orient ud_enable
 *     v_orient_enable_u8     |    result
 *  ------------------------- |------------------
 *     0x00                   | Generates Interrupt
 *     0x01                   | Do not generate interrupt
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_orient_enable(u8 *v_orient_enable_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ORIENT_UD_ENABLE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_orient_enable_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ORIENT_UD_ENABLE);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *  the interrupt enable of orient ud_enable in the register 0x2D bit 6
 *
 *
 *  @param v_orient_enable_u8 : The value of orient ud_enable
 *     v_orient_enable_u8     |    result
 *  ------------------------- |------------------
 *     0x00                   | Generates Interrupt
 *     0x01                   | Do not generate interrupt
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_orient_enable(u8 v_orient_enable_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ORIENT_UD_ENABLE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_ORIENT_UD_ENABLE, v_orient_enable_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ORIENT_UD_ENABLE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the interrupt enable of flat hysteresis("flat_hy)
 *	in the register 0x2F bit 0 to 2
 *
 *
 *
 *
 *  @param v_flat_hyst_u8 : The value of flat hysteresis
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_flat_hyst(u8 *v_flat_hyst_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_FLAT_HYST__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			*v_flat_hyst_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_FLAT_HYST);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the interrupt enable of flat hysteresis("flat_hy)
 *	in the register 0x2F bit 0 to 2
 *
 *
 *
 *
 *  @param v_flat_hyst_u8 : The value of flat hysteresis
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_flat_hyst(u8 v_flat_hyst_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC(
			p_bma2x2->dev_addr,
			BMA2x2_FLAT_HYST__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_FLAT_HYST, v_flat_hyst_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_FLAT_HYST__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *  the interrupt enable of flat hold time(flat_hold_time)
 *	in the register 0x2F bit 4 and 5
 *
 *
 *  @param  v_flat_hold_time_u8 : The value of flat hold time
 *     v_flat_hold_time_u8    |    result
 *  ------------------------- |------------------
 *     0x00                   | 0ms
 *     0x01                   | 512ms
 *     0x02                   | 1024ms
 *     0x03                   | 2048ms
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_flat_hold_time(
u8 *v_flat_hold_time_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read the flat hold time */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_FLAT_HOLD_TIME__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_flat_hold_time_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_FLAT_HOLD_TIME);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *  the interrupt enable of flat hold time(flat_hold_time)
 *	in the register 0x2F bit 4 and 5
 *
 *
 *  @param  v_flat_hold_time_u8 : The value of flat hold time
 *     v_flat_hold_time_u8    |    result
 *  ------------------------- |------------------
 *     0x00                   | 0ms
 *     0x01                   | 512ms
 *     0x02                   | 1024ms
 *     0x03                   | 2048ms
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_flat_hold_time(
u8 v_flat_hold_time_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* write the flat hold time */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_FLAT_HOLD_TIME__REG, &v_data_u8,
			C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_FLAT_HOLD_TIME, v_flat_hold_time_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_FLAT_HOLD_TIME__REG, &v_data_u8,
			C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the fifo water mark level trigger in the register 0x30 bit from 0 to 5
 *
 *
 *
 *
 *  @param fifo_wml_trig: The value of fifo watermark trigger level
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_fifo_wml_trig(
u8 *fifo_wml_trig)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read the fifo water mark trigger */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_FIFO_WML_TRIG_RETAIN__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*fifo_wml_trig = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_FIFO_WML_TRIG_RETAIN);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the fifo water mark level trigger in the register 0x30 bit from 0 to 5
 *
 *
 *
 *
 *  @param fifo_wml_trig: The value of fifo watermark trigger level
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_fifo_wml_trig(
u8 fifo_wml_trig)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		if (fifo_wml_trig < C_BMA2x2_THIRTYTWO_U8X) {
			/* write the fifo watermark trigger*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_FIFO_WML_TRIG_RETAIN__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_FIFO_WML_TRIG_RETAIN,
			fifo_wml_trig);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_FIFO_WML_TRIG_RETAIN__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		} else {
		com_rslt = E_OUT_OF_RANGE;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is for to get
 *	the self test axis(self_test_axis) in the register ox32 bit 0 to 2
 *
 *
 *
 *  @param v_selftest_axis_u8 : The value of selftest axis
 *     v_selftest_axis_u8     |    result
 *  ------------------------- |------------------
 *     0x00                   | self test disable
 *     0x01                   | x-axis
 *     0x02                   | y-axis
 *     0x03                   | z-axis
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_selftest_axis(
u8 *v_selftest_axis_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read the self test axis*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SELFTEST__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_selftest_axis_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_SELFTEST);
		}
	return com_rslt;
}
/*!
 *	@brief This API is for to set
 *	the self test axis(self_test_axis) in the register ox32 bit 0 to 2
 *
 *
 *
 *  @param v_selftest_axis_u8 : The value of selftest axis
 *     v_selftest_axis_u8     |    result
 *  ------------------------- |------------------
 *     0x00                   | self test disable
 *     0x01                   | x-axis
 *     0x02                   | y-axis
 *     0x03                   | z-axis
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_selftest_axis(
u8 v_selftest_axis_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		if (v_selftest_axis_u8 < C_BMA2x2_FOUR_U8X) {
			/* write the self test axis*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SELFTEST__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_SELFTEST, v_selftest_axis_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SELFTEST__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		 } else {
		com_rslt = E_OUT_OF_RANGE;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is for to get
 *	the Self Test sign(selftest_sign) in the register 0x32 bit 2
 *
 *
 *
 *  @param v_selftest_sign_u8 : The value of self test sign
 *     v_selftest_sign_u8     |    result
 *  ------------------------- |------------------
 *     0x00                   | negative sign
 *     0x01                   | positive sign
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_selftest_sign(
u8 *v_selftest_sign_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read self test sign */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_NEG_SELFTEST__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_selftest_sign_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_NEG_SELFTEST);
		}
	return com_rslt;
}
/*!
 *	@brief This API is for to set
 *	the Self Test sign(selftest_sign) in the register 0x32 bit 2
 *
 *
 *
 *  @param v_selftest_sign_u8 : The value of self test sign
 *     v_selftest_sign_u8     |    result
 *  ------------------------- |------------------
 *     0x00                   | negative sign
 *     0x01                   | positive sign
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_selftest_sign(
u8 v_selftest_sign_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		if (v_selftest_sign_u8 < C_BMA2x2_TWO_U8X) {
			/* write self test sign */
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_NEG_SELFTEST__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_NEG_SELFTEST, v_selftest_sign_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_NEG_SELFTEST__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		} else {
		com_rslt = E_OUT_OF_RANGE;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get
 * the nvm program mode(nvm_prog_mode)in the register 0x33 bit 0
 *
 *
 *  @param  v_nvmprog_mode_u8 : The value of nvm program mode
 *     v_nvmprog_mode_u8      |    result
 *  ------------------------- |------------------
 *     0x00                   | Disable program mode
 *     0x01                   | Enable program mode
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_nvmprog_mode(
u8 *v_nvmprog_mode_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		return  E_BMA2x2_NULL_PTR;
	} else {
		/* read the nvm program mode*/
		com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
		(p_bma2x2->dev_addr,
		BMA2x2_UNLOCK_EE_PROG_MODE__REG,
		&v_data_u8, C_BMA2x2_ONE_U8X);
		*v_nvmprog_mode_u8 = BMA2x2_GET_BITSLICE
		(v_data_u8, BMA2x2_UNLOCK_EE_PROG_MODE);
	}
	return com_rslt;
}
/*!
 * @brief This API is used to set
 * the nvm program mode(nvm_prog_mode)in the register 0x33 bit 0
 *
 *
 *  @param  v_nvmprog_mode_u8 : The value of nvm program mode
 *     v_nvmprog_mode_u8      |    result
 *  ------------------------- |------------------
 *     0x00                   | Disable program mode
 *     0x01                   | Enable program mode
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_nvmprog_mode(u8 v_nvmprog_mode_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
	} else {
		/* write the nvm program mode*/
		com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
		(p_bma2x2->dev_addr,
		BMA2x2_UNLOCK_EE_PROG_MODE__REG,
		&v_data_u8, C_BMA2x2_ONE_U8X);
		v_data_u8 = BMA2x2_SET_BITSLICE
		(v_data_u8, BMA2x2_UNLOCK_EE_PROG_MODE, v_nvmprog_mode_u8);
		com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
		(p_bma2x2->dev_addr,
		BMA2x2_UNLOCK_EE_PROG_MODE__REG,
		&v_data_u8, C_BMA2x2_ONE_U8X);
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the value of nvm program trig in the register 0x33 bit 1
 *
 *
 *
 *
 *  @param v_nvprog_trig_u8: The value of nvm program trig
 *     v_nvprog_trig_u8       |    result
 *  ------------------------- |------------------
 *     0x00                   | Do not trigger nvm program
 *     0x01                   | Trigger nvm program
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_nvprog_trig(u8 v_nvprog_trig_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
	} else {
		/* set the nvm program trigger */
		com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
		(p_bma2x2->dev_addr,
		BMA2x2_START_EE_PROG_TRIG__REG,
		&v_data_u8, C_BMA2x2_ONE_U8X);
		v_data_u8 = BMA2x2_SET_BITSLICE
		(v_data_u8, BMA2x2_START_EE_PROG_TRIG, v_nvprog_trig_u8);
		com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
		(p_bma2x2->dev_addr,
		BMA2x2_START_EE_PROG_TRIG__REG,
		&v_data_u8, C_BMA2x2_ONE_U8X);
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get
 * the nvm program ready in the register bit 2
 *
 *
 *  @param v_nvprog_ready_u8: The value of nvm program ready
 *     v_nvprog_ready_u8      |    result
 *  ------------------------- |------------------
 *     0x00                   | nvm write/update operation is in progress
 *     0x01                   | nvm is ready to accept a new write
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_nvmprog_ready(u8 *v_nvprog_ready_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
	} else {
		/* read the nvm program ready*/
		com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
		(p_bma2x2->dev_addr,
		BMA2x2_EE_PROG_READY__REG,
		&v_data_u8, C_BMA2x2_ONE_U8X);
		*v_nvprog_ready_u8 = BMA2x2_GET_BITSLICE
		(v_data_u8, BMA2x2_EE_PROG_READY);
	}
	return com_rslt;
}
/*!
 * @brief This API is used to set
 * the nvm program ready in the register bit 2
 *
 *
 *  @param v_nvprog_remain_u8: The value of nvm program ready
 *     v_nvprog_remain_u8     |    result
 *  ------------------------- |------------------
 *     0x00                   | nvm write/update operation is in progress
 *     0x01                   | nvm is ready to accept a new write
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_nvmprog_remain(u8 *v_nvprog_remain_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
	} else {
		/* write the nvm program ready*/
		com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
		(p_bma2x2->dev_addr,
		BMA2x2_EE_REMAIN__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		*v_nvprog_remain_u8 = BMA2x2_GET_BITSLICE
		(v_data_u8, BMA2x2_EE_REMAIN);
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get the enable status of spi3
 *	in the register 0x34 bit 0
 *
 *
 *
 *  @param  v_spi3_u8 : The value of SPI 3 or 4 wire enable
 *     v_spi3_u8              |    result
 *  ------------------------- |------------------
 *     0x00                   |     spi3
 *     0x01                   |     spi4
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_spi3(u8 *v_spi3_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* read the spi status*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SPI_MODE_3__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_spi3_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_SPI_MODE_3);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set the enable status of spi3
 *	in the register 0x34 bit 0
 *
 *
 *
 *  @param  v_spi3_u8 : The value of SPI 3 or 4 wire enable
 *     v_spi3_u8              |    result
 *  ------------------------- |------------------
 *     0x00                   |     spi3
 *     0x01                   |     spi4
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_spi3(u8 v_spi3_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/* write the spi status*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SPI_MODE_3__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_SPI_MODE_3, v_spi3_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SPI_MODE_3__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get the i2c
 *	watch dog timer period and I2C interface mode is selected
 *	in the register 0x34 bit 1 and 2
 *
 *
 *  @param v_channel_u8: The i2c option selection
 *     v_channel_u8           |    result
 *  ------------------------- |------------------
 *        0                   |   BMA2x2_ACCEL_I2C_SELECT
 *        1                   |   BMA2x2_ACCEL_I2C_ENABLE
 *
 *  @param v_i2c_wdt_u8: watch dog timer period
 *	and I2C interface mode is selected
 *     BMA2x2_ACCEL_I2C_SELECT|    result
 *  ------------------------- |------------------
 *     0x00                   | Disable the watchdog at SDI pin
 *     0x01                   | Enable watchdog
 *
 *     BMA2x2_I2C_ENABLE      |    result
 *  ------------------------- |------------------
 *     0x00                   | 1ms
 *     0x01                   | 50ms
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_i2c_wdt(u8 v_channel_u8,
u8 *v_i2c_wdt_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		case BMA2x2_I2C_SELECT:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_I2C_WDT_PERIOD__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_i2c_wdt_u8 = BMA2x2_GET_BITSLICE(v_data_u8,
			BMA2x2_I2C_WDT_PERIOD);
		break;
		case BMA2x2_I2C_ENABLE:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_I2C_WDT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_i2c_wdt_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_I2C_WDT);
		break;
		default:
		com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to set the i2c
 *	watch dog timer period and I2C interface mode is selected
 *	in the register 0x34 bit 1 and 2
 *
 *
 *  @param v_channel_u8: The i2c option selection
 *     v_channel_u8           |    result
 *  ------------------------- |------------------
 *        0                   |   BMA2x2_ACCEL_I2C_SELECT
 *        1                   |   BMA2x2_ACCEL_I2C_ENABLE
 *
 *  @param v_i2c_wdt_u8: watch dog timer period
 *	and I2C interface mode is selected
 *     BMA2x2_ACCEL_I2C_SELECT|    result
 *  ------------------------- |------------------
 *     0x00                   | Disable the watchdog at SDI pin
 *     0x01                   | Enable watchdog
 *
 *     BMA2x2_I2C_ENABLE      |    result
 *  ------------------------- |------------------
 *     0x00                   | 1ms
 *     0x01                   | 50ms
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_i2c_wdt(u8 v_channel_u8,
u8 v_i2c_wdt_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		case BMA2x2_I2C_SELECT:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_I2C_WDT_PERIOD__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_I2C_WDT_PERIOD, v_i2c_wdt_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_I2C_WDT_PERIOD__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_I2C_ENABLE:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_I2C_WDT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_I2C_WDT, v_i2c_wdt_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_I2C_WDT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	slow compensation(hp_x_enable, hp_y_enable and hp_z_enable) enable
 *	in the register 0x36 bit 0 to 2
 *	@note SLOW_COMP_X -> bit 0
 *	@note SLOW_COMP_Y -> bit 1
 *	@note SLOW_COMP_Z -> bit 2
 *
 *
 *	@param v_channel_u8: The value of slow compensation selection
 *     v_channel_u8           |    result
 *  ------------------------- |------------------
 *        0                   |   BMA2x2_ACCEL_SLOW_COMP_X
 *        1                   |   BMA2x2_ACCEL_SLOW_COMP_Y
 *        2                   |   BMA2x2_ACCEL_SLOW_COMP_Z
 *
 *  @param v_slow_comp_u8: The value of slow compensation enable
 *     v_slow_comp_u8         |    result
 *  ------------------------- |------------------
 *         0x00               |    Disable
 *        0x01                |    Enable
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_slow_comp(u8 v_channel_u8,
u8 *v_slow_comp_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		case BMA2x2_SLOW_COMP_X:
			/*SLOW COMP X*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SLOW_COMP_X__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_slow_comp_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_SLOW_COMP_X);
		break;
		case BMA2x2_SLOW_COMP_Y:
			/*SLOW COMP Y*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SLOW_COMP_Y__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_slow_comp_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_SLOW_COMP_Y);
		break;
		case BMA2x2_SLOW_COMP_Z:
			/*SLOW COMP Z*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SLOW_COMP_Z__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_slow_comp_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_ENABLE_SLOW_COMP_Z);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	slow compensation(hp_x_enable, hp_y_enable and hp_z_enable) enable
 *	in the register 0x36 bit 0 to 2
 *	@note SLOW_COMP_X -> bit 0
 *	@note SLOW_COMP_Y -> bit 1
 *	@note SLOW_COMP_Z -> bit 2
 *
 *
 *	@param v_channel_u8: The value of slow compensation selection
 *     v_channel_u8           |    result
 *  ------------------------- |------------------
 *        0                   |   BMA2x2_ACCEL_SLOW_COMP_X
 *        1                   |   BMA2x2_ACCEL_SLOW_COMP_Y
 *        2                   |   BMA2x2_ACCEL_SLOW_COMP_Z
 *
 *  @param v_slow_comp_u8: The value of slow compensation enable
 *     v_slow_comp_u8         |    result
 *  ------------------------- |------------------
 *         0x00               |    Disable
 *        0x01                |    Enable
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_slow_comp(u8 v_channel_u8,
u8 v_slow_comp_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		case BMA2x2_SLOW_COMP_X:
			/*SLOW COMP X*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SLOW_COMP_X__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_SLOW_COMP_X, v_slow_comp_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SLOW_COMP_X__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_SLOW_COMP_Y:
			/*SLOW COMP Y*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SLOW_COMP_Y__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_SLOW_COMP_Y, v_slow_comp_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SLOW_COMP_Y__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_SLOW_COMP_Z:
			/*SLOW COMP Z*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SLOW_COMP_Z__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_ENABLE_SLOW_COMP_Z, v_slow_comp_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_ENABLE_SLOW_COMP_Z__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the status of fast offset compensation(cal_rdy) in the register 0x36
 *	bit 4(Read Only Possible)
 *
 *
 *
 *  @param  v_cal_rdy_u8: The value of cal_ready
 *
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_cal_rdy(u8 *v_cal_rdy_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
		(p_bma2x2->dev_addr,
		BMA2x2_FAST_CAL_RDY_STAT__REG,
		&v_data_u8, C_BMA2x2_ONE_U8X);
		*v_cal_rdy_u8 = BMA2x2_GET_BITSLICE(v_data_u8,
		BMA2x2_FAST_CAL_RDY_STAT);
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the status of fast offset compensation(cal_rdy) in the register 0x36
 *	bit 4(Read Only Possible)
 *
 *
 *
 *  @param  v_cal_trigger_u8: The value of cal_ready
 *
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_cal_trigger(u8 v_cal_trigger_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_CAL_TRIGGER__REG, &v_data_u8,
			C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_CAL_TRIGGER, v_cal_trigger_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_CAL_TRIGGER__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the offset reset(offset_reset) in the register 0x36
 *	bit 7(Write only possible)
 *
 *
 *
 *  @param  v_offset_rst_u8: The offset reset value
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_offset_rst(u8 v_offset_rst_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_RST_OFFSET__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_RST_OFFSET,
			v_offset_rst_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_RST_OFFSET__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the status of offset target axis(offset_target_x, offset_target_y and
 *	offset_target_z) and cut_off in the register 0x37
 *	@note CUT_OFF -> bit 0
 *	@note OFFSET_TRIGGER_X -> bit 1 and 2
 *	@note OFFSET_TRIGGER_Y -> bit 3 and 4
 *	@note OFFSET_TRIGGER_Z -> bit 5 and 6
 *
 *
 *  @param v_channel_u8: The value of offset axis selection
 *     v_channel_u8           |    result
 *  ------------------------- |------------------
 *        0                   |   BMA2x2_ACCEL_CUT_OFF
 *        1                   |   BMA2x2_ACCEL_OFFSET_TRIGGER_X
 *        2                   |   BMA2x2_ACCEL_OFFSET_TRIGGER_Y
 *        2                   |   BMA2x2_ACCEL_OFFSET_TRIGGER_Z
 *
 *  @param  v_offset_u8: The offset target value
 *     CUT_OFF                |    result
 *  ------------------------- |------------------
 *        0                   |   1Hz
 *        1                   |   10Hz
 *
 *
 *     OFFSET_TRIGGER         |    result
 *  ------------------------- |------------------
 *        0x00                |   0g
 *        0x01                |   +1g
 *        0x02                |   -1g
 *        0x03                |   0g
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_offset_target(u8 v_channel_u8,
u8 *v_offset_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		case BMA2x2_CUT_OFF:
			/*CUT-OFF*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_COMP_CUTOFF__REG, &v_data_u8,
			C_BMA2x2_ONE_U8X);
			*v_offset_u8 = BMA2x2_GET_BITSLICE(v_data_u8,
			BMA2x2_COMP_CUTOFF);
		break;
		case BMA2x2_OFFSET_TRIGGER_X:
			/*OFFSET TRIGGER X*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_COMP_TARGET_OFFSET_X__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_offset_u8 = BMA2x2_GET_BITSLICE(v_data_u8,
			BMA2x2_COMP_TARGET_OFFSET_X);
		break;
		case BMA2x2_OFFSET_TRIGGER_Y:
			/*OFFSET TRIGGER Y*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_COMP_TARGET_OFFSET_Y__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_offset_u8 = BMA2x2_GET_BITSLICE(v_data_u8,
			BMA2x2_COMP_TARGET_OFFSET_Y);
		break;
		case BMA2x2_OFFSET_TRIGGER_Z:
			/*OFFSET TRIGGER Z*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_COMP_TARGET_OFFSET_Z__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_offset_u8 = BMA2x2_GET_BITSLICE
			(v_data_u8, BMA2x2_COMP_TARGET_OFFSET_Z);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the status of offset target axis(offset_target_x, offset_target_y and
 *	offset_target_z) and cut_off in the register 0x37
 *	@note CUT_OFF -> bit 0
 *	@note OFFSET_TRIGGER_X -> bit 1 and 2
 *	@note OFFSET_TRIGGER_Y -> bit 3 and 4
 *	@note OFFSET_TRIGGER_Z -> bit 5 and 6
 *
 *
 *  @param v_channel_u8: The value of offset axis selection
 *     v_channel_u8           |    result
 *  ------------------------- |------------------
 *        0                   |   BMA2x2_ACCEL_CUT_OFF
 *        1                   |   BMA2x2_ACCEL_OFFSET_TRIGGER_X
 *        2                   |   BMA2x2_ACCEL_OFFSET_TRIGGER_Y
 *        2                   |   BMA2x2_ACCEL_OFFSET_TRIGGER_Z
 *
 *  @param  v_offset_u8: The offset target value
 *     CUT_OFF                |    result
 *  ------------------------- |------------------
 *        0                   |   1Hz
 *        1                   |   10Hz
 *
 *
 *     OFFSET_TRIGGER         |    result
 *  ------------------------- |------------------
 *        0x00                |   0g
 *        0x01                |   +1g
 *        0x02                |   -1g
 *        0x03                |   0g
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_offset_target(u8 v_channel_u8,
u8 v_offset_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		case BMA2x2_CUT_OFF:
			/*CUT-OFF*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_COMP_CUTOFF__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_COMP_CUTOFF, v_offset_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_COMP_CUTOFF__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_OFFSET_TRIGGER_X:
			/*OFFSET TARGET X*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_COMP_TARGET_OFFSET_X__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_COMP_TARGET_OFFSET_X, v_offset_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_COMP_TARGET_OFFSET_X__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_OFFSET_TRIGGER_Y:
			/*OFFSET TARGET Y*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_COMP_TARGET_OFFSET_Y__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_COMP_TARGET_OFFSET_Y, v_offset_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_COMP_TARGET_OFFSET_Y__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_OFFSET_TRIGGER_Z:
			/*OFFSET TARGET Z*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_COMP_TARGET_OFFSET_Z__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8, BMA2x2_COMP_TARGET_OFFSET_Z, v_offset_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_COMP_TARGET_OFFSET_Z__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get the status of offset
 *	(offset_x, offset_y and offset_z) in the registers 0x38,0x39 and 0x3A
 *	@note offset_x -> register 0x38 bit 0 to 7
 *	@note offset_y -> register 0x39 bit 0 to 7
 *	@note offset_z -> register 0x3A bit 0 to 7
 *
 *
 *  @param v_channel_u8: The value of offset selection
 *     v_channel_u8           |    result
 *  ------------------------- |------------------
 *        0                   |   BMA2x2_ACCEL_X_AXIS
 *        1                   |   BMA2x2_ACCEL_Y_AXIS
 *        2                   |   BMA2x2_ACCEL_Z_AXIS
 *
 *  @param v_offset_u8: The value of offset
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_offset(u8 v_channel_u8,
s8 *v_offset_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		case BMA2x2_X_AXIS:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_OFFSET_X_AXIS_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			*v_offset_u8 = (s8)v_data_u8;
		break;
		case BMA2x2_Y_AXIS:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_OFFSET_Y_AXIS_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			*v_offset_u8 = (s8)v_data_u8;
		break;
		case BMA2x2_Z_AXIS:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_OFFSET_Z_AXIS_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			*v_offset_u8 = (s8)v_data_u8;
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to set the status of offset
 *	(offset_x, offset_y and offset_z) in the registers 0x38,0x39 and 0x3A
 *	@note offset_x -> register 0x38 bit 0 to 7
 *	@note offset_y -> register 0x39 bit 0 to 7
 *	@note offset_z -> register 0x3A bit 0 to 7
 *
 *
 *  @param v_channel_u8: The value of offset selection
 *     v_channel_u8           |    result
 *  ------------------------- |------------------
 *        0                   |   BMA2x2_ACCEL_X_AXIS
 *        1                   |   BMA2x2_ACCEL_Y_AXIS
 *        2                   |   BMA2x2_ACCEL_Z_AXIS
 *
 *  @param v_offset_u8: The value of offset
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_offset(u8 v_channel_u8,
s8 v_offset_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (v_channel_u8) {
		case BMA2x2_X_AXIS:
			v_data_u8 = v_offset_u8;
			com_rslt = p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_OFFSET_X_AXIS_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_Y_AXIS:
			v_data_u8 = v_offset_u8;
			com_rslt = p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_OFFSET_Y_AXIS_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		case BMA2x2_Z_AXIS:
			v_data_u8 = v_offset_u8;
			com_rslt = p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_OFFSET_Z_AXIS_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
		break;
		default:
			com_rslt = E_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the status of fifo (fifo_mode) in the register 0x3E bit 6 and 7
 *
 *
 *  @param v_fifo_mode_u8 : The value of fifo mode
 *     v_fifo_mode_u8         |    result
 *  ------------------------- |------------------
 *        0x00                |   BYPASS
 *        0x01                |   FIFO
 *        0x02                |   STREAM
 *        0x03                |   RESERVED
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_fifo_mode(u8 *v_fifo_mode_u8)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC(
			p_bma2x2->dev_addr,
			BMA2x2_FIFO_MODE__REG, &v_data_u8,
			C_BMA2x2_ONE_U8X);
			*v_fifo_mode_u8 = BMA2x2_GET_BITSLICE(v_data_u8,
			BMA2x2_FIFO_MODE);
		}
	return com_rslt;
}
/*!
 *	@brief This API is used to set
 *	the status of fifo (fifo_mode) in the register 0x3E bit 6 and 7
 *
 *
 *  @param v_fifo_mode_u8 : The value of fifo mode
 *     v_fifo_mode_u8         |    result
 *  ------------------------- |------------------
 *        0x00                |   BYPASS
 *        0x01                |   FIFO
 *        0x02                |   STREAM
 *        0x03                |   RESERVED
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_fifo_mode(u8 v_fifo_mode_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_config_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		if (v_fifo_mode_u8 < C_BMA2x2_FOUR_U8X) {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_FIFO_MODE__REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE(v_data_u8,
			BMA2x2_FIFO_MODE, v_fifo_mode_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_FIFO_MODE__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			if (com_rslt == BMA2x2_SUCCESS) {
				com_rslt += bma2x2_read_reg(
				BMA2x2_FIFO_MODE_REG,
				&v_config_data_u8, C_BMA2x2_ONE_U8X);
				p_bma2x2->fifo_config = v_config_data_u8;
			}
		} else {
		com_rslt = E_OUT_OF_RANGE;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API is used to get
 * the axis enable of fifo data select in the register 0x3E bit 0 and 1
 *
 *
 *  @param v_fifo_data_select_u8 : The value of FIFO axis data select
 *   v_fifo_data_select_u8    |    result
 *  ------------------------- |------------------
 *        0x00                |   XYZ
 *        0x01                |   Y
 *        0x02                |   X
 *        0x03                |   Z
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_fifo_data_select(
u8 *v_fifo_data_select_u8)
{
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC(
			p_bma2x2->dev_addr,
			BMA2x2_FIFO_DATA_SELECT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_fifo_data_select_u8 = BMA2x2_GET_BITSLICE(v_data_u8,
			BMA2x2_FIFO_DATA_SELECT);
		}
	return com_rslt;
}
/*!
 * @brief This API is used to set
 * the axis enable of fifo data select in the register 0x3E bit 0 and 1
 *
 *
 *  @param v_fifo_data_select_u8 : The value of FIFO axis data select
 *   v_fifo_data_select_u8    |    result
 *  ------------------------- |------------------
 *        0x00                |   XYZ
 *        0x01                |   Y
 *        0x02                |   X
 *        0x03                |   Z
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_set_fifo_data_select(
u8 v_fifo_data_select_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_config_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		if (v_fifo_data_select_u8 < C_BMA2x2_FOUR_U8X) {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_FIFO_DATA_SELECT__REG, &v_data_u8,
			C_BMA2x2_ONE_U8X);
			v_data_u8 = BMA2x2_SET_BITSLICE
			(v_data_u8,
			BMA2x2_FIFO_DATA_SELECT, v_fifo_data_select_u8);
			com_rslt += p_bma2x2->BMA2x2_BUS_WRITE_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_FIFO_DATA_SELECT__REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			if (com_rslt == BMA2x2_SUCCESS) {
				com_rslt += bma2x2_read_reg(
				BMA2x2_FIFO_MODE_REG,
				 &v_config_data_u8, C_BMA2x2_ONE_U8X);
				p_bma2x2->fifo_config = v_config_data_u8;
			}
		} else {
		com_rslt = E_OUT_OF_RANGE;
		}
	}
	return com_rslt;
}
/*!
 *	@brief This API is used to get
 *	the fifo data in the register 0x3F bit 0 to 7
 *
 *
 *  @param  v_output_reg_u8 : The value of fifo data
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_get_fifo_data_output_reg(
u8 *v_output_reg_u8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			/*GET FIFO DATA OUTPUT REGISTER*/
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC(
			p_bma2x2->dev_addr,
			BMA2x2_FIFO_DATA_OUTPUT_REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_output_reg_u8 = v_data_u8;
		}
	return com_rslt;
}
/*!
 * @brief This API is used to read the temp
 * from register 0x08
 *
 *
 *
 *  @param  v_temp_s8: The value of temperature
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_read_temp(s8 *v_temp_s8)
{
	u8 v_data_u8 = C_BMA2x2_ZERO_U8X;
		/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC(
			p_bma2x2->dev_addr,
			BMA2x2_TEMP_REG,
			&v_data_u8, C_BMA2x2_ONE_U8X);
			*v_temp_s8 = (s8)v_data_u8;
		}
	return com_rslt;
}
/*!
 * @brief This API reads accelerometer data X,Y,Z values and
 * temperature data from location 02h to 08h
 *
 *
 *
 *
 *  @param accel : The value of accel xyz and temperature data
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_read_accel_xyzt(
struct bma2x2_accel_data_temp *accel)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8 v_data_u8[ARRAY_SIZE_SEVEN] = {
	C_BMA2x2_ZERO_U8X, C_BMA2x2_ZERO_U8X,
	C_BMA2x2_ZERO_U8X, C_BMA2x2_ZERO_U8X,
	C_BMA2x2_ZERO_U8X, C_BMA2x2_ZERO_U8X,
	C_BMA2x2_ZERO_U8X};
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
		switch (V_BMA2x2RESOLUTION_U8) {
		case BMA2x2_12_RESOLUTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_ACCEL_X12_LSB__REG,
			v_data_u8, C_BMA2x2_SEVEN_U8X);

			/* read x v_data_u8*/
			accel->x = (s16)((((s32)((s8)v_data_u8[MSB_ONE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_ZERO] & BMA2x2_12_BIT_SHIFT));
			accel->x = accel->x >> C_BMA2x2_FOUR_U8X;

			/* read y v_data_u8*/
			accel->y = (s16)((((s32)((s8)v_data_u8[MSB_THREE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_TWO] & BMA2x2_12_BIT_SHIFT));
			accel->y = accel->y >> C_BMA2x2_FOUR_U8X;

			/* read z v_data_u8*/
			accel->z = (s16)((((s32)((s8)v_data_u8[MSB_FIVE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_FOUR] & BMA2x2_12_BIT_SHIFT));
			accel->z = accel->z >> C_BMA2x2_FOUR_U8X;

			accel->temp = (s8)v_data_u8[INDEX_SIX];
		break;
		case BMA2x2_10_RESOLUTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_ACCEL_X10_LSB__REG,
			v_data_u8, C_BMA2x2_SEVEN_U8X);

			/* read x v_data_u8*/
			accel->x = (s16)((((s32)((s8)v_data_u8[MSB_ONE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_ZERO] & BMA2x2_10_BIT_SHIFT));
			accel->x = accel->x >> C_BMA2x2_SIX_U8X;

			/* read y v_data_u8*/
			accel->y = (s16)((((s32)((s8)v_data_u8[MSB_THREE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_TWO] & BMA2x2_10_BIT_SHIFT));
			accel->y = accel->y >> C_BMA2x2_SIX_U8X;

			/* read z v_data_u8*/
			accel->z = (s16)((((s32)((s8)v_data_u8[MSB_FIVE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_FOUR] & BMA2x2_10_BIT_SHIFT));
			accel->z = accel->z >> C_BMA2x2_SIX_U8X;

			/* read v_temp_s8 v_data_u8*/
			accel->temp = (s8)v_data_u8[INDEX_SIX];
		break;
		case BMA2x2_14_RESOLUTION:
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr, BMA2x2_ACCEL_X14_LSB__REG,
			v_data_u8, C_BMA2x2_SEVEN_U8X);

			/* read x v_data_u8*/
			accel->x = (s16)((((s32)((s8)v_data_u8[MSB_ONE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_ZERO] & BMA2x2_14_BIT_SHIFT));
			accel->x = accel->x >> C_BMA2x2_TWO_U8X;

			/* read y v_data_u8*/
			accel->y = (s16)((((s32)((s8)v_data_u8[MSB_THREE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_TWO] & BMA2x2_14_BIT_SHIFT));
			accel->y = accel->y >> C_BMA2x2_TWO_U8X;

			/* read z v_data_u8*/
			accel->z = (s16)((((s32)((s8)v_data_u8[MSB_FIVE]))
			<< C_BMA2x2_EIGHT_U8X) |
			(v_data_u8[LSB_FOUR] & BMA2x2_14_BIT_SHIFT));
			accel->z = accel->z >> C_BMA2x2_TWO_U8X;

			/* read temp v_data_u8*/
			accel->temp = (s8)v_data_u8[INDEX_SIX];
		break;
		default:
		break;
		}
	}
	return com_rslt;
}
/*!
 * @brief This API reads accelerometer data X,Y,Z values and
 * temperature data from location 0x02 to 0x08
 *
 *
 *
 *
 *  @param accel : The value of accel xyz and temperature data
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BMA2x2_RETURN_FUNCTION_TYPE bma2x2_read_accel_eight_resolution_xyzt(
struct bma2x2_accel_eight_resolution_temp *accel)
{
	/*  Variable used to return value of
	communication routine*/
	BMA2x2_RETURN_FUNCTION_TYPE com_rslt = BMA2x2_ERROR;
	u8	v_data_u8 = C_BMA2x2_ZERO_U8X;
	if (p_bma2x2 == BMA2x2_NULL) {
		/* Check the struct p_bma2x2 is empty */
		return E_BMA2x2_NULL_PTR;
		} else {
			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_X_AXIS_MSB_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			accel->x = BMA2x2_GET_BITSLICE(v_data_u8,
			BMA2x2_ACCEL_X_MSB);

			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_Y_AXIS_MSB_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			accel->y = BMA2x2_GET_BITSLICE(v_data_u8,
			BMA2x2_ACCEL_Y_MSB);

			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC
			(p_bma2x2->dev_addr,
			BMA2x2_Z_AXIS_MSB_REG, &v_data_u8, C_BMA2x2_ONE_U8X);
			accel->z = BMA2x2_GET_BITSLICE(v_data_u8,
			BMA2x2_ACCEL_Z_MSB);

			com_rslt = p_bma2x2->BMA2x2_BUS_READ_FUNC(
			p_bma2x2->dev_addr,
			BMA2x2_TEMP_REG, &v_data_u8,
			C_BMA2x2_ONE_U8X);
			accel->temp = (s8)v_data_u8;
		}
	return com_rslt;
}
