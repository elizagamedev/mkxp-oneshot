
typedef int (*PluginInit)();
typedef double (*PluginInitD)();

typedef bool (*PluginIsPlatformSupported)();
typedef double (*PluginIsPlatformSupportedD)();

typedef bool (*PluginIsInitialized)();
typedef double (*PluginIsInitializedD)();

typedef bool (*PluginIsDialogOpen)();
typedef double (*PluginIsDialogOpenD)();

typedef int (*PluginOpenEditorDialog)(const char* path);
typedef double (*PluginOpenEditorDialogD)(const char* path);

typedef int (*PluginOpenEditorDialogAndPlay)(const char* path);
typedef double (*PluginOpenEditorDialogAndPlayD)(const char* path);

typedef int (*PluginOpenAnimation)(const char* path);
typedef double (*PluginOpenAnimationD)(const char* path);

typedef int (*PluginLoadAnimation)(int animationId);
typedef double (*PluginLoadAnimationD)(double animationId);

typedef int (*PluginUnloadAnimation)(int animationId);
typedef double (*PluginUnloadAnimationD)(double animationId);

typedef int (*PluginPlayAnimation)(int animationId);
typedef double (*PluginPlayAnimationD)(double animationId);

typedef bool (*PluginIsPlaying)(int animationId);
typedef double (*PluginIsPlayingD)(double animationId);

typedef int (*PluginStopAnimation)(int animationId);
typedef double (*PluginStopAnimationD)(double animationId);

typedef int (*PluginCloseAnimation)(int animationId);
typedef double (*PluginCloseAnimationD)(double animationId);

typedef int (*PluginUninit)();
typedef double (*PluginUninitD)();

typedef int (*PluginCreateAnimationInMemory)(int deviceType, int device);
typedef int (*PluginCreateAnimation)(const char* path, int deviceType, int device);
typedef int (*PluginSaveAnimation)(int animationId, const char* path);
typedef int (*PluginResetAnimation)(int animationId);

typedef int (*PluginGetDeviceType)(int animationId);
typedef int (*PluginGetDeviceTypeName)(const char* path);
typedef double (*PluginGetDeviceTypeNameD)(const char* path);

typedef int (*PluginGetDevice)(int animationId);
typedef int (*PluginGetDeviceName)(const char* path);
typedef double (*PluginGetDeviceNameD)(const char* path);

typedef int (*PluginSetDevice)(int animationId, int deviceType, int device);

typedef int (*PluginGetMaxLeds)(int device);
typedef double (*PluginGetMaxLedsD)(double device);

typedef int (*PluginGetMaxRow)(int device);
typedef double (*PluginGetMaxRowD)(double device);

typedef int (*PluginGetMaxColumn)(int device);
typedef double (*PluginGetMaxColumnD)(double device);

typedef int (*PluginAddFrame)(int animationId, float duration, int* colors, int length);
typedef int (*PluginUpdateFrame)(int animationId, int frameIndex, float duration, int* colors, int length);
typedef int (*PluginGetFrame)(int animationId, int frameIndex, float* duration, int* colors, int length);

typedef int (*PluginPreviewFrame)(int animationId, int frameIndex);
typedef double (*PluginPreviewFrameD)(double animationId, double frameIndex);

typedef int (*PluginOverrideFrameDuration)(int animationId, float duration);
typedef double (*PluginOverrideFrameDurationD)(double animationId, double duration);

typedef int (*PluginReverse)(int animationId);
typedef int (*PluginMirrorHorizontally)(int animationId);
typedef int (*PluginMirrorVertically)(int animationId);

typedef int (*PluginGetAnimation)(const char* name);
typedef double (*PluginGetAnimationD)(const char* name);

typedef void (*PluginCloseAnimationName)(const char* path);
typedef double (*PluginCloseAnimationNameD)(const char* path);

typedef void (*PluginPlayAnimationLoop)(int animationId, bool loop);
typedef void (*PluginPlayAnimationName)(const char* path, bool loop);
typedef double (*PluginPlayAnimationNameD)(const char* path, double loop);

typedef void (*PluginPlayAnimationFrame)(int animationId, int frameId, bool loop);
typedef void (*PluginPlayAnimationFrameName)(const char* path, int frameId, bool loop);
typedef double (*PluginPlayAnimationFrameNameD)(const char* path, double frameId, double loop);

typedef void (*PluginStopAnimationName)(const char* path);
typedef double (*PluginStopAnimationNameD)(const char* path);

typedef void (*PluginStopAnimationType)(int deviceType, int device);
typedef double (*PluginStopAnimationTypeD)(double deviceType, double device);

typedef bool (*PluginIsPlayingName)(const char* path);
typedef double (*PluginIsPlayingNameD)(const char* path);

typedef bool (*PluginIsPlayingType)(int deviceType, int device);
typedef double (*PluginIsPlayingTypeD)(double deviceType, double device);

typedef void (*PluginPlayComposite)(const char* name, bool loop);
typedef double (*PluginPlayCompositeD)(const char* name, double loop);

typedef void (*PluginStopComposite)(const char* name);
typedef double (*PluginStopCompositeD)(const char* name);

typedef void (*PluginCloseComposite)(const char* name);
typedef double (*PluginCloseCompositeD)(const char* name);

typedef int (*PluginGetKeyColor)(int animationId, int frameId, int rzkey);
typedef int (*PluginGetKeyColorName)(const char* path, int frameId, int rzkey);
typedef double (*PluginGetKeyColorD)(const char* path, double frameId, double rzkey);

typedef int (*PluginGet1DColor)(int animationId, int frameId, int led);
typedef int (*PluginGet1DColorName)(const char* path, int frameId, int led);
typedef double (*PluginGet1DColorNameD)(const char* path, double frameId, double led);

typedef int (*PluginGet2DColor)(int animationId, int frameId, int row, int column);
typedef int (*PluginGet2DColorName)(const char* path, int frameId, int row, int column);
typedef double (*PluginGet2DColorNameD)(const char* path, double frameId, double row, double column);

typedef void (*PluginSetKeyColor)(int animationId, int frameId, int rzkey, int color);
typedef void (*PluginSetKeyColorName)(const char* path, int frameId, int rzkey, int color);
typedef double (*PluginSetKeyColorNameD)(const char* path, double frameId, double rzkey, double color);

typedef void (*PluginSet1DColor)(int animationId, int frameId, int led, int color);
typedef void (*PluginSet1DColorName)(const char* path, int frameId, int led, int color);
typedef double (*PluginSet1DColorNameD)(const char* path, double frameId, double led, double color);

typedef void (*PluginSet2DColor)(int animationId, int frameId, int row, int column, int color);
typedef void (*PluginSet2DColorName)(const char* path, int frameId, int row, int column, int color);
typedef double (*PluginSet2DColorNameD)(const char* path, double frameId, double rowColumnIndex, double color);

typedef void (*PluginCopyKeyColor)(int sourceAnimationId, int targetAnimationId, int frameId, int rzkey);
typedef void (*PluginCopyKeyColorName)(const char* sourceAnimation, const char* targetAnimation, int frameId, int rzkey);
typedef double (*PluginCopyKeyColorNameD)(const char* sourceAnimation, const char* targetAnimation, double frameId, double rzkey);

typedef void (*PluginCopyNonZeroKeyColor)(int sourceAnimationId, int targetAnimationId, int frameId, int rzkey);
typedef void (*PluginCopyNonZeroKeyColorName)(const char* sourceAnimation, const char* targetAnimation, int frameId, int rzkey);
typedef double (*PluginCopyNonZeroKeyColorNameD)(const char* sourceAnimation, const char* targetAnimation, double frameId, double rzkey);

typedef int (*PluginGetFrameCount)(int animationId);
typedef int (*PluginGetFrameCountName)(const char* path);
typedef double (*PluginGetFrameCountNameD)(const char* path);

typedef int (*PluginGetCurrentFrame)(int animationId);
typedef int (*PluginGetCurrentFrameName)(const char* path);
typedef double (*PluginGetCurrentFrameNameD)(const char* path);

typedef void (*PluginSetCurrentFrame)(int animationId, int frameId);
typedef void (*PluginSetCurrentFrameName)(const char* path, int frameId);
typedef double (*PluginSetCurrentFrameNameD)(const char* path, double frameId);

typedef void (*PluginPauseAnimation)(int animationId);
typedef void (*PluginPauseAnimationName)(const char* path);
typedef double (*PluginPauseAnimationNameD)(const char* path);

typedef bool (*PluginIsAnimationPaused)(int animationId);
typedef bool (*PluginIsAnimationPausedName)(const char* path);
typedef double (*PluginIsAnimationPausedNameD)(const char* path);

typedef bool (*PluginHasAnimationLoop)(int animationId);
typedef bool (*PluginHasAnimationLoopName)(const char* path);
typedef double (*PluginHasAnimationLoopNameD)(const char* path);

typedef void (*PluginResumeAnimation)(int animationId, bool loop);
typedef void (*PluginResumeAnimationName)(const char* path, bool loop);
typedef double (*PluginResumeAnimationNameD)(const char* path, double loop);
