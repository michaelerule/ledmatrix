
#define N 10
#define NN ((N)*(N))

#define SCANRATE 28

uint16_t b0[N+1],b1[N+1];
uint16_t *disp = &b0[0];
uint16_t *buff = &b1[0];
volatile uint8_t scanI = 0;
volatile uint8_t scanJ = 0;
uint8_t pins[11] = {0,1,2,3,4,10,11,12,13,14,15};

uint32_t getPinstate(uint32_t vector)
{
	uint32_t value = 0;
	uint8_t i;
	for (i=0;i<11;i++) {
		value |= (vector&1)<<pins[i];
		vector >>= 1;
	}
	return value;
}

uint8_t get(uint8_t r, uint8_t c,uint16_t *d)  {
	return d[r==c?N:c]>>r&1;
}

void set(uint8_t r,uint8_t c,uint8_t v,uint16_t *d)  {
	if (get(r,c,d)!=v) d[r==c?N:c]^=1<<r;
}

void flipBuffers() {
	uint16_t *temp = buff;
	buff = disp;
	disp = temp;
}

void scan_display()
{
	uint32_t pin = 1<<pins[scanI];
	uint32_t ddr=getPinstate(disp[scanI]);
	ddr |= pin;
	if (scanI!=N) pin=~pin;
	pin &= ddr;
	setDDR(0);
	setPort(pin);
	setDDR(ddr);
	if (++scanI>N) scanI=0;
}

