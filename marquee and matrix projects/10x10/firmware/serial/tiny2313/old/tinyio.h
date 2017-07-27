void setDDR(uint32_t ddr) 
{
    DDRB = ddr;
    ddr >>= 8;
    DDRD = ddr&0x7f;
    ddr >>= 7;
    DDRA = ddr&3;
}

void setPort(uint32_t pins)
{
    PORTB = pins;
    pins >>= 8;
    PORTD = pins&0x7f;
    pins >>= 7;
    PORTA = pins&3;
}

