#define RAM_START  0x20000000U
#define RAM_LENGTH 8192U
#define RAM_END    RAM_START + RAM_LENGTH
#define STACK      RAM_END - 4

int main();


int reset_hdlr() {
    volatile int x;
    volatile unsigned int y = *((unsigned int *)0x00);

    main();

    while(1) {
        if(x > 50) {
            x = 0;
        }
        else {
            x ++;
        }
    }
}

volatile unsigned int __attribute__((section(".vector_table"))) vector_table [83] = {
    STACK,
    (unsigned int)&reset_hdlr
};