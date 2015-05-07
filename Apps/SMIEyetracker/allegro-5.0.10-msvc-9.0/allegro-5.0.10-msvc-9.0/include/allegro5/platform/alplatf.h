/* alplatf.h is generated from alplatf.h.cmake */
/* #undef ALLEGRO_MINGW32 */
/* #undef ALLEGRO_UNIX */
#define ALLEGRO_MSVC
#define ALLEGRO_CFG_D3D
#define ALLEGRO_CFG_D3D9EX
#define ALLEGRO_CFG_OPENGL
/* #undef ALLEGRO_MACOSX */
/* #undef ALLEGRO_BCC32 */
/* #undef ALLEGRO_GP2XWIZ */
/* #undef ALLEGRO_IPHONE */
/* #undef ALLEGRO_ANDROID */
/* #undef ALLEGRO_RASPBERRYPI */
/* #undef ALLEGRO_CFG_ALLOW_SSE */
#define ALLEGRO_NO_ASM
/* #undef ALLEGRO_CFG_NO_FPU */
/* #undef ALLEGRO_CFG_DLL_TLS */
/* #undef ALLEGRO_CFG_PTHREADS_TLS */
#define ALLEGRO_CFG_RELEASE_LOGGING

/* #undef ALLEGRO_CFG_GLSL_SHADERS */
/* #undef ALLEGRO_CFG_HLSL_SHADERS */
/* #undef ALLEGRO_CFG_CG_SHADERS */

/* #undef ALLEGRO_CFG_OPENGLES */
/* #undef ALLEGRO_CFG_NO_GLES2 */
/* #undef ALLEGRO_CFG_ANDROID_LEGACY */

#ifdef ALLEGRO_ANDROID
#define ALLEGRO_CFG_ANDROID_APP_NAME ${ANDROID_APP_NAME}
#define ALLEGRO_CFG_ANDROID_APP_NAME_SLASH "${ANDROID_APP_NAME_SLASH}"
#endif

/*---------------------------------------------------------------------------*/

/* TODO: rename this */
#define RETSIGTYPE void

/* This is defined on the command-line in the autotools build. */
#define ALLEGRO_MODULES_PATH 

/*---------------------------------------------------------------------------*/

/* Define to 1 if you have the corresponding header file. */
/* #undef ALLEGRO_HAVE_DIRENT_H */
/* #undef ALLEGRO_HAVE_INTTYPES_H */
/* #undef ALLEGRO_HAVE_LINUX_AWE_VOICE_H */
/* #undef ALLEGRO_HAVE_LINUX_INPUT_H */
/* #undef ALLEGRO_HAVE_LINUX_JOYSTICK_H */
/* #undef ALLEGRO_HAVE_LINUX_SOUNDCARD_H */
/* #undef ALLEGRO_HAVE_MACHINE_SOUNDCARD_H */
/* #undef ALLEGRO_HAVE_SOUNDCARD_H */
/* #undef ALLEGRO_HAVE_STDBOOL_H */
/* #undef ALLEGRO_HAVE_STDINT_H */
/* #undef ALLEGRO_HAVE_SV_PROCFS_H */
/* #undef ALLEGRO_HAVE_SYS_IO_H */
/* #undef ALLEGRO_HAVE_SYS_SOUNDCARD_H */
#define ALLEGRO_HAVE_SYS_STAT_H
/* #undef ALLEGRO_HAVE_SYS_TIME_H */
#define ALLEGRO_HAVE_TIME_H
/* #undef ALLEGRO_HAVE_SYS_UTSNAME_H */
#define ALLEGRO_HAVE_SYS_TYPES_H
/* #undef ALLEGRO_HAVE_OSATOMIC_H */
/* #undef ALLEGRO_HAVE_SYS_INOTIFY_H */
/* #undef ALLEGRO_HAVE_SYS_TIMERFD_H */

/* Define to 1 if the corresponding functions are available. */
/* #undef ALLEGRO_HAVE_GETEXECNAME */
/* #undef ALLEGRO_HAVE_MKSTEMP */
/* #undef ALLEGRO_HAVE_MMAP */
/* #undef ALLEGRO_HAVE_MPROTECT */
/* #undef ALLEGRO_HAVE_SCHED_YIELD */
/* #undef ALLEGRO_HAVE_SYSCONF */
/* #undef ALLEGRO_HAVE_FSEEKO */
/* #undef ALLEGRO_HAVE_FTELLO */
/* #undef ALLEGRO_HAVE_VA_COPY */

/* Define to 1 if procfs reveals argc and argv */
/* #undef ALLEGRO_HAVE_PROCFS_ARGCV */

/*---------------------------------------------------------------------------*/

/* Define if target machine is little endian. */
#define ALLEGRO_LITTLE_ENDIAN

/* Define if target machine is big endian. */
/* #undef ALLEGRO_BIG_ENDIAN */

/* Define for Unix platforms, to use C convention for bank switching. */
#define ALLEGRO_NO_ASM

/* Define if compiler prepends underscore to symbols. */
/* #undef ALLEGRO_ASM_PREFIX */

/* Define if assembler supports MMX. */
/* #undef ALLEGRO_MMX */

/* Define if assembler supports SSE. */
/* #undef ALLEGRO_SSE */

/* Define if target platform is Darwin. */
/* #undef ALLEGRO_DARWIN */

/* Define if you have the pthread library. */
/* #undef ALLEGRO_HAVE_LIBPTHREAD */

/* Define if constructor attribute is supported. */
/* #undef ALLEGRO_USE_CONSTRUCTOR */

/* Define if dynamically loaded modules are supported. */
/* #undef ALLEGRO_WITH_MODULES */

/*---------------------------------------------------------------------------*/

/* Define if you need support for X-Windows. */
/* #undef ALLEGRO_WITH_XWINDOWS */

/* Define if MIT-SHM extension is supported. */
/* #undef ALLEGRO_XWINDOWS_WITH_SHM */

/* Define if XCursor ARGB extension is available. */
/* #undef ALLEGRO_XWINDOWS_WITH_XCURSOR */

/* Define if DGA version 2.0 or newer is supported */
/* #undef ALLEGRO_XWINDOWS_WITH_XF86DGA2 */

/* Define if XF86VidMode extension is supported. */
/* #undef ALLEGRO_XWINDOWS_WITH_XF86VIDMODE */

/* Define if Xinerama extension is supported. */
/* #undef ALLEGRO_XWINDOWS_WITH_XINERAMA */

/* Define if XRandR extension is supported. */
/* #undef ALLEGRO_XWINDOWS_WITH_XRANDR */

/* Define if XIM extension is supported. */
/* #undef ALLEGRO_XWINDOWS_WITH_XIM */

/* Define if xpm bitmap support is available. */
/* #undef ALLEGRO_XWINDOWS_WITH_XPM */

/*---------------------------------------------------------------------------*/

/* Define if target platform is linux. */
/* #undef ALLEGRO_LINUX */

/* Define to enable Linux console fbcon driver. */
/* #undef ALLEGRO_LINUX_FBCON */

/* Define to enable Linux console SVGAlib driver. */
/* #undef ALLEGRO_LINUX_SVGALIB */

/* Define if SVGAlib driver can check vga_version. */
/* #undef ALLEGRO_LINUX_SVGALIB_HAVE_VGA_VERSION */

/* Define to enable Linux console VBE/AF driver. */
/* #undef ALLEGRO_LINUX_VBEAF */

/* Define to enable Linux console VGA driver. */
/* #undef ALLEGRO_LINUX_VGA */

/*---------------------------------------------------------------------------*/

/* Define to the installed ALSA version. */
/* #undef ALLEGRO_ALSA_VERSION */

/* Define if ALSA DIGI driver is supported. */
/* #undef ALLEGRO_WITH_ALSADIGI */

/* Define if ALSA MIDI driver is supported. */
/* #undef ALLEGRO_WITH_ALSAMIDI */

/* Define if aRts DIGI driver is supported. */
/* #undef ALLEGRO_WITH_ARTSDIGI */

/* Define if ESD DIGI driver is supported. */
/* #undef ALLEGRO_WITH_ESDDIGI */

/* Define if JACK DIGI driver is supported. */
/* #undef ALLEGRO_WITH_JACKDIGI */

/* Define if OSS DIGI driver is supported. */
/* #undef ALLEGRO_WITH_OSSDIGI */

/* Define if OSS MIDI driver is supported. */
/* #undef ALLEGRO_WITH_OSSMIDI */

/* Define if SGI AL DIGI driver is supported. */
/* #undef ALLEGRO_WITH_SGIALDIGI */

/*---------------------------------------------------------------------------*/

/* TODO: Define to (void *)-1, if MAP_FAILED is not defined. */
/* TODO: rename this */
/* # cmakedefine MAP_FAILED */

/*---------------------------------------------------------------------------*/
/* vi: set ft=c ts=3 sts=3 sw=3 et: */
