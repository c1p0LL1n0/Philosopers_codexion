/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   give.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <ihavauch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/14 13:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Два замка по адресу: дедлока нет. */
static void	lock2(t_dongle *l, t_dongle *r)
{
	if (l == r)
	{
		pthread_mutex_lock(&l->m);
		return ;
	}
	if (l < r)
	{
		pthread_mutex_lock(&l->m);
		pthread_mutex_lock(&r->m);
	}
	else
	{
		pthread_mutex_lock(&r->m);
		pthread_mutex_lock(&l->m);
	}
}

/* Открыть два замка. */
static void	unlock2(t_dongle *l, t_dongle *r)
{
	if (l == r)
	{
		pthread_mutex_unlock(&l->m);
		return ;
	}
	pthread_mutex_unlock(&l->m);
	pthread_mutex_unlock(&r->m);
}

/* Пара свободна? Замки уже закрыты. */
static int	pair_free(t_all *a, int idx, uint64_t now)
{
	t_dongle	*l;
	t_dongle	*r;

	l = &a->dongles[idx];
	r = &a->dongles[(idx + 1) % a->n];
	if (l == r)
		return (l->owner == -1 && l->ready <= now);
	if (l->owner != -1 || l->ready > now)
		return (0);
	if (r->owner != -1 || r->ready > now)
		return (0);
	return (1);
}

/* Кричим, что взяли донглы. */
static void	say_took(t_all *a, t_coder *c, int one, uint64_t now)
{
	if (one)
		say(a, c->id, "has taken a dongle", now);
	else
	{
		say(a, c->id, "has taken a dongle", now);
		say(a, c->id, "has taken a dongle", now);
	}
	say(a, c->id, "is compiling", now);
}

/* Даём оба донгла сразу. 1 = вышло. */
int	give_one_pub(t_all *a, int idx, uint64_t now)
{
	t_coder		*c;
	int			one;

	c = &a->coders[idx];
	lock2(&a->dongles[idx], &a->dongles[(idx + 1) % a->n]);
	if (!pair_free(a, idx, now))
	{
		unlock2(&a->dongles[idx], &a->dongles[(idx + 1) % a->n]);
		return (0);
	}
	a->dongles[idx].owner = c->id;
	one = (&a->dongles[idx] == &a->dongles[(idx + 1) % a->n]);
	if (!one)
		a->dongles[(idx + 1) % a->n].owner = c->id;
	unlock2(&a->dongles[idx], &a->dongles[(idx + 1) % a->n]);
	say_took(a, c, one, now);
	c->state = COMPILING;
	c->last = now;
	c->count++;
	return (1);
}
