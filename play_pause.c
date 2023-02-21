#include<stdio.h>
#include<string.h>
#include<gst/gst.h>
#include<pthread.h>
/*
       1) ask for the video file path
       2) play the given file
       3) provide a play/pause option
*/
GstElement *pipeline;
void* keyboard_handler(GstElement *pipeline)
{
  char input;
  gboolean var=TRUE;
  GstState state;
  printf("press 'p' to pause or play\n");
  while(var) {
   scanf("%c",&input);
   switch(input)
   {
    case 'p': gst_element_get_state(pipeline, &state, NULL, GST_CLOCK_TIME_NONE);
             if (state == GST_STATE_PLAYING) {
                gst_element_set_state(pipeline, GST_STATE_PAUSED);
            } else if (state == GST_STATE_PAUSED) {
                gst_element_set_state(pipeline, GST_STATE_PLAYING);
            }
            break;
    default :
            break;
   }
  }
 return NULL;
}

int main(int argc, char *argv[])
{
  
    GstBus *bus;
    GstMessage *msg;
    gboolean var=TRUE;
    int input;
    /* Initialize GStreamer */
    gst_init (&argc, &argv);
    char path[100];
    char desc[1000];
    printf("Enter the path of the video: ");
    scanf("%s",path);
    sprintf(desc, "playbin uri=file://%s", path);
    pipeline = gst_parse_launch(desc,NULL);
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    pthread_t thread;
    pthread_create(&thread, NULL, keyboard_handler, pipeline);
    /* Wait until error or EOS */
    bus = gst_element_get_bus (pipeline);
    msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR |  GST_MESSAGE_EOS);
    /* Parse message */
    if (msg != NULL) {
      GError *err;
      gchar *debug_info;

      switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error (msg, &err, &debug_info);
            g_printerr ("Error received from element %s: %s\n",
            GST_OBJECT_NAME (msg->src), err->message);
            g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error (&err);
            g_free (debug_info);
            break;
        case GST_MESSAGE_EOS:
            g_print ("End-Of-Stream reached.\n");
            break;
        default:
          /* We should not reach here because we only asked for ERRORs and EOS */
          g_printerr ("Unexpected message received.\n");
          break;
    }
    gst_message_unref (msg);
  }
 
  /* Free resources */
  gst_message_unref (msg);
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  return 0;
}
