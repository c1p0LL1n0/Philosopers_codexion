/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <digglega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/14 13:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Говорим "я голодный". */
static void	say_hungry(t_all *a, t_coder *c, uint64_t now)
{
	pthread_mutex_lock(&a->lock);
	if (!a->stop && c->state == WAITING && c->want == 0)
	{
		c->want = now;
		if (c->want == 0)
			c->want = 1;
		pthread_cond_broadcast(&a->bell);
	}
	pthread_mutex_unlock(&a->lock);
}

/* Ждём колокола. 1 = конец. */
static int	wait_food(t_all *a, t_coder *c)
{
	pthread_mutex_lock(&a->lock);
	while (!a->stop && c->state == WAITING)
		pthread_cond_wait(&a->bell, &a->lock);
	if (a->stop)
	{
		pthread_mutex_unlock(&a->lock);
		return (1);
	}
	pthread_mutex_unlock(&a->lock);
	return (0);
}

/* Меняем дело. 1 = конец. */
static int	change_job(t_all *a, t_coder *c, int st, const char *m)
{
	pthread_mutex_lock(&a->lock);
	if (a->stop)
	{
		pthread_mutex_unlock(&a->lock);
		return (1);
	}
	c->state = st;
	say(a, c->id, m, now_ms(a));
	pthread_cond_broadcast(&a->bell);
	pthread_mutex_unlock(&a->lock);
	return (0);
}

/* Тарелка съедена. */
static void	rest_plate(t_all *a, t_coder *c)
{
	pthread_mutex_lock(&a->lock);
	if (!a->stop)
	{
		c->state = WAITING;
		c->want = 0;
		pthread_cond_broadcast(&a->bell);
	}
	pthread_mutex_unlock(&a->lock);
}

/* Один круг жизни. 1 = конец. */
int	one_round(t_coder *c, t_all *a)
{
	say_hungry(a, c, now_ms(a));
	if (wait_food(a, c))
		return (1);
	naps(a, a->comp);
	if (is_stop(a))
		return (1);
	put_back(a, c->idx, now_ms(a));
	if (change_job(a, c, DEBUGGING, "is debugging"))
		return (1);
	naps(a, a->deb);
	if (change_job(a, c, REFACTORING, "is refactoring"))
		return (1);
	naps(a, a->ref);
	rest_plate(a, c);
	return (0);
}
