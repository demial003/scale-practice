#include <alsa/asoundlib.h>
#include <stdlib.h>

static char *device = "pipewire";
unsigned char buf[16 * 1024];
void getDevices();
int main() {
  getDevices();
  int err;
  unsigned int i;
  snd_pcm_t *handle;
  snd_pcm_sframes_t frames;

  for (i = 0; i < sizeof(buf); i++) {
    buf[i] = random() & 0xff;
  }

  if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
    printf("Playback open error: %s\n", snd_strerror(err));
    exit(EXIT_FAILURE);
  }

  if ((err = snd_pcm_set_params(handle, SND_PCM_FORMAT_U8,
                                SND_PCM_ACCESS_RW_INTERLEAVED, 1, 48000, 1,
                                500000)) < 0) {
    printf("Playback open error %s\n", snd_strerror(err));
    exit(EXIT_FAILURE);

    for (i = 0; i < 16; i++) {
      frames = snd_pcm_writei(handle, buf, sizeof(buf));
      if (frames < 0) {
        frames = snd_pcm_recover(handle, frames, 0);
      }
      if (frames < 0) {
        printf("snd_pcm_writei failed: %s\n", snd_strerror(frames));
        break;
      }
    }
    if (frames > 0 && frames < (long)sizeof(buf)) {
      printf("short write (expected %li, wrote %li)\n", (long)sizeof(buf),
             frames);
    }
  }

  err = snd_pcm_drain(handle);
  if (err < 0) {
    printf("snd_pcm_drain failed: %s\n", snd_strerror(err));
    snd_pcm_close(handle);
  }

  exit(0);
}

void getDevices() {

  void **hints;
  int err = snd_device_name_hint(-1, "pcm", (void ***)&hints);

  if (err < 0) {
    printf("failed gg");
    exit(1);
  }
  char *NAME, *DESC, *IOID;
  for (void **cur = hints; *cur != NULL; cur++) {
    void *hint = *cur;
    NAME = snd_device_name_get_hint(hint, "NAME");
    DESC = snd_device_name_get_hint(hint, "DESC");
    IOID = snd_device_name_get_hint(hint, "IOID");
    if (!IOID) {
      IOID = "both";
      printf("%s\n %s\n %s\n\n ", NAME, DESC, IOID);
    } else {
      printf("%s\n %s\n %s\n\n ", NAME, DESC, IOID);
      free(IOID);
    }
    free(NAME);
    free(DESC);
  }
  snd_device_name_free_hint(hints);
}
