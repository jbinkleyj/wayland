
# rm *.dot

gst-launch-1.0 webmmux name=mux ! filesink location=voko-1.mkv \
   uridecodebin uri=file:///home/vk/wayland/xdg-portal/build-src-Desktop-Release/vokoscreenNG.mkv name=demux \
   demux. ! videoconvert ! vp8enc ! queue ! mux.video_0
exit





gst-launch-1.0 videotestsrc pattern=5 ! \
  video/x-raw, framerate=10/1, width=320, height=240 ! \
  compositor name=comp ! videoconvert ! xvimagesink \
  pipewiresrc fd=11 path=21 do-timestamp=true \
  ! video/x-raw, framerate=10/1, width=160, height=120 \
  ! .comp

#  v4l2src device=/dev/video1 !  \
#  video/x-raw, framerate=10/1, width=160, height=120 ! comp. 
exit


gst-launch-1.0 -v videotestsrc num-buffers=1000 \
   ! video/x-raw, framerate=25/1 \
   ! compositor name=comp ! videoconvert \
   ! x264enc qp-min=18 \
   ! matroskamux \
   ! filesink location=videotestsrc.mkv
exit 


gst-launch-1.0 -e \
 videotestsrc \
  ! video/x-raw, framerate=25/1, width=1920, height=1080 \
  ! compositor name=comp \
  ! videoconvert \
  ! x264enc qp-min=18 \
  ! matroskamux \
  ! filesink location=videotestsrc.mkv \
 pipewiresrc fd=11 path=30 do-timestamp=true \
  ! queue \
  ! .comp
exit

 
gst-launch-1.0 videotestsrc pattern=5 ! \
  video/x-raw, framerate=\(fraction\)10/1, width=320, height=240 ! \
  compositor name=comp ! videoconvert ! ximagesink \
  videotestsrc !  \
  video/x-raw, framerate=\(fraction\)10/1, width=320, height=240 ! comp. 
exit

gst-launch-1.0 -e -v filesrc location=vokoscreenNG.mkv ! matroskademux ! h264parse ! x264enc ! videorate silent=false ! matroskamux name=mux ! filesink location=out.mkv
exit
