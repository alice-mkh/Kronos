project('kronos-highscore', ['c', 'cpp'],
  version : '3.12'
)

version = meson.project_version()
commit = run_command('git', 'describe', '--always', '--dirty', '--match', '').stdout().strip()

cc = meson.get_compiler('c')

cores_dir = get_option('prefix') / get_option('libdir') / 'highscore' / 'cores'

kronos_deps = [
  dependency('highscore-1'),
  dependency('gio-2.0'),
  dependency('epoxy'),
  dependency('zlib'),
  dependency('threads'),
  cc.find_library('m', required : false),
]

kronos_common_flags = [
  '-D__HIGHSCORE__',
  '-D__LIBRETRO__',
  '-D_OGL3_',
  '-DHAVE_LIBGL',
  '-DVERSION="@0@"'.format(version),
  '-DHAVE_LROUND',
  '-D_7ZIP_ST',
  '-DFLAC__HAS_OGG=0',
  '-DFLAC__NO_DLL',
  '-DNO_CLI',
  '-DHAVE_BUILTIN_BSWAP16=1',
  '-DHAVE_BUILTIN_BSWAP32=1',
  '-DHAVE_C99_VARIADIC_MACROS=1',
  '-DHAVE_FLOORF=1',
  '-DHAVE_GETTIMEOFDAY=1',
  '-DHAVE_STDINT_H=1',
  '-DHAVE_SYS_PARAM_H=1',
  '-DHAVE_SYS_TIME_H=1',
  '-DIMPROVED_SAVESTATES',
  '-DCELL_ASYNC=1',
  '-DHAVE_THREADS=1',
  '-DUSE_CACHE=1',
  '-DARCH_IS_LINUX=1',
  '-fvisibility=hidden',
  '-Wno-dangling-pointer',
  '-Wno-int-in-bool-context',
  '-Wno-maybe-uninitialized',
  '-Wno-memset-elt-size',
  '-Wno-misleading-indentation',
  '-Wno-missing-braces',
  '-Wno-parentheses',
  '-Wno-return-type',
  '-Wno-stringop-truncation',
  '-Wno-switch',
  '-Wno-unused-but-set-variable',
  '-Wno-unused-function',
  '-Wno-unused-value',
  '-Wno-unused-variable',
]

kronos_common_flags += ['-msse', '-mfpmath=sse' ]

kronos_cflags = [
  '-Wno-pointer-sign',
]

kronos_source = [
  '../core/cd/libretro/cd-libretro.c',
  '../core/m68k/m68kcore.c',
  '../core/m68k/m68kmusashi.c',
  '../core/m68k/musashi/m68kcpu.c',
  '../core/osd/src/osdcore.c',
  '../core/peripheral/common/peripheral.c',
  '../core/sh2/sh2_kronos/src/sh2int.c',
  '../core/sh2/sh2_kronos/src/sh2_opcodes.c',
  '../core/sound/dummy/snddummy.c',
  '../core/video/opengl/common/src/commongl.c',
  '../core/video/opengl/common/src/common_glshader.c',
  '../core/video/opengl/common/src/ogl_shader.c',
  '../core/video/opengl/common/src/rbg_compute.cpp',
  '../core/video/opengl/common/src/upscale_shader.c',
  '../core/video/opengl/common/src/vidshared.c',
  '../core/video/opengl/compute_shader/src/vdp1_compute.c',
  '../core/video/opengl/compute_shader/src/vidcs.c',
  '../core/video/opengl/compute_shader/src/yglcs.c',
  '../core/video/opengl/core/src/vidogl.c',
  '../core/video/opengl/core/src/yglcache.c',
  '../ctrl/src/movie.c',
  '../ctrl/src/stv.c',
  '../ctrl/src/yabause.c',
  '../ctrl/threads/thr-linux.c',
  '../libretro/libretro-common/compat/compat_strl.c',
  '../libretro/libretro-common/encodings/encoding_utf.c',
  '../libretro/libretro-common/file/file_path.c',
  '../libretro/libretro-common/file/file_path_io.c',
  '../libretro/libretro-common/rthreads/rthreads.c',
  '../libretro/libretro-common/streams/file_stream.c',
  '../libretro/libretro-common/string/stdstring.c',
  '../libretro/libretro-common/time/rtime.c',
  '../libretro/libretro-common/vfs/vfs_implementation.c',
  '../sys/bios/src/bios.c',
  '../sys/memory/src/cs0.c',
  '../sys/memory/src/cs1.c',
  '../sys/memory/src/cs2.c',
  '../sys/memory/src/decrypt.c',
  '../sys/memory/src/eeprom.c',
  '../sys/memory/src/memory.c',
  '../sys/scsp/src/scsp.c',
  '../sys/scsp/src/scspdsp.c',
  '../sys/scu/src/scu.c',
  '../sys/sh2/src/sh2core.c',
  '../sys/smpc/src/smpc.c',
  '../sys/vdp1/src/vdp1.c',
  '../sys/vdp2/src/vdp2.c',
  '../tools/junzip/junzip.c',
  '../tools/libchdr/src/bitstream.c',
  '../tools/libchdr/src/cdrom.c',
  '../tools/libchdr/src/chd.c',
  '../tools/libchdr/src/flac.c',
  '../tools/libchdr/src/huffman.c',
  '../tools/libchdr/deps/flac-1.3.2/src/libFLAC/bitreader.c',
  '../tools/libchdr/deps/flac-1.3.2/src/libFLAC/cpu.c',
  '../tools/libchdr/deps/flac-1.3.2/src/libFLAC/crc.c',
  '../tools/libchdr/deps/flac-1.3.2/src/libFLAC/fixed.c',
  '../tools/libchdr/deps/flac-1.3.2/src/libFLAC/format.c',
  '../tools/libchdr/deps/flac-1.3.2/src/libFLAC/lpc.c',
  '../tools/libchdr/deps/flac-1.3.2/src/libFLAC/md5.c',
  '../tools/libchdr/deps/flac-1.3.2/src/libFLAC/memory.c',
  '../tools/libchdr/deps/flac-1.3.2/src/libFLAC/stream_decoder.c',
  '../tools/libchdr/deps/lzma-16.04/C/LzmaDec.c',
  '../tools/libchdr/deps/lzma-16.04/C/LzmaEnc.c',
  '../tools/libchdr/deps/lzma-16.04/C/LzFind.c',
  '../utils/src/cheat.c',
  '../utils/src/db.c',
  '../utils/src/error.c',
  '../utils/src/gameinfo.c',
  '../utils/src/japmodem.c',
  '../utils/src/netlink.c',
  'kronos-highscore.c',
]

kronos_include_dirs = [
  '../core/m68k',
  '../core/m68k/musashi',
  '../core/net',
  '../core/net/linux',
  '../core/net/windows',
  '../core/osd/include',
  '../core/peripheral/common',
  '../core/sh2/sh2_kronos/include',
  '../core/video/opengl/common/include',
  '../core/video/opengl/compute_shader/include',
  '../ctrl/include',
  '../ctrl/threads',
  '../ctrl/threads/linux',
  '../libretro/libretro-common/include',
  '../sys/bios/include',
  '../sys/memory/include',
  '../sys/scsp/include',
  '../sys/scu/include',
  '../sys/sh2/include',
  '../sys/smpc/include',
  '../sys/vdp1/include',
  '../sys/vdp2/include',
  '../tools/libchdr/src',
  '../tools/libchdr/deps/crypto',
  '../tools/libchdr/deps/flac-1.3.2/include',
  '../tools/libchdr/deps/flac-1.3.2/src/libFLAC/include',
  '../tools/libchdr/deps/lzma-16.04/C',
  '../tools/junzip',
  '../utils/include',
]

m68kmake = executable('m68kmake',
  '../core/m68k/musashi/m68kmake.c',
  install: false
)

musashi = custom_target('musashi',
  input: '../core/m68k/musashi/m68k_in.c',
  output: [ 'm68kopac.c', 'm68kopdm.c', 'm68kopnz.c', 'm68kops.c', 'm68kops.h' ],
  command: [m68kmake, '.', '@INPUT@']
)

kronos_source += [
  musashi[0], musashi[1], musashi[2], musashi[3]
]

musashi_dep = declare_dependency(
  sources: musashi
)

shared_library('kronos-highscore',
  kronos_source,
  dependencies : [ kronos_deps, musashi_dep ],
  include_directories : include_directories(kronos_include_dirs),
  c_args: kronos_common_flags + kronos_cflags,
  cpp_args: kronos_common_flags,
  name_prefix: '',
  install_dir : cores_dir,
  install : true,
)

descriptor = configuration_data()
descriptor.set('version', version)
descriptor.set('commit', commit)
configure_file(
  input: 'kronos.highscore.in',
  output: 'kronos.highscore',
  configuration: descriptor,
  install_dir: cores_dir,
  install: true
)