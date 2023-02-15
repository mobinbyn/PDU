#ifndef _PDU_H_
#define _PDU_H_


#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#define UD_SIZE												280
#define PDU_CODE_SIZE                 350

/**
 * @defgroup RP_Replay_Patch
 */
#define RP_NOT_SET										0x00
#define RP_SET												0x01

#define UDHI_ONLY_MSG									0x00
#define UDHI_HEADER_MSG								0x01

#define SRR_NOT_REQUESTED							0x00
#define SRR_REQUESTED									0x01


#define MMS_MORE_MESSAGE							0x00
#define MMS_NO_MORE_MESSAGE						0x01

#define RD_ACCEPT											0x00
#define RD_REJECT											0x01


/**
 * @defgroup Message type indicator
 * @{
 *              Bit 1 Bit 0 	Direction SC->MS 		Direction MS->SC
 *                 0    0 		SMS-DELIVER 				SMS-DELIVER REPORT
 *                 1 		0 		SMS-STATUS REPORT 	SMS-COMMAND
 *	               0 		1 		SMS-SUBMIT REPORT 	SMS-SUBMIT
 *	               1 		1 		RESERVED
 *
 */
#define MTI_SMS_DELIVER 							0x00	// SMSC -> MS
#define MTI_SMS_DELIVER_REPORT				0x00	// MS -> SMSC
#define MTI_SMS_SUBMIT								0x01	// MS -> SMSC
#define MTI_SMS_SUBMIT_REPORT					0x01	// SMSC -> MS
#define MTI_SMS_STATUS_REPORT					0x02	// SMSC -> MS
#define MTI_SMS_COMMAND								0x02	// MS -> SMSC
/**
 * @}
 */

#define PID_SHORT_MESSAGE							0x00
#define PID_SHORT_MESSAGE_TYPE1				0x41
#define PID_SHORT_MESSAGE_TYPE2				0x42
#define PID_SHORT_MESSAGE_TYPE3				0x43
#define PID_SHORT_MESSAGE_TYPE4				0x44
#define PID_SHORT_MESSAGE_TYPE5				0x45
#define PID_SHORT_MESSAGE_TYPE6				0x46
#define PID_SHORT_MESSAGE_TYPE7				0x47

#define MAX_NUMBER_LEN								12
#define MAX_USER_DATA_LEN							160

typedef enum
{
    NF_UNKNOWN = 0x80,
    NF_INTERNATIONAL = 0x91,
    NF_NATIONAL = 0x81,
    NF_CHAR  = 0xD0
}PDU_NumberFormat_t;

typedef enum
{
    SMS 		= 0x00,
    SMS1		= 0x41,
    SMS2		= 0x42,
    SMS3		=	0x43,
    SMS4		= 0x44,
    SMS5  	= 0x45,
    SMS6		= 0x46,
    SMS7		= 0x47
//	MMS			= 0x
//	FAX			= 0x,
//	VOICE		= 0x
}PDU_PID_t;

typedef enum
{
    DCS_7BIT 	= 0x00,
    DCS_8BIT	= 0x04,
    DCS_16BIT = 0x08
}PDU_DataCodingScheme_t;

typedef enum
{
    VPF_NOT_PRESENT	=							0x00,
    VPF_PRESENT_RELATIVE = 				0x02,
    VPF_PRESENT_ABSOLUTE =				0x03,
}PDU_VPF_t;

typedef union
{    struct{
        uint8_t MTI: 2;				// Message Type Indicator: Parameter describing the message type 01 for SMS-SUBMIT
        uint8_t RD : 1;				// Reject Duplicate
        uint8_t VPF : 2;			// Validity Period Format: Parameter indicating whether the VP field is present
        uint8_t SRR : 1;			// Status Report Request: Parameter indicating if the MS has requested a status report
        uint8_t UDHI : 1;			// User Data Header Indicator: Parameter indicating that the UD field contains a submit_header
        uint8_t RP : 1; 			// Reply Path: Parameter indicating that replay path exists
    }submit;
    uint8_t allfields;			// PDU-Type octet
}PDU_TypeSubmit_t;

typedef union
{
    struct{
        uint8_t MTI: 2;				// Message Type Indicator: Parameter describing the message type 00 for SMS-DELIVER
        uint8_t MMS : 1;			// More Message to Send: Parameter indicating whether there are more message to send
        uint8_t BIT_4_3 : 2;	// Bits 3 and 4 are not used and are set to 0
        uint8_t SRI : 1;			// Status Report Indication: Parameter indicating if the SME has requested a status report
        uint8_t UDHI : 1;			// User Data Header Indicator: Parameter indicating that the UD field contains a submit_header
        uint8_t RP : 1; 			// Reply Path: Parameter indicating that replay path exists
    }deliver;
    uint8_t allfields;			// PDU-Type octet
}PDU_TypeDeliver_t;


typedef struct
{
    char 										SCA[17];								// Service Center Address: Telephone number of the Service Center

    PDU_TypeSubmit_t 	      PDU_TYPE;								// Protocol Data Unit Type

    uint8_t									MR;											// Message Reference: successive number (0..255) of all SMS-SUBMIT Frames set by the MOBILE

    char										DA[16];									// Destination Address

    PDU_PID_t	 							PID;										// Protocol Identifier: Parameter showing the SMSC how to process the SM (as FAX, Voice etc).

    PDU_DataCodingScheme_t 	DCS;										// Data Coding Scheme: Parameter identifying the coding scheme within the User Data(UD)

    char										VP[3];									//	Validity Period: Parameter identifying the time from where the message is no longer valid in the SMSC

    uint8_t									UDL;										// User Data Length: Parameter indicating the length of the UD-field
    char										UDH[15];								// User Data Header
    char										UD[UD_SIZE];						// Data of the SM
}PDU_SubmitHeader_t;


typedef struct
{
    char 										SCA[18];								// Service Center Address: Telephone number of the Service Center

    PDU_TypeDeliver_t				PDU_TYPE;								// Protocol Data Unit Type

    PDU_NumberFormat_t      numberFormat;
    char										OA[17];									// Originator Address

    PDU_PID_t	 							PID;										// Protocol Identifier: Parameter showing the SMSC how to process the SM (as FAX, Voice etc).

    PDU_DataCodingScheme_t 	DCS;										// Data Coding Scheme: Parameter identifying the coding scheme within the User Data(UD)

    char 										SCTS[15];								// Service Center Time Stamp: Parameter identifying time when the SMSC received the message

    uint8_t									UDL;										// User Data Length: Parameter indicating the length of the UD-field
    char										UD[UD_SIZE];						// Data of the SM
}PDU_DeliverHeader_t;


typedef struct
{
    PDU_SubmitHeader_t 			submit_header;
    PDU_DeliverHeader_t     deliver_header;

    char 										pdu_code[PDU_CODE_SIZE];//196
    char 										*text_message;//160

    uint16_t 								source_port;//4
    uint16_t 								destination_port;//4

}PDU_Details_t;


extern PDU_Details_t pdu;



/**
 *
 */
void PDU_Init();

/**
 *
 * @param number
 * @param format
 */
void PDU_setSMSCNumber(char *number, PDU_NumberFormat_t format);

/**
 * Parameter indicating whether or not the SC shall accept an SMS-SUBMIT
 * for an SM still held in the SC which has the same MR
 * and the same DA as a previously submitted SM from the same OA
 * @param RD: true accept duplicated
 */
void PDU_setDuplicateReject(bool RD);

/**
 * Parameter indicating whether or not the VP field is present
 * Parameter identifying the time from where the message is no longer valid
 * @param VPF: validity period @ref PDU_VPF_t
 * @param VP: validity period value
 *          	VP value 						Validity period value
 *          	0 to 143 						(VP + 1) x 5 minutes (i.e. 5 minutes intervals up to 12 hours)
 *          	144 to 167 12 			hours + ((VP -143) x 30 minutes)
 *          	168 to 196 					(VP - 166) x 1 day
 *           197 to 255 					(VP - 192) x 1 week
 */
void PDU_setValidityPeriod(PDU_VPF_t VPF, uint8_t VP);

/**
 *
 * @param status_report
 */
void PDU_setStatusReport(bool status_report);

/**
 *
 * @param source_port
 * @param destination_port
 */
void PDU_setPort(uint16_t source_port, uint16_t destination_port);

/**
 *
 * @param reference_no
 */
void PDU_setMessageReferenceNo(uint8_t reference_no);

/**
 *
 * @param number
 * @param format
 */
void PDU_setDestinationNumber(char *number, PDU_NumberFormat_t format);

/**
 *
 * @param PID
 */
void PDU_setPID(PDU_PID_t PID);

/**
 *
 * @param DCS
 */
void PDU_setAlphabet(PDU_DataCodingScheme_t DCS);

/**
 *
 * @param flash_message
 */

/**
 *
 * @param flash_message
 */
void PDU_setFlashMessage(bool flash_message);

/**
 *
 */
void PDU_setMessageClass();

/**
 *
 * @param txt_message
 * @param encoding_format
 */
void PDU_setTextMessage(char *txt_message, PDU_DataCodingScheme_t encoding_format);

/**
 *
 * @param data_message
 * @param encoding_format
 */
void PDU_setDataMessage(uint16_t data_message[], int length, PDU_DataCodingScheme_t encoding_format);

/**
 *
 */
void PDU_setPDUCode(char *code);

/**
 *
 * @return
 */
int PDU_getPDUCodeSize();

/* Make PDU Code */
/**
 *
 */
void PDU_encode();

/* Extract details from PDU */
/**
 *
 */
void PDU_decode();

void PDU_getPhoneNumber(char *number, bool sign);
#endif //_PDU_H_
