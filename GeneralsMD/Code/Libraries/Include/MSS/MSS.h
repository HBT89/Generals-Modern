// MSS.h - Stub header for Miles Sound System
// This is a minimal stub to allow compilation without the proprietary Miles SDK.
// Replace with real MSS.h from RAD Game Tools for actual audio functionality.

#pragma once
#ifndef __MSS_STUB_H__
#define __MSS_STUB_H__

#include <windows.h>

// Miles calling convention
#define AILCALLBACK __cdecl

// Miles integer types
typedef unsigned int U32;
typedef int          S32;

// Filter/processor constants
#define DP_FILTER 0

// Handle types
typedef void* HSAMPLE;
typedef void* HPRESAMPLEFILTER;   // must match HPROVIDER for AIL_set_sample_processor calls
typedef void* H3DSAMPLE;
typedef void* HSTREAM;
typedef void* HDIGDRIVER;
typedef void* H3DPOBJECT;
typedef void* HPROVIDER;
typedef void* HAUDIO;

// Sound info structure
typedef struct {
    int format;
    void* data_ptr;
    unsigned int data_len;
    unsigned int rate;
    int bits;
    int channels;
    unsigned int samples;
    unsigned int block_size;
    void* initial_ptr;
} AILSOUNDINFO;

// Stub function declarations - these all return failure/null by default
#ifdef __cplusplus
extern "C" {
#endif

// Initialization
inline int AIL_startup(void) { return 0; }
inline void AIL_shutdown(void) {}
inline void AIL_set_preference(int, int) {}

// Digital driver
inline HDIGDRIVER AIL_open_digital_driver(unsigned int freq, int bits, int channels, unsigned int flags) { return NULL; }
inline void AIL_close_digital_driver(HDIGDRIVER) {}

// Sample management
inline HSAMPLE AIL_allocate_sample_handle(HDIGDRIVER) { return NULL; }
inline void AIL_release_sample_handle(HSAMPLE) {}
inline void AIL_init_sample(HSAMPLE) {}
inline void AIL_set_sample_file(HSAMPLE, void*, int) {}
inline void AIL_set_sample_address(HSAMPLE, void*, unsigned int) {}
inline void AIL_set_sample_type(HSAMPLE, int, int) {}
inline void AIL_set_sample_playback_rate(HSAMPLE, int) {}
inline void AIL_set_sample_volume(HSAMPLE, int) {}
inline void AIL_set_sample_pan(HSAMPLE, int) {}
inline void AIL_set_sample_loop_count(HSAMPLE, int) {}
inline void AIL_start_sample(HSAMPLE) {}
inline void AIL_stop_sample(HSAMPLE) {}
inline void AIL_resume_sample(HSAMPLE) {}
inline void AIL_end_sample(HSAMPLE) {}
inline unsigned int AIL_sample_status(HSAMPLE) { return 0; }
inline unsigned int AIL_sample_position(HSAMPLE) { return 0; }
inline void AIL_set_sample_position(HSAMPLE, unsigned int) {}

// 3D sample management
inline H3DSAMPLE AIL_allocate_3D_sample_handle(HPROVIDER) { return NULL; }
inline void AIL_release_3D_sample_handle(H3DSAMPLE) {}
inline void AIL_set_3D_sample_file(H3DSAMPLE, void*) {}
inline void AIL_start_3D_sample(H3DSAMPLE) {}
inline void AIL_stop_3D_sample(H3DSAMPLE) {}
inline void AIL_resume_3D_sample(H3DSAMPLE) {}
inline void AIL_end_3D_sample(H3DSAMPLE) {}
inline unsigned int AIL_3D_sample_status(H3DSAMPLE) { return 0; }
inline void AIL_set_3D_position(H3DSAMPLE, float, float, float) {}
inline void AIL_set_3D_velocity(H3DSAMPLE, float, float, float) {}
inline void AIL_set_3D_sample_volume(H3DSAMPLE, int) {}
inline void AIL_set_3D_sample_distances(H3DSAMPLE, float, float) {}
inline void AIL_set_3D_sample_loop_count(H3DSAMPLE, int) {}

// Stream management
inline HSTREAM AIL_open_stream(HDIGDRIVER, const char*, int) { return NULL; }
inline void AIL_close_stream(HSTREAM) {}
inline void AIL_start_stream(HSTREAM) {}
inline void AIL_pause_stream(HSTREAM, int) {}
inline unsigned int AIL_stream_status(HSTREAM) { return 0; }
inline void AIL_set_stream_volume(HSTREAM, int) {}
inline void AIL_set_stream_pan(HSTREAM, int) {}
inline void AIL_set_stream_loop_count(HSTREAM, int) {}
inline int AIL_stream_position(HSTREAM) { return 0; }
inline void AIL_set_stream_position(HSTREAM, int) {}
// AIL_set_stream_ms_position and AIL_stream_ms_position use 3-arg signatures defined in C++ section below

// Provider enumeration types
typedef int HPROENUM;
#define HPROENUM_FIRST 0

// DirectSound integration
typedef void* AILLPDIRECTSOUND;

// 3D provider management
inline HPROVIDER AIL_open_3D_provider(HDIGDRIVER) { return NULL; }
inline void AIL_close_3D_provider(HPROVIDER) {}
inline H3DPOBJECT AIL_open_3D_listener(HPROVIDER) { return NULL; }
inline void AIL_close_3D_listener(H3DPOBJECT) {}
inline void AIL_set_3D_provider_preference(HPROVIDER, int, void*) {}
inline int AIL_enumerate_3D_providers(HPROENUM*, HPROVIDER*, char**) { return 0; }
inline const char* AIL_3D_provider_attribute(HPROVIDER, const char*) { return ""; }

// Listener
inline void AIL_set_3D_listener_position(H3DPOBJECT, float, float, float) {}
inline void AIL_set_3D_listener_velocity(H3DPOBJECT, float, float, float) {}
inline void AIL_set_3D_listener_orient(H3DPOBJECT, float, float, float, float, float, float) {}

// Audio utility
inline int AIL_WAV_info(void*, AILSOUNDINFO*) { return 0; }
inline int AIL_decompress_ADPCM(AILSOUNDINFO*, void**, unsigned int*) { return 0; }
inline void AIL_mem_free_lock(void*) {}
inline const char* AIL_last_error(void) { return "Miles Sound System not available (stub)"; }

// Timer
typedef void (*AILTIMERCB)(unsigned int);
inline unsigned int AIL_register_timer(AILTIMERCB) { return 0; }
inline void AIL_set_timer_period(unsigned int, unsigned int) {}
inline void AIL_start_timer(unsigned int) {}
inline void AIL_stop_timer(unsigned int) {}
inline void AIL_release_timer_handle(unsigned int) {}

// Speaker types
#define AIL_3D_2_SPEAKER  0
#define AIL_3D_HEADPHONE  1
#define AIL_3D_SURROUND   2
#define AIL_3D_4_SPEAKER  3
#define AIL_3D_51_SPEAKER 4
#define AIL_3D_71_SPEAKER 5

// Sample status values
#define SMP_FREE      0
#define SMP_DONE      1
#define SMP_PLAYING   2
#define SMP_STOPPED   3
#define SMP_PLAYINGBUTRELEASED 4

// Filter types
#define DSPFILTER_NONE 0

// Callback types
typedef void (AILCALLBACK *AILSAMPLECB)(HSAMPLE);
typedef void (AILCALLBACK *AIL3DSAMPLECB)(H3DSAMPLE);
typedef void (AILCALLBACK *AILSTREAMCB)(HSTREAM);
typedef U32  (AILCALLBACK *AIL_FILE_OPEN_CALLBACK)(char const*, U32*);
typedef void (AILCALLBACK *AIL_FILE_CLOSE_CALLBACK)(U32);
typedef S32  (AILCALLBACK *AIL_FILE_SEEK_CALLBACK)(U32, S32, U32);
typedef U32  (AILCALLBACK *AIL_FILE_READ_CALLBACK)(U32, void*, U32);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
// Additional C++ stubs (outside extern "C")

// Callback registration
inline void AIL_register_EOS_callback(HSAMPLE, AILSAMPLECB) {}
inline void AIL_register_3D_EOS_callback(H3DSAMPLE, AIL3DSAMPLECB) {}
inline void AIL_register_stream_callback(HSTREAM, AILSTREAMCB) {}
inline void AIL_set_file_callbacks(AIL_FILE_OPEN_CALLBACK, AIL_FILE_CLOSE_CALLBACK, AIL_FILE_SEEK_CALLBACK, AIL_FILE_READ_CALLBACK) {}

// Volume/pan functions
inline void AIL_sample_volume_pan(HSAMPLE, float*, float*) {}
inline void AIL_set_sample_volume_pan(HSAMPLE, float, float) {}
inline void AIL_stream_volume_pan(HSTREAM, float*, float*) {}
inline void AIL_set_stream_volume_pan(HSTREAM, float, float) {}
inline int  AIL_stream_loop_count(HSTREAM) { return 0; }
inline void AIL_set_stream_ms_position(HSTREAM, long, long*) {}
inline void AIL_stream_ms_position(HSTREAM, long*, long*) {}

// Playback rate (AIL_set_sample_playback_rate defined in extern "C" above - no duplicate here)
inline int  AIL_sample_playback_rate(HSAMPLE) { return 22050; }
inline int  AIL_3D_sample_playback_rate(H3DSAMPLE) { return 22050; }
inline void AIL_set_3D_sample_playback_rate(H3DSAMPLE, int) {}
inline void AIL_set_3D_sample_occlusion(H3DSAMPLE, float) {}

// User data
inline void  AIL_set_sample_user_data(HSAMPLE, unsigned, int) {}
inline int   AIL_sample_user_data(HSAMPLE, unsigned) { return 0; }
inline void  AIL_set_3D_user_data(H3DSAMPLE, unsigned, int) {}
inline int   AIL_3D_user_data(H3DSAMPLE, unsigned) { return 0; }

// Sample processor/filter
inline void  AIL_set_sample_processor(HSAMPLE, int, HPRESAMPLEFILTER) {}
inline void  AIL_set_filter_sample_preference(HSAMPLE, const char*, void*) {}
inline int   AIL_enumerate_filters(HPROENUM*, HPROVIDER*, char**) { return 0; }
inline void  AIL_set_3D_speaker_type(HPROVIDER, int) {}
inline void  AIL_set_3D_orientation(H3DPOBJECT, float, float, float, float, float, float) {}

// Quick sound API
inline HAUDIO AIL_quick_load_and_play(const char*, unsigned, int) { return NULL; }
inline void   AIL_quick_unload(HAUDIO) {}
inline void   AIL_quick_set_volume(HAUDIO, float, float) {}
inline int    AIL_quick_startup(int, int, unsigned int, int, int) { return 1; }
inline void   AIL_quick_handles(HDIGDRIVER*, void*, void*) {}

// Misc
inline void   AIL_MSS_version(char*, int) {}
inline int    AIL_get_timer_highest_delay(void) { return 0; }
inline void   AIL_get_DirectSound_info(HSAMPLE, void*, void*) {}
inline void   AIL_set_redist_directory(const char*) {}

// Open/close 3D (AIL_open_3D_provider defined in extern "C" above)
inline HDIGDRIVER AIL_open_digital_driver_ex(unsigned, int, int, unsigned, int) { return NULL; }

#endif // __cplusplus

#endif // __MSS_STUB_H__
