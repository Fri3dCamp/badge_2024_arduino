#define SAMPLE_BUFFER_SIZE 30000
#define SAMPLE_RATE 8000

void record(int32_t *buffer);
void playback(int32_t *buffer);

void LEDsetup();
void showRed();
void showGreen();