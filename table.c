/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <ihavauch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/14 13:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Учим кодеров и будим замки. */
static int	make_locks(t_all *a)
{
	int	i;

	if (pthread_mutex_init(&a->lock, NULL))
		return (1);
	if (pthread_mutex_init(&a->pen, NULL))
		return (1);
	if (pthread_cond_init(&a->bell, NULL))
		return (1);
	i = 0;
	while (i < a->n)
	{
		if (pthread_mutex_init(&a->dongles[i].m, NULL))
			return (1);
		a->dongles[i].owner = -1;
		a->coders[i].id = i + 1;
		a->coders[i].idx = i;
		a->coders[i].all = a;
		i++;
	}
	return (0);
}

/* Строим стол. */
int	table_init(t_all *a)
{
	a->coders = (t_coder *)malloc(sizeof(t_coder) * (size_t)a->n);
	a->dongles = (t_dongle *)malloc(sizeof(t_dongle) * (size_t)a->n);
	a->heap = (int *)malloc(sizeof(int) * (size_t)a->n);
	if (!a->coders || !a->dongles || !a->heap)
		return (1);
	memset(a->coders, 0, sizeof(t_coder) * (size_t)a->n);
	memset(a->dongles, 0, sizeof(t_dongle) * (size_t)a->n);
	a->hsize = 0;
	a->stop = 0;
	if (make_locks(a))
		return (1);
	return (0);
}

/* Убираем стол. */
void	table_free(t_all *a)
{
	int	i;

	if (a->dongles)
	{
		i = 0;
		while (i < a->n)
		{
			pthread_mutex_destroy(&a->dongles[i].m);
			i++;
		}
	}
	pthread_mutex_destroy(&a->lock);
	pthread_mutex_destroy(&a->pen);
	pthread_cond_destroy(&a->bell);
	if (a->coders)
		free(a->coders);
	if (a->dongles)
		free(a->dongles);
	if (a->heap)
		free(a->heap);
}

/* Все наелись? Только с закрытым lock. */
int	all_done(t_all *a)
{
	int	i;

	if (a->need <= 0)
		return (0);
	i = 0;
	while (i < a->n)
	{
		if (a->coders[i].count < a->need)
			return (0);
		i++;
	}
	return (1);
}
