#include<stdio.h>
#include<string.h>
#include<gst/gst.h>
#include<pthread.h>
/*
using structures
       1) ask for the video file path
       2) play the given file
       3) provide a play/pause option
       4) option for playing the new video file
       5)exit
*/
struct data
{
 GstElement *pipeline;
 GstBus *bus;
 GstMessage *msg;
};


void* keyboard_handler(struct data *ptr)
{
  char input;
  gboolean var=TRUE;
  GstState state;
  char path[100];
  char desc[1000];
  printf("press 'p' to pause or play\n");
 while(1) {
  scanf("%c",&input);

  switch(input)
  {
   case 'p': gst_element_get_state(ptr->pipeline, &state, NULL, GST_CLOCK_TIME_NONE);
             if (state == GST_STATE_PLAYING) {
                gst_element_set_state(ptr->pipeline, GST_STATE_PAUSED);
            } else if (state == GST_STATE_PAUSED) {
                gst_element_set_state(ptr->pipeline, GST_STATE_PLAYING);
            }
            break;
   case 'n': gst_element_set_state (ptr->pipeline, GST_STATE_NULL);
             gst_object_unref (ptr->pipeline);
             printf("Enter the path of the video: ");
             scanf("%s",path);
             sprintf(desc, "playbin uri=file://%s", path);
             ptr->pipeline = gst_parse_launch(desc,NULL);
             gst_element_set_state(ptr->pipeline, GST_STATE_PLAYING);
             break;
   case 'e':exit(0);
           break;
  default:
          break;
   }
 }
 return NULL;
}

int main(int argc, char *argv[])
{
    struct data ptr_s;
    struct data *ptr=&ptr_s;
    
    gboolean var=TRUE;
    int input;
    /* Initialize GStreamer */
    gst_init (&argc, &argv);
    char path[100];
    char desc[1000];
    printf("Enter the path of the video: ");
    scanf("%s",path);
    sprintf(desc, "playbin uri=file://%s", path);
    ptr->pipeline = gst_parse_launch(desc,NULL);
    gst_element_set_state(ptr->pipeline, GST_STATE_PLAYING);
    pthread_t thread;
    pthread_create(&thread, NULL, keyboard_handler, ptr);
   // pthread_join(&thread, NULL);
  /* Wait until error or EOS */
  ptr->bus = gst_element_get_bus (ptr->pipeline);
  ptr->msg = gst_bus_timed_pop_filtered (ptr->bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR |  GST_MESSAGE_EOS);
  /* Parse message */
  if (ptr->msg != NULL) {
    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE (ptr->msg)) {
      case GST_MESSAGE_ERROR:
        gst_message_parse_error (ptr->msg, &err, &debug_info);
        g_printerr ("Error received from element %s: %s\n",
            GST_OBJECT_NAME (ptr->msg->src), err->message);
        g_printerr ("Debugging information: %s\n",
            debug_info ? debug_info : "none");
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
    gst_message_unref (ptr->msg);
  }
 pthread_join(&thread, NULL);
  /* Free resources */
  gst_message_unref (ptr->msg);
  gst_object_unref (ptr->bus);
  gst_element_set_state (ptr->pipeline, GST_STATE_NULL);
  gst_object_unref (ptr->pipeline);
  return 0;
}
