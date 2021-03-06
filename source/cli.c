/* scrypt-utility command-line interface.

   copyright 2013-2016 Julian Kalbhenn <jkal@posteo.eu>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "scrypt.h"
#include "../foreign/base91/base91.c"
#include "shared.c"

#define version "0.3"

void display_help () {
  puts(
    "scrypt-kdf [options ...] password [salt N r p size salt-size]\n"
    "                         string [string integer integer integer integer integer]\n"
    "options\n"
    //"  -i|--inputfile path  read password in binary from file at path\n"
    //"  -s|--saltfile path  read salt in binary from file at path\n"
    //"  -o|--outputfile path  write the result string to file at path\n"
    "  -b|--base91-input  password and salt arguments are base91 encoded\n"
    "  -c|--check hash  test if hash is derived from a password\n"
    "  -h|--help  display this text and exit\n"
    "  -p|--crypt  use unix crypt format\n"
    "  -v|--version  output version information and exit");
}

int main (int argc, char **argv) {
  uint8_t use_ascii_input = 1;
  uint8_t use_base91_input = 0;
  uint8_t use_crypt_output = 0;
  uint8_t use_base91_output = 1;
  uint8_t* check_string = 0;
  int opt;
  struct option longopts[9] = {
    {"inputfile", required_argument, 0, 'i'},
    {"saltfile", required_argument, 0, 's'},
    {"outputfile", required_argument, 0, 'o'},
    {"base91-input", no_argument, 0, 'b'},
    {"crypt", no_argument, 0, 'p'},
    {"check", required_argument, 0, 'c'},
    {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'v'},
    {0, 0, 0, 0}
  };
  while ((opt = getopt_long(argc, argv, "c:i:s:o:bphv", longopts, 0)) != -1) {
    switch (opt) {
    case 'v':
      printf("%s\n", version);
      return(0);
    case 'b': use_base91_input = 1; break;
    case 'p': use_crypt_output = 1; break;
    case 'c': check_string = optarg; break;
    case 'i':
    case 's':
    case 'o':
    case 'h':
      display_help();
      return(0);
    }
  }

  uint8_t* password = 0;
  uint8_t* salt = 0;
  size_t password_len = 0;
  size_t salt_len = 0;
  size_t size = 0;
  uint64_t N = 0;
  uint32_t r = 0;
  uint32_t p = 0;

  //-- input
  //argument 1
  if (optind < argc) {
    if (use_base91_input) {
      password = malloc(strlen(argv[optind]));
      password_len = base91_decode(password, argv[optind], strlen(argv[optind]));
    }
    else {
      password = argv[optind];
      password_len = strlen(argv[optind]);
    }
    optind += 1;
    //argument 2
    if (!check_string && (optind < argc)) {
      if (*argv[optind] == '-') { salt = 0; salt_len = 0; }
      else if (use_base91_input) {
	salt = malloc(strlen(argv[optind]));
	salt_len = base91_decode(salt, argv[optind], strlen(argv[optind]));
      }
      else {
	salt = argv[optind];
	salt_len = strlen(argv[optind]);
      }
      optind += 1;
      //argument 3
      if (optind < argc) {
	N = atol(argv[optind]); optind += 1;
	//argument 4
	if (optind < argc) {
	  r = atoi(argv[optind]); optind += 1;
	  //argument 5
	  if (optind < argc) {
	    p = atoi(argv[optind]); optind += 1;
	    //argument 6
	    if (optind < argc) {
	      size = atol(argv[optind]) / 8; optind += 1;
	      //argument 7
	      if (!salt && (optind <= argc)) {
		salt_len = atoi(argv[optind]) / 8;
	      }
	    }
	  }
	}
      }
    }
  }
  else {
    puts("missing argument \"password\"\n");
    display_help();
    return(1);
  }

#define require_success(status) if (status) { puts(scrypt_strerror(status)); return(status); }

  //-- output
  uint8_t* res;
  size_t res_len;
  uint32_t status;
  if (check_string) {
    uint8_t* key;
    size_t key_len;
    if (use_crypt_output) {
      status = scrypt_parse_string_crypt(check_string, strlen(check_string), &salt, &salt_len, &N, &r, &p);
#if verbose
      printf("salt %s, N %lu, r %d, p %d, salt_len %lu\n", salt, N, r, p, salt_len);
#endif
      require_success(status);
      status = scrypt_to_string_crypt(password, password_len, salt, salt_len, N, r, p, &res, &res_len);
    }
    else {
      status = scrypt_parse_string_base91(check_string, strlen(check_string), &key, &key_len, &salt, &salt_len, &N, &r, &p);
#if verbose
    printf("salt %s, N %lu, r %d, p %d, key_len %lu, salt_len %lu\n", salt, N, r, p, key_len, salt_len);
#endif
      require_success(status);
      status = scrypt_to_string_base91(password, password_len, salt, salt_len, N, r, p, key_len, &res, &res_len);
    }
    require_success(status);
    puts((memcmp(res, check_string, res_len) == 0) ? "success" : "failure");
  }
  else {
    if (use_crypt_output) {
      status = scrypt_to_string_crypt(password, password_len, salt, salt_len, N, r, p, &res, &res_len);
    } else {
      status = scrypt_to_string_base91(password, password_len, salt, salt_len, N, r, p, size, &res, &res_len);
    }
    require_success(status);
    puts(res);
  }
}
