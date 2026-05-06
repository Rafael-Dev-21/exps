#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const char *cons[] = {
	"k",
	"p",
	"m",
	"n",
	"t",
	"j",
	NULL
};
int ccount = 6;

const char *vows[] = {
	"i",
	"e",
	"a",
	"o",
	"u",
	NULL
};
int vcount = 5;

const char *rand_con(void)
{
	return cons[rand()%ccount];
}

const char *rand_vow(void)
{
	return vows[rand()%vcount];
}

const char *parse_single(const char *fmt)
{
	switch (*fmt) {
	case 'c':
		return rand_con();
	case 'v':
		return rand_vow();
	default:
		return "";
	}
}

int parse_paren(char *res, const char *fmt, int *len)
{
  int got = 0;
  while (*fmt != ')' && *fmt != 0) {
	  if (rand()%2) {
      if (*fmt == 'c' || *fmt == 'v') {
		    (*res++) = *parse_single(fmt);
        (*len)++;
      } else if (*fmt == '(') {
        int oldgot = got, oldlen = *len;
        got += parse_paren(res, fmt, len);
        fmt += got - oldgot;
        fmt--;
        res += *len - oldlen;
      }
		}
    got++;
    fmt++;
	}
  return got;
}

int parse_syll(char *res, const char *fmt)
{
	int len, got, oldlen;
  len = 0;
	while (*fmt) {
		switch(*fmt) {
		case 'c':
		case 'v':
			(*res++) = *parse_single(fmt);
			len++;
			break;
    case '(':
      oldlen = len;
      got = parse_paren(res, fmt, &len);
      fmt += got;
      fmt--;
      res += len - oldlen;
			break;
		}
    if (*fmt == '\0') {
      return len;
    }
		fmt++;
	}
	return len;
}

void parse_word(char *res, const char *sfmt, int min, int max)
{
	int length = min + (rand() % (max-min));
	int off=0;
	for (int i = 0; i < length; i++) {
		off += parse_syll((res+off), sfmt);
	}
}

int main(int argc, char** argv) {
  char result[50]={0};
  srand((unsigned)time(NULL));
	parse_word(result, "(c(v))v(c)", 5, 7);
	printf("%s\n", result);
    return 0;
}
