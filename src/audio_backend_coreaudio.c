#include "audio_backend.h"
#include <AudioToolbox/AudioToolbox.h>

static AudioUnit au;
static AudioCallback synth_cb;
static AudioBackendConfig config;

static OSStatus render_callback(void *inRefCon,
                                AudioUnitRenderActionFlags *flags,
                                const AudioTimeStamp *ts, UInt32 bus,
                                UInt32 frames, AudioBufferList *outBuf) {
    void *cb_user_data = inRefCon;
    (void)flags;
    (void)ts;
    (void)bus;
    UInt32 numBuffers = outBuf->mNumberBuffers;

    // fprintf(stderr, "outBuf->mNumberBuffers = %u\n",
    //         (unsigned)outBuf->mNumberBuffers);
    // for (UInt32 i = 0; i < outBuf->mNumberBuffers; ++i)
    //     fprintf(stderr,
    //             "mBuffers[%u]: data=%p, channelsPerFrame=%u,
    //             dataByteSize=%u\n", (unsigned)i, outBuf->mBuffers[i].mData,
    //             config.channels,
    //             (unsigned)outBuf->mBuffers[i].mDataByteSize);

    if (numBuffers > 8)
        numBuffers = 8;

    float *channels[8];
    for (UInt32 i = 0; i < numBuffers; i++) {
        channels[i] = (float *)outBuf->mBuffers[i].mData;
    }
    synth_cb(channels, frames, cb_user_data);
    return noErr;
}

int audio_backend_init(const AudioBackendConfig *cfg) {
    config = *cfg;
    if (config.channels > 8)
        return -1;

    AudioComponentDescription description;
    description.componentType = kAudioUnitType_Output;
    description.componentSubType = kAudioUnitSubType_DefaultOutput;
    description.componentManufacturer = kAudioUnitManufacturer_Apple;

    AudioComponent comp = AudioComponentFindNext(NULL, &description);
    if (!comp)
        return -1;

    if (AudioComponentInstanceNew(comp, &au) != noErr)
        return -1;

    AudioStreamBasicDescription fmt = {0};
    fmt.mSampleRate = config.sample_rate;
    fmt.mFormatID = kAudioFormatLinearPCM;
    fmt.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked |
                       kAudioFormatFlagIsNonInterleaved;
    fmt.mChannelsPerFrame = config.channels;
    fmt.mFramesPerPacket = 1;
    fmt.mBitsPerChannel = 8 * sizeof(float);
    fmt.mBytesPerFrame = sizeof(float);
    fmt.mBytesPerPacket = fmt.mBytesPerFrame;
    AudioUnitSetProperty(au, kAudioUnitProperty_StreamFormat,
                         kAudioUnitScope_Input, 0, &fmt, sizeof(fmt));
    if (AudioUnitInitialize(au) != noErr)
        return -1;
    // AudioStreamBasicDescription actual;
    // UInt32 sz = sizeof(actual);
    // AudioUnitGetProperty(au, kAudioUnitProperty_StreamFormat,
    //                      kAudioUnitScope_Input, 0, &actual, &sz);
    //
    // fprintf(stderr, "Device format: %.0f Hz %.0f bytes/frame %u ch\n",
    //         actual.mSampleRate, actual.mBytesPerFrame,
    //         actual.mChannelsPerFrame);
    //
    return 0;
}

void audio_backend_set_callback(AudioCallback cb, void *user_data) {
    synth_cb = cb;

    AURenderCallbackStruct au_cb = {.inputProc = render_callback,
                                    .inputProcRefCon = user_data};
    AudioUnitSetProperty(au, kAudioUnitProperty_SetRenderCallback,
                         kAudioUnitScope_Input, 0, &au_cb, sizeof(au_cb));
}
void audio_backend_start(void) {
    if (!synth_cb) {
        fprintf(stderr, "audio_backen_start(): no render callback set!\n");
        return;
    }
    AudioOutputUnitStart(au);
}
void audio_backend_stop(void) { AudioOutputUnitStop(au); }
void audio_backend_shutdown(void) {
    AudioOutputUnitStop(au);
    AudioUnitUninitialize(au);
    AudioComponentInstanceDispose(au);
    au = NULL;
    synth_cb = NULL;
}
