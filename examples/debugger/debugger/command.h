#ifndef __COMMAND_H__
#define __COMMAND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <list.h>
#include <xnes.h>

struct command_t
{
	struct list_head list;
	const char * name;
	const char * desc;

	void (*usage)(void);
	int (*exec)(struct xnes_ctx_t * ctx, int argc, char ** argv);
};

extern struct list_head __command_list;
extern struct command_t cmd_breakpoint;
extern struct command_t cmd_dasm;
extern struct command_t cmd_dump;
extern struct command_t cmd_exit;
extern struct command_t cmd_flag;
extern struct command_t cmd_help;
extern struct command_t cmd_pause;
extern struct command_t cmd_run;
extern struct command_t cmd_step;

struct command_t * search_command(const char * name);
int register_command(struct command_t * cmd);
int unregister_command(struct command_t * cmd);

#ifdef __cplusplus
}
#endif

#endif /* __COMMAND_H__ */
