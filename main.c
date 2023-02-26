#include <stdio.h>
#include "pdu.h"

char SMC[] = "+98935000140";
char code[] = "06918919015000240C918919927258350008321070616513410C00FF018001020101010100FE";
//char code[] = "06918919015000640C91891992725835000832107061651341130605041B391B3900FF018001020101010100FE";
char number1[] = "+989123456789";
char message[] = "سلام";
int main()
{
    PDU_Init(75);
    int result = PDU_encode(SMC, false, true, false, VPF_PRESENT_RELATIVE, 0,  number1, 1, message,  3, 5, 6969, 6969);
    if(result > 0)
    {
        printf("%s\n", PDU_getPDUBuffer());
        printf("len=%d\n", PDU_getPDUCodeSize());
    } else
        printf("error\n");

        if(PDU_decode(code))
            printf("%s\n", PDU_getPDUBuffer());
        else
            printf("error\n");
        printf("%s", deliverHeader.OA);

        return 0;
}
