
#define N 10
#define RNGMUL 13
#define RNGMOD 0x4CE3
uint16_t rng = 79;
uint16_t randIntN() {
	return (rng=(RNGMUL*rng+RNGMUL)%RNGMOD)%N;
}
