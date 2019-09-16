#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maximum_possible_header_size 256
#define maximum_possible_data_size   (3840 * 2160 * 3)

// For those wondering: 32 bits gives us up to 1.6M frames before overflow.
static uint32_t totals[maximum_possible_data_size] = {0};
static uint8_t  frame[maximum_possible_data_size + maximum_possible_header_size];

int main()
{
  // We need to infer these from the first image.
  int header_length = -1;
  int width         = -1;
  int height        = -1;

  uint32_t frames = 0;

  while (1)
  {
    long read_so_far = 0;

    if (header_length == -1)
    {
      // It's ok to read a bit more than we need here. We'll take up the slack
      // after this if-branch.
      while (read_so_far < maximum_possible_header_size)
        read_so_far += read(0, frame + read_so_far,
                               maximum_possible_header_size);

      // Note: irresponsible use of memchr, but it's probably fine for this use
      // case.
      void* const line1 = memchr(frame, '\n', maximum_possible_header_size) + 1;
      void* const line2 = memchr(line1, '\n', maximum_possible_header_size) + 1;
      void* const line3 = memchr(line2, '\n', maximum_possible_header_size) + 1;

      header_length = (long long) line3 - (long long) frame;
      fprintf(stderr, "inferred a header length of %d\n", header_length);

      // Now parse out the width and height.
      if (sscanf(frame, "P6\n%d %d\n255\n", &width, &height))
        fprintf(stderr, "width = %d; height = %d\n", width, height);
      else
      {
        fprintf(stderr, "failed to parse width and height from header\n");
        exit(1);
      }
    }

    // OK, all done parsing the header (for the first image). Now do the normal
    // thing and fill up the frame buffer.
    for (ssize_t n;
         read_so_far < header_length + width * height * 3;
         read_so_far += n)
    {
      // Subtle point: we won't call read() and get EOF until the beginning of
      // the last frame -- not the frame prior. This means ++frames can happen
      // after we do the if/goto and still be accurate.
      n = read(0, frame + read_so_far,
                  header_length + width*height*3 - read_so_far);
      if (!n) goto done_reading_frames;
    }

    ++frames;

    for (long i = 0; i < width*height*3; ++i)
      totals[i] += frame[header_length + i];

    fprintf(stderr, "\r%d frame(s)", frames);
    fflush(stderr);
  }

done_reading_frames:
  fprintf(stderr, "\n");
  fflush(stderr);

  for (long i = 0; i < width*height*3; ++i)
    frame[header_length + i] = (uint8_t) (totals[i] / frames);

  write(1, frame, header_length + width*height*3);
  return 0;
}
