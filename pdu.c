#include "pdu.h"

PDU_Details_t pdu = {0};


static char* swapchars(char* string);
static void pop(int length, int *value, char *str);

void PDU_Init()
{
    PDU_setSMSCNumber(NULL, NF_UNKNOWN);
    pdu.submit_header.PDU_TYPE.submit.MTI = MTI_SMS_SUBMIT;
    PDU_setDuplicateReject(true);
    PDU_setValidityPeriod(VPF_PRESENT_RELATIVE, 1);
    PDU_setStatusReport(false);
    PDU_setPort(0, 6969);
    pdu.submit_header.PDU_TYPE.submit.RP = RP_NOT_SET;
    PDU_setMessageReferenceNo(0);
    // Destination address
    PDU_setPID(SMS);
    PDU_setAlphabet(DCS_16BIT);
//    PDU_setDataMessage(data, DCS_16BIT);
    // Message
}

//********************************************************
void PDU_setSMSCNumber(char number[], PDU_NumberFormat_t format)
{
    // Automatic select SMSC number
    if(number == NULL)
    {
        sprintf(pdu.submit_header.SCA, "00");
        return;
    }
    // Add 'F' at the end of string if number length is odd.
    if(strlen(number)%2)
    {
        strcat(number, "F");
    }
    int length = strlen(number)/2+1;
    snprintf(pdu.submit_header.SCA, sizeof(pdu.submit_header.SCA), "%02X%02X%s", length, format, swapchars(number));
}

//********************************************************
void PDU_setDuplicateReject(bool RD)
{
    pdu.submit_header.PDU_TYPE.submit.RD = RD ? RD_ACCEPT : RD_REJECT;
}

//********************************************************
void PDU_setValidityPeriod(PDU_VPF_t VPF, uint8_t VP)
{
    pdu.submit_header.PDU_TYPE.submit.VPF = VPF;
    if(VPF == VPF_PRESENT_RELATIVE)
    {
        sprintf(pdu.submit_header.VP, "%02X", VP);
//		if(hour <= 12.0f)
//		{
//			sprintf(pdu.submit_header.VP, "%02X", (uint8_t)round( hour * 60 / 5 - 1));
//		}
//		else if(hour <= 144.0f)
//		{
//			sprintf(pdu.submit_header.VP, "%02X", (uint8_t)round( hour * 60 / (30) -12));
//		}
//		else if(hour <= 196)
//		{
//		}
//		else if (hour <= 255)
//		{
//		}
    }
}

//********************************************************
void PDU_setStatusReport(bool status_report)
{
    pdu.submit_header.PDU_TYPE.submit.SRR = status_report ? SRR_REQUESTED : SRR_NOT_REQUESTED;
}

//********************************************************
void PDU_setPort(uint16_t source_port, uint16_t destination_port)
{
    pdu.submit_header.PDU_TYPE.submit.UDHI = 1;
    sprintf(pdu.submit_header.UDH, "060504%04X%04X", destination_port, source_port);
}

//********************************************************
void PDU_setMessageReferenceNo(uint8_t reference_no)
{
    pdu.submit_header.MR = reference_no;
}

//********************************************************
void PDU_setDestinationNumber(char number[], PDU_NumberFormat_t format)
{
    // Unknown number
    if(number == NULL)
    {
        sprintf(pdu.submit_header.DA, "0000");
        return;
    }

    char tmpNumber[13];
    strcpy(tmpNumber, number);
    // Add 'F' at the end of string if number length is odd.
    int length = strlen(tmpNumber);
    if(strlen(tmpNumber)%2)
    {
        strcat(tmpNumber, "F");
    }
    sprintf(pdu.submit_header.DA, "%02X%02X%s", length, format, swapchars(tmpNumber));
}

//********************************************************
void PDU_setPID(PDU_PID_t PID)
{
    pdu.submit_header.PID = PID;
}

//********************************************************
void PDU_setAlphabet(PDU_DataCodingScheme_t DCS)
{
    pdu.submit_header.DCS = DCS;
}

void PDU_setFlashMessage(bool flash_message)
{
}

//********************************************************
void PDU_setTextMessage(char *txt_message, PDU_DataCodingScheme_t encoding_format)
{
    int length = strlen((char *)txt_message);
    char tmp[9];

    memset(pdu.submit_header.UD, 0, sizeof(pdu.submit_header.UD[0]) * UD_SIZE);

    switch(encoding_format)
    {
        case DCS_7BIT:
            break;

        case DCS_8BIT:
            if(pdu.submit_header.PDU_TYPE.submit.UDHI)
                pdu.submit_header.UDL = length + 7;
            else
                pdu.submit_header.UDL = length;

            for(int i = 0; i < length - 1; i++)
            {
                sprintf(tmp, "%02X", txt_message[i]);
                strcat(pdu.submit_header.UD, tmp);
            }
            break;

        case DCS_16BIT:
            break;
    }

}
void PDU_setDataMessage(uint16_t data_message[], int length, PDU_DataCodingScheme_t encoding_format)
{
    char tmp[9];

    memset(pdu.submit_header.UD, 0, sizeof(pdu.submit_header.UD[0]) * UD_SIZE);

    switch(encoding_format)
    {
        case DCS_7BIT:
        case DCS_8BIT:
            break;

        case DCS_16BIT:
            if(pdu.submit_header.PDU_TYPE.submit.UDHI)
                pdu.submit_header.UDL = length*2 + 7;
            else
                pdu.submit_header.UDL = length*2;

            for(int i = 0; i < length; i++)
            {
                sprintf(tmp, "%04X", data_message[i]);
                strcat(pdu.submit_header.UD, tmp);
            }
            break;
    }
}

//********************************************************
void PDU_setPDUCode(char * code)
{
    memset(pdu.pdu_code, 0, strlen(pdu.pdu_code));
    memcpy(pdu.pdu_code, code, strlen(code));
}

//********************************************************
/* The total bytes of PDU message is excluded the SCA address field expressed in decimal value. */
int PDU_getPDUCodeSize()
{
    int SMSCL;
    int length = 0;
    char hold[3];

    PDU_encode();
    length = strlen(pdu.pdu_code);
    if(strcmp(pdu.submit_header.SCA, "00") == 0)
    {
        length -= 2;
    }
    else
    {
        memcpy(hold, pdu.pdu_code, 2);
        sscanf(hold, "%d", &SMSCL);
        length -= (SMSCL*2+2);
    }
    return length/2;
}
//void PDU_setMessageClass();

// Make PDU Code
void PDU_encode()
{
    // Message have submit_header data
    if(pdu.submit_header.PDU_TYPE.submit.UDHI)
        sprintf(pdu.pdu_code, "%s%02X%02X%s%02X%02X%s%02X%s%s"
                , pdu.submit_header.SCA
                , pdu.submit_header.PDU_TYPE.allfields
                , pdu.submit_header.MR
                , pdu.submit_header.DA
                , pdu.submit_header.PID
                , pdu.submit_header.DCS
                , pdu.submit_header.VP
                , pdu.submit_header.UDL
                , pdu.submit_header.UDH
                , pdu.submit_header.UD);
        // Message just contain user data
    else
        sprintf(pdu.pdu_code, "%s%02X%02X%s%02X%02X%s%02X%s"
                , pdu.submit_header.SCA
                , pdu.submit_header.PDU_TYPE.allfields
                , pdu.submit_header.MR
                , pdu.submit_header.DA
                , pdu.submit_header.PID
                , pdu.submit_header.DCS
                , pdu.submit_header.VP
                , pdu.submit_header.UDL
                , pdu.submit_header.UD);
}

// Extract details from PDU
void PDU_decode()
{
    int val;
    //   +++ SCA
    pop(2, &val, NULL);
    if(val)
    {
        pop(2, NULL, NULL);
        val--;
        pop(val*2, NULL, pdu.deliver_header.SCA);
        if(val%2) // Odd
        {
            // Remove 'F' from SMSC number here..
        }
    }
    //   --- SCA
    //   +++ PDU-Type
    pop(2, &val, NULL);
    pdu.deliver_header.PDU_TYPE.allfields = val;
    //   --- PDU-Type
    //   +++ OA
    pop(2, &val, NULL);
    int tmp = val;

    if(val%2)
        tmp++;

    pop(2, &val, NULL);
    pdu.deliver_header.numberFormat = val;
    if(pdu.deliver_header.numberFormat == NF_CHAR)
        return;
    pop(tmp, NULL, pdu.deliver_header.OA);
    //   --- OA
    //   +++ PID
    pop(2, &val, NULL);
    pdu.deliver_header.PID = val;
    //   --- PID
    //   +++ DCS
    pop(2, &val, NULL);
    pdu.deliver_header.DCS = val;
    //   --- DCS
    //   +++ SCTS
    pop(14, NULL, pdu.deliver_header.SCTS);
    //   --- SCTS
    //   +++ UDL
    pop(2, &val, NULL);
    pdu.deliver_header.UDL = val;
    int userDataSize = val;
    //   --- UDL
    //   +++ UDH
    if(pdu.deliver_header.PDU_TYPE.deliver.UDHI)
    {
        /* Header size */
        pop(2, &val, NULL);
        int headerSize = val;
        userDataSize -= (headerSize+1);
        /* Header type */
        pop(2, &val, NULL);
        headerSize--;
        /* IEI */
        switch(val)
        {
            case 0: /* Concatenated short messages, 8-bit reference number	 */
                /* length = 3 */
            case 1: /* Special SMS Message Indication */
                /* length = 2 */
            //case 2: /* Reserved */
                /* length = N/A */
            case 3: /* Not used to avoid misinterpretation as <LF> character */
                /* length = N/A */
                /* But it should be removed */
                pop(headerSize*2, NULL, NULL);
                break;
            case 4: /* Application port addressing scheme, 8-bit address */
                /* length = 2 */
                pop(2, NULL, NULL);
                /* Source Port */
                pop(2, &val, NULL);
                pdu.source_port = val;
                /* Destination Port */
                pop(2, &val, NULL);
                pdu.destination_port = val;
                break;
            case 5: /* Application port addressing scheme, 16-bit address	 */
                /* length = 4 */
                pop(2, NULL, NULL);
                /* Source Port */
                pop(2, &val, NULL);
                int port = val << 8;
                pop(2, &val, NULL);
                port |= val & 0x00FF;
                pdu.source_port = port;
                /* Destination Port */
                pop(2, &val, NULL);
                port = val << 8;
                pop(2, &val, NULL);
                port |= val & 0x00FF;
                pdu.destination_port = port;
                break;
            default: /* Unknown */
                /* But it should be removed */
                pop(headerSize*2, NULL, NULL);
                break;
        }
    }
    //   --- UDH
    //   +++ UD
    memset(pdu.deliver_header.UD, 0, UD_SIZE);
    pop(userDataSize*2, NULL, pdu.deliver_header.UD);
    //   --- UD
}

void PDU_getPhoneNumber(char* number, bool sign)
{
    int length = strlen(pdu.deliver_header.OA);
    char *tmp = malloc(length+1);
    memset(tmp, 0, length+1);
    memset(number, 0, length+1);
    memcpy(tmp, pdu.deliver_header.OA, length);

    swapchars(tmp);

    /* Remove 'F' from number */
    if(strstr(tmp, "F"))
        tmp[strlen(tmp)-1] = '\0';

    if(sign)
    {
        if(pdu.deliver_header.numberFormat == NF_INTERNATIONAL)
        {
            number[0] = '+';
        }
        else if(pdu.deliver_header.numberFormat == NF_NATIONAL)
        {
            number[0] = '0';
        }
        strcat(number, tmp);
    }
    else
    {
        memcpy(number, tmp, length);
    }
}

static void pop(int length, int *value, char *str)
{
    if(length >= UD_SIZE)
        return;
    char octets[length+1];
    memset(octets, 0, length+1);
    memcpy(octets, pdu.pdu_code, length);
    memmove(pdu.pdu_code, pdu.pdu_code + length, PDU_CODE_SIZE);
    if(value != NULL)
        sscanf((char *)octets, "%02x", value);
    else if(str != NULL)
        memcpy(str, octets, length);
}


/* Swap every second character */
static char* swapchars(char *string)
{
    int Length;
    int position;
    char c;

    Length = strlen(string);
    for (position = 0; position < Length - 1; position += 2)
    {
        c = string[position];
        string[position] = string[position + 1];
        string[position + 1] = c;
    }
    return string;
}
