/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   give.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <digglega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/16 00:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Гениальный минимум: соседей НЕ смотрим вообще. */
/* fifo: у кого тикет меньше — тот старше, ждём свой черёд. */
/* edf: знак срочности — чем ближе дедлайн, тем короче сон. */
static uint64_t	backoff_ns(t_all *a, t_coder *c, uint64_t now)
{
	uint64_t	deadline;
	uint64_t	left;
	uint64_t	age;

	if (a->scheduler == EDF)
	{
		deadline = atomic_load(&c->last) + a->burn;
		if (now >= deadline)
			return (0);
		left = deadline - now;
		if (left < 8)
			return (0);
		if (left < 60)
			return (50000L);
		return (200000L);
	}
	age = now - atomic_load(&c->ticket);
	if (age > 400)
		return (0);
	return (200000L);
}

/* Самозахват пары CAS-ом. 1 = взял и ем. Соседей не читаем. */
int	try_take(t_all *a, t_coder *c, uint64_t now)
{
	int	l;
	int	r;

	l = c->id - 1;
	r = c->id % a->n;
	if (atomic_load(&c->state) != WAITING)
		return (0);
	if (atomic_load(&c->count) >= a->need)
		return (0);
	if (!bits_try_take2(a->fork_bits, l, r))
		return (0);
	if (atomic_load(&a->dongles[l].ready) > now
		|| atomic_load(&a->dongles[r].ready) > now)
	{
		bits_release2(a->fork_bits, l, r);
		return (0);
	}
	atomic_store(&a->dongles[l].owner, c->id);
	atomic_store(&a->dongles[r].owner, c->id);
	atomic_store(&c->last, now);
	atomic_fetch_add(&c->count, 1);
	atomic_store(&c->state, COMPILING);
	return (1);
}

/* Сколько спать после неудачи. Знак +/-: срочным 0, остальным пауза. */
uint64_t	yield_ns(t_all *a, t_coder *c, uint64_t now)
{
	return (backoff_ns(a, c, now));
}

void	put_back(t_all *a, int idx, uint64_t now)
{
	int	l;
	int	r;

	l = idx;
	r = (idx + 1) % a->n;
	atomic_store(&a->dongles[l].owner, -1);
	atomic_store(&a->dongles[l].ready, now + a->cool);
	atomic_store(&a->dongles[r].owner, -1);
	atomic_store(&a->dongles[r].ready, now + a->cool);
	bits_release2(a->fork_bits, l, r);
}
