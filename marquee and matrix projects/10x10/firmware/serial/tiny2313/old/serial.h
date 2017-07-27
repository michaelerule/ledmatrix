
#define DATA_AVAILABLE (UCSRA&(1<<RXC))
#define OK_TO_SEND (UCSRA & (1 << UDRE))

void init_UART () {
    UBRRL = 0x1A;                // Set the baud rate 
    UCSRB = 1<<RXEN  | 1<<TXEN ; // Enable UART receive transmit 
    UCSRC = 1<<UCSZ1 | 1<<UCSZ0; // 8 data bits, 1 stop bit 
}

uint8_t maybe_send(uint8_t data )
{
    if (!OK_TO_SEND) return 0;
    UDR = data;
    return 1;
}

uint8_t maybe_read(uint8_t *target)
{
    if (!DATA_AVAILABLE) return 0;
    *target = UDR;
    return 1;
}


/*
#define buffersize 32
#define buffermask (buffersize-1)

volatile uint8_t receive_buffer[buffersize];
volatile uint8_t receive_head=0;
volatile uint8_t receive_tail=0;
#define N 10
#define NN ((N)*(N))
#define SCANRATE 28

uint16_t b0[N+1],b1[N+1];
uint16_t *disp = &b0[0];
uint16_t *buff = &b1[0];
volatile uint8_t scanI = 0;

void handle_serial() 
{
    uint8_t data;
    if (maybe_read(&data))
    {
        receive_buffer[receive_head] = data;
        receive_head = receive_head + 1 & buffermask;
        if ( receive_head == receive_tail )
        {
            data = receive_buffer[receive_tail];
            receive_tail = receive_tail + 1 & buffermask;
            while(!maybe_send(data));
        }
    }
}
*/
