# 100% Objectively Accurate Video Thumbnailer (TM)
...by using youtube-dl and ffmpeg.

This is the source for [Linux data science part 6](https://youtu.be/pNuLkEG8RIg).

```sh
$ ./thumbnail 'https://www.youtube.com/watch?v=pNuLkEG8RIg' > accurate.ppm
$ ./thumbnail some-file.flv > accurate.ppm

# use ImageMagick (sudo apt install imagemagick) to compress as jpg,
# although no longer 100% Objectively Accurate (TM)
$ convert accurate.ppm mostly-accurate.jpg
```
