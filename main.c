#include <stdio.h>
#include "pdu.h"
char SMC[] = "98935000140";
//char code[] = "0791893905801900440ED04979D83D2EB3D90008322050219534415C050003DC020206280627002006270631063306270644002006A9062706450644062700200631062706CC06AF062706460021000A0068007400740070003A002F002F00690033006C002E00690072002F007000420068006C0071000A";
char code[] = "0791893905801900440ED04979D83D2EB3D90008322041017114418C050003B202010647062F062706CC062706CC0020064806CC069806470020062E063106CC062F0020063306CC0645002006A906270631062A002000AB06F006F906F006F000BB0020062706320020064106310648063406AF062706470020062706";
//char code[] = "06918919015000240C918919927258350008321070616513410C00FF018001020101010100FE";
//char code[] = "06918919015000640C91891992725835000832107061651341130605041B391B3900FF018001020101010100FE";
//uint16_t data[5] = {344, 400, 401, 402, 304};
//char number1[] = "989129278553";
char number[17];
//char test[5] = {'C', '1', '2', 'B'};
int main()
{
//char buffer[]= "\r\n+CMGR: 2,\"\",31\r\n06918919015000240C918919927258350008321070616513410C00FF018001020101010100FE\r\n\r\nOK\r\n";
//    char f[5];
//    char ss[3];
//unsigned int out;
//uint8_t cnt;
//    sscanf((char*)test, "%x", &out);
//    sscanf((char *)buffer, "\r\n+CMGR: %[^,],\"\",%[^\r]\r\n%[^\r]",f, ss ,pdu.pdu_code);
//    int ssss = sizeof (test);
//
//    char *end = strstr(buffer, "\r\n\r\nOK\r\n");
//    char *s = strtok((char*) buffer, "\r\n");
//    s = strtok(NULL, "\r\n");
//    strncpy((char*) &buffer[0], s, end - s);
//    memset(&buffer[end - s], 0, sizeof(buffer) - (end - s));
//    PDU_Init();
//    PDU_setDataMessage(data,5, DCS_16BIT);
//    PDU_setDestinationNumber(number1, NF_INTERNATIONAL);
//    PDU_setSMSCNumber(SMC, NF_INTERNATIONAL);
//    PDU_encode();
//    printf("%s\r\n", pdu.pdu_code);
//    printf("%d\r\n", PDU_getPDUCodeSize());

    PDU_setPDUCode(code);
    PDU_decode();
    PDU_getPhoneNumber(number, false);
    printf("%s\r\n", number);
    printf("%s", pdu.deliver_header.UD);

    return 0;
}
