#include "binding-util.h"
#include "binding-types.h"
#include "debugwriter.h"

#include "chromasdk/ChromaApi.h"

bool INIT_SUCCESS = false;

#ifdef _WIN32
  HINSTANCE hLib = NULL;
#else
  int* hLib = NULL;
#endif

//dynamic-loaded proc pointers
PluginPlayAnimation _playAnimation = NULL;
PluginOpenAnimation _openAnimation = NULL;
PluginIsInitialized _pluginIsInitialized = NULL;
PluginInit _pluginInit = NULL;
PluginUninit _pluginUninit = NULL;

int lastAnimId = -1;


void _attemptLinkSdk() {
  Debug() << "Attempting to bind Chroma SDK";
  #ifdef _WIN32
    hLib = LoadLibrary(L"ChromaApi.dll");
    if (hLib != NULL) {
      Debug() << "Chroma Impl @" << hLib;
      INIT_SUCCESS = true;

      _playAnimation = (PluginPlayAnimation) GetProcAddress(hLib, "PluginPlayAnimation");
      INIT_SUCCESS &= _playAnimation != NULL;
      Debug() << "Plugin method @" << (long)_playAnimation;

      _openAnimation = (PluginOpenAnimation) GetProcAddress(hLib, "PluginOpenAnimation");
      INIT_SUCCESS &= _openAnimation != NULL;

      _pluginInit = (PluginInit) GetProcAddress(hLib, "PluginInit");
      INIT_SUCCESS &= _pluginInit != NULL;

      _pluginIsInitialized = (PluginIsInitialized) GetProcAddress(hLib, "PluginIsInitialized");
      INIT_SUCCESS &= _pluginInit != NULL;
      if (INIT_SUCCESS && !_pluginIsInitialized()) {
        Debug() << "Initializing chroma plugin";
        _pluginInit();
      }
      _pluginUninit = (PluginUninit) GetProcAddress(hLib, "PluginUninit");
      INIT_SUCCESS &= _pluginUninit != NULL;
      Debug() << "Plugin init success:" << INIT_SUCCESS;
    }
  #else
    // Currently the Razor Chroma API is Windows-only.
    // So, if we are not building for Windows platform,
    // then don't bother with Chroma stuff.
    Debug() << "Chroma: Unsupported Platform";
  #endif
}


RB_METHOD(chromaPlayAnimation) {
  RB_UNUSED_PARAM;

  Debug() << "Chroma: Attempting to play animation.";
  const char* animfile;
  bool loop;

  rb_get_args(argc, argv, "zb", &animfile, &loop RB_ARG_END);

  if (INIT_SUCCESS) {
    Debug() << "Opening animation:" << animfile;
    lastAnimId = _openAnimation(animfile);
    Debug() << "ID:" << lastAnimId;
    _playAnimation(lastAnimId);
  }

  return Qnil;
}

void chromaBindingInit() {
  _attemptLinkSdk();

	VALUE module = rb_define_module("Chroma");
	_rb_define_module_function(module, "playAnim", chromaPlayAnimation);
}

void chromaBindingRelease() {
  #ifdef _WIN32
    if (hLib != NULL) {
      _pluginUninit();
      FreeLibrary(hLib);
      hLib = NULL;
      INIT_SUCCESS = false;
    }
  #endif
}
