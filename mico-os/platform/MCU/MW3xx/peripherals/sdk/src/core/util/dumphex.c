/*
 * Copyright (C) 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */
#include <ctype.h>
#include <wmstdio.h>

#ifdef CONFIG_DEBUG_BUILD
#define DUMP_WRAPAROUND 80
void dump_hex(const void *data, unsigned len)
{
	wmprintf("**** Dump @ %p Len: %d ****\n\r", data, len);

	int i;
	const char *data8 = (const char *)data;
	for (i = 0; i < len;) {
		wmprintf("%02x ", data8[i++]);
		if (!(i % DUMP_WRAPAROUND))
			wmprintf("\n\r");
	}

	wmprintf("\n\r******** End Dump *******\n\r");
}

void dump_hex_ascii(const void *data, unsigned len)
{
	wmprintf("**** Dump @ %p Len: %d ****\n\r", data, len);

	int i;
	const unsigned char *data8 = (const unsigned char *)data;
	for (i = 0; i < len;) {
		wmprintf("%02x", data8[i]);
		if (!iscntrl(data8[i]))
			wmprintf(" == %c  ", data8[i]);
		else
			wmprintf(" ==    ");

		i++;
		if (!(i % DUMP_WRAPAROUND / 2))
			wmprintf("\n\r");
	}

	wmprintf("\n\r******** End Dump *******\n\r");
}

void dump_ascii(const void *data, unsigned len)
{
	wmprintf("**** Dump @ %p Len: %d ****\n\r", data, len);

	int i;
	const unsigned char *data8 = (const unsigned char *)data;
	for (i = 0; i < len;) {
		if (isprint(data8[i]))
			wmprintf("%c", data8[i++]);
		else
			wmprintf("(%02x)", data8[i++]);
		if (!(i % DUMP_WRAPAROUND))
			wmprintf("\n\r");
	}

	wmprintf("\n\r******** End Dump *******\n\r");
}

void print_ascii(const void *data, unsigned len)
{
	wmprintf("**** Print @ %p Len: %d ****\n\r", data, len);

	int i;
	const char *data8 = (const char *)data;
	for (i = 0; i < len;) {
		wmprintf("%c", data8[i++]);
	}

	wmprintf("\n\r******** End Print *******\n\r");
}


void dump_json(const void *buffer, unsigned len)
{
	wmprintf("**** Print @ %p Len: %d ****\n\r", buffer, len);

	int i;
	int indentation_level = 0;
	const char *data8 = (const char *)buffer;
	for (i = 0; i < len; i++) {
		if (data8[i] == '{') {
			wmprintf("\r\n");
			int l_indent_level = indentation_level;
			while (l_indent_level--)
				wmprintf(" ");
			wmprintf("{\r\n");
			indentation_level += 2;
			continue;
		}

		if (data8[i] == '}') {
			wmprintf("\r\n");
			indentation_level -= 2;
			int l_indent_level = indentation_level;
			while (l_indent_level--)
				wmprintf(" ");
			wmprintf("}\r\n");
			continue;
		}

		if (data8[i] == ',') {
			wmprintf(",\r\n");
			continue;
		}

		wmprintf("%c", data8[i]);
	}

	wmprintf("\n\r******** End Print *******\n\r");
}
#endif /* CONFIG_DEBUG_BUILD */
