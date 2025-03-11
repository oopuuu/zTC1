#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#


NAME := Lib_mqtt_client_c


$(NAME)_SOURCES := 	MQTTClient.c \
					mico/MQTTMiCO.c \
					MQTTPacket/MQTTConnectClient.c\
					MQTTPacket/MQTTConnectServer.c\
					MQTTPacket/MQTTDeserializePublish.c\
					MQTTPacket/MQTTFormat.c\
					MQTTPacket/MQTTPacket.c\
					MQTTPacket/MQTTSerializePublish.c\
					MQTTPacket/MQTTSubscribeClient.c\
					MQTTPacket/MQTTSubscribeServer.c\
					MQTTPacket/MQTTUnsubscribeClient.c\
					MQTTPacket/MQTTUnsubscribeServer.c\
				   
				   
GLOBAL_INCLUDES := 	. \
					mico \
					MQTTPacket
					
					
				   	