/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <digglega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/16 00:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	make_locks(t_all *a)
{
	int	i;

	if (pthread_mutex_init(&a->pen, NULL))
		return (1);
	i = 0;
	while (i < a->n)
	{
		atomic_store(&a->dongles[i].owner, -1);
		atomic_store(&a->dongles[i].ready, 0);
		a->coders[i].id = i + 1;
		a->coders[i].all = a;
		atomic_store(&a->coders[i].state, WAITING);
		atomic_store(&a->coders[i].count, 0);
		atomic_store(&a->coders[i].last, 0);
		atomic_store(&a->coders[i].ticket, 0);
		i++;
	}
	return (0);
}

int	table_init(t_all *a)
{
	char	*bench_path;

	a->coders = (t_coder *)malloc(sizeof(t_coder) * (size_t)a->n);
	a->dongles = (t_dongle *)malloc(sizeof(t_dongle) * (size_t)a->n);
	if (!a->coders || !a->dongles)
		return (1);
	memset(a->coders, 0, sizeof(t_coder) * (size_t)a->n);
	memset(a->dongles, 0, sizeof(t_dongle) * (size_t)a->n);
	atomic_store(&a->stop, 0);
	atomic_store(&a->next_ticket, 1);
	a->bench = NULL;
	if (make_locks(a))
		return (1);
	bits_init(a);
	if (!a->fork_bits)
		return (1);
	bench_path = getenv("CODEXION_BENCH");
	if (bench_path)
		a->bench = fopen(bench_path, "w");
	return (0);
}

void	table_free(t_all *a)
{
	pthread_mutex_destroy(&a->pen);
	bits_free(a);
	if (a->coders)
		free(a->coders);
	if (a->dongles)
		free(a->dongles);
	if (a->bench)
		fclose(a->bench);
}

int	all_done(t_all *a)
{
	int	i;

	if (a->need <= 0)
		return (0);
	i = 0;
	while (i < a->n)
	{
		if (atomic_load(&a->coders[i].count) < a->need)
			return (0);
		i++;
	}
	return (1);
}
