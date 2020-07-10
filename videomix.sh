#export GST_DEBUG=5
gst-launch-1.0 -e -v \
        videotestsrc pattern=2 \
            ! video/x-raw, framerate=25/1, width=1920, height=1080 \
            ! mix. \
        pipewiresrc fd=11 path=67 do-timestamp=true \
            ! video/x-raw, framerate=25/1 \
            ! mix. \
        videomixer name=mix \
            ! videoconvert \
            ! videorate \
            ! video/x-raw,framerate=25/1 \
            ! x264enc key-int-max=25 qp-min=17 qp-max=17 speed-preset=superfast threads=4 \
            ! video/x-h264, profile=baseline \
            ! mux. \
        matroskamux name=mux \
            ! filesink location=vokoscreenNG-bad.mkv
        
exit

        ximagesrc display-name=:0 use-damage=false show-pointer=true startx=0 starty=0 endx=1919 endy=1079 \
        pipewiresrc fd=11 path=67 do-timestamp=true \
