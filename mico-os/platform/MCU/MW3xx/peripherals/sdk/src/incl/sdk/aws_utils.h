/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

/*! \file aws_utils.h
 *  \brief AWS configuration APIs
 *
 */

#ifndef _AWS_UTILS_H_
#define _AWS_UTILS_H_

#define AWS_PUB_CERT_SIZE 2046
#define AWS_PRIV_KEY_SIZE 2046
#define AWS_MAX_REGION_SIZE 126
#define AWS_MAX_THING_SIZE 126

/** Read the configured AWS Certificate
 *
 * This API reads the AWS certificate that is configured during provisioning.
 *
 * \param[out] cert Pointer to a buffer that should hold the certificate.
 * \param[in] cert_len The length of the buffer pointed to by cert above
 *
 * \return WM_SUCCESS on success
 * \return error code otherwise
 */
int read_aws_certificate(char *cert, unsigned cert_len);

/** Read the configured AWS Key
 *
 * This API reads the AWS key that is configured during provisioning.
 *
 * \param[out] key Pointer to a buffer that should hold the key.
 * \param[in] key_len The length of the buffer pointed to by key above
 *
 * \return WM_SUCCESS on success
 * \return error code otherwise
 */
int read_aws_key(char *key, unsigned key_len);

/** Read the configured AWS region
 *
 * This API reads the AWS region that is configured during provisioning.
 *
 * \param[out] region Pointer to a buffer that should hold the region.
 * \param[in] region_len The length of the buffer pointed to by region above
 *
 * \return WM_SUCCESS on success
 * \return error code otherwise
 */
int read_aws_region(char *region, unsigned region_len);

/** Read the configured AWS thing name
 *
 * This API reads the AWS thing name that is configured during provisioning.
 *
 * \param[out] thing Pointer to a buffer that should hold the thing name.
 * \param[in] thing_len The length of the buffer pointed to by thing above
 *
 * \return WM_SUCCESS on success
 * \return error code otherwise
 */
int read_aws_thing(char *thing, unsigned thing_len);

/** Enable AWS Configuration during provisioning
 *
 * When the wm_wlan_start() API is called, and if the device is
 * unconfigured, a Web App is available for users to configure the home network
 * settings in the device. This Web App can be accessed through a browser after
 * connecting to the micro-AP network of the device.
 *
 * Calling this API before wm_wlan_start() will add the steps for AWS
 * configuration into this Web App's wizard.
 */
int enable_aws_config_support();

#endif /* ! _AWS_UTILS_H_ */
