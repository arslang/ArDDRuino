const unsigned long mil = 1000000;

// song1: Numb by Linkin Park
const int bpm1 = 110; // beats per minute of song
const unsigned long start1 = 4.386*mil; //first even in song
const unsigned long bpm1_dt = 60.0 / 110.0 * mil; // time inbetween beats in microseconds

const prog_uint32_t song1[] PROGMEM = {
	start1, 1,
	start1 + bpm1_dt * 1.5, 3,
	start1 + bpm1_dt * 3, 2,
	start1 + bpm1_dt * 8, 4,
	start1 + bpm1_dt * 9.5, 3,
	start1 + bpm1_dt * 11, 1,
	start1 + bpm1_dt * 14.5, 1,
	start1 + bpm1_dt * 15, 2,
	start1 + bpm1_dt * 15.5, 1,
	start1 + bpm1_dt * 16, 1,
	start1 + bpm1_dt * 16, 2,
	start1 + bpm1_dt * 17.5, 1,
	start1 + bpm1_dt * 17.5, 4,
	start1 + bpm1_dt * 19, 1,
	start1 + bpm1_dt * 19, 3,
	start1 + bpm1_dt * 22.5, 1,
	start1 + bpm1_dt * 23, 2,
	start1 + bpm1_dt * 23.5, 1,
	start1 + bpm1_dt * 24, 1,
	start1 + bpm1_dt * 24, 4,
	start1 + bpm1_dt * 25.5, 1,
	start1 + bpm1_dt * 25.5, 3,
	start1 + bpm1_dt * 27, 1,
	start1 + bpm1_dt * 27, 2,
	start1 + bpm1_dt * 30.5, 1,
	start1 + bpm1_dt * 31, 2,
	start1 + bpm1_dt * 31.5, 1,
	start1 + bpm1_dt * 32, 2,
	start1 + bpm1_dt * 32, 4,
	start1 + bpm1_dt * 33, 3,
	start1 + bpm1_dt * 33.5, 3,
	start1 + bpm1_dt * 34, 3,
	start1 + bpm1_dt * 34.5, 3,
	start1 + bpm1_dt * 35, 4,
	start1 + bpm1_dt * 35.5, 3,
	start1 + bpm1_dt * 36.5, 3,
	start1 + bpm1_dt * 37, 2,
	start1 + bpm1_dt * 38, 1,
	start1 + bpm1_dt * 39.5, 1,
	start1 + bpm1_dt * 40, 4,
	start1 + bpm1_dt * 40.5, 4,
	start1 + bpm1_dt * 41, 3,
	start1 + bpm1_dt * 41.5, 2,
	start1 + bpm1_dt * 41.5, 3,
	start1 + bpm1_dt * 42.5, 3,
	start1 + bpm1_dt * 42.5, 4,
	start1 + bpm1_dt * 43.5, 1,
	start1 + bpm1_dt * 44, 4,
	start1 + bpm1_dt * 44.5, 4,
	start1 + bpm1_dt * 45, 3,
	start1 + bpm1_dt * 45.5, 2,
	start1 + bpm1_dt * 45.5, 3,
	start1 + bpm1_dt * 46.5, 3,
	start1 + bpm1_dt * 46.5, 4
};
