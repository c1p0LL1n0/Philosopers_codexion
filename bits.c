/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bits.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <digglega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/16 00:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	bits_set(uint64_t *bits, int idx)
{
	__atomic_fetch_or(&bits[idx / 64], 1ULL << (idx % 64), __ATOMIC_RELEASE);
}

void	bits_clear(uint64_t *bits, int idx)
{
	__atomic_fetch_and(&bits[idx / 64],
		~(1ULL << (idx % 64)), __ATOMIC_RELEASE);
}

int	bits_get(uint64_t *bits, int idx)
{
	return ((__atomic_load_n(&bits[idx / 64], __ATOMIC_ACQUIRE)
			>> (idx % 64)) & 1);
}

/* CAS-захват пары без соседей: только свои биты. 1 = взял. */
int	bits_try_take2(uint64_t *bits, int l, int r)
{
	int			wl;
	int			wr;
	uint64_t	ml;
	uint64_t	mr;
	uint64_t	old;
	uint64_t	nw;

	wl = l / 64;
	ml = 1ULL << (l % 64);
	wr = r / 64;
	mr = 1ULL << (r % 64);
	if (wl == wr)
	{
		old = __atomic_load_n(&bits[wl], __ATOMIC_ACQUIRE);
		while (1)
		{
			if ((old & (ml | mr)) != (ml | mr))
				return (0);
			nw = old & ~(ml | mr);
			if (__atomic_compare_exchange_n(&bits[wl], &old, nw,
					0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED))
				return (1);
		}
	}
	if ((__atomic_fetch_and(&bits[wl], ~ml, __ATOMIC_ACQ_REL) & ml) == 0)
		return (0);
	if ((__atomic_fetch_and(&bits[wr], ~mr, __ATOMIC_ACQ_REL) & mr) == 0)
	{
		__atomic_fetch_or(&bits[wl], ml, __ATOMIC_RELEASE);
		return (0);
	}
	return (1);
}

void	bits_release2(uint64_t *bits, int l, int r)
{
	__atomic_fetch_or(&bits[l / 64], 1ULL << (l % 64), __ATOMIC_RELEASE);
	__atomic_fetch_or(&bits[r / 64], 1ULL << (r % 64), __ATOMIC_RELEASE);
}

void	bits_init(t_all *a)
{
	int	i;

	a->words = (a->n + 63) / 64;
	a->fork_bits = (uint64_t *)calloc(a->words, sizeof(uint64_t));
	if (!a->fork_bits)
		return ;
	i = 0;
	while (i < a->n)
	{
		bits_set(a->fork_bits, i);
		i++;
	}
}

void	bits_free(t_all *a)
{
	if (a->fork_bits)
		free(a->fork_bits);
	a->fork_bits = NULL;
}
