/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihavauch <ihavauch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/14 13:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>
# include <stdint.h>
# include <unistd.h>
# include <time.h>

/* Что делает кодер. */
# define WAITING 0
# define COMPILING 1
# define DEBUGGING 2
# define REFACTORING 3

typedef struct s_all	t_all;

/* Кодер: один человечек. */
typedef struct s_coder
{
	int			id;
	int			idx;
	int			state;
	int			count;
	uint64_t	last;
	uint64_t	want;
	t_all		*all;
	pthread_t	th;
}	t_coder;

/* Донгл: замок + хозяин + время. */
typedef struct s_dongle
{
	pthread_mutex_t	m;
	int				owner;
	uint64_t		ready;
}	t_dongle;

/* Все общее: стол, куча, замки. */
struct s_all
{
	int				n;
	uint64_t		burn;
	uint64_t		comp;
	uint64_t		deb;
	uint64_t		ref;
	int				need;
	uint64_t		cool;
	int				fifo;
	uint64_t		start;
	int				stop;
	t_coder			*coders;
	t_dongle		*dongles;
	int				*heap;
	int				hsize;
	pthread_mutex_t	lock;
	pthread_cond_t	bell;
	pthread_mutex_t	pen;
};

/* Время. */
uint64_t	wall_ms(void);
uint64_t	now_ms(t_all *a);
int			is_stop(t_all *a);
void		naps(t_all *a, uint64_t ms);
/* Цифры. */
int			parse_args(t_all *a, char **av);
/* Куча. */
int			better(t_all *a, int x, int y);
void		hpush(t_all *a, int idx);
int			hpop(t_all *a);
/* Стол. */
int			table_init(t_all *a);
void		table_free(t_all *a);
int			all_done(t_all *a);
int			pick_give(t_all *a, uint64_t now);
int			give_one_pub(t_all *a, int idx, uint64_t now);
void		put_back(t_all *a, int idx, uint64_t now);
/* Печать. */
void		say(t_all *a, int id, const char *m, uint64_t ts);
/* Люди. */
void		*coder_job(void *arg);
int			one_round(t_coder *c, t_all *a);
void		*plan_job(void *arg);
void		*watch_job(void *arg);
/* Старт. */
int			start_all(t_all *a, pthread_t *pl, pthread_t *mo);
void		join_all(t_all *a, pthread_t pl, pthread_t mo);

#endif
