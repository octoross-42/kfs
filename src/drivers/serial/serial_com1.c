#include "serial.h"

// x86 a 4 ports serial: COM1, COM2, COM3, COM4
// 		-> qemu redirige COM1 vers vers terminal host (option -serial stdio)

// COM1 géré par chip  UART 16550, avec ports de 0x3F8 à 0x3FF:
// 0x3F8 + offset = registres:
// 			+0		: Data Register (DR)		-> lire/écrire un octet
// 			+1		: Interrupt Enable (IER)	-> activer/désactiver les interruptions
// 			+2		: FIFO Control (FCR) 		-> configurer le buffer interne
// 			+3		: Line Control (LCR)		-> format des données + DLAB
// 			+4		: Modem Control (MCR)		-> signaux RTS/DTR
// 			+5		: Line Status (LSR)			-> état de la transmission

void serial_init_com1(void)
{
	// Disable interrupts: kfs-1 n'a pas d'idt, polling à la boucle while
	outb(COM1 + 1, 0x00);

 	// Enable DLAB (divisor latch access bit), 0x80
	// 										   -> 0b01000000
	// 												-> bit 7 = 1 -> active DLAB
	//		-> pour set baud rate divisor par la suite
	// 			-> baud rate = vitesse de transmission en bits/seconde, UART est en 115200Hz
	// 		-> dlab fait que les offset +0 ou +1 ne pointent plus vers DR/IER mais les 2 octets du diviseur baud rate
	// 			-> baud rate divisor peut valoir jusqu'a 115200 (115200 / 1)
	//			-> 115200 hz: 16 bits (un word): 2 octets -> 2 registres pour un offset :)
	// 			-> sauf que ces 2 octets ne sont utiles qu'à l'init youhou !
	// 			-> on remape +0 +1 temporairement pour le diviseur puis on remet +0 +1 sur DR et IER
	//  RECAP:	DLAB = 1 -> +0 = baud diviseur low octet
	// 						+1 = baud divisuer high octet
	// 			DLAB = 0 -> +0 = DR
	// 						+1 = IER
	// 	ICI: bit 7 = 1 -> active DLAB
	outb(COM1 + 3, 0x80);
	outb(COM1 + 0, 0x01); // Divisor low byte  → diviseur=1, 115200 baud rate
	outb(COM1 + 1, 0x00); // Divisor high byte				 38400 baud rate est le compromis historique pour rapide mais pas trop sensible aux bruits, mais qemu donc on met le max pour bcp log :)


	// LCR (line control register), 0x03 = 8N1
	// 								-> 0b00000011
	// 									-> bit 7 = 0 -> désactive dlab
	// 									-> bit 0,1 = 1 -> 8 data bits
	// 									-> bit 2 = 0 -> 1 stop bit
	// 									-> bit 3-5 = 0 -> no parity bit
	// 		-> 8: 8 bits data
	// 		-> N: pas de bit de parité
	// 			-> (pas de bruit sur qemu donc pas de détection de corruption), 1 bit de stop
	//		-> 1: 1 bit de stop
	// paquet 8N1 = [bit de start][d1][d2][d3][d4][d5][d6][d7][d8][bit de stop]
	// 		-> toujours un bit de start, pas configurable
	outb(COM1 + 3, 0x03); // 8 bits, no parity, 1 stop bit (8N1), clear DLAB


	// FCR (fifo control registre), 0xC7
	// 								-> 0b11000111
	// 									-> bit 0 = 1 -> FIFO activé
	// 									-> bit 1 = 1 -> clear RX
	// 									-> bit 1 = 1 -> clear TX
	// 									-> bit 6,7 = 1 -> seuls de 14 octets
	// 	-> configure RX et TX en fifo, avec les options suivantes:
	// on les configure à 0xC7:
	// 		-> on active FIFO pour stocker 16 octets dans le buffer avant de recevoir autre chose (vidage par lecture in, ou transmission du chip)
	// 		-> on vide buffer RX (receive buffer)	-> on clear RX, TX à l'init pour éviter les résidus au démarrage
	// 		-> on vide buffer TX (transmit buffer)		-> sur qemu ca n'arrivera pas normalement mais c'est plus propre
	// 		-> seul d'interruption à 14 octets (14 octets dans FIFO avant de déclencher une interruption, marge de 2 octets pour pas overflow si un peu de retard sur l'interrupt)
	// 			-> on rappelle qu'on n'a pas d'idt, configuration useless mais convention et pour la suite
	outb(COM1 + 2, 0xC7);


	// MCR (modem control register), 0x0B
	// 								 -> 0b00001011
	// 								 	-> bit 0 = 1 -> DTR
	// 								 	-> bit 1 = 1 -> RTS
	// 								 	-> bit 3 = 1 -> IRQ
	// 	-> on déclare DTR (data terminal ready)
	// 	-> on déclare RTS (ready to send)
	// 	-> on active IRQ (interrupt request -> signal de périphérique pour le cpu pour demander d'étre traité)
	// 		-> convention, les IRQ sont désactivées en amont depuis IER de toute maniere :)
	outb(COM1 + 4, 0x0B);
}

void serial_write_char_com1(char c)
{
	while (!serial_com1_tx_empty())		// on attend qu'il soit dispo
		;
	outb(COM1, c);
}

void serial_write_com1(const char *s)
{
    while (*s)
        serial_write_char_com1(*(s ++));
}
