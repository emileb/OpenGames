/**
 *
 * TODO: change printf to output things on the Quake console instead
 */
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <stropts.h>
#include <pthread.h>
#include <sys/audioio.h>
#include <unistd.h>
#include <errno.h>

#include "../client/client.h"
#include "../client/snd_loc.h"

#define BLOCK_SIZE 1024
#define CHUNKS 2


static int audio_fd = 0;
static int audioctl_fd = 0;
static char audio_buf[BLOCK_SIZE * 16] = "";
static int audio_bps = 0;

dma_t dma;

qboolean SNDDMA_Init( void )
{
  audio_info_t info;

  /* set up audio device */
  if( (audio_fd = open ("/dev/audio", O_WRONLY|O_NDELAY)) == -1 ) {
    perror( "can't open audio device" );
    return 0;
  }
  if( (audioctl_fd = open ("/dev/audioctl", O_RDWR)) == -1 ) {
    perror( "can't open audio control device" );
    close( audio_fd );
    return 0;
  }
  if( ioctl( audio_fd, AUDIO_GETINFO, &info ) == -1 ) {
    perror( "Could not get audio info" );
    close( audio_fd );
    close( audioctl_fd );
    return 0;
  }
  info.play.encoding = AUDIO_ENCODING_LINEAR;
  info.play.sample_rate = 11025;
  info.play.channels = 2;
  info.play.precision = 16;
  if (ioctl(audio_fd, AUDIO_SETINFO, &info) == -1) {
    perror("Could not set audio device info");
    close( audio_fd );
    close( audioctl_fd );
    return 0;
  }

  // these need to be set up
  dma.speed = 11025;
  dma.samplebits = 16;
  dma.channels = 2;
  dma.samples = sizeof(audio_buf) / (dma.samplebits / 8);
  dma.samplepos = 0;
  dma.submission_chunk = BLOCK_SIZE / (dma.samplebits / 8);
  dma.buffer = (unsigned char *)audio_buf;

  // other useful precalculated variables
  audio_bps = (dma.samplebits / 8) * dma.channels;

  /* DEBUG set up buffer with a lovely tone */
  // for (n = 0; n < sizeof(audio_buf); n++)
  //   audio_buf[n] = n*2;
  // dma.buffer = (unsigned char*)audio_xxx;

  return 1;
}


int SNDDMA_GetDMAPos(void)
{
  /*  dma.samplepos = (audio_ptr / audio_bps);*/
  /*return dma.samplepos; */
  return 0;
}

void SNDDMA_Shutdown( void )
{
}

void SNDDMA_BeginPainting (void)
{
}

void SNDDMA_Submit (void)
{
}

int main( void )
{
  sigset_t sigs;

  sigemptyset( &sigs );
  sigaddset( &sigs, SIGPOLL );
  pthread_sigmask( SIG_BLOCK, &sigs, NULL );

  SNDDMA_Init();

  for( ;; ) {
    char str[ 1024 ];
    int length;
    int pos;
    int c;
    short *p;
    int b;

    printf( "tone length: " );
    fflush( stdout );
    gets( str );
    length = strtol( str, NULL, 0 );

    b = BLOCK_SIZE / (dma.samplebits / 8);
    pos = SNDDMA_GetDMAPos();
    printf( "MAIN: pos=%d\n", pos );
    if( length > 0 ) {
      pos = (pos | (b-1)) + 1;
      printf( "MAIN: submitting %d,%d\n", pos );
      for( c = 0 ; c < length ; c++ ) {
	((short *)dma.buffer)[ pos ] = sin( (double)c / 10 ) * 30000;
	if( ++pos >= dma.samples ) {
	  pos -= dma.samples;
	}
      }
    }
  }
      
  return 0;
}
