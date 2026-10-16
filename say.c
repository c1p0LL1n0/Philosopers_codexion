/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   say.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <digglega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/16 00:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	put_num(char *buf, uint64_t v)
{
	char	tmp[24];
	int		len;
	int		i;

	len = 0;
	if (v == 0)
		tmp[len++] = '0';
	while (v > 0)
	{
		tmp[len++] = (char)('0' + v % 10);
		v /= 10;
	}
	i = 0;
	while (len > 0)
		buf[i++] = tmp[--len];
	return (i);
}

void	say(t_all *a, int id, const char *m, uint64_t ts)
{
	char	buf[128];
	int		len;
	int		i;

	len = 0;
	len += put_num(buf + len, ts);
	buf[len++] = ' ';
	len += put_num(buf + len, (uint64_t)id);
	buf[len++] = ' ';
	i = 0;
	while (m[i] && len < 120)
		buf[len++] = m[i++];
	buf[len++] = '\n';
	pthread_mutex_lock(&a->pen);
	write(1, buf, (size_t)len);
	pthread_mutex_unlock(&a->pen);
}

void	say_took(t_all *a, int idx, uint64_t now)
{
	int	id;

	id = a->coders[idx].id;
	if (a->n == 1)
		say(a, id, "has taken a dongle", now);
	else
	{
		say(a, id, "has taken a dongle", now);
		say(a, id, "has taken a dongle", now);
	}
	say(a, id, "is compiling", now);
}