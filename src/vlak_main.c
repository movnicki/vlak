#include"vlak.h"

int main(void)
{
    vlak_init(800, 600, NULL);

    vlak_main_loop();
    return 0;
}
