#include "os.h"

static int _vscanf(const char* s, va_list vl)
{
	int format = 0;
	int res = 0;
	char last = 0;
	int success = -1;
	
	for (; *s; s++) {
		if (format) {
			switch(*s) {
			case 'd': {
				success = 0;
				int *dst = va_arg(vl, int *);
				int val = 0;
				char c;
				while(1) {
					if(last) {
						c = last, last = 0;
					} else {
						c = uart_getc();
					}
					if(c >= '0' && c <= '9') {
						val = val * 10 + c - '0';
						success = 1;
					} else {
						last = c;
						break;
					}
				}
				*dst = val;
				format = 0;
				break;
			}
			case 's': {
				success = 0;
				char *dst = va_arg(vl, char *);
				char c;
				while(1) {
					if(last) {
						c = last, last = 0;
					} else {
						c = uart_getc();
					}
					if(c != ' ' && !(c >= 9 && c <= 13)) {
						*dst = c;
						dst++;
						success = 1;
					} else {
						last = c;
						break;
					}
				}
				format = 0;
				break;
			}
			default:
				break;
			}
		} else if (*s == '%') {
			format = 1;
		} else if (*s == ' ') {
			while(1) {
				char c;
				if(last) {
					c = last, last = 0;
				} else {
					c = uart_getc();
				}
				if(c != ' ' && !(c >= 9 && c <= 13)) {
					last = c;
					break;
				}
			}
		} else {
			char c;
			if(last) {
				c = last, last = 0;
			} else {
				c = uart_getc();
			}
			if(*s != c) {
				break;
			}
		}
		if(success == 1) {
			res++;
			success = -1;
		} else if(!success) {
			break;
		}
	}

	return res;
}

int kscanf(const char* s, ...)
{
	int res = 0;
	va_list vl;
	va_start(vl, s);
	res = _vscanf(s, vl);
	va_end(vl);
	return res;
}
