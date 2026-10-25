#import "ryolib/audio/stub/AudioEngine.h"

@implementation RYAudioEngineStub

- (instancetype)initBackendWithError:(RYError **)error {
    self = [super initCommon];

    if (!self) {
        return nil;
    }

    _backend = RYAudioEngineBackendStub;

    // Add non-stub initialization here

    return self;
}

- (BOOL)applyBackendMasterVolume:(float)volume
                           error:(RYError **)error {
    // Add non-stub master volume setting here

    return YES;
}

@end
