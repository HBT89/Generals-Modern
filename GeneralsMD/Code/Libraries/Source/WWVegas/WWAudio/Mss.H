#pragma once
#ifndef MSS_H
#define MSS_H

// Miles Sound System stub - proprietary library removed
// Comprehensive declarations for compilation

#include <cstddef>
#include <windows.h>

// ---------------------------------------------------------------------------
// Basic Miles types
// ---------------------------------------------------------------------------
typedef unsigned long U32;
typedef signed long S32;
typedef float F32;

// ---------------------------------------------------------------------------
// Callback convention
// ---------------------------------------------------------------------------
#define AILCALLBACK __stdcall

// ---------------------------------------------------------------------------
// Handle typedefs (use unsigned long to match MILES_HANDLE)
// ---------------------------------------------------------------------------
typedef unsigned long HAUDIO;
typedef unsigned long HSTREAM;
typedef unsigned long HSAMPLE;
typedef unsigned long H3DPOBJECT;
typedef unsigned long H3DSAMPLE;
typedef unsigned long HPROVIDER;
typedef unsigned long HATTRIB;
typedef unsigned long HTIMER;
typedef unsigned long HMDIDRIVER;
typedef unsigned long HDLSDEVICE;
typedef unsigned long HPROENUM;

// HDIGDRIVER is a pointer to a struct so code can access emulated_ds field
struct HDIGDRIVER_STRUCT {
    int emulated_ds;
};
typedef HDIGDRIVER_STRUCT* HDIGDRIVER;

// NOTE: MILES_HANDLE is intentionally NOT defined here.
// It is defined in audiblesound.h as: typedef unsigned long MILES_HANDLE

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------
#define SMP_PLAYING  4
#define SMP_DONE     2
#define SMP_STOPPED  1
#define SMP_FREE     0

#define AILFILETYPE_UNKNOWN   0
#define AILFILETYPE_PCM_WAV   1
#define AILFILETYPE_ADPCM_WAV 2
#define AILFILETYPE_OTHER     100

#define DIG_USE_WAVEOUT       0
#define AIL_LOCK_PROTECTION   1
#define AIL_NO_ERROR          0
#define DP_FILTER             0
#define M3D_NOERR             0

#define HPROENUM_FIRST        ((HPROENUM)0)

// Boolean preference values
#define YES                   1
#define NO                    0

// Wave format
#ifndef WAVE_FORMAT_IMA_ADPCM
#define WAVE_FORMAT_IMA_ADPCM 0x0011
#endif

// EAX environment presets
#define ENVIRONMENT_GENERIC   0

// 3D speaker types
#define AIL_3D_2_SPEAKER      0
#define AIL_3D_HEADPHONE      1
#define AIL_3D_SURROUND       2
#define AIL_3D_4_SPEAKER      3

// File seek constants
#define AIL_FILE_SEEK_BEGIN   0
#define AIL_FILE_SEEK_CURRENT 1
#define AIL_FILE_SEEK_END     2

// ---------------------------------------------------------------------------
// Structures
// ---------------------------------------------------------------------------
typedef struct
{
    int format;
    int data_len;
    int rate;
    int bits;
    int channels;
    int samples;
    int block_size;
    void* data_ptr;
} AILSOUNDINFO;

typedef struct
{
    char name[128];
} AILFILTERDESC;

// ---------------------------------------------------------------------------
// Core functions
// ---------------------------------------------------------------------------

inline int AIL_startup(void) { return 0; }
inline void AIL_shutdown(void) {}
inline int AIL_set_preference(unsigned int number, int value) { (void)number; (void)value; return 0; }
inline const char* AIL_last_error(void) { return ""; }
inline void AIL_lock(void) {}
inline void AIL_unlock(void) {}

// ---------------------------------------------------------------------------
// File callback registration
// ---------------------------------------------------------------------------
typedef U32 (AILCALLBACK *AIL_file_open_callback)(char const*, U32*);
typedef void (AILCALLBACK *AIL_file_close_callback)(U32);
typedef S32 (AILCALLBACK *AIL_file_seek_callback)(U32, S32, U32);
typedef U32 (AILCALLBACK *AIL_file_read_callback)(U32, void*, U32);

inline void AIL_set_file_callbacks(
    AIL_file_open_callback open_cb,
    AIL_file_close_callback close_cb,
    AIL_file_seek_callback seek_cb,
    AIL_file_read_callback read_cb)
{
    (void)open_cb; (void)close_cb; (void)seek_cb; (void)read_cb;
}

// ---------------------------------------------------------------------------
// Digital driver functions
// ---------------------------------------------------------------------------

inline S32 AIL_waveOutOpen(HDIGDRIVER* driver, void* hw, S32 waveDevice, void* format)
{
    (void)driver; (void)hw; (void)waveDevice; (void)format;
    return AIL_NO_ERROR;
}

inline void AIL_waveOutClose(HDIGDRIVER driver) { (void)driver; }

inline HDIGDRIVER AIL_open_digital_driver(unsigned int frequency, int bits, int channel, unsigned int flags)
{
    (void)frequency; (void)bits; (void)channel; (void)flags;
    return 0;
}

inline void AIL_close_digital_driver(HDIGDRIVER driver) { (void)driver; }

// ---------------------------------------------------------------------------
// 2D Sample functions
// ---------------------------------------------------------------------------

inline HSAMPLE AIL_allocate_sample_handle(HDIGDRIVER driver) { (void)driver; return 0; }
inline void AIL_release_sample_handle(HSAMPLE sample) { (void)sample; }
inline void AIL_init_sample(HSAMPLE sample) { (void)sample; }
inline int AIL_set_sample_file(HSAMPLE sample, const void* file_image, int block) { (void)sample; (void)file_image; (void)block; return 0; }
inline int AIL_set_named_sample_file(HSAMPLE sample, const char* name, const void* file_image, int size, int block) { (void)sample; (void)name; (void)file_image; (void)size; (void)block; return 0; }
inline void AIL_set_sample_address(HSAMPLE sample, const void* data, unsigned int size) { (void)sample; (void)data; (void)size; }
inline void AIL_start_sample(HSAMPLE sample) { (void)sample; }
inline void AIL_stop_sample(HSAMPLE sample) { (void)sample; }
inline void AIL_resume_sample(HSAMPLE sample) { (void)sample; }
inline void AIL_end_sample(HSAMPLE sample) { (void)sample; }
inline unsigned int AIL_sample_status(HSAMPLE sample) { (void)sample; return SMP_DONE; }

inline void AIL_set_sample_volume(HSAMPLE sample, int volume) { (void)sample; (void)volume; }
inline void AIL_set_sample_pan(HSAMPLE sample, int pan) { (void)sample; (void)pan; }
inline void AIL_set_sample_playback_rate(HSAMPLE sample, int rate) { (void)sample; (void)rate; }
inline void AIL_set_sample_loop_count(HSAMPLE sample, int count) { (void)sample; (void)count; }
inline void AIL_set_sample_ms_position(HSAMPLE sample, int ms) { (void)sample; (void)ms; }
inline void AIL_set_sample_user_data(HSAMPLE sample, U32 index, S32 data) { (void)sample; (void)index; (void)data; }

inline int AIL_sample_volume(HSAMPLE sample) { (void)sample; return 0; }
inline int AIL_sample_pan(HSAMPLE sample) { (void)sample; return 0; }
inline int AIL_sample_playback_rate(HSAMPLE sample) { (void)sample; return 0; }
inline int AIL_sample_loop_count(HSAMPLE sample) { (void)sample; return 0; }
inline void AIL_sample_ms_position(HSAMPLE sample, S32* total, S32* current) { (void)sample; if(total) *total=0; if(current) *current=0; }
inline S32 AIL_sample_user_data(HSAMPLE sample, U32 index) { (void)sample; (void)index; return 0; }

// ---------------------------------------------------------------------------
// Stream functions
// ---------------------------------------------------------------------------

inline HSTREAM AIL_open_stream(HDIGDRIVER driver, const char* filename, int stream_mem) { (void)driver; (void)filename; (void)stream_mem; return 0; }
inline HSTREAM AIL_open_stream_by_sample(HDIGDRIVER driver, HSAMPLE sample, const char* filename, int stream_mem) { (void)driver; (void)sample; (void)filename; (void)stream_mem; return 0; }
inline void AIL_close_stream(HSTREAM stream) { (void)stream; }
inline void AIL_start_stream(HSTREAM stream) { (void)stream; }
inline void AIL_pause_stream(HSTREAM stream, int onoff) { (void)stream; (void)onoff; }
inline unsigned int AIL_stream_status(HSTREAM stream) { (void)stream; return SMP_DONE; }

inline void AIL_set_stream_volume(HSTREAM stream, int volume) { (void)stream; (void)volume; }
inline void AIL_set_stream_pan(HSTREAM stream, int pan) { (void)stream; (void)pan; }
inline void AIL_set_stream_loop_count(HSTREAM stream, int count) { (void)stream; (void)count; }
inline void AIL_set_stream_loop_block(HSTREAM stream, int start, int end) { (void)stream; (void)start; (void)end; }
inline void AIL_set_stream_ms_position(HSTREAM stream, int ms) { (void)stream; (void)ms; }
inline void AIL_set_stream_playback_rate(HSTREAM stream, int rate) { (void)stream; (void)rate; }

inline int AIL_stream_volume(HSTREAM stream) { (void)stream; return 0; }
inline int AIL_stream_pan(HSTREAM stream) { (void)stream; return 0; }
inline int AIL_stream_position(HSTREAM stream) { (void)stream; return 0; }
inline int AIL_stream_loop_count(HSTREAM stream) { (void)stream; return 0; }
inline void AIL_stream_ms_position(HSTREAM stream, S32* total, S32* current) { (void)stream; if(total) *total=0; if(current) *current=0; }
inline int AIL_stream_playback_rate(HSTREAM stream) { (void)stream; return 0; }
inline void AIL_service_stream(HSTREAM stream, int fillup) { (void)stream; (void)fillup; }

// ---------------------------------------------------------------------------
// 3D audio functions
// ---------------------------------------------------------------------------

inline void AIL_set_3D_position(H3DPOBJECT obj, float x, float y, float z) { (void)obj; (void)x; (void)y; (void)z; }
inline void AIL_set_3D_velocity(H3DPOBJECT obj, float dx, float dy, float dz, float magnitude) { (void)obj; (void)dx; (void)dy; (void)dz; (void)magnitude; }
inline void AIL_set_3D_velocity_vector(H3DPOBJECT obj, float dx, float dy, float dz) { (void)obj; (void)dx; (void)dy; (void)dz; }
inline void AIL_set_3D_orientation(H3DPOBJECT obj, float fx, float fy, float fz, float ux, float uy, float uz) { (void)obj; (void)fx; (void)fy; (void)fz; (void)ux; (void)uy; (void)uz; }
inline void AIL_set_listener_3D_position(HDIGDRIVER driver, float x, float y, float z) { (void)driver; (void)x; (void)y; (void)z; }

inline int AIL_open_3D_provider(HPROVIDER provider) { (void)provider; return 0; }
inline void AIL_close_3D_provider(HPROVIDER provider) { (void)provider; }
inline H3DPOBJECT AIL_3D_open_listener(HPROVIDER provider) { (void)provider; return 0; }

inline H3DSAMPLE AIL_allocate_3D_sample_handle(HPROVIDER provider) { (void)provider; return 0; }
inline void AIL_release_3D_sample_handle(H3DSAMPLE sample) { (void)sample; }
inline int AIL_set_3D_sample_file(H3DSAMPLE sample, const void* file_image) { (void)sample; (void)file_image; return 0; }
inline void AIL_start_3D_sample(H3DSAMPLE sample) { (void)sample; }
inline void AIL_stop_3D_sample(H3DSAMPLE sample) { (void)sample; }
inline void AIL_resume_3D_sample(H3DSAMPLE sample) { (void)sample; }
inline void AIL_end_3D_sample(H3DSAMPLE sample) { (void)sample; }
inline unsigned int AIL_3D_sample_status(H3DSAMPLE sample) { (void)sample; return SMP_DONE; }

inline void AIL_set_3D_sample_volume(H3DSAMPLE sample, int volume) { (void)sample; (void)volume; }
inline void AIL_set_3D_sample_distances(H3DSAMPLE sample, float max_dist, float min_dist) { (void)sample; (void)max_dist; (void)min_dist; }
inline void AIL_set_3D_sample_loop_count(H3DSAMPLE sample, int count) { (void)sample; (void)count; }
inline void AIL_set_3D_sample_offset(H3DSAMPLE sample, U32 offset) { (void)sample; (void)offset; }
inline void AIL_set_3D_sample_playback_rate(H3DSAMPLE sample, int rate) { (void)sample; (void)rate; }
inline void AIL_set_3D_sample_effects_level(H3DSAMPLE sample, float level) { (void)sample; (void)level; }
inline void AIL_set_3D_object_user_data(H3DSAMPLE sample, U32 index, S32 data) { (void)sample; (void)index; (void)data; }

inline int AIL_3D_sample_volume(H3DSAMPLE sample) { (void)sample; return 0; }
inline U32 AIL_3D_sample_offset(H3DSAMPLE sample) { (void)sample; return 0; }
inline U32 AIL_3D_sample_length(H3DSAMPLE sample) { (void)sample; return 0; }
inline int AIL_3D_sample_playback_rate(H3DSAMPLE sample) { (void)sample; return 0; }
inline S32 AIL_3D_object_user_data(H3DSAMPLE sample, U32 index) { (void)sample; (void)index; return 0; }

// ---------------------------------------------------------------------------
// Provider/Filter enumeration
// ---------------------------------------------------------------------------

inline int AIL_enumerate_3D_providers(HPROVIDER* next, HPROVIDER* dest, char** name) { (void)next; (void)dest; (void)name; return 0; }
inline int AIL_enumerate_filters(HPROENUM* next, HPROVIDER* dest, char** name) { (void)next; (void)dest; (void)name; return 0; }
inline void AIL_set_filter_sample_preference(H3DSAMPLE sample, const char* name, void* value) { (void)sample; (void)name; (void)value; }

// ---------------------------------------------------------------------------
// Timer functions
// ---------------------------------------------------------------------------

typedef void (AILCALLBACK *AILTIMERCB)(U32 user);

inline HTIMER AIL_allocate_timer_handle(void) { return 0; }
inline void AIL_set_timer_frequency(HTIMER timer, U32 freq) { (void)timer; (void)freq; }
inline void AIL_set_timer_user(HTIMER timer, U32 user) { (void)timer; (void)user; }
inline void AIL_start_timer(HTIMER timer) { (void)timer; }
inline void AIL_stop_timer(HTIMER timer) { (void)timer; }
inline void AIL_release_timer_handle(HTIMER timer) { (void)timer; }

// ---------------------------------------------------------------------------
// Utility functions
// ---------------------------------------------------------------------------

inline int AIL_WAV_info(const void* data, AILSOUNDINFO* info) { (void)data; (void)info; return 0; }
inline int AIL_file_type(const void* data, unsigned int size) { (void)data; (void)size; return AILFILETYPE_UNKNOWN; }

inline void* AIL_mem_alloc_lock(unsigned int size) { (void)size; return 0; }
inline void AIL_mem_free_lock(void* ptr) { (void)ptr; }

inline HPROVIDER AIL_set_sample_processor(HSAMPLE sample, int pipeline_stage, HPROVIDER provider) { (void)sample; (void)pipeline_stage; (void)provider; return 0; }
inline HPROVIDER AIL_set_stream_processor(HSTREAM stream, int pipeline_stage, HPROVIDER provider) { (void)stream; (void)pipeline_stage; (void)provider; return 0; }

// Additional missing functions
inline void AIL_set_3D_speaker_type(HPROVIDER provider, S32 type) { (void)provider; (void)type; }
inline S32 AIL_3D_sample_loop_count(H3DSAMPLE sample) { (void)sample; return 0; }


#endif // MSS_H
