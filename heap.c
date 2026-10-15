/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <ihavauch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/14 13:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Кто важнее? x должен быть выше y? */
int	better(t_all *a, int x, int y)
{
	uint64_t	dx;
	uint64_t	dy;

	if (!a->fifo)
	{
		dx = a->coders[x].last + a->burn;
		dy = a->coders[y].last + a->burn;
		if (dx != dy)
			return (dx < dy);
	}
	if (a->coders[x].want != a->coders[y].want)
		return (a->coders[x].want < a->coders[y].want);
	return (a->coders[x].id < a->coders[y].id);
}

/* Меняем местами. */
static void	myswap(int *p, int *q)
{
	int	t;

	t = *p;
	*p = *q;
	*q = t;
}

/* Кладём в кучу. */
void	hpush(t_all *a, int idx)
{
	int	i;
	int	p;

	i = a->hsize;
	a->heap[i] = idx;
	a->hsize++;
	while (i > 0)
	{
		p = (i - 1) / 2;
		if (!better(a, a->heap[i], a->heap[p]))
			break ;
		myswap(&a->heap[i], &a->heap[p]);
		i = p;
	}
}

/* Лучший из троицы. */
static int	best3(t_all *a, int i, int l, int r)
{
	int	b;

	b = i;
	if (l < a->hsize && better(a, a->heap[l], a->heap[b]))
		b = l;
	if (r < a->hsize && better(a, a->heap[r], a->heap[b]))
		b = r;
	return (b);
}

/* Достаём лучшего. */
int	hpop(t_all *a)
{
	int	top;
	int	i;
	int	nx;

	if (a->hsize == 0)
		return (-1);
	top = a->heap[0];
	a->hsize--;
	if (a->hsize > 0)
	{
		a->heap[0] = a->heap[a->hsize];
		i = 0;
		while (1)
		{
			nx = best3(a, i, i * 2 + 1, i * 2 + 2);
			if (nx == i)
				break ;
			myswap(&a->heap[i], &a->heap[nx]);
			i = nx;
		}
	}
	return (top);
}
