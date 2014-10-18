
typedef struct 
{
	uint8_t pin;
	float hum, temp;
} dhtsample_t;

int dhtAcquire(dhtsample_t *sample);

