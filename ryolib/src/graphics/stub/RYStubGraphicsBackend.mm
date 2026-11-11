#include "RYStubGraphicsBackend.h"

@implementation RYStubGraphicsBackend

- (instancetype)initBackendWithConfig:(const RYRendererConfig *)config
                         shareBackend:(RYGraphicsBackend *)shareBackend
                                error:(RYError **)error {
    self = [super init];
    if (!self) {
        return nil;
    }

    if (error) {
        *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsFeatureNotSupported
                                       message:"Graphics backend is not available"];
    }

    [self release];
    return nil;
}

- (RYGraphicsBackendType)backendType {
    return RYGraphicsBackendStub;
}

@end
