/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jobs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <ihavauch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/14 13:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Жизнь кодера. */
void	*coder_job(void *arg)
{
	t_coder	*c;
	t_all	*a;

	c = (t_coder *)arg;
	a = c->all;
	if (a->n > 1 && c->id % 2 == 0)
		usleep(500);
	while (!is_stop(a))
	{
		if (one_round(c, a))
			break ;
	}
	return (NULL);
}

/* Разносчик донглов. */
void	*plan_job(void *arg)
{
	t_all	*a;

	a = (t_all *)arg;
	while (!is_stop(a))
	{
		if (!pick_give(a, now_ms(a)))
			usleep(500);
	}
	pthread_mutex_lock(&a->lock);
	pthread_cond_broadcast(&a->bell);
	pthread_mutex_unlock(&a->lock);
	return (NULL);
}

/* Сгорел? Не кушает и опоздал. */
static int	is_burned(t_all *a, t_coder *c, uint64_t now)
{
	if (c->state == COMPILING)
		return (0);
	return (now >= c->last + a->burn);
}

/* Ищем сгоревшего. -1 = все живы. */
static int	find_burned(t_all *a)
{
	int			i;
	uint64_t	now;

	now = now_ms(a);
	i = 0;
	while (i < a->n)
	{
		if (is_burned(a, &a->coders[i], now))
			return (i);
		i++;
	}
	return (-1);
}

/* Смотритель с часами. */
void	*watch_job(void *arg)
{
	t_all	*a;
	int		who;

	a = (t_all *)arg;
	while (1)
	{
		pthread_mutex_lock(&a->lock);
		if (a->stop)
		{
			pthread_mutex_unlock(&a->lock);
			break ;
		}
		who = find_burned(a);
		if (who >= 0)
		{
			say(a, a->coders[who].id, "burned out", now_ms(a));
			a->stop = 1;
			pthread_cond_broadcast(&a->bell);
			pthread_mutex_unlock(&a->lock);
			return (NULL);
		}
		pthread_mutex_unlock(&a->lock);
		usleep(1000);
	}
	return (NULL);
}
