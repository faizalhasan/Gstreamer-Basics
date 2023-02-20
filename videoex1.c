/*code to play the video stored in local directory by giving the path of the file through command prompt*/
#include <stdio.h>
#include<gst/gst.h>

int main(int argc,char *argv[]) {
    // Your GStreamer code to play the video goes here
  GstElement *pipeline;
  GstBus *bus;
  GstMessage *msg;

  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  /* Build the pipeline - it plays a video file which is stored in my local directory*/
  /*playbin does all the work for creating source, sink elements etc*/
  pipeline = gst_parse_launch("playbin uri=file:///home/faizalhasan/Downloads/pexels-mart-production-7565438.mp4",NULL);

  /* Start playing */
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  /* Wait until error or EOS */
  bus = gst_element_get_bus (pipeline);
  msg =gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
  
  if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR) {
    g_error ("An error occurred! Re-run with the GST_DEBUG=*:WARN environment "
        "variable set for more details.");
  }

  /* Free resources */
  gst_message_unref (msg);
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
    return 0;
}
