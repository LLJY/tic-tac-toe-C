#include <gst/gst.h>
#include <include/sound.h>
#include <stdio.h>

const char *BGM_SND = "sweden.ogg";
const char *START_SND = "raidhorn_02.ogg";
const char *BTN_CLICK_SND = "Click_stereo.ogg";
const char *WIN_SND = "levelup.ogg";
const char *DRAW_SND = "say3.ogg";
const char *LOSE_SND = "Villager_deny1.ogg";
const char *SURRENDER_SND = "hurt1.ogg";
// Bus callback function to repeat the pipeline when stopped (for BGM)
static gboolean bus_callback_repeat(GstBus *bus, GstMessage *msg, gpointer data)
{
    GstElement *pipeline = (GstElement *)data;

    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_EOS:
    {
        // End-of-stream reached, restart the pipeline
        /* restart playback if at end */
        if (!gst_element_seek(data,
                              1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
                              GST_SEEK_TYPE_SET, 2000000000, // 2 seconds (in nanoseconds)
                              GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
        {
            g_print("Seek failed!\n");
        }
        break;
    }
    case GST_MESSAGE_ERROR:
    {
        // Handle errors
        GError *err;
        gchar *debug_info;
        gst_message_parse_error(msg, &err, &debug_info);
        fprintf(stderr, "ERROR: From element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
        fprintf(stderr, "Debugging information: %s\n", debug_info ? debug_info : "none");
        g_clear_error(&err);
        g_free(debug_info);

        gst_element_set_state(pipeline, GST_STATE_NULL);
        break;
    }
    default:
        break;
    }

    return TRUE;
}

// Bus callback function to handle messages from the pipeline
static gboolean bus_callback(GstBus *bus, GstMessage *msg, gpointer data)
{
    GstElement *pipeline = (GstElement *)data;

    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_ERROR:
    {
        // Handle errors
        GError *err;
        gchar *debug_info;
        gst_message_parse_error(msg, &err, &debug_info);
        fprintf(stderr, "ERROR: From element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
        fprintf(stderr, "Debugging information: %s\n", debug_info ? debug_info : "none");
        g_clear_error(&err);
        g_free(debug_info);

        gst_element_set_state(pipeline, GST_STATE_NULL);
        break;
    }
    default:
        break;
    }

    return TRUE;
}

void init_audio()
{
    gst_init(NULL, NULL);
}

void play_sound(const char *sound_file, bool isRepeat)
{
    // Create a GStreamer pipeline for playing the sound file
    char pipeline_string[256];
    sprintf(pipeline_string, "playbin uri=file://%s/%s", g_get_current_dir(), sound_file);
    GstElement *pipeline = gst_parse_launch(pipeline_string, NULL);
    if (pipeline == NULL)
    {
        fprintf(stderr, "ERROR: Failed to create GStreamer pipeline\n");
        return;
    }

    // Set the pipeline to the PLAYING state to start playback
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Add a bus watch to handle messages from the pipeline
    GstBus *bus = gst_element_get_bus(pipeline);
    if (isRepeat)
    {
        gst_bus_add_watch(bus, (GstBusFunc)bus_callback_repeat, pipeline);
    }
    else
    {
        gst_bus_add_watch(bus, (GstBusFunc)bus_callback, pipeline);
    }
    gst_object_unref(bus);
}
