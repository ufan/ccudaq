#include "display.h"

int main(int argc,char* argv[])
{
    CDisplay *pDisplay=new CDisplay();
    while(pDisplay->getCmd()){
    }
    return 0;
}
