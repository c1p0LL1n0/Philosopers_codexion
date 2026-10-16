/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <digglega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/16 00:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>
# include <stdint.h>
# include <stdatomic.h>
# include <unistd.h>
# include <time.h>

# define WAITING     0
# define COMPILING   1
# define DEBUGGING   2
# define REFACTORING 3

# define FIFO  0
# define EDF   1

typedef struct s_coder	t_coder;
typedef struct s_dongle	t_dongle;
typedef struct s_all	t_all;

struct s_coder
{
	int				id;
	_Atomic int		state;
	_Atomic int		count;
	_Atomic uint64_t	ticket;
	_Atomic uint64_t	last;
	t_all			*all;
	pthread_t		th;
};

struct s_dongle
{
	_Atomic int		owner;
	_Atomic uint64_t	ready;
};

struct s_all
{
	int				n;
	int				words;
	uint64_t		burn;
	uint64_t		comp;
	uint64_t		deb;
	uint64_t		ref;
	int				need;
	uint64_t		cool;
	uint64_t		start;
	_Atomic int		stop;
	_Atomic uint64_t	next_ticket;
	int				scheduler;
	t_coder			*coders;
	t_dongle		*dongles;
	uint64_t		*fork_bits;
	pthread_mutex_t	pen;
	FILE			*bench;
};

uint64_t	wall_ms(void);
uint64_t	now_ms(t_all *a);
int			is_stop(t_all *a);
void		naps(t_all *a, uint64_t ms);
int			parse_args(t_all *a, char **av);
void		bits_set(uint64_t *bits, int idx);
void		bits_clear(uint64_t *bits, int idx);
int			bits_get(uint64_t *bits, int idx);
int			bits_try_take2(uint64_t *bits, int l, int r);
void		bits_release2(uint64_t *bits, int l, int r);
void		bits_init(t_all *a);
void		bits_free(t_all *a);
int			table_init(t_all *a);
void		table_free(t_all *a);
int			all_done(t_all *a);
int			try_take(t_all *a, t_coder *c, uint64_t now);
uint64_t	yield_ns(t_all *a, t_coder *c, uint64_t now);
void		put_back(t_all *a, int idx, uint64_t now);
void		say(t_all *a, int id, const char *m, uint64_t ts);
void		say_took(t_all *a, int idx, uint64_t now);
void		*coder_job(void *arg);
int			one_round(t_coder *c, t_all *a);
void		*watch_job(void *arg);
int			start_all(t_all *a, pthread_t *mo);
void		join_all(t_all *a, pthread_t mo);

#endif
