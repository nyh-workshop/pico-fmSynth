#include "miditones.h"

// Len Shustek's Miditones for arduino:
// https://github.com/LenShustek/arduino-playtune/blob/master/Playtune.cpp

PlayTune::PlayTune(const uint8_t* inputScore, fmChannel* inputFmChannel) {
    score = (uint8_t*)inputScore;
	pfmc = inputFmChannel;
    score_cursor = 0;
    tune_playing = false;
    volume_present = false;
}

void PlayTune::stopScore()
{
    const uint8_t _tune_num_chans = sizeof(pfmc)/sizeof(pfmc[0]);

    for (uint8_t i = 0; i < _tune_num_chans; ++i) {
        // note stop here.
        //stopnote(i);
    }
    tune_playing = false;
}

void PlayTune::playScore(const uint8_t* inputScore) {

  score = (uint8_t*)inputScore;

  if (tune_playing) stopScore();

#if DBUG
    Serial.print("header: volume_present="); Serial.println(volume_present);
#endif

  tune_playing = true;  /* release the interrupt routine */
}

void PlayTune::stepScore(mutex_t* pInputMutex)
{
    uint8_t cmd, opcode, chan, note;
    uint32_t duration;
    /* Do score commands until a "wait" is found, or the score is stopped.
    This is called initially from tune_playcore, but then is called
    from the interrupt routine when waits expire.
  */
    const uint8_t CMD_PLAYNOTE = 0x90;   /* play a note: low nibble is generator #, note is next byte */
    const uint8_t CMD_STOPNOTE = 0x80;   /* stop a note: low nibble is generator # */
    const uint8_t CMD_INSTRUMENT = 0xc0; /* change instrument; low nibble is generator #, instrument is next byte */
    const uint8_t CMD_RESTART = 0xe0;    /* restart the score from the beginning */
    const uint8_t CMD_STOP = 0xf0;       /* stop playing */
    /* if CMD < 0x80, then the other 7 bits and the next byte are a 15-bit big-endian number of msec to wait */

    const uint8_t SCORE_START = 0x00;

    uint32_t owner;

    while (tune_playing)
    {
        cmd = score[score_cursor++];
        printf("cmd: 0x%x\n", cmd);
        if (cmd < 0x80)
        { /* wait count in msec. */
            duration = ((unsigned)cmd << 8) | (score[score_cursor++]);
            if (!mutex_try_enter(pInputMutex, &owner))
            {
                printf("at core 0 - locked, owner: %d, core_num: %d\n", owner, get_core_num());
                mutex_enter_blocking(pInputMutex);
            }
            
            printf("delay: 0x%x\n", (uint32_t)(duration));
            //printf("delay: %d\n", (uint32_t)((float)delta * TempoDivisor * 1000000));

            mutex_exit(pInputMutex);

            sleep_ms(duration);
            //wait_toggle_count = ((unsigned long)wait_timer_frequency2 * duration + 500) / 1000;
            //if (wait_toggle_count == 0)
            //   wait_toggle_count = 1;
#if DBUG
            Serial.print("wait ");
            Serial.print(duration);
            Serial.print("ms, cnt ");
            Serial.print(wait_toggle_count);
            Serial.print(" freq ");
            Serial.println(wait_timer_frequency2);
#endif
            break;
        }
        opcode = cmd & 0xf0;
        chan = cmd & 0x0f;
        if (opcode == CMD_STOPNOTE)
        { /* stop note */
            if (!mutex_try_enter(pInputMutex, &owner))
				{
					printf("at core 0 - locked, owner: %d, core_num: %d\n", owner, get_core_num());
					mutex_enter_blocking(pInputMutex);
				}
				pfmc[chan].noteOff();
				printf("note off event! number: %d\n", chan);
				mutex_exit(pInputMutex);

                sleep_ms(10);
        }
        else if (opcode == CMD_PLAYNOTE)
        {                                 /* play note */
            note = score[score_cursor++]; // argument evaluation order is undefined in C!
            if (volume_present)
                ++score_cursor; // ignore volume if present
			
            if (!mutex_try_enter(pInputMutex, &owner))
			{
				printf("at core 0 - locked, owner: %d, core_num: %d\n", owner, get_core_num());
				mutex_enter_blocking(pInputMutex);
			}
			pfmc[chan].setFrequency(convertMidiNoteToFreq(note));
			pfmc[chan].noteOn();
			printf("note on event! number: %d, frequency: %f\n", note, convertMidiNoteToFreq(note));				
			mutex_exit(pInputMutex);
        }
        else if (opcode == CMD_INSTRUMENT)
        {                   /* change a channel's instrument */
            score_cursor++; // ignore it
        }
        else if (opcode == CMD_RESTART)
        { /* restart score */
            score_cursor = SCORE_START;
        }
        else if (opcode == CMD_STOP)
        { /* stop score */
            tune_playing = false;
            break;
        }
    }
}