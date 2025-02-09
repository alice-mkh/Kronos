#include "kronos-highscore.h"

#include "core.h"
#include "cs0.h"
#include "m68kcore.h"
#include "peripheral.h"
#include "sh2int_kronos.h"
#include "vidcs.h"
#include "yabause.h"
#include "ygl.h"
#include "yui.h"

#define SAMPLE_RATE 44100
#define N_SOUND_BLOCKS 4
#define SNDCORE_HIGHSCORE 11

static KronosCore *core;

struct _KronosCore
{
  HsCore parent_instance;

  HsGLContext *context;
  char *bios_path[HS_SEGA_SATURN_BIOS_N_BIOS];

  int last_width;
  int last_height;
  gboolean frame_drawn;

  yabauseinit_struct yinit;

  u32 audio_frame_size;
  u32 audio_size;
  s16 *audio_buffer;
};

static void kronos_sega_saturn_core_init (HsSegaSaturnCoreInterface *iface);

G_DEFINE_FINAL_TYPE_WITH_CODE (KronosCore, kronos_core, HS_TYPE_CORE,
                               G_IMPLEMENT_INTERFACE (HS_TYPE_SEGA_SATURN_CORE, kronos_sega_saturn_core_init))

static int
SNDHighscoreInit (void)
{
  int vert_freq = (int) hs_core_get_frame_rate (HS_CORE (core));
  u32 frame_samples = (SAMPLE_RATE / vert_freq) * 2;
  u32 buffer_samples = frame_samples * N_SOUND_BLOCKS;

  core->audio_frame_size = frame_samples * sizeof (s16);
  core->audio_size = buffer_samples * sizeof (s16);
  core->audio_buffer = g_new0 (s16, buffer_samples);

  return 0;
}

static void
SNDHighscoreDeInit (void)
{
  g_clear_pointer (&core->audio_buffer, g_free);
}

static int
SNDHighscoreReset (void)
{
  return 0;
}

static int
SNDHighscoreChangeVideoFormat (int vert_freq)
{
  u32 frame_samples = (SAMPLE_RATE / vert_freq) * 2;
  u32 buffer_samples = frame_samples * N_SOUND_BLOCKS;

  g_free (core->audio_buffer);

  core->audio_frame_size = frame_samples * sizeof (s16);
  core->audio_size = buffer_samples * sizeof (s16);
  core->audio_buffer = g_new0 (s16, buffer_samples);

  return 0;
}

static void
convert_32u_to_16s (int32_t *srcL, int32_t *srcR, int16_t *dst, size_t len)
{
  u32 i;

  for (i = 0; i < len; i++) {
    // Left Channel
    if (*srcL > 0x7FFF)
      *dst = 0x7FFF;
    else if (*srcL < -0x8000)
      *dst = -0x8000;
    else 
      *dst = *srcL;

    srcL++;
    dst++;

    // Right Channel
    if (*srcR > 0x7FFF)
      *dst = 0x7FFF;
    else if (*srcR < -0x8000)
      *dst = -0x8000;
    else 
      *dst = *srcR;

    srcR++;
    dst++;
  }
}

static void
SNDHighscoreUpdateAudio (u32 *leftchanbuffer, u32 *rightchanbuffer, u32 num_samples)
{
  convert_32u_to_16s ((int32_t*) leftchanbuffer, (int32_t*) rightchanbuffer, core->audio_buffer, num_samples);

  hs_core_play_samples (HS_CORE (core), core->audio_buffer, num_samples * 2);

  core->audio_size += num_samples * sizeof(s16) * 2;
}

static u32
SNDHighscoreGetAudioSpace (void)
{
  return core->audio_size;
}

void
SNDHighscoreMuteAudio (void)
{
}

void
SNDHighscoreUnMuteAudio (void)
{
}

void
SNDHighscoreSetVolume (int volume)
{
}

SoundInterface_struct SNDHighscore = {
  SNDCORE_HIGHSCORE,
  "Highscore Sound Interface",
  SNDHighscoreInit,
  SNDHighscoreDeInit,
  SNDHighscoreReset,
  SNDHighscoreChangeVideoFormat,
  SNDHighscoreUpdateAudio,
  SNDHighscoreGetAudioSpace,
  SNDHighscoreMuteAudio,
  SNDHighscoreUnMuteAudio,
  SNDHighscoreSetVolume
};

M68K_struct *M68KCoreList[] = {
  &M68KDummy,
  &M68KMusashi,
  NULL
};

SH2Interface_struct *SH2CoreList[] = {
  &SH2KronosInterpreter,
  &SH2KronosDebugInterpreter,
  NULL
};

PerInterface_struct *PERCoreList[] = {
  &PERDummy,
  // TODO
  NULL
};

CDInterface *CDCoreList[] = {
  &DummyCD,
  &ISOCD,
  NULL
};

SoundInterface_struct *SNDCoreList[] = {
  &SNDDummy,
  &SNDHighscore,
  NULL
};

VideoInterface_struct *VIDCoreList[] = {
  &VIDCS,
  NULL
};

void
YuiMsg (const char *format, ...)
{
  va_list args;
  va_start (args, format);
  hs_core_log_valist (HS_CORE (core), HS_LOG_INFO, format, args);
  va_end (args);
}

void
YuiErrorMsg (const char *message)
{
  hs_core_log_literal (HS_CORE (core), HS_LOG_CRITICAL, message);
}

int
YuiGetFB (void)
{
  return hs_gl_context_get_default_framebuffer (core->context);
}

void
YuiSwapBuffers (void)
{
  if (_Ygl->width != core->last_width || _Ygl->height != core->last_height) {
    hs_gl_context_set_size (core->context, _Ygl->width, _Ygl->height);
    core->last_width = _Ygl->width;
    core->last_height = _Ygl->height;
  }

  if (core->frame_drawn) {
    hs_gl_context_swap_buffers (core->context);
    core->frame_drawn = FALSE;
  }
}

void
YuiEndOfFrame (void)
{
  if (_Ygl->width != core->last_width || _Ygl->height != core->last_height) {
    hs_gl_context_set_size (core->context, _Ygl->width, _Ygl->height);
    core->last_width = _Ygl->width;
    core->last_height = _Ygl->height;
  }

  core->frame_drawn = FALSE;
//  hs_gl_context_swap_buffers (core->context);
}

static gboolean
kronos_core_load_rom (HsCore      *core,
                      const char **rom_paths,
                      int          n_rom_paths,
                      const char  *save_path,
                      GError     **error)
{
  KronosCore *self = KRONOS_CORE (core);

  self->yinit.vidcoretype = VIDCORE_CS;
  self->yinit.percoretype = PERCORE_DUMMY;//LIBRETRO;
  self->yinit.sh2coretype = SH2CORE_KRONOS_INTERPRETER;
  self->yinit.sndcoretype = SNDCORE_HIGHSCORE;
  self->yinit.m68kcoretype = M68KCORE_MUSASHI;
  self->yinit.regionid = REGION_AUTODETECT;
  self->yinit.languageid = LANGUAGE_ENGLISH; // GERMAN FRENCH SPANISH ITALIAN JAPANESE
  self->yinit.mpegpath = NULL;
  self->yinit.vsyncon = 0;
  self->yinit.clocksync = 0;
  self->yinit.basetime = 0;
  self->yinit.usethreads = 1;
  self->yinit.numthreads = g_get_num_processors ();
  self->yinit.usecache = 0;
  self->yinit.skip_load = 0;
  self->yinit.stretch = STRETCH_RATIO;
  self->yinit.extend_backup = 0;
  self->yinit.buppath = NULL;
  self->yinit.meshmode = ORIGINAL_MESH;
  self->yinit.bandingmode = ORIGINAL_BANDING;
  self->yinit.wireframe_mode = 0;
  self->yinit.skipframe = 0;
  self->yinit.stv_favorite_region = 0;
  self->yinit.resolution_mode = RES_ORIGINAL;
  self->yinit.auto_cart = 1;

  self->yinit.cdcoretype = CDCORE_ISO;
  self->yinit.cdpath = rom_paths[0];
  self->yinit.biospath = self->bios_path[HS_SEGA_SATURN_BIOS_JP];
  self->yinit.carttype = CART_NONE;
  self->yinit.cartpath = NULL;
  self->yinit.supportdir = NULL;

  self->yinit.stvgame = ""; // TODO NULL makes it fail, and libretro uses ininitialized value, contribute upstream
  self->yinit.stvgamepath = NULL;
  self->yinit.stvbiospath = NULL;
  self->yinit.eepromdir = save_path;

  self->context = hs_core_create_gl_context (core,
                                             HS_GL_PROFILE_CORE,
                                             4, 3,
                                             HS_GL_FLAGS_FLIPPED | HS_GL_FLAGS_STENCIL);

  if (!hs_gl_context_realize (self->context, error))
    return FALSE;

  if (YabauseInit (&self->yinit) != 0) {
    g_set_error (error, HS_CORE_ERROR, HS_CORE_ERROR_INTERNAL, "Failed to initialize Yabause");

    return FALSE;
  }

  OSDChangeCore (OSDCORE_DUMMY);

  if (VIDCore) {
    VIDCore->Init ();
    VIDCore->Resize (0, 0, 1280, 720, 0);
  }

  return TRUE;
}

static void
kronos_core_poll_input (HsCore *core, HsInputState *input_state)
{
}

static void
kronos_core_run_frame (HsCore *core)
{
  KronosCore *self = KRONOS_CORE (core);

  self->audio_size -= self->audio_frame_size;
  YabauseExec ();

  self->frame_drawn = TRUE;
}

static void
kronos_core_reset (HsCore *core, gboolean hard)
{
  if (hard)
    YabauseResetNoLoad ();
  else
    YabauseResetButton ();
}

static void
kronos_core_stop (HsCore *core)
{
  KronosCore *self = KRONOS_CORE (core);

  if (VIDCore)
    VIDCore->DeInit ();

  YabauseDeInit ();

  hs_gl_context_unrealize (self->context);
  g_clear_object (&self->context);
}

static gboolean
kronos_core_reload_save (HsCore      *core,
                         const char  *save_path,
                         GError    **error)
{
  return FALSE;
}

static gboolean
kronos_core_sync_save (HsCore  *core,
                       GError **error)
{
  YabFlushBackups ();
}

static void
kronos_core_load_state (HsCore          *core,
                        const char      *path,
                        HsStateCallback  callback)
{
  callback (core, NULL);
}

static void
kronos_core_save_state (HsCore          *core,
                        const char      *path,
                        HsStateCallback  callback)
{
  callback (core, NULL);
}

static double
kronos_core_get_frame_rate (HsCore *core)
{
  return yabsys.IsPal ? 50.0 : 60.0;
}

static double
kronos_core_get_aspect_ratio (HsCore *core)
{
  return 4.0 / 3.0;
}

static double
kronos_core_get_sample_rate (HsCore *core)
{
  return SAMPLE_RATE;
}

static int
kronos_core_get_channels (HsCore *core)
{
  return 2;
}

static HsRegion
kronos_core_get_region (HsCore *core)
{
  return yabsys.IsPal ? HS_REGION_PAL : HS_REGION_NTSC;
}

static guint
kronos_core_get_current_media (HsCore *core)
{
  return 0;
}

static void
kronos_core_set_current_media (HsCore *core, guint media)
{
}

static void
kronos_core_finalize (GObject *object)
{
  KronosCore *self = KRONOS_CORE (object);

  for (int i = 0; i < HS_SEGA_SATURN_BIOS_N_BIOS; i++)
    g_free (self->bios_path[i]);

  core = NULL;

  G_OBJECT_CLASS (kronos_core_parent_class)->finalize (object);
}

static void
kronos_core_class_init (KronosCoreClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  HsCoreClass *core_class = HS_CORE_CLASS (klass);

  object_class->finalize = kronos_core_finalize;

  core_class->load_rom = kronos_core_load_rom;
  core_class->poll_input = kronos_core_poll_input;
  core_class->run_frame = kronos_core_run_frame;
  core_class->reset = kronos_core_reset;
  core_class->stop = kronos_core_stop;

  core_class->reload_save = kronos_core_reload_save;
  core_class->sync_save = kronos_core_sync_save;

  core_class->load_state = kronos_core_load_state;
  core_class->save_state = kronos_core_save_state;

  core_class->get_frame_rate = kronos_core_get_frame_rate;
  core_class->get_aspect_ratio = kronos_core_get_aspect_ratio;

  core_class->get_sample_rate = kronos_core_get_sample_rate;
  core_class->get_channels = kronos_core_get_channels;

  core_class->get_region = kronos_core_get_region;

  core_class->get_current_media = kronos_core_get_current_media;
  core_class->set_current_media = kronos_core_set_current_media;
}

static void
kronos_core_init (KronosCore *self)
{
  g_assert (!core);

  core = self;
}

static void
kronos_sega_saturn_core_set_bios_path (HsSegaSaturnCore *core, HsSegaSaturnBios type, const char *path)
{
  KronosCore *self = KRONOS_CORE (core);

  g_set_str (&self->bios_path[type], path);
}

static HsSegaSaturnBios
kronos_sega_saturn_core_get_used_bios (HsSegaSaturnCore *core)
{
  return HS_SEGA_SATURN_BIOS_US_EU;
}

static void
kronos_sega_saturn_core_init (HsSegaSaturnCoreInterface *iface)
{
  iface->set_bios_path = kronos_sega_saturn_core_set_bios_path;
  iface->get_used_bios = kronos_sega_saturn_core_get_used_bios;
}

GType
hs_get_core_type (void)
{
  return KRONOS_TYPE_CORE;
}
