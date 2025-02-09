#pragma once

#include <highscore/libhighscore.h>

G_BEGIN_DECLS

#define KRONOS_TYPE_CORE (kronos_core_get_type())

G_DECLARE_FINAL_TYPE (KronosCore, kronos_core, KRONOS, CORE, HsCore)

G_MODULE_EXPORT GType hs_get_core_type (void);

G_END_DECLS
