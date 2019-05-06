#ifndef XDG_USER_DIR_LOOKUP_H
#define XDG_USER_DIR_LOOKUP_H

#ifdef __cplusplus
extern "C" {
#endif
	char *
	xdg_user_dir_lookup_with_fallback (const char *type, const char *fallback);
	char *
	xdg_user_dir_lookup (const char *type);
#ifdef __cplusplus
}
#endif

#endif // XDG_USER_DIR_LOOKUP_H
