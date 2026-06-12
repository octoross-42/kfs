/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serial.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: octoross <octoross@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 13:28:55 by octoross          #+#    #+#             */
/*   Updated: 2026/06/12 14:59:27 by octoross         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERIAL_H
#define SERIAL_H

# include "io.h"

# define COM1 0x3F8
// # define COM2 0x2F8
// # define COM3 0x3E8
// # define COM4 0x2E8

void		serial_init_com1(void);
static inline int	serial_com1_tx_empty(void)
{
	return (inb(COM1 + 5) & 0x20); // +5: Line Status Register, 0x20 = THR empty
}

void		serial_write_char_com1(char c);
void		serial_write_com1(const char *s);

#endif