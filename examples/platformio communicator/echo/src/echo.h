#define SAMPLE_BUFFER_SIZE 30000
#define SAMPLE_RATE 8000


size_t record(int8_t *buffer, bool (*canContinue)());
void playback(int8_t *buffer, size_t length);

void LEDsetup();
void showRed();
void showGreen();
void showBlue();